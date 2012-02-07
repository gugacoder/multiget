/***************************************************************************
*            mgsingletask.cpp
*
*  Thu Sep  7 19:46:43 2006
*  Copyright  2006  liubin, China
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

#ifdef WIN32
#include <winsock2.h>
#endif

#include "mgsingletask.h"
#include "mgfilemanager.h"
#include "mgurlparser.h"
#include "showtablewindow.h" //for ui feedback
#include "threadinfowindow.h"
#include "taskmanager.h" //for ui feedback
#include "mainframe.h"
#include "mgapp.h"

#include "mgftpants.h"
#include "mghttpants.h"
#include "mgftpinfo.h"
#include "mghttpinfo.h"
//#include "mgtaskinit.h"
#include "proants.h"

#include <string>

using namespace std;
extern void gfTaskLog( int ntaskid, std::string info, _MSGTYPE ntype, int ntime );
CMgSingleTask::CMgSingleTask(
    CTaskManager* parent,
    std::string url,
    std::string savepath,
    std::string savename,
    int taskid,
    int ants,
    int retry,
    int retrywait,
    std::string refer
)
{
    m_pFM = NULL;

    for ( int i = 0;i < 10;i++ )
    {
        m_pAnts[ i ] = NULL;
    }

    m_pShowTableWin = NULL;
    m_pThreadInfoWin = NULL;

    m_pFtpInfo = NULL;
    m_pHttpInfo = NULL;

    m_pParent = parent;
    m_nFileLen = -1;
    m_nTaskID = taskid;

    m_nRetry = retry;
    m_nRetryWait = retrywait;
    m_nError = 0;
    m_bUI = false;

    m_nMsgOutThreadId = 0;
    m_nLastFinishByte = 0L;


    m_sFilename = savename;
    m_sUrl = url;
    m_Refer = refer;
    m_sSavePath = savepath;
    m_nRunningAnts = 0;
    m_nStatus = _TASK_RUNNING;

    if ( ants < 0 )
        m_nAnts = 1;
    else if ( ants > 10 )
        m_nAnts = 10;
    else
        m_nAnts = ants;

    pthread_mutex_init ( &m_AntsMutex, NULL );

    pthread_mutex_init ( &m_QuitMutex, NULL );

    //socks proxy
    m_bUseProxy = false;

    m_ProxyPort = 0;

    m_ProxyVersion = V5;

    //HTTP proxy
    m_bUseHttpProxy = false;

    m_HttpProxyPort = 0;

    //FTP proxy
    m_bUseFtpProxy = false;

    m_FtpProxyPort = 0;

    m_bInitAlive = false;

    m_InitThread = NULL;

	m_bInitAnts = false;
}

CMgSingleTask::~CMgSingleTask()
{
	
    pthread_mutex_lock( &m_QuitMutex );
	
    pthread_cancel( m_nInitThread );

    pthread_join( m_nInitThread, NULL );

    pthread_mutex_unlock( &m_QuitMutex );
	

	
    if ( m_pFM != NULL )
        m_pFM->Stop();

    for ( int i = 0;i < 10;i++ )
    {
        if ( m_pAnts[ i ] != NULL )
        {
            delete m_pAnts[ i ];
            m_pAnts[ i ] = NULL;
        }
    }

    if ( m_pFM != NULL )
    {
        delete m_pFM;
        m_pFM = NULL;
    }

    if ( m_pFtpInfo != NULL )
    {
        delete m_pFtpInfo;
        m_pFtpInfo = NULL;
    }

    if ( m_pHttpInfo != NULL )
    {
        delete m_pHttpInfo;
        m_pHttpInfo = NULL;
    }

}

void CMgSingleTask::ThreadQuit( int tid )
{

    pthread_mutex_lock( &m_AntsMutex );

    --m_nRunningAnts;

    if ( m_nRunningAnts <= 0 )
    {
        //get last filesize from fm,sometime can't get the size ahead

        if ( m_pFM->IsTaskFinish( m_nFileLen ) )
            FinishTask( _TASK_FINISH );
        else
            FinishTask( _TASK_ERROR );
    }

    pthread_mutex_unlock( &m_AntsMutex );
}

//swap to the init thread
void* CMgSingleTask::SwapInitThread( void* pthis )
{
    ((CMgSingleTask*)pthis)->InitThread();
    return NULL;
}

void CMgSingleTask::FinishTask( _SINGLE_TASK_STATUS status )
{
    m_nStatus = status;
    LogError(); //记录一些日志，有利于查找问题

    pthread_mutex_lock( &m_QuitMutex );

    wxCommandEvent event( mgEVT_TASK_FINISH, m_nTaskID );
    MainFrame *mainwin = ( MainFrame* ) ( wxGetApp().GetTopWindow() );

    mainwin->AddPendingEvent( event );
    //wxPostEvent( mainwin, event );
    pthread_mutex_unlock( &m_QuitMutex );

}

void CMgSingleTask::InitThread()
{
    OutMsg( -1, _("Job running..."), MSG_INFO );

    pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );

    CUrlParser upar;

    if ( !m_Refer.empty() )
        upar.SetRefer( m_Refer );

    int oldstate;

    vector<string> cookie; //for http

again:

    OutMsg( -1, _("Analysis URL..."), MSG_INFO );

    if ( !upar.SetUrl( m_sUrl ) )
    {
        OutMsg( -1, c_str(_("Not a support URL type.")) + m_sUrl, MSG_ERROR );
        FinishTask( _TASK_ERROR );
        pthread_exit( 0 );
    }

    OutMsg( -1, _("URL OK."), MSG_INFO );
    //任务参数
    m_Server = upar.GetServer();

    m_ServerPort = upar.GetPort();

    m_Username = upar.GetUser();

    m_Password = upar.GetPass();

    m_EscFilePathName = upar.GetEscFilePathName(); //文件名


    if ( upar.GetUrlType() == FTP_PROTOCOL )   //ftp
    {
        OutMsg( -1, _("Main URL is ftp type."), MSG_INFO );

        m_pFtpInfo = new CMgFtpInfo(
                         this,
                         m_sUrl,
                         m_nRetry,
                         m_nRetryWait,
                         -1
                     );
        //set proxy here

        if ( m_bUseProxy )
        {
            OutMsg( -1, _("Task will use socks proxy."), MSG_INFO );
            m_pFtpInfo->SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );

        }

        if ( m_bUseFtpProxy )
        {
            OutMsg( -1, _("Task will use ftp proxy."), MSG_INFO );
            m_pFtpInfo->SetFtpProxy( m_FtpProxy, m_FtpProxyPort );
        }

        //go

        if ( !m_pFtpInfo->GetInfo() )
        { //error when get info. 2006/09/21

            OutMsg( -1, _("Get file info error."), MSG_ERROR );
            pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );
            delete m_pFtpInfo;
            m_pFtpInfo = NULL;
            pthread_setcancelstate ( oldstate, NULL );
            FinishTask( _TASK_ERROR );
            pthread_exit( 0 );
        }

        m_nFileLen = m_pFtpInfo->FileSize();
        bool bresume = m_pFtpInfo->IsResume();

        pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );
        delete m_pFtpInfo;
        m_pFtpInfo = NULL;
        pthread_setcancelstate ( oldstate, NULL );


        if ( m_nFileLen == -1 || !bresume )
        {
            if ( m_nFileLen == -1 )
            {
                OutMsg( -1, _("No file length got."), MSG_WARNNING );
            }

            if ( !bresume )
            {
                OutMsg( -1, _("Server cannot support resume method."), MSG_WARNNING );
            }

            m_nAnts = 1;
            //FinishTask(_TASK_ERROR);
            //InitThreadQuit();
        }


    } //main url ftp
    else if ( upar.GetUrlType() == HTTP_PROTOCOL )
    { //http
        OutMsg( -1, _("Main URL is http type."), MSG_INFO );

        m_pHttpInfo =
            new CMgHttpInfo(
                this,
                m_sUrl,
                m_nRetry,
                m_nRetryWait,
                upar.GetRefer(),
                -1
            );
        //if need proxy, make here

        if ( m_bUseProxy )
        {
            OutMsg( -1, _("Task will use socks proxy."), MSG_INFO );
            m_pHttpInfo->SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );

        }

        if ( m_bUseHttpProxy )
        {
            OutMsg( -1, _("Task will use http proxy."), MSG_INFO );
            m_pHttpInfo->SetHttpProxy( m_HttpProxy, m_HttpProxyPort );
        }

        if ( !m_pHttpInfo->GetInfo() )
        { //error when get info.

            OutMsg( -1, _("Get file info error."), MSG_ERROR );
            pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );
            delete m_pHttpInfo;
            m_pHttpInfo = NULL;
            pthread_setcancelstate ( oldstate, NULL );
            FinishTask( _TASK_ERROR );
            pthread_exit( 0 );
        }

        if ( m_pHttpInfo->IsRedirect() )
        {
            upar.SetRefer( m_sUrl );
            m_sUrl = m_pHttpInfo->GetRedirect();

            pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );
            delete m_pHttpInfo;
            m_pHttpInfo = NULL;
            pthread_setcancelstate ( oldstate, NULL );
            //跳转后就不再用原来的名字了。

            if ( upar.SetUrl( m_sUrl ) )
            {
                if ( upar.GetFileName() != string( "" ) )
                    m_sFilename = upar.GetFileName();
            }

            goto again;
        }

        //		for( int coo=0; coo < m_pHttpInfo->GetCookieNum(); coo++)
        //		{
        //
        //			cookie.push_back( m_pHttpInfo->GetCookie( coo ) );
        //		}

        m_nFileLen = m_pHttpInfo->GetFileSize();

        pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );

        delete m_pHttpInfo;

        m_pHttpInfo = NULL;

        pthread_setcancelstate ( oldstate, NULL );

        if ( m_nFileLen == -1 )
        {
            OutMsg( -1, _("No file length got."), MSG_WARNNING );
            m_nAnts = 1;
        }


    } //http
    else
    {
        OutMsg( -1, c_str(_("Not a support URL type.")) + m_sUrl, MSG_ERROR );

        FinishTask( _TASK_ERROR );
        pthread_exit( 0 );
    }


    //create file manager
    pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );

    m_pFM = new CMgFileManager ( this, m_sUrl, m_sFilename, m_sSavePath, m_nFileLen, upar.GetRefer() );

    pthread_setcancelstate ( oldstate, NULL );

    //check file
    if ( !m_pFM->CheckFile() )
    {
        pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );
        delete m_pFM;
        m_pFM = NULL;
        pthread_setcancelstate ( oldstate, NULL );
        FinishTask( _TASK_ERROR );
        pthread_exit( 0 );
    }

    //input mirror url
    std::vector<std::string>::const_iterator it;

    for ( it = m_sMirrorUrl.begin();it != m_sMirrorUrl.end();it++ )
    {
        m_pFM->AddMirrorUrl( *it );
    }

    //start work thread
    pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &oldstate );
	pthread_mutex_lock( &m_AntsMutex );

    for ( int i = 0;i < m_nAnts;i++ )
    {
        //not run when construct 2006/09/03
        m_pAnts[ i ] = new CProAnts(
                           this,   //用于线程消息回送
                           m_pFM,
                           i + 1,  //线程自然数编号，回送消息时带上，用于分辨消息来源
                           m_nFileLen,
                           m_nRetry,
                           m_nRetryWait
                       );


        //if need proxy, make here

        if ( m_bUseProxy )
        {
            m_pAnts[ i ] ->SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );
        }

        if ( m_bUseFtpProxy )
        {
            m_pAnts[ i ] ->SetFtpProxy( m_FtpProxy, m_FtpProxyPort );
        }

        if ( m_bUseHttpProxy )
        {

            m_pAnts[ i ] ->SetHttpProxy( m_HttpProxy, m_HttpProxyPort );
        }

        m_pAnts[ i ] ->Go();

    }


    m_nRunningAnts = m_nAnts;
	m_bInitAnts = true;
	pthread_mutex_unlock( &m_AntsMutex );
    pthread_setcancelstate ( oldstate, NULL );

    pthread_exit( 0 );

}

//goto work thread
bool CMgSingleTask::Go()
{
    pthread_create( &m_nInitThread, NULL, SwapInitThread, this );
#ifdef WIN32
    return ( m_nInitThread.p != NULL );
#else
    return ( m_nInitThread != 0 );
#endif
}

void CMgSingleTask::UpdateRunningTable( CShowTableWindow* pwin )
{
    //设置显示范围
    m_pShowTableWin = pwin;

    //如果此时还没有获得文件长度呢？
    //m_pShowTableWin->SetRange( m_nFileLen );

    if ( m_pFM )
    {
        m_pFM->UpdateRunningTable( m_pShowTableWin );
    }

    m_bUI = true;
    m_nUItype = 1;
}


void CMgSingleTask::CloseUpdateUI()
{
    if ( m_bUI && m_nUItype == 1 && m_pFM != NULL )
    {
        m_pFM->CloseUpdateUI();
    }

    m_bUI = false;
}


//tid从1开始计算
//tid可以是负数，－1代表信息提取，－2代表文件管理
void CMgSingleTask::UpdateThreadInfo( CThreadInfoWindow* pwin, int tid )
{
    m_pThreadInfoWin = pwin;
    m_bUI = true;
    m_nUItype = 2; //线程信息，非图表信息
    m_nMsgOutThreadId = tid;

    pwin->DeleteAllItems(); //清空

    if ( tid > 0 && tid <= 10 )
    {
        //dump all msg first time

        for ( int i = 0;i < int( m_sThreadInfo[ tid - 1 ].size() );i++ )
        {
            pwin->AddInfo( m_sThreadInfo[ tid - 1 ][ i ].type,
                           m_sThreadInfo[ tid - 1 ][ i ].info, m_sThreadInfo[ tid - 1 ][ i ].time );
        }
    }
    else if ( tid == -1 )
    {
        //dump all msg first time

        for ( int i = 0;i < int( m_sInfoThread.size() );i++ )
        {
            pwin->AddInfo( m_sInfoThread[ i ].type, m_sInfoThread[ i ].info , m_sInfoThread[ i ].time );
        }
    }
    else if ( tid == -2 )
    {
        //dump all msg first time

        for ( int i = 0;i < int( m_sFileManager.size() );i++ )
        {
            pwin->AddInfo( m_sFileManager[ i ].type, m_sFileManager[ i ].info, m_sFileManager[ i ].time );
        }
    }
    else
    {
        //invalid threadid
        //DBGOUT( "invalid tid" );
		return;
    }
}

void CMgSingleTask::OutMsg( int tid, const wxChar * str, _MSGTYPE type )
{
	OutMsg(tid, c_str(str),type);
}
//tid是线程编号，从1开始计算，负数代表另外两个
//output message,tid is 1 based
void CMgSingleTask::OutMsg( int tid, const std::string& str, _MSGTYPE type )
{


    _task_info x;
    x.time = time( NULL );
    x.type = type;
    x.info = str;


    if ( tid > 0 && tid < 11 )
    {
        m_sThreadInfo[ tid - 1 ].push_back( x );
    }
    else if ( tid == -1 )
    {
        m_sInfoThread.push_back( x );
    }
    else if ( tid == -2 )
    {
        m_sFileManager.push_back( x );
    }
    else
    {
		return;
    }

    //输出到界面
    if ( m_bUI && m_nUItype == 2 && m_pThreadInfoWin != NULL && m_nMsgOutThreadId == tid )
    {
        m_pThreadInfoWin->AddInfo( type, str, x.time );
    }

}

void CMgSingleTask::GetRunningArgs( float& fratio, llong& finish, int& speed, int& ttf )
{
    if ( m_pFM != NULL )
    {
        m_pFM->GetRunningArgs( fratio, finish, speed, ttf );
    }
    else
    {
        fratio = 0.0f;
        finish = 0;
        speed = 0;
        ttf = -1;
    }
}


bool CMgSingleTask::ThreadDown()
{
    if ( m_nAnts <= 1 )
        return false;

    if ( m_nRunningAnts <= 1 )
        return false;


    pthread_mutex_lock( &m_AntsMutex );

	if( !m_bInitAnts ) {
		if( m_nAnts <=1 ) {
			pthread_mutex_unlock( &m_AntsMutex );
			return false;			
		}else{
			m_nAnts--;
			pthread_mutex_unlock( &m_AntsMutex );
			return true;
		}
	}
    //要把最后的线程拿掉，如果最后的线程已经退出了，这次就只拿掉这个退出的线程

    if ( m_pAnts[ m_nAnts - 1 ] != NULL )
    {
        delete m_pAnts[ m_nAnts - 1 ];
        m_pAnts[ m_nAnts - 1 ] = NULL;
        m_sThreadInfo[ m_nAnts - 1 ].clear();  //index??
        m_pFM->ThreadQuit( m_nAnts );		//index ??
        m_nRunningAnts--;
        m_nAnts--;
    }
    else
    {
        m_nAnts--;
        m_sThreadInfo[ m_nAnts ].clear();
    }


    pthread_mutex_unlock( &m_AntsMutex );
    return true;
}


bool CMgSingleTask::ThreadUp()
{
    if ( m_nAnts >= 10 )
        return false;

    if ( m_nRunningAnts >= 10 )
        return false;

    pthread_mutex_lock( &m_AntsMutex );

	if( !m_bInitAnts ) {
		if( m_nAnts >=10 ) {
			pthread_mutex_unlock( &m_AntsMutex );
			return false;			
		}else{
			m_nAnts++;
			pthread_mutex_unlock( &m_AntsMutex );
			return true;
		}
	}

    if ( m_pAnts[ m_nAnts ] == NULL )
    {
        m_pAnts[ m_nAnts ] = new CProAnts(
                                 this,   //用于线程消息回送
                                 m_pFM,
                                 m_nAnts + 1,    //线程自然数编号，回送消息时带上，用于分辨消息来源
                                 m_nFileLen,
                                 m_nRetry,
                                 m_nRetryWait
                             );

        if ( m_bUseProxy )
        {
            m_pAnts[ m_nAnts ] ->SetProxy(
                m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );
        }

        if ( m_bUseFtpProxy )
        {
            m_pAnts[ m_nAnts ] ->SetFtpProxy( m_FtpProxy, m_FtpProxyPort );
        }

        if ( m_bUseHttpProxy )
        {
            m_pAnts[ m_nAnts ] ->SetHttpProxy( m_HttpProxy, m_HttpProxyPort );
        }

        if( !m_pAnts[ m_nAnts ] ->Go() )
		{
			//sleep(50);
			m_pAnts[ m_nAnts ] ->Go();
		}

        m_nRunningAnts++;
        m_nAnts++;
    }
    else
    {
        pthread_mutex_unlock( &m_AntsMutex );
        return false;
    }


    pthread_mutex_unlock( &m_AntsMutex );
    return true;
}

void CMgSingleTask::SetProxy(
    std::string server,
    int port,
    std::string user,
    std::string pass,
    _SOCKVER ver )
{


    m_bUseProxy = true;
    m_Proxy = server;
    m_ProxyPort = port;
    m_ProxyUser = user;
    m_ProxyPass = pass;
    m_ProxyVersion = ver;

}

void CMgSingleTask::SetHttpProxy( std::string server, int port )
{

    m_bUseHttpProxy = true;
    m_HttpProxy = server;
    m_HttpProxyPort = port;
}

void CMgSingleTask::SetFtpProxy( std::string server, int port )
{

    m_bUseFtpProxy = true;
    m_FtpProxy = server;
    m_FtpProxyPort = port;
}

void CMgSingleTask::HeartBeat()
{
    if ( m_pFM != NULL )
        m_pFM->HeartBeat();
}

CTaskManager* CMgSingleTask::GetParent()
{
    return m_pParent;
}

CThreadInfoWindow* CMgSingleTask::GetThreadInfoWin()
{
    return m_pThreadInfoWin;
}

CShowTableWindow* CMgSingleTask::GetShowTableWin()
{
    return m_pShowTableWin;
}

int CMgSingleTask::UpdateUItype()
{
    return m_nUItype;
}

bool CMgSingleTask::UpdateUI()
{
    return m_bUI;
}

std::string CMgSingleTask::GetTaskUrl()
{
    return m_sUrl;
}

std::string CMgSingleTask::GetFilename()
{
    return m_sFilename;
}

int CMgSingleTask::GetErrorTime()
{
    return m_nError;
}

llong CMgSingleTask::GetFileLen()
{
    return m_nFileLen;
}

int CMgSingleTask::GetTaskID()
{
    return m_nTaskID;
}

_SINGLE_TASK_STATUS CMgSingleTask::GetStatus()
{
    return m_nStatus;
}

int	CMgSingleTask::GetRunningThreadNum()
{
    return m_nRunningAnts;
}

int CMgSingleTask::GetThreadNum()
{
    return m_nAnts;
};

//记录错误事件
void CMgSingleTask::LogError()
{
    //记录全部
    std::vector<_task_info>::const_iterator it;

    for ( it = m_sInfoThread.begin();it != m_sInfoThread.end();it++ )
    {
        gfTaskLog( m_nTaskID, it->info, it->type, it->time );
    }

    for ( it = m_sFileManager.begin();it != m_sFileManager.end();it++ )
    {
        gfTaskLog( m_nTaskID, it->info, it->type, it->time );
    }

}

//before call go(),you can add mirror url
void CMgSingleTask::AddMirrorUrl( std::string url )
{
    m_sMirrorUrl.push_back( url );

    if ( m_pFM != NULL )
        m_pFM->AddMirrorUrl( url );
}

