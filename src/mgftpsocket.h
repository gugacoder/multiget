/***************************************************************************
*  mgftpsocket.h
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

#ifndef _MG_FTP_SOCKET_H
#define _MG_FTP_SOCKET_H 
/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase -> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket*-> CMgFtpBase -> CMgFtpInfo
															`->CMgFtpAnts
*/ 
/*
Error code range define;
CMgSocket [-1000~-1099]
 
CMgProxySocket [-2000~-2099]
 
CMgHttpSocket [-3000~-3099]
CMgFtpSocket [-3100~-3199]*
 
CMgHttpBase [-4000~-4099]
CMgFtpBase [-4100~-4199]
 
CMgHttpInfo [-5000~-5099]
CMgHttpAnts [-5100~-5199]
 
CMgFtpInfo [-5200~-5299]
CMgFtpAnts [-5300~-5399]
 
*/
#include "mgproxysocket.h"

class CMgFtpSocket : public CMgProxySocket
{

public:
    CMgFtpSocket();
    virtual ~CMgFtpSocket();
    void SetFtpProxy( std::string , int );
    void SetFtpProxy( CMgFtpSocket& old );
    virtual bool Connect( const char* server, int port );

protected:
    std::string m_FtpProxyServer;
    int m_FtpProxyPort;
    bool m_bUseFtpProxy;
};

#endif

