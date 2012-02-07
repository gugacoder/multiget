/***************************************************************************
*  mghttpants.h
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
#ifndef _MG_HTTP_ANTS_H
#define _MG_HTTP_ANTS_H

#include "mghttpbase.h"
#include "common.h"
#include <wx/wx.h>
#include <wx/thread.h> 
/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase*-> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															`->CMgFtpAnts
*/

class CMgSingleTask;

class CMgFileManager;

class CMgHttpAnts : public CMgHttpBase
{

public:

    CMgHttpAnts(
        CMgSingleTask	*parent,
        CMgFileManager *fm,
        std::string url,
        int tid,
        int retry,
        int retrywait,
        llong from,
        llong tsize,
        std::string refer
    );

    virtual ~CMgHttpAnts();

    int Go(); //同步

protected:


    //unsigned char m_buffer[ RECVBUF ];

private:
    /*
    void AnalysisHeader(
    		char buf,	//IN buf have header content ended with 0
    		std::string prestr, //IN
    		bool& redirect,  //OUT
    		std::string& redirection, //OUT
    		llong&  filesize, 	//OUT
    		int&	retcode		//OUT
    		);
    void Trim(std::string& str);
    */
    bool GetFile( const char* filename, llong pos, const char* refer );
    int GetData( llong spos, llong& bytes );
    bool GetHead( );
    int WriteData( llong offset, int len, unsigned char *buf, bool end = false );

    virtual void OutMsg( const std::string& str, _MSGTYPE type = MSG_INFO );
	virtual void OutMsg( const wxChar * str, _MSGTYPE type = MSG_INFO );
    bool Retry() ;

    std::string m_sUrl;
    std::string m_sServer;
    std::string m_host; //cal
    std::string m_file; //cal
    std::string m_refer; //cal

    CMgSingleTask	*m_pParent;
    CMgFileManager *m_pFM;
    int m_Port;
    int m_nAntId; //1 based
    int	m_nRetry;
    int m_nRetryWait;
    llong	m_nFileSize;
    llong m_nFrom;
    llong m_nTotalByte;

};

#endif

