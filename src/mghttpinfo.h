/***************************************************************************
*  mghttpinfo.h
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
#ifndef _MG_HTTP_INFO_H
#define _MG_HTTP_INFO_H

#include "mghttpbase.h"
#include <string> 
/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase-> CMgHttpInfo*
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															`->CMgFtpAnts
*/

class CMgSingleTask;

class CMgHttpInfo : public CMgHttpBase
{

public:

    CMgHttpInfo(
        CMgSingleTask	*parent,
        std::string fullurl,
        int retry,
        int retrywait,
        std::string refer,
        int msgid = -1
    );
    virtual ~CMgHttpInfo()
    {}

    ;
    bool IsResume()
    {
        return m_bResume;
    };

    int IsHeadEnd( char *buf, int pos );
    llong GetFileSize()
    {
        return m_nFileSize;
    };

    bool IsRedirect()
    {
        return ( m_RetCode / 100 == 3 ) && m_bRedirect;
    };

    int GetRetCode()
    {
        return m_RetCode;
    };

    std::string GetRedirect()
    {
        return m_Redirection;
    };

    bool GetInfo();

protected:
    std::string m_server;
    int m_port;
    std::string m_host;
    std::string m_refer;
    std::string m_file;

    void OutMsg( const std::string& str, _MSGTYPE type = MSG_INFO );
    void OutMsg( const wxChar* str, _MSGTYPE type = MSG_INFO );
    bool GetInfoByGet();
    bool m_bResume;
    llong m_nFileSize;

    bool m_bRedirect;
    std::string m_Redirection;	//转移地址，如果有
    int m_RetCode;	//返回码

private:
    int m_nRetry, m_nRetryWait, m_nMsgId;
    bool Retry();
    CMgSingleTask* m_pParent;
};

#endif

