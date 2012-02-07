/***************************************************************************
*  mghttpbase.h
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


#ifndef _MG_HTTP_BASE_H
#define _MG_HTTP_BASE_H

#include "common.h"
#include "mghttpsocket.h"
#include <string>
#include <vector>

/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase*-> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															`->CMgFtpAnts
*/

class CMgHttpBase : public CMgHttpSocket
{

public:
    CMgHttpBase();
    virtual ~CMgHttpBase();
    bool SendHttpGetRequest(
        const char* shost, const char* sfile, llong offset, const char* srefer );
    bool SendHttpGetRequest(
        const char* shost, const char* sfile, const char* srefer );
    bool SendHttpHeadRequest(
        const char* shost, const char* sfile, const char* srefer );


    int GetBufLine( char *buf, int maxpos, char *line );
    virtual void OutMsg( const std::string& str, _MSGTYPE type ) = 0;
    virtual void OutMsg( const wxChar* str, _MSGTYPE type ) = 0;

    int GetCookieNum()
    {
        return m_AutoCookie.size();
    }

    std::string GetCookie( int seq )
    {
        return m_AutoCookie[ seq % m_AutoCookie.size() ];
    }

    void AddCookie( std::string cookie );

private:
    std::vector<std::string> m_AutoCookie;
};

#endif

