/***************************************************************************
*  mgsocket.cpp
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
	UTF-8 Chinese comment will make VC6 compile fail.
	
    filename	:	mgsocket.cpp
    create on   :	2006/09/02
    author		:	liubin
    email		:	multiget@gmail.com
    version		:	0.3
	license		:	GPL
 
	***history***
		2006/09/03 
		pass with gcc-4.0 and vc6,only support tcp socket with sock5 proxy,
		anonymous or user/pass login.
		2006/09/06
		add function tConnect make connect timeout work
		2006/10/16
		modify tconnect to tConnect
		
*/

/*
Error code [0x01~0xFF]:
0x01: create socket fail;
0x02: gethostbyname fail,maybe host is not valid;
0x03: address family not support (neither AF_INET nor AF_INET6)
0x04: connect fail,maybe host unreachable;
0x05: set timeout on invalid socket.
0x06: set timeout out range.
0x07: send without connection.
0x08: send find connection closed;
0x09: send find network error;
0x0A: send inconsistant error;
0x0B: read without connection;
0x0C: read with wrong args;
0x0D: read find network error;
0x0E: read find connection close;
0x0F: 
*/

#include "mgsocket.h"
extern int gConnectTimeOut;
extern int gReceiveTimeOut;
CMgSocket::CMgSocket()
{

#ifdef WIN32
    WSADATA wsadata;
    WSAStartup( MAKEWORD( 2, 0 ), &wsadata );
#endif

    m_bConnected = false;
    m_bSockValid = false;
    m_nLastError = 0;
    m_nConnectTimeOut = gConnectTimeOut;
    m_nTimeOut = gReceiveTimeOut;
}

CMgSocket::~CMgSocket()
{
    Close();

#ifdef WIN32

    WSACleanup();
#endif
}


