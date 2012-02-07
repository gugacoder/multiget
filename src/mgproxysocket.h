/***************************************************************************
*  mgproxysocket.h
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

#ifndef _MG_PROXY_SOCKET_H
#define _MG_PROXY_SOCKET_H

#include "mgsocket.h"

#include <string> 
/*
 
CMgSocket -> CMgProxySocket*-> CMgHttpSocket -> CMgHttpBase -> CMgHttpInfo
							|								\->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															\->CMgFtpAnts
*/

/*
Error code range define;
[0x0000**00~0x0000**00]
 
*/

class CMgProxySocket : public CMgSocket
{

public:
    CMgProxySocket();
    virtual ~CMgProxySocket();
    //proxy func

    bool SetProxy( const char* , int , const char* user = "", const char* pass = "", _SOCKVER ver = V5 );
    bool SetProxy( std::string , int , std::string, std::string, _SOCKVER ver = V5 );
    void SetProxy( CMgProxySocket& asock );
    virtual bool Connect( const char* server, int port );

protected:
    //bool DoProxyConnect(const char* server ,int port);

    bool DoV4Connect( const char* server , int port );
    bool DoV4AConnect( const char* server , int port );
    bool DoV5Connect( const char* server , int port );
    bool DoV5Login();
    bool m_bAppConnected;
    //proxy varis
    std::string m_ProxyServer;
    int m_ProxyPort;
    std::string m_ProxyUser;
    std::string m_ProxyPass;
    _SOCKVER m_ProxyVersion;

    bool m_bUseProxy;
};

#endif

