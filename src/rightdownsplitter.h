
#ifndef _RIGHTDOWN_SPLITTER_H
#define _RIGHTDOWN_SPLITTER_H

#include <wx/wx.h>
#include <wx/splitter.h>

#include <wx/listctrl.h>
#include "common.h"

class CThreadInfoWindow;

class CShowTableWindow;

class CRightTree;

class CTaskDetailReport;

class CRightDownSplitter : public wxSplitterWindow
{

public:
    CRightDownSplitter( wxWindow* parent );
    virtual ~CRightDownSplitter();

    //整体控制左右两部分显示的调用
    void ShowTask( _TaskAttr* task ); //按任务属性显示

    //零散控制函数

    void OnRightTreeSelectItem( int nodedata );

    CShowTableWindow* GetShowTableWin()
    {
        return m_pShowTableWin;
    }

    CThreadInfoWindow* GetThreadInfoWin()
    {
        return m_pThreadInfoWin;
    }

    //上层调用这个函数来在右边显示任务的详细信息，如果这时右边的显示模式不是
    //任务的详细信息模式，那么报告一个不一致的错误
    void ShowTaskDetailReport( _TaskAttr& task );
    void DynamicLang();

private:

    //这三个子窗口在本类构造时就同时做掉，放在这里容易切换
    CRightTree*	m_pRightTreeWin; //这个有它自己的模式控制

    //右边的窗口在什么模式上，5类模式分别是
    //0 空白模式，什么也没选中的模式
    //1 线程信息模式，正在下载的任务显示
    //2 图表模式，正在下载的任务显示
    //3 详细信息模式，下载任务已完成或暂停时的显示
    //4 链接信息模式，目前没做

    wxListCtrl* m_pEmptyWin;
    CThreadInfoWindow* m_pThreadInfoWin; //1
    CShowTableWindow* m_pShowTableWin; //2
    CTaskDetailReport* m_pTaskDetailWin; //3

};

#endif