bool CMgSocket::Create()
{
#ifdef WIN32
    m_Sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( m_Sock == INVALID_SOCKET )
    {
#else
    m_Sock = socket( AF_INET, SOCK_STREAM, 0 );

    if ( m_Sock == -1 )
    {
#endif
        m_nLastError = 0x01;
        return false;
    }
    else
    {
        m_bSockValid = true;
        return true;
    }

}


//args not set
bool CMgSocket::Connect( const char * server, int port )
{
    /* 2006/10/31
    if(!m_bSockValid) 
    {
    	if( !Create() ) return false;
    }
    else if(m_bConnected)
    {
    	Close();
    	if( !Create() ) return false;		
    }	*/

    Close();

    if ( !Create() )
        return false;

    struct hostent *hp;

    hp = gethostbyname( server );

    if ( !hp )
    {
        m_nLastError = 0x02;
        return false;
    }

    if ( hp->h_addrtype != AF_INET && hp->h_addrtype != AF_INET6 )
    { //neither AF_INET nor AF_INET6
        m_nLastError = 0x03;
        return false;
    }

    struct sockaddr_in servaddr;

    memset( &servaddr, 0, sizeof( servaddr ) );

    servaddr.sin_family = hp->h_addrtype;

    servaddr.sin_port = htons( ( short ) port );

    servaddr.sin_addr = *( ( struct in_addr* ) ( hp->h_addr ) );

    //try connect

    if ( tConnect( m_Sock, ( struct sockaddr* ) & servaddr, sizeof( servaddr ) ) < 0 )
    {
        m_nLastError = 0x04;
        return false;
    }
    else
    {
        m_bConnected = true;

        //set timeout here
        //set the timeout value to socket
#ifdef WIN32
		int winout=m_nTimeOut*1000;
        setsockopt( m_Sock, SOL_SOCKET, SO_SNDTIMEO, ( char* ) & winout, sizeof( int ) );
        setsockopt( m_Sock, SOL_SOCKET, SO_RCVTIMEO, ( char* ) & winout, sizeof( int ) );
#else

        struct timeval tv;
        tv.tv_sec = m_nTimeOut;
        tv.tv_usec = 0;

        //A returned value of 0 indicates the system will not time out. The maximum value is 32 767 seconds.
        //Return code 0 indicates that the function was successful.A return code equal to -1 indicates an error
        setsockopt( m_Sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof( tv ) );
        setsockopt( m_Sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof( tv ) );

#endif

        return true;
    }

}

//after create call this
bool CMgSocket::SetTimeOut( unsigned int nsec )
{
    if ( !m_bSockValid )
    {
        m_nLastError = 0x05;
        return false;
    }

    if ( nsec <= 5 || nsec > 300 )
    {
        m_nLastError = 0x06;
        return false;
    }

    m_nTimeOut = nsec;
    /*
    #ifdef WIN32	
       setsockopt( m_Sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&nsec, sizeof( int ) );
       setsockopt( m_Sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&nsec, sizeof( int ) );	
    #else
       setsockopt( m_Sock, SOL_SOCKET, SO_SNDTIMEO, &nsec, sizeof( int ) );
       setsockopt( m_Sock, SOL_SOCKET, SO_RCVTIMEO, &nsec, sizeof( int ) );	
    #endif
    */ 
    return true;
}

//return true when all data sent out

bool CMgSocket::Send( const void * buf, int buflen )
{
    int pos = 0;
    int nRet = 0;

    char* movehead = ( char* ) buf;

    if ( !m_bConnected )
    {
        m_nLastError = 0x07;
        return false;
    }

    while ( pos < buflen )
    {

        //nRet = send ( m_Sock, (void*)(movehead + pos), buflen - pos, 0/*flags*/ );
#ifdef WIN32
        nRet = send ( m_Sock, ( char* ) ( movehead + pos ), buflen - pos, 0 /*flags*/ );
#else

        nRet = send ( m_Sock, ( void* ) ( movehead + pos ), buflen - pos, 0 /*flags*/ );
#endif
        //nRet=write(m_Sock,(void*)(movehead + pos), buflen - pos);

        if ( nRet == 0 )
        {
            m_nLastError = 0x08;
            Close();
            return false;
        }
        else if ( nRet < 0 )
        {
            m_nLastError = 0x09;
            Close();
            return false;
        }
        else
        {
            pos += nRet;

            if ( pos != buflen )
            {
                continue;
            }
            else
            {
                return true;
            }
        }
    }

    m_nLastError = 0x0A;

    return false;
}

//return bytes
int CMgSocket::Read( void * buf, int buflen )
{
    if ( !m_bConnected )
    {
        m_nLastError = 0x0B;
        return -1;
    }

    if ( buflen <= 0 || buf == NULL )
    {
        m_nLastError = 0x0C;
        return -1;
    }

#ifdef WIN32
    int nret = recv( m_Sock, ( char* ) buf, buflen, 0 );

#else

    int nret = recv( m_Sock, buf, buflen, 0 );

#endif
    //int nret=read(m_Sock,buf,buflen);

    if ( nret < 0 )
    {
        m_nLastError = 0x0D;
        Close();
        return -1;
    }
    else if ( nret == 0 )
    {
        m_nLastError = 0x0E;
        Close();
        return 0;
    }

    return nret;

}

void CMgSocket::Close()
{
    if ( m_bSockValid )
    {
        if ( m_bConnected )
            shutdown( m_Sock, 0 );

#ifdef WIN32

        closesocket( m_Sock );

#else

        close( m_Sock );

#endif

        m_bSockValid = false;

        m_bConnected = false;

    }
}

//with timeout connect
#ifdef WIN32

int CMgSocket::tConnect( SOCKET sock, const struct sockaddr * addr, unsigned int len )
#else

int CMgSocket::tConnect( int sock, const struct sockaddr * addr, unsigned int len )
#endif
{

    int ret, val;

    struct timeval tv;
    fd_set sset;

#ifdef WIN32

    int lon;
    u_long iMode = 1;
    ioctlsocket( sock, FIONBIO, &iMode );
#else

    int flags, old_flags;
    socklen_t lon;
    flags = old_flags = fcntl( sock, F_GETFL, 0 );
    flags |= O_NONBLOCK;

    if ( fcntl( sock, F_SETFL, flags ) == -1 )
    {
        return -1;
    }

#endif

    ret = connect( sock, addr, len );

    if ( ret < 0 )
    {
#ifdef WIN32

        if ( WSAEWOULDBLOCK == WSAGetLastError() )
#else

        if ( errno == EINPROGRESS )
#endif

        {
            tv.tv_sec = m_nConnectTimeOut;
            tv.tv_usec = 0;
            FD_ZERO( &sset );
            FD_SET( sock, &sset );

            if ( select( sock + 1, NULL, &sset, NULL, &tv ) > 0 )
            {
                lon = sizeof( int );
#ifdef WIN32

                getsockopt( sock, SOL_SOCKET, SO_ERROR, ( char* ) ( &val ), &lon );
#else

                getsockopt( sock, SOL_SOCKET, SO_ERROR, ( void* ) ( &val ), &lon );
#endif

                if ( val != 0 )
                {
                    ret = -1;
                }
                else
                {
                    ret = 0;
                }
            }
            else
            {
                ret = -1;
            }

        }
        else
        {
            ret = -1;
        }
    }

#ifdef WIN32
    iMode = 0;

    ioctlsocket( sock, FIONBIO, &iMode );

#else

    fcntl( sock, F_SETFL, old_flags );

#endif

    return ret;

}

//after create call this
void CMgSocket::SetConnectTimeOut( unsigned int nsec )
{
    if ( nsec < 5 || nsec > 300 )
        m_nConnectTimeOut = 30;
    else
        m_nConnectTimeOut = nsec;
}

