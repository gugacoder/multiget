/*

    	filename	:	taskmanager.cpp
       create date	:	2006/07/07
       author		:	liubin
       email		:	multiget@gmail.com
       version		:	0.5	


这个任务管理器专门管理正在下载的文件，和系统的文件管理范围不同

***修订历史***
2006/07/25 : 因为增加GUI界面，原来在这里管理的下载任务的编号需要调整到上层
修改AddTask为 AddTask(int ntaskid,string url,int priority,int ants)
取消本类中的ID管理功能，添加任务时外部传入ID
*/

#include "taskmanager.h"
#include "mgsingletask.h"
#include "showtablewindow.h"
#include "threadinfowindow.h"
#include "mgurlparser.h"
#include "mainframe.h"
#include <iostream>

using namespace std;

bool CTaskManager::RemoveTask( int ntask )
{
    return DeleteTask( ntask );
}

llong CTaskManager::GetTaskSize( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->GetFileLen();

    return -1;
}

int CTaskManager::GetTaskIDbySeq( int seq )
{
    return m_TaskList[ seq ] ->GetTaskID();
}

string CTaskManager::GetTaskFilename( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->GetFilename();

    return string( "Error!" );
}

int CTaskManager::GetTaskIndex( int ntask, bindex b[], int bnum )
{ //maybe not used
    DBGOUT( "GetTaskIndex used" );
    //CMgSingleTask* obj=GetTaskObj(ntask);
    //if(obj!=NULL)	return obj->GetTaskIndex(b, bnum);
    return 0;
}

string CTaskManager::GetTaskUrl( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->GetTaskUrl();

    return string( "error:not find task." );
}

_SINGLE_TASK_STATUS CTaskManager::GetTaskStatus( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->GetStatus();

    return _TASK_NONE;
}

//任务线程数量
int CTaskManager::GetTaskThreadNum( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->GetThreadNum();

    return -1;
}

llong CTaskManager::GetFinishByte( int ntask, int& speed )
{
    DBGOUT( "GetFinishByte use" );
    /*
    CMgSingleTask* obj=GetTaskObj(ntask);
    if(obj!=NULL)
    {
    	llong thistime, lasttime;
    	obj ->GetFinishByte( thistime, lasttime );
    	speed = ( thistime - lasttime );
    	return ( thistime );		
    }
    */ 
    return 0;
}

bool CTaskManager::DeleteTask( int ntask )
{
    //DBGOUT("before delete task tasklist size="<<m_TaskList.size());

    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
    {

        delete obj;

        m_TaskList.erase(

            remove
                ( m_TaskList.begin(), m_TaskList.end(), obj ) );

        return true;
    }

    DBGOUT( "not find task to delete !" );
    return false;
}

int CTaskManager::GetTaskNumber()
{
    return m_TaskList.size();
}

float CTaskManager::GetTaskProgress( int ntask )
{
    DBGOUT( "GetTaskProgress used" );
    /*
    CMgSingleTask* obj=GetTaskObj(ntask);

    if(obj!=NULL)
    {
    	llong totallen = obj->GetFileLen();

    	if ( totallen == 0 ||totallen==-1)
    		return -1.0;

    	bindex data[ 10 ];

    	int count = obj ->GetTaskIndex( data, 10 );

    	llong finishlen = 0;

    	for ( int i = 0;i < count;i++ )
    	{
    		finishlen += data[ i ].end - data[ i ].start;
    	}

    	return float( finishlen ) / float( totallen );		
    }
    */ 
    return -1.0;
}

