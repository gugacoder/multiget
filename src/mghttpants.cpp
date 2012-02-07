
/***************************************************************************
 *  mghttpants.cpp
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
/*
	the UTF-8 Chinese char will break vc6 compile
	
	this file is created on 2006/09/04 by liubin,China
	
	
*/

#ifdef WIN32
#include <winsock2.h>
#endif
#include "mgapp.h"
#include "mghttpants.h"
#include "mgfilemanager.h"
#include "mgsingletask.h"
#include "mgurlparser.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <pthread.h>

using namespace std;

CMgHttpAnts::CMgHttpAnts(
    CMgSingleTask	*parent,
    CMgFileManager *fm,
    std::string url,
    int antid, //1 based
    int retry,
    int retrywait,
    llong from,
    llong tsize,
    std::string refer
)
{
    m_pParent = parent;
    m_pFM = fm;

    m_nAntId = antid;
    m_nRetry = retry;
    m_nRetryWait = retrywait;
    m_nFrom = from;
    m_nFileSize = tsize;

    //get refer,host,file from url
    CUrlParser par;
    par.SetUrl( url );
    m_sUrl = par.GetRawUrl();
    m_file = par.GetEscFilePathName();
    m_host = par.GetServer();
    m_sServer = par.GetServer();
    m_Port = par.GetPort();
    m_refer = refer.empty() ? par.GetRefer() : refer;
    m_nTotalByte = 0;
}

CMgHttpAnts::~CMgHttpAnts()
{
}



void CMgHttpAnts::OutMsg( const wxChar * str, _MSGTYPE type )
{
    OutMsg( c_str(str), type );
}

void CMgHttpAnts::OutMsg( const string& str, _MSGTYPE type )
{
    m_pParent->OutMsg( m_nAntId, str, type );
}


//return value
// <0  : something wrong ,task fail
// =0  : redirect to new url
// >0  : finish piece
int CMgHttpAnts::Go()
{

    int nret;

    //int oldstate;

again:
    //make connection
    OutMsg( c_str(_("Connecting ")) + m_sServer + string( "..." ) );

    if ( !Connect( m_sServer.c_str(), m_Port ) )
    {
        OutMsg( _("Connect fail."), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -1;
        }

    }


    //remove http header info we don't want
    if ( !GetFile( m_file.c_str(), m_nFrom + m_nTotalByte, m_refer.c_str() ) )
    {
        OutMsg( _("Fail to get file"), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -2;
        }
    }


    char buf[ 100 ];
    sprintf( buf, c_str(_("Receiving file data from %lld ...")).c_str(), m_nFrom + m_nTotalByte );
    OutMsg( buf, MSG_INFO );

    llong ndata = 0;

    nret = GetData( m_nFrom + m_nTotalByte, ndata );

    if ( nret == 0 )
    { //ok

        OutMsg( _("Finish task session normally, quit.") , MSG_SUCCESS );
        return 1;

    }
    else
    { //error

        switch ( nret )
        {

            case -1:
            OutMsg( _("Write file error."), MSG_WARNNING );
            break;

            case - 2:
            OutMsg( _("Network error."), MSG_WARNNING );
            break;

            case - 3:
            OutMsg( _("change a URL"), MSG_WARNNING );
            break;

            default:
            OutMsg( "Other error", MSG_WARNNING );
            break;
        }


        m_nTotalByte += ndata;

        if ( nret == -3 )
        {
            //goto again;
            return -3;
        }
        else if ( Retry() )
        {
            goto again;

        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return nret;
        }
    }
}


//发送请求文件命令
bool CMgHttpAnts::GetFile( const char* filename, llong pos, const char* refer )
{

    if ( SendHttpGetRequest( m_host.c_str(), filename, pos, refer ) )
    {
        return GetHead();
    }

    return false;

}

