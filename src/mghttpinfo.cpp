/***************************************************************************
*  mghttpinfo.cpp
*
*  Wed Sep  6 22:19:52 2006
*  Copyright  2006  liubin,China
*  Email multiget@gmail.com
****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#include "mgapp.h"
#include "mghttpinfo.h"
#include "mgsingletask.h" //for outmsg
#include "mgfilemanager.h"
#include "mgurlparser.h"

#include <sys/types.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

CMgHttpInfo::CMgHttpInfo(
    CMgSingleTask	*parent,
    std::string fullurl,
    int retry,
    int retrywait,
    std::string	refer,
    int	msgid
)
{
    m_pParent = parent;
    m_nMsgId = msgid;
    CUrlParser par;
    par.SetUrl( fullurl );
    m_server = par.GetServer();
    m_port = par.GetPort();
    m_file = par.GetEscFilePathName();
    m_host = par.GetServer();
    m_refer = refer.empty() ? par.GetRefer() : refer;
    m_bResume = true; //default is true for http
    m_nFileSize = -1;
    m_nRetry = retry; //
    m_nRetryWait = retrywait;
    m_bRedirect = false;
    m_RetCode = -1;
}

//don't use return value;
bool CMgHttpInfo::GetInfo()
{

    char buf[ 1024 ];

again:

    sprintf( buf, c_str(_("connecting  %s:%d ...")).c_str(), m_server.c_str(), m_port );
    OutMsg( buf, MSG_INFO );

    if ( !Connect( m_server.c_str(), m_port ) )
    {
        OutMsg( _("Connect fail."), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return false;
        }
    }


    if ( !SendHttpHeadRequest( m_host.c_str(), m_file.c_str(), m_refer.c_str() ) )
        //	if(!SendHttpGetRequest(m_host.c_str(),m_file.c_str(),m_refer.c_str()))
    {
        OutMsg( "send requrest fail", MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return false;
        }

    }

    //m_file have full path from /
    //if m_file= "/dir/list/abc.txt" we try to locate the last /
    //and give "/dir/list"
    std::string prestr;
    size_t last = m_file.find_last_of( '/' );

    if ( last != std::string::npos )
    {
        prestr = m_file.substr( 0, last );
    }


    int pos = 0;
    int headpos = 0;

    while ( !( headpos = IsHeadEnd( buf, pos ) ) )
    {
        int n = Read( buf + pos, 1024 - pos );

        if ( n < 0 )
        {
			OutMsg("read head error",MSG_ERROR);
            return false;
        }

        if ( n == 0 )
            break;

        pos += n;
    }

    Close();

    buf[ headpos ] = 0;

    int movepos = 0;
    char line[ 256 ];
    int linebyte;

    m_RetCode = -1;
    m_nFileSize = -1;
    //m_bRedirect=false;

    while ( ( linebyte = GetBufLine( buf + movepos, headpos, line ) ) > 0 )
    {
        if ( strlen( line ) > 0 )
        {
            OutMsg( line, MSG_IN );
        }

        movepos += linebyte;

#ifdef WIN32

        if ( strnicmp( line, "HTTP/1.", 7 ) == 0 )
#else

        if ( strncasecmp( line, "HTTP/1.", 7 ) == 0 )
#endif

        {

            char retcode[ 4 ];
            memcpy( retcode, line + 9, 3 );
            retcode[ 3 ] = 0;
            m_RetCode = atoi( retcode );

        }
#ifdef WIN32
        else if ( strnicmp( line, "CONTENT-LENGTH:", 15 ) == 0 )
#else

        else if ( strncasecmp( line, "CONTENT-LENGTH:", 15 ) == 0 )
#endif

        {

            char slen[ 30 ];

			if(strlen(line+15) > 25) {
				OutMsg("length error",MSG_ERROR);
				return false; //check buf out. no retry
			}
			
            strcpy( slen, line + 15 );

#ifdef WIN32

			m_nFileSize = _atoi64(slen);
#else 
			sscanf( slen, "%lld", &m_nFileSize );
#endif
        }
#ifdef WIN32
        else if ( strnicmp( line, "LOCATION:", 9 ) == 0 )
#else

        else if ( strncasecmp( line, "LOCATION:", 9 ) == 0 )
#endif

        {
            //some time not begin with http:// or ftp://
            char redirect[ 512 ] = {0};
            memcpy( redirect, line + 9, strlen( line ) - 9 );
            int emp = 0;

            while ( redirect[ emp ] == ' ' )
                emp++;

            m_Redirection = std::string( redirect + emp );

            m_bRedirect = true;

#ifdef WIN32

            if ( strnicmp( m_Redirection.c_str(), "HTTP://", 7 ) != 0 &&
                    strnicmp( m_Redirection.c_str(), "FTP://", 6 ) != 0 )
#else

            if ( strncasecmp( m_Redirection.c_str(), "HTTP://", 7 ) != 0 &&
                    strncasecmp( m_Redirection.c_str(), "FTP://", 6 ) != 0 )
#endif

            {
				if( m_Redirection.length() > 0 && m_Redirection[0] == '/' )
				{
					//some give /dir/to/file location
					m_Redirection = std::string( "http://" ) + m_server + m_Redirection;
				}
				else
				{
                	//not a full url,append something to head
                	m_Redirection = std::string( "http://" ) + m_server + prestr +
                                std::string( "/" ) + m_Redirection;
				}
            }

        }
#ifdef WIN32
        else if ( strnicmp( line, "SET-COOKIE:", 11 ) == 0 )
#else

        else if ( strncasecmp( line, "SET-COOKIE:", 11 ) == 0 )
#endif

        {
            char cookie[ 512 ] = {0};

            int emp = 0, end;

            while ( line[ emp + 11 ] == ' ' )
                emp++;

            end = emp;

            while ( line[ end + 11 ] != ';' && line[ end ] )
                end++;

            memcpy( cookie, line + 11 + emp, end - emp );

            cookie [ end - emp ] = 0;

            AddCookie( std::string( cookie ) );

        }
    }




    if ( m_RetCode == -1 )
    {

        m_nLastError = -202;
        return false;
    }

    if ( m_RetCode >= 400 || m_RetCode < 200 )
    {
        OutMsg( "retcode fail", MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            m_nLastError = -203;
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return false;
        }

    }

    if ( m_RetCode / 100 == 2 )
    {

        if ( m_nFileSize != -1 )
        {

            char buf[ 100 ];
            sprintf( buf, "Get File Length=%lld", m_nFileSize );
            OutMsg( buf, MSG_SUCCESS );
            return true;
        }
        else
        {
            m_nLastError = -200;
            OutMsg( "no file length info!", MSG_WARNNING ); //返回正确但没长度
            return true;

        }

    }

    if ( m_RetCode / 100 == 3 )
    {
        if ( m_bRedirect )
        {
            OutMsg( std::string( "Redirect to:" ) + m_Redirection, MSG_INFO );
            //we auto redirect
            return true;

        }
        else if ( m_nFileSize == -1 )  //maybe 304 not modified
        {
            return GetInfoByGet();
        }
        else
        {
            m_nLastError = -201;
            return false;
        }

    }

    return false;
}

bool CMgHttpInfo::Retry()
{
    if ( --m_nRetry > 0 )
    {
        char buf[ 50 ];
        sprintf( buf, c_str(_("Wait %d seconds to retry...")).c_str(), m_nRetryWait );
        OutMsg( buf, MSG_INFO );

        m_pParent->m_nError++;
#ifdef WIN32
        Sleep( 1000*m_nRetryWait );
#else
        sleep( m_nRetryWait );
#endif
        return true;
    }

    return false;
}


void CMgHttpInfo::OutMsg( const wxChar * str, _MSGTYPE type )
{
    OutMsg( c_str(str), type );
}

void CMgHttpInfo::OutMsg( const std::string& str, _MSGTYPE type )
{
    m_pParent->OutMsg( m_nMsgId, str, type );
}

//检查缓存是否有\r\n\r\n标志头结束，如果有，返回最后一个\n位置，如果没有返回0
int CMgHttpInfo::IsHeadEnd( char *buf, int len )
{

    for ( int i = 3;i < len;i++ )
    {
        if ( buf[ i - 3 ] == '\r' &&
                buf[ i - 2 ] == '\n' &&
                buf[ i - 1 ] == '\r' &&
                buf[ i - 0 ] == '\n' )
        {
            return i;
        }
    }

    return 0;
}

bool CMgHttpInfo::GetInfoByGet()
{
    char buf[ 1024 ];

again:

    if ( !Connect( m_server.c_str(), m_port ) )
    {
        OutMsg( "connect fail", MSG_WARNNING );

        if ( Retry() )
        {

            goto again;

        }
        else
        {
            OutMsg( "no retry,quit", MSG_ERROR );
            return false;
        }
    }


    if ( !SendHttpGetRequest( m_host.c_str(), m_file.c_str(), m_refer.c_str() ) )
    {
        OutMsg( "send request fail", MSG_WARNNING );

        if ( Retry() )
        {

            goto again;
        }
        else
        {
            OutMsg( "no retry,quit", MSG_ERROR );
            return false;
        }
    }

    //m_file have full path from /
    //if m_file= "/dir/list/abc.txt" we try to locate the last /
    //and give "/dir/list"
    std::string prestr;

    size_t last = m_file.find_last_of( '/' );

    if ( last != std::string::npos )
    {
        prestr = m_file.substr( 0, last );
    }


    int pos = 0;
    int headpos = 0;

    while ( !( headpos = IsHeadEnd( buf, pos ) ) )
    {
        int n = Read( buf + pos, 1024 - pos );

        if ( n < 0 )
        {
            return false;
        }

        if ( n == 0 )
            break;

        pos += n;
    }


    buf[ headpos ] = 0;

    int movepos = 0;
    char line[ 256 ];
    int linebyte;

    m_RetCode = -1;
    m_nFileSize = -1;
    //m_bRedirect=false;

    while ( ( linebyte = GetBufLine( buf + movepos, headpos, line ) ) > 0 )
    {
        if ( strlen( line ) > 0 )
        {
            OutMsg( line, MSG_IN );
        }

        movepos += linebyte;

#ifdef WIN32

        if ( strnicmp( line, "HTTP/1.", 7 ) == 0 )
#else

        if ( strncasecmp( line, "HTTP/1.", 7 ) == 0 )
#endif

        {

            char retcode[ 4 ];
            memcpy( retcode, line + 9, 3 );
            retcode[ 3 ] = 0;
            m_RetCode = atoi( retcode );

        }
#ifdef WIN32
        else if ( strnicmp( line, "CONTENT-LENGTH:", 15 ) == 0 )
#else

        else if ( strncasecmp( line, "CONTENT-LENGTH:", 15 ) == 0 )
#endif

        {

            char slen[ 24 ] = {0};
            strcpy( slen, line + 15 );
            sscanf( slen, "%lld", &m_nFileSize );

        }
#ifdef WIN32
        else if ( strnicmp( line, "LOCATION:", 9 ) == 0 )
#else

        else if ( strncasecmp( line, "LOCATION:", 9 ) == 0 )
#endif

        {
            //some time not begin with http:// or ftp://
            char redirect[ 512 ] = {0};
            memcpy( redirect, line + 9, strlen( line ) - 9 );
            int emp = 0;

            while ( redirect[ emp ] == ' ' )
                emp++;

            m_Redirection = std::string( redirect + emp );

            m_bRedirect = true;

#ifdef WIN32

            if ( strnicmp( m_Redirection.c_str(), "HTTP://", 7 ) != 0 &&
                    strnicmp( m_Redirection.c_str(), "FTP://", 6 ) != 0 )
#else

            if ( strncasecmp( m_Redirection.c_str(), "HTTP://", 7 ) != 0 &&
                    strncasecmp( m_Redirection.c_str(), "FTP://", 6 ) != 0 )
#endif

            {
                //not a full url,append something to head
                m_Redirection = std::string( "http://" ) + m_server + prestr +
                                std::string( "/" ) + m_Redirection;
            }

        }
#ifdef WIN32
        else if ( strnicmp( line, "SET-COOKIE:", 11 ) == 0 )
#else

        else if ( strncasecmp( line, "SET-COOKIE:", 11 ) == 0 )
#endif

        {
            char cookie[ 512 ] = {0};

            int emp = 0, end;

            while ( line[ emp + 11 ] == ' ' )
                emp++;

            end = emp;

            while ( line[ end + 11 ] != ';' && line[ end ] )
                end++;

            memcpy( cookie, line + 11 + emp, end - emp );

            cookie [ end - emp ] = 0;

            AddCookie( std::string( cookie ) );

        }
    }

    Close();


    if ( m_RetCode == -1 )
    {

        m_nLastError = -202;
        return false;
    }

    if ( m_RetCode >= 400 || m_RetCode < 200 )
    {
        OutMsg( "retcode fail", MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            m_nLastError = -203;
            OutMsg( "no retry,quit", MSG_ERROR );
            return false;
        }
    }

    if ( m_RetCode / 100 == 2 )
    {

        if ( m_nFileSize != -1 )
        {

            char buf[ 100 ];
            sprintf( buf, "Get File Length=%lld", m_nFileSize );
            OutMsg( buf, MSG_SUCCESS );
            return true;
        }
        else
        {
            m_nLastError = -200;
            OutMsg( "no file length info!", MSG_WARNNING );
            return true;

        }

    }

    if ( m_RetCode / 100 == 3 )
    {
        if ( m_bRedirect )
        {
            OutMsg( std::string( "Redirect to:" ) + m_Redirection, MSG_INFO );
            //we auto redirect
            return true;

        }
        else
        {
            m_nLastError = -201;
            OutMsg( "retcode=3xx,but not redirect,quit.", MSG_ERROR );
            return false;
        }

    }

    return false;
}