bool CTaskManager::AddTask( _TaskAttr& task )
{

    CMgSingleTask * newtask
    = new CMgSingleTask(
          this,
          task.sURL,
          task.sSavePath,
          task.sName,
          task.nID,
          task.nThread,
          task.nRetry,
          task.nRetryWait,
          task.sRefer	);

    if ( newtask == NULL )
    {
        return false;
    }

    //set proxy here
    if ( !task.sSockProxy.empty() )
    {
        int type, port;
        std::string server, user, pass;

        if ( m_pParent->GetProxyByName( task.sSockProxy, type, server, port, user, pass ) )
        {
            switch ( type )
            {

                case 0:
                newtask->SetProxy( server, port, user, pass, AUTOVERSION );
                break;

                case 1:
                newtask->SetProxy( server, port, user, pass, V4 );
                break;

                case 2:
                newtask->SetProxy( server, port, user, pass, V4A );
                break;

                case 3:
                newtask->SetProxy( server, port, user, pass, V5 );
                break;

                default:
                break;
            }
        }
        else
        {
            DBGOUT( "Get proxy args fail" );
        }
    }

    if ( !task.sFtpProxy.empty() )
    {
        int type, port;
        std::string server, user, pass;

        if ( m_pParent->GetProxyByName( task.sFtpProxy, type, server, port, user, pass ) )
        {
            newtask->SetFtpProxy( server, port );
        }
        else
        {
            DBGOUT( "Get proxy args fail" );
        }
    }



    if ( !task.sHttpProxy.empty() )
    {
        int type, port;
        std::string server, user, pass;

        if ( m_pParent->GetProxyByName( task.sHttpProxy, type, server, port, user, pass ) )
        {
            newtask->SetHttpProxy( server, port );
        }
        else
        {
            DBGOUT( "Get proxy args fail" );
        }
    }

    //mirror url
    std::vector<std::string>::const_iterator it;

    for ( it = task.sMirrorURL.begin();it != task.sMirrorURL.end();it++ )
    {
        newtask->AddMirrorUrl( *it );
    }

    //run
    if ( newtask->Go() )
    {
        m_TaskList.push_back( newtask );
        return true;
    }
    else
    { 
		/*//retry onetime
        sleep( 100 );
        //retry

        if ( newtask->Go() )
        {
            m_TaskList.push_back( newtask );
            return true;
        }

        delete newtask;*/
        return false;
    }
}


//任务发生错误的次数
int CTaskManager::GetErrorTime( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->GetErrorTime();

    return -1;
}

//检查是否有效任务id,直接传下去
void CTaskManager::UpdateRunningTable( int ntask, CShowTableWindow* pwin )
{
    CloseUpdateUI( m_nTaskUI );

    CMgSingleTask* obj = GetTaskObj( ntask );

    if ( obj != NULL )
    {
        m_nTaskUI = ntask;
        obj->UpdateRunningTable( pwin );
    }

}

//检查是否有效任务id,直接传下去
void CTaskManager::UpdateThreadInfo( int ntask, CThreadInfoWindow* pwin, int nthreadid )
{
    CloseUpdateUI( m_nTaskUI );

    CMgSingleTask* obj = GetTaskObj( ntask );

    if ( obj != NULL )
    {
        m_nTaskUI = ntask;
        obj->UpdateThreadInfo( pwin, nthreadid );
    }

}

bool CTaskManager::ThreadDown( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->ThreadDown();

    return false;
}

bool CTaskManager::ThreadUp( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        return obj->ThreadUp();

    return false;

}

//检查是否有效任务id,直接传下去
void CTaskManager::CloseUpdateUI( int ntask )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        obj->CloseUpdateUI();
}

CMgSingleTask* CTaskManager::GetTaskObj( int ntask )
{
    vector<CMgSingleTask*>::iterator it;

    for ( it = m_TaskList.begin();it != m_TaskList.end();it++ )
    {
        if ( ( *it ) ->GetTaskID() == ntask )
        {
            return * it;
        }
    }

    return NULL;
}

CTaskManager::CTaskManager( MainFrame* parent )
{
    m_pParent = parent;
    m_nDefaultThreadNum = 5;
    m_nDefaultMaxTask = 5;
    m_nTaskUI = -1;
    m_TaskList.reserve( m_nDefaultMaxTask );
}

struct _DeleteTask
{
    void operator() ( CMgSingleTask* task )
    {
        delete task;
    }
};

CTaskManager::~CTaskManager()
{
    for_each( m_TaskList.begin(), m_TaskList.end(), _DeleteTask() );
}

struct _HeartBeat
{
    void operator() ( CMgSingleTask* task )
    {
        task->HeartBeat();
    }
};

void CTaskManager::HeartBeat()
{
    for_each( m_TaskList.begin(), m_TaskList.end(), _HeartBeat() );
}

//没找大任务返回false,否则true;
bool CTaskManager::GetRunningArgs(
    int ntask,
    float& fratio,
    llong& finish,
    int& speed,
    int& ttf )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
    {
        obj->GetRunningArgs( fratio, finish, speed, ttf );
        return true;
    }

    return false;
}

int CTaskManager::RemoveAllTask()
{
    for_each( m_TaskList.begin(), m_TaskList.end(), _DeleteTask() );
    int nret = m_TaskList.size();
    m_TaskList.clear();
    return nret;
}

void CTaskManager::AddTaskMirrorUrl( int ntask, std::string mirror )
{
    CMgSingleTask * obj = GetTaskObj( ntask );

    if ( obj != NULL )
        obj->AddMirrorUrl( mirror );
}

