/***************************************************************************
*  mgftpinfo.h
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
#ifndef _MG_FTP_INFO_H
#define _MG_FTP_INFO_H

#include "common.h"
#include "mgftpbase.h"
#include <string>

/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase-> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo*
															`->CMgFtpAnts
*/

class CMgSingleTask;

class CMgFtpInfo : public CMgFtpBase
{

public:
    CMgFtpInfo(
        CMgSingleTask* parent,
        std::string fullurl,
        int retry,
        int retrywait,
        int msgid = -1
    );
    virtual ~CMgFtpInfo()
    {}

    ;
    bool IsResume()
    {
        return m_bResume;
    };

    llong FileSize()
    {
        return m_nFileSize;
    };

    bool GetInfo();

protected:
    std::string m_server;
    int m_port;
    std::string m_user;
    std::string m_pass;
    std::string m_file;

    void OutMsg( const std::string& str, _MSGTYPE type = MSG_INFO );
    void OutMsg( const wxChar* str, _MSGTYPE type = MSG_INFO );
    bool GetFile( const char* filename );
    bool m_bResume;
    llong m_nFileSize;

private:
    int m_nRetry, m_nRetryWait, m_nMsgId;
    bool Retry();
    CMgSingleTask* m_pParent;
};


#endif