//获取数据
int CMgHttpAnts::GetData( llong spos, llong& bytes )
{

    int movepos = 0;
    int nret;
    llong total = 0;	//本次已经接收的数据,最后返回给bytes
    unsigned char buffer[ RECVBUF ];

    while ( 1 )
    {
        nret = Read( buffer + movepos, RECVBUF - movepos );

        if ( nret < 0 )
        {
            //OutMsg( "network error while read data.", MSG_WARNNING );
            //if buffer have old data, write out
            /*/
            			bytes=total;
            			return -2;
            /*/

            if ( movepos > 0 )
            {

                int wret = WriteData( spos + total, movepos, buffer );

                if ( wret < 0 && wret != -3 )
                { //write file error
                    bytes = total;
                    return -1;
                }
                else if ( wret < 0 && wret == -3 )
                {
                    bytes = total;
                    return -3;
                }
                else if ( wret == 0 )
                { //write file ok
                    bytes = total + movepos;
                    return 0;
                }
                else
                { //data not end ,net fail
                    bytes = total + movepos;
                    return -2;
                }
            }
            else
            {
                bytes = total;
                return -2;
            }

            //*/
        }
        else if ( nret == 0 )
        {
            int wret = WriteData( spos + total, movepos, buffer, true );

            if ( wret < 0 && wret != -3 )
            {
                bytes = total;
                return -1;
            }
            else if ( wret < 0 && wret == -3 )
            {
                bytes = total;
                return -3;
            }
            else if ( wret == 0 )
            {
                bytes = total + movepos;
                return 0;
            }
            else
            { //wret > 0
                //OutMsg( "file length doubt!!", MSG_WARNNING );
                bytes = total + movepos;
                return 0;
            }

        }
        else //nret>0 网络接收正常
        {
            movepos += nret;

            if ( movepos >= RECVBUF - 1024 )
            {
                //buffer full , write outCMgFtpAnts
                int wret = WriteData( spos + total, movepos, buffer );

                if ( wret < 0 && wret != -3 )
                {
                    bytes = total;

                    return -1;  //写数据错
                }
                else if ( wret < 0 && wret == -3 )
                {
                    bytes = total;
                    return -3;
                }
                else if ( wret == 0 )
                {
                    total += movepos;
                    movepos = 0;
                    bytes = total;

                    return 0;
                }
                else
                {

                    total += movepos;
                    movepos = 0;

                    continue;
                }
            }
            else
            {
                continue;
            }
        } //if
    } //while(1)

    return 0;
}

int CMgHttpAnts::WriteData( llong offset, int len, unsigned char *buf, bool end )
{

    int OldState, nret, us;
    pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &OldState );

    nret = m_pFM->FileData( m_nAntId , offset, len, buf, end, us );

    pthread_setcancelstate ( OldState, NULL );

    if ( us > 0 )
#ifdef WIN32
		Sleep(1000*us);
#else
        usleep( us );
#endif
    return nret;

}


bool CMgHttpAnts::GetHead()
{

    char buf[ 1024 ];
    int pos = 0;
    int nret;

    if ( ( nret = Read( buf + pos, 4 ) ) <= 0 )
    {
        return false; //先收4个字节
    }

    pos += nret;

    while ( !( buf[ pos - 1 ] == '\n' && buf[ pos - 2 ] == '\r' &&
               buf[ pos - 3 ] == '\n' && buf[ pos - 4 ] == '\r' ) && pos < 1022 )
    {
        if ( ( nret = Read( buf + pos, 1 ) ) <= 0 )
        {
            return false;
        }
        else
        {
            pos += nret;
        }
    }

    if ( pos >= 1022 )
        return false;


    buf[ pos ] = 0;

    int movepos = 0;

    char line[ 256 ];

    int linebyte;

    int nRetCode = -1;

    while ( ( linebyte = GetBufLine( buf + movepos, pos, line ) ) > 0 )
    {
        movepos += linebyte;

        if ( strlen( line ) > 0 )
            OutMsg( line, MSG_IN );



        if ( strncasecmp( line, "HTTP/1.", 7 ) == 0 )
        {

            char retcode[ 4 ];
            memcpy( retcode, line + 9, 3 );
            retcode[ 3 ] = 0;
            nRetCode = atoi( retcode );
        }
    }


    if ( nRetCode / 100 != 2 )
    {
        char buf[ 20 ];
        sprintf( buf, "retcode=%d,fail", nRetCode );

        OutMsg( buf, MSG_ERROR );

        return false;
    }

    return true;
}

