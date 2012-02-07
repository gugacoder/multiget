/***************************************************************************
*  mgftpbase.h
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
#ifndef _MG_FTP_BASE_H
#define _MG_FTP_BASE_H

#include "common.h"
#include "mgftpsocket.h"
#include <string>

/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase-> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase* -> CMgFtpInfo
															`->CMgFtpAnts
*/

class CMgFtpBase : public CMgFtpSocket
{

public:
    CMgFtpBase();
    virtual ~CMgFtpBase();

    bool MakeCtrlConn( const char *server, int port );
    bool Login( const char *user, const char *pass );
    int GetRetCode();
    int GetBufLine( char *buf, int maxpos, char *line );

    bool SetResume( llong pos );
    bool EnterBinaryMode();
    bool EnterTextMode();
    bool EnterPasv( char *dip, int *dport );
    bool AbortTransfer();
    bool Logout();
    bool ListFile( const char* filename ); //send list to server
    bool ListFile( const char* filename, llong& size ); //to get size
    bool SizeFile( const char* filename, llong& size ); //send size to get size

protected:
    bool GetLenFromBuf( const char* listdataline, llong& size );
    virtual void OutMsg( const std::string& str, _MSGTYPE type ) = 0;
    virtual void OutMsg( const wxChar * str, _MSGTYPE type ) = 0;

private:
    //bool m_bBinaryMode;
    std::string m_Server;
    int m_Port;
};

#endif

