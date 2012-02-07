/***************************************************************************
*  mghttpsocket.cpp
*
*  Wed Sep  6 22:19:52 2006
*  Copyright  2006  liubin,China
*  Email sysnotdown@yahoo.com
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
#include "mghttpsocket.h" 
/*
Error code [-3000~-3099]:
-3000: connect http proxy error;
-3001: connect host error;
*/
CMgHttpSocket::CMgHttpSocket()
{
    m_HttpProxyPort = 0;
    m_bUseHttpProxy = false;
}

CMgHttpSocket::~CMgHttpSocket()
{
}

bool CMgHttpSocket::SetHttpProxy(
    std::string server, int port )
{
    m_HttpProxyServer = server;
    m_HttpProxyPort = port;
    m_bUseHttpProxy = true;
    return true;
}

bool CMgHttpSocket::Connect( const char* server, int port )
{

    if ( m_bUseHttpProxy )
    {
        if ( !CMgProxySocket::Connect( m_HttpProxyServer.c_str(), m_HttpProxyPort ) )
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