bool CMgHttpAnts::Retry()
{
    if ( --m_nRetry > 0 )
    {
        char buf[ 50 ];

        sprintf( buf, c_str(_("Wait %d seconds to retry...")).c_str(), m_nRetryWait );

        OutMsg( buf );

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

/*
void CMgHttpAnts::AnalysisHeader(
		char buf,	//IN buf have header content ended with 0
		std::string prestr, //IN
		bool& redirect,  //OUT
		std::string& redirection, //OUT
		llong&  filesize, 	//OUT
		int&	retcode		//OUT
		)
{
	
	filesize=-1;
	retcode=-1;
	redirect=false;
	
    int movepos = 0;
    char line[ 200 ];  //a line don't longer than 200
    int linebyte;  //line length
 
//	m_RetCode = -1;
//	m_nFileSize=-1;
	
    while ( ( linebyte = GetBufLine( buf + movepos, headpos, line ) ) > 0 )
    {
        if ( strlen( line ) > 0 )	{ OutMsg( line, 2 );}
 
        movepos += linebyte;
 
#ifdef WIN32
		if ( strnicmp(line,"HTTP/1.", 7 ) == 0 )
#else
        if ( strncasecmp( line, "HTTP/1.", 7 ) == 0 )
#endif
        {
 
            char tempcode[ 4 ];
            memcpy( tempcode, line + 9, 3 );
            tempcode[ 3 ] = 0;
            retcode = atoi( tempcode );
 
        }
#ifdef WIN32
		else if(strnicmp( line, "CONTENT-LENGTH:", 15 ) == 0 )
#else
        else if ( strncasecmp( line, "CONTENT-LENGTH:", 15 ) == 0 )
#endif
        {
 
            char slen[ 24 ] = {0};
            strcpy( slen, line + 15 );
            sscanf( slen, "%lld", &filesize );
 
        }
#ifdef WIN32
		else if( strnicmp( line, "LOCATION:", 9 ) == 0 )
#else
        else if ( strncasecmp( line, "LOCATION:", 9 ) == 0 )
#endif
        {
			//some time not begin with http:// or ftp://
            char redirect[ 512 ] = {0};
            memcpy( redirect, line + 9, strlen( line ) - 9 );
            redirection = std::string( redirect );
			Trim(redirection);
            redirect = true;
#ifdef WIN32
			if( strnicmp( m_Redirection.c_str(), "HTTP://", 7 )!=0 &&
				strnicmp( m_Redirection.c_str(), "FTP://", 6)!=0)
#else
			if( strncasecmp( m_Redirection.c_str(), "HTTP://", 7 )!=0 &&
				strncasecmp( m_Redirection.c_str(), "FTP://", 6)!=0)
#endif
			{
				//not a full url,append something to head
				redirection=std::string("http://")+ m_server + prestr +
						std::string("/") + redirection;
			}
 
        }
#ifdef WIN32
		else if(strnicmp( line, "SET-COOKIE:", 11 ) == 0 )
#else
        else if ( strncasecmp( line, "SET-COOKIE:", 11 ) == 0 )
#endif
        {
            char cookie[ 512 ] = {0};
 
			int emp=0,end;
            while ( line[ emp + 11 ] == ' ' )
                emp++;
			
			end=emp;
			while ( line[ end + 11 ] != ';' && line[ end ] )
				end++;
 
			memcpy( cookie, line + 11 + emp, end - emp );
			cookie [ end - emp ]=0;
 
			AddCookie( std::string( cookie ) );
 
        }
    }	
}
 
//trim space at head and rear
void CMgHttpAnts::Trim(std::string& str)
{
	string::size_type pos = str.find_last_not_of(' ');
	if(pos != string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());	
}
*/

