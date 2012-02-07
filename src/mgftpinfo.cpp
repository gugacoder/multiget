/***************************************************************************
*  mgftpinfo.cpp
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

#include <iostream>
#include "mgftpinfo.h"
#include "mgsingletask.h" //for outmsg
#include "mgurlparser.h"

CMgFtpInfo::CMgFtpInfo(
    CMgSingleTask* parent,
    std::string fullurl,
    int retry,
    int retrywait,
    int msgid
)
{
    m_pParent = parent;
    m_nMsgId = msgid;
    CUrlParser par;
    par.SetUrl( fullurl );

    m_server = par.GetServer();
    m_port = par.GetPort();
    m_user = par.GetUser();
    m_pass = par.GetPass();
    m_file = par.GetEscFilePathName();
    m_bResume = false;
    m_nFileSize = -1;
    m_nRetry = retry; //
    m_nRetryWait = retrywait;
}

//don't use return value;
bool CMgFtpInfo::GetInfo()
{
    char buf[ 512 ];

again:
    //make connection

    sprintf( buf, "connecting %s:%d ...", m_server.c_str(), m_port );
    OutMsg( buf, MSG_INFO );

    if ( ! MakeCtrlConn( m_server.c_str(), m_port ) )
    {
        OutMsg( "connect fail.", MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( "no retry ,quit.", MSG_ERROR );
            return false;
        }

    }

    OutMsg( "control connection ok", MSG_SUCCESS );

    //login

    if ( !Login( m_user.c_str(), m_pass.c_str() ) )
    {

        Close();
        OutMsg( "login fail.", MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( "no retry,quit.", MSG_ERROR );
            return false;
        }

    }

    //some server need type I mode before setresume
    if ( !EnterBinaryMode() )
    {
        OutMsg( "can't enter binary mode,ignore", MSG_WARNNING );
    }

    //login ok,check if can resume
#ifdef WIN32
    if ( ! SetResume( 100L ) )
#else

    if ( ! SetResume( 100LL ) )
#endif

    {
        m_bResume = false;
        OutMsg( "server NOT support RESUME", MSG_WARNNING );
    }
    else
    {
        m_bResume = true;
        OutMsg( "server support resume", MSG_SUCCESS );
    }

    if ( !SizeFile( m_file.c_str(), m_nFileSize ) )
    {
        if ( !ListFile( m_file.c_str(), m_nFileSize ) )
        {
            //是否文件不存在？检查一下

            if ( !EnterBinaryMode( ) )
            {
                OutMsg( "get file error,quit", MSG_ERROR );
                return false;
            }

            if ( !GetFile( m_file.c_str() ) )
            {
                OutMsg( "file NOT exists,quit", MSG_ERROR );
                return false;
            }

            //文件存在，只是不能提取长度
            m_nFileSize = -1;
        }
    }

    char slen[ 24 ];
    sprintf( slen, "%lld", m_nFileSize );
    OutMsg( std::string( "file size=" ) + std::string( slen ), MSG_SUCCESS );
    return true;

}

bool CMgFtpInfo::Retry()
{
    if ( --m_nRetry > 0 )
    {
        char buf[ 40 ];
        sprintf( buf, "Wait %d seconds retry ...", m_nRetryWait );
        OutMsg( buf, MSG_INFO );

        m_pParent->m_nError++;

#ifdef WIN32

        Sleep( m_nRetryWait * 1000 );
#else

        sleep( m_nRetryWait );
#endif

        return true;
    }

    return false;
}


void CMgFtpInfo::OutMsg( const wxChar *str, _MSGTYPE type )
{
    OutMsg( c_str(str), type );
}

void CMgFtpInfo::OutMsg( const std::string& str, _MSGTYPE type )
{
    m_pParent->OutMsg( m_nMsgId, str, type );
}

//控制连接上发送请求文件命令
bool CMgFtpInfo::GetFile( const char* filename )
{
    int nret;

    char buffer[ 512 ];
    sprintf( buffer, "RETR %s\r\n", filename );

    if ( !Send( buffer, strlen( buffer ) ) )
    {
        return false; //write data error
    }

    sprintf( buffer, "RETR %s", filename );
    OutMsg( buffer, MSG_OUT );

    nret = GetRetCode( );

    if ( nret == 150 )
        return true;
    else
        return false;
}

