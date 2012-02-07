/***************************************************************************
*  mgsocket.h
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
#ifndef _MG_SOCKET_H
#define _MG_SOCKET_H

#ifdef WIN32
#include <winsock2.h> 
//#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#endif

#if defined(__BSD__)||defined(__FreeBSD__)||defined(__OpenBSD__)||defined(__NetBSD__)	//wx preprocessor
//bsd need it, mac do not
#include <netinet/in.h> 
#endif

/*
 
CMgSocket* -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase -> CMgHttpInfo
							|								\->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															\->CMgFtpAnts
*/

/*
Error code range define;
//use the last 8 bit
CMgSocket [0x000000**~0x000000**]
*/

#include "common.h"

class CMgSocket
{

public:
    CMgSocket();
    virtual ~CMgSocket();

public:
    bool Send( const void* buf, int buflen ); //block
    int Read( void* buf, int buflen ); //block
    bool SetTimeOut( unsigned int nsec );
    void SetConnectTimeOut( unsigned int nsec );
    virtual bool Connect( const char* server, int port ); //block
#ifdef WIN32

    int tConnect( SOCKET sock, const struct sockaddr *addr, unsigned int len );
#else

    int tConnect( int sock, const struct sockaddr *addr, unsigned int len );
#endif

    int GetLastError()
    {
        return m_nLastError;
    };

    bool Create();
    void Close();

protected:
    int m_nLastError;  //V
    bool m_bConnected; //V
    unsigned int m_nConnectTimeOut;
    unsigned int m_nTimeOut;

private:

#ifdef WIN32

    SOCKET m_Sock;
#else

    int m_Sock;
#endif

    bool m_bSockValid; //V

};

#endif

