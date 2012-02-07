/***************************************************************************
*  mgftpants.h
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

#ifndef _MG_FTP_ANTS_H
#define _MG_FTP_ANTS_H

/*
CMgSocket -> CMgProxySocket -> CMgHttpSocket -> CMgHttpBase-> CMgHttpInfo
							|								`->CMgHttpAnts
							\-> CMgFtpSocket -> CMgFtpBase -> CMgFtpInfo
															`->CMgFtpAnts*
*/

#include "mgftpbase.h"
#include "common.h"
#include <pthread.h>

class CMgFileManager;

class CMgSingleTask;

class CMgFtpAnts : public CMgFtpBase
{

public:

    CMgFtpAnts(
        CMgSingleTask	*parent,
        CMgFileManager *fm,
        std::string fullurl,
        int tid,
        int retry,
        int retrywait,
        llong from,
        llong tsize
    );

    ~CMgFtpAnts();
    int Go();

protected:
    std::string m_Server; 	//服务器
    std::string m_user;		//用户名
    std::string m_pass;		//密码
    std::string m_file;		//文件名
    std::string m_sURL;


    int m_Port;		//端口
    CMgFileManager *m_pFM;	//文件管理器
    bool m_resume;
    llong m_nFileSize;
    int m_nRetry, m_nRetryWait;
    llong m_from;

    //  unsigned char m_buffer[ RECVBUF ];
    //消息类型定义0：错误消息，1：发出的信息，2：接收的信息，3：提示信息
    void OutMsg( const std::string& str, _MSGTYPE type = MSG_INFO ); //输出消息
	void OutMsg( const wxChar* str, _MSGTYPE type = MSG_INFO );

    void ThreadQuit();

    //CMgSocket m_DataSock;

private:

    //	int EnterPasv(int sock,char *dip,int *dport);
    bool GetFile( const char* filename );
    int GetData( CMgFtpSocket& sock, llong spos, llong& bytes );
    int WriteData( llong offset, int len, unsigned char *buf, bool end = false );

    static void* SwapThread( void* p );
    void WorkThread();

    bool Retry() ;

    CMgSingleTask* m_pParent;
    pthread_t	m_ThreadID;
    pthread_mutex_t m_QuitMutex;

    int m_nAntId;
    llong m_nTotalByte;

};


#endif

