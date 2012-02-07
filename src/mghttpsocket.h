/***************************************************************************
*  mghttpsocket.h
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
#ifndef _MG_HTTP_SOCKET_H
#define _MG_HTTP_SOCKET_H

#include "mgproxysocket.h" 
/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket* -> CMgHttpBase -> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															`->CMgFtpAnts
*/


/*
Error code range define;
 
 
*/ 
//this class try to make http proxy work


//http://www.ietf.org/rfc/rfc2818.txt  RFC2818(HTTP Over TLS)
//http://community.roxen.com/developers/idocs/rfc/rfc2246.html RFC2246(The TLS Protocol Version 1.0)

class CMgHttpSocket : public CMgProxySocket
{

public:
    CMgHttpSocket();
    virtual ~CMgHttpSocket();
    bool SetHttpProxy( std::string , int );
    virtual bool Connect( const char* server, int port );

protected:
    std::string m_HttpProxyServer;
    int m_HttpProxyPort;
    bool m_bUseHttpProxy;
};

#endif

