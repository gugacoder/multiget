/***************************************************************************
*            mgsingletask.h
*
*  Thu Sep  7 19:47:38 2006
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



#ifndef _MG_SINGLETASK_H
#define _MG_SINGLETASK_H
#include "common.h"

#include <vector>

#include <wx/wx.h>
#include <wx/thread.h>

#include <pthread.h>

class CUrlParser;

class CMgFileManager;

class CTaskManager;

class CShowTableWindow;

class CThreadInfoWindow;
//class CMgFtpAnts;

class CMgFtpInfo;
//class CMgHttpAnts;

class CMgHttpInfo;

class CProAnts;

class CMgTaskInit;
//

class CMgSingleTask
{

    friend class CMgTaskInit;

public:
    CMgSingleTask(
        CTaskManager	*parent,
        std::string url,
        std::string savepath,
        std::string savename,
        int taskid,
        int ants = 5,
        int retry = 10,
        int retrywait = 60,
        std::string	refer = ""
    );

    virtual ~CMgSingleTask();

    void AddMirrorUrl( std::string url );

    bool Go();

    bool IsRunning();

    int GetThreadNum();
    int	GetRunningThreadNum();

    _SINGLE_TASK_STATUS GetStatus();
    int GetTaskID();

    llong GetFileLen();
    int GetTaskIndex( bindex data[], int num );
    int GetErrorTime();

    void GetFinishByte( llong& thistime, llong& lasttime );
    std::string GetFilename();

    std::string GetTaskUrl();


    bool	ThreadUp();
    bool	ThreadDown();
    bool UpdateUI();
    int UpdateUItype();
    CShowTableWindow* GetShowTableWin();
    CThreadInfoWindow* GetThreadInfoWin();
    CTaskManager* GetParent();

    void HeartBeat();

    void UpdateRunningTable( CShowTableWindow* pwin );
    void UpdateThreadInfo( CThreadInfoWindow* pwin, int nthreadid );
    void CloseUpdateUI();

    void OutMsg( int nth, const std::string& str, _MSGTYPE type = MSG_INFO );
    void OutMsg( int nth, const wxChar *str, _MSGTYPE type = MSG_INFO );
    //working thread call it to quit
    void ThreadQuit( int threadnum );
    void GetRunningArgs( float& fratio, llong& finish, int& speed, int& ttf );
    //socks proxy set
    void SetProxy( std::string server, int port, std::string user, std::string pass, _SOCKVER ver = V5 );
    //http proxy set
    void SetHttpProxy( std::string server, int port );
    //ftp proxy set
    void SetFtpProxy( std::string server, int port );
    //log error msg
    void LogError();
    //error time
    int m_nError;

protected:
    static void* SwapInitThread( void* );
    void FinishTask( _SINGLE_TASK_STATUS status = _TASK_RUNNING );
    void InitThread();
    int m_nTaskID;
    int m_nStage;

private:

    _SINGLE_TASK_STATUS m_nStatus;

    pthread_t m_WorkingMain;
    //init thread
    pthread_t m_nInitThread;
    //
    CTaskManager *m_pParent;
    //
    CMgFileManager *m_pFM;

    int m_nAnts;
    int m_nRunningAnts;
    //
    int m_nRetry, m_nRetryWait;

    llong m_nLastFinishByte;

    std::string m_sSavePath;

    std::string m_Server;
    int m_ServerPort;
    std::string m_Username;
    std::string m_Password;
    std::string m_EscFilePathName;
    std::string m_ReferServer;
    std::string m_Refer;
    //SOCKS proxy
    bool m_bUseProxy;
    std::string m_Proxy;
    int m_ProxyPort;
    std::string m_ProxyUser;
    std::string m_ProxyPass;
    _SOCKVER m_ProxyVersion;
    //HTTP proxy
    bool m_bUseHttpProxy;
    std::string m_HttpProxy;
    int m_HttpProxyPort;
    //FTP proxy
    bool m_bUseFtpProxy;
    std::string m_FtpProxy;
    int m_FtpProxyPort;
    //ants lock
    pthread_mutex_t m_AntsMutex;
    //quit lock
    pthread_mutex_t m_QuitMutex;
    //CMgFtpAnts	*m_ftpAnts[ 10 ];
    //CMgHttpAnts	*m_httpAnts[ 10 ];
    CProAnts *m_pAnts[ 10 ];
    CMgFtpInfo *m_pFtpInfo;  //??
    CMgHttpInfo	*m_pHttpInfo; //??
    //cancel mark
    bool m_bCancelMark;
    //
    bool m_bAfterFM;
    CMgTaskInit *m_InitThread; //detach thread
    bool m_bInitAlive;  //detach thread alive?
    std::string m_sUrl;
    std::string m_sFilename;
    llong m_nFileLen;
    std::vector<std::string> m_sMirrorUrl;

    bool m_bUI;
    int m_nUItype;
    CShowTableWindow* m_pShowTableWin;
    CThreadInfoWindow* m_pThreadInfoWin;

    int m_nMsgOutThreadId;
	bool m_bInitAnts;

    struct _task_info
    {
        int time;
        _MSGTYPE type;
        std::string info;
    };

    std::vector<_task_info> m_sThreadInfo[ 11 ];

    std::vector<_task_info> m_sInfoThread;

    std::vector<_task_info> m_sFileManager;


};

#endif

