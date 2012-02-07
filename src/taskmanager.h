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

#ifndef _TASK_MANAGER_H
#define _TASK_MANAGER_H


#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include "common.h"

class CMgSingleTask;

class CShowTableWindow;

class CThreadInfoWindow;

class MainFrame;

class CTaskManager
{

public:
    CTaskManager( MainFrame* parent );
    ~CTaskManager();
    std::string GetTaskFilename( int ntask );
    //bool AddTask( int ntaskid, std::string url, int priority = 1, int ants = 0 );	//添加新任务
    bool AddTask( _TaskAttr& task );
    bool RemoveTask( int ntask );
    void AddTaskMirrorUrl( int ntask, std::string mirror );
    int RemoveAllTask();
    float GetTaskProgress( int ntask ); 	//获得任务总体进度
    llong GetTaskSize( int ntask ); //文件大小
    std::string GetTaskUrl( int ntask );
    int GetTaskIndex( int ntask, bindex b[], int bnum );	//任务完成的片信息
    llong GetFinishByte( int ntask, int& speed ); //文件完成大小
    _SINGLE_TASK_STATUS GetTaskStatus( int ntask );	//任务当前进行状态，完成，下载中，停止中
    int GetTaskNumber();
    int GetTaskIDbySeq( int seq );
    int GetTaskThreadNum( int ntask );
    int GetErrorTime( int ntask );
    void HeartBeat();
    //int SetDefaultTaskNum(int num);		//设置最大运行任务数量，大于这个则排队
    //int SetDefaultThreadNum(int num);	//设置缺省线程数量，不影响当前的下载任务
    //int SetTaskPriority(int ntask,int priority);	//设置和调整任务优先级

    //
    bool ThreadDown( int taskid );
    bool ThreadUp( int taskid );
    //UI接口通知

    void UpdateRunningTable( int ntask, CShowTableWindow* pwin ); //确定要更新图表
    void UpdateThreadInfo( int ntask, CThreadInfoWindow* pwin, int nthreadid ); //确定要更新线程输出
    void CloseUpdateUI( int ntask ); //关闭更新

    //完成比例，完成数量，平均速度，完成时间，4个动态数据一个函数提取，用在任务显示更新上
    bool GetRunningArgs( int ntask, float& fratio, llong& finish, int& speed, int& ttf );

protected:
    bool DeleteTask( int ntask );
    //	int StopTask(int ntask);

private:
    CMgSingleTask* GetTaskObj( int ntask );
    int m_nDefaultMaxTask;	//最大同时任务
    int m_nDefaultThreadNum;
    std::vector<CMgSingleTask*> m_TaskList;	//任务表
    std::string m_sSavePath;
    MainFrame* m_pParent;
    int m_nTaskUI; //需要更新显示的任务ID，-1就是没有需要更新的
};

#endif

