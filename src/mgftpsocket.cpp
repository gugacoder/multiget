/***************************************************************************
*  mgftpsocket.cpp
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
/*
Error code [-3100~-3199]:
-3000: connect ftp proxy error;
-3001: connect host error;
*/


#include "mgftpsocket.h"
CMgFtpSocket::CMgFtpSocket()
{
    m_FtpProxyPort = 0;
    m_bUseFtpProxy = false;
}

CMgFtpSocket::~CMgFtpSocket()
{}

void CMgFtpSocket::SetFtpProxy(
    std::string server, int port )
{
    m_FtpProxyServer = server;
    m_FtpProxyPort = port;
    m_bUseFtpProxy = true;
}

void CMgFtpSocket::SetFtpProxy( CMgFtpSocket& old )
{
    m_FtpProxyServer = old.m_FtpProxyServer;
    m_FtpProxyPort = old.m_FtpProxyPort;
    m_bUseFtpProxy = old.m_bUseFtpProxy;
}

bool CMgFtpSocket::Connect( const char* server, int port )
{
    if ( m_bUseFtpProxy )
    {
        if ( !CMgProxySocket::Connect( m_FtpProxyServer.c_str(), m_FtpProxyPort ) )
        {
            m_nLastError = -3000;
            return false;
        }
    }
    else
    {
        if ( !CMgProxySocket::Connect( server, port ) )
        {
            m_nLastError = -3001;
            return false;
        }
    }

    return true;
}

