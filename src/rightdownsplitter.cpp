
/*
右下的splitter，单独做是因为它包含了变化的元素，便于处理
*/

#include "rightdownsplitter.h"
#include "threadinfowindow.h" //显示线程信息的窗口，右边
#include "showtablewindow.h"  //显示图表的窗口，右边
#include "taskdetailreport.h" //任务详细信息窗口，右边
#include "righttree.h"		  //左边的选择控件
#include "mainframe.h"

#include <iostream>

using namespace std;
extern std::vector<_tasklog> gTaskLog;

void CRightDownSplitter::ShowTask( _TaskAttr* task )
{
    if ( task == NULL )
    {
        GetWindow2() ->Show( false );
        ReplaceWindow( GetWindow2(), m_pEmptyWin );
        m_pEmptyWin->Show( true );
    }

    //当它选择root时，没有消息返上来，奇怪？

    m_pRightTreeWin->ShowTask( task ); //让它来决定右边怎么显示

    return ;

}


CRightDownSplitter::CRightDownSplitter( wxWindow* parent )
        : wxSplitterWindow(
            parent,
            wxID_ANY,
            wxDefaultPosition,
            wxDefaultSize,
            wxSP_LIVE_UPDATE,
            wxT( "rds" )
        )
{


    m_pThreadInfoWin = new CThreadInfoWindow( this );

    m_pShowTableWin = new CShowTableWindow( this );  //这个构造函数可能导致对话框有问题

    m_pRightTreeWin = new CRightTree( this );

    m_pEmptyWin = new wxListView( this, -1 );
    m_pEmptyWin->SetSingleStyle( wxLC_NO_HEADER );

    m_pEmptyWin->SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    m_pEmptyWin->SetBackgroundColour( wxColour( 255, 255, 255 ) );

    m_pTaskDetailWin = new CTaskDetailReport( this );


    assert( m_pThreadInfoWin != NULL );
    assert( m_pShowTableWin != NULL );
    assert( m_pRightTreeWin != NULL );
    assert( m_pEmptyWin != NULL );
    assert( m_pTaskDetailWin != NULL );

    m_pThreadInfoWin->Show( false );
    m_pShowTableWin->Show( false ); //如果没有这个对话框显示就会出问题
    m_pEmptyWin->Show( false );
    m_pTaskDetailWin->Show( false );
    m_pRightTreeWin->Show( false );


    SetSashGravity( 0 );
    SetMinimumPaneSize( 140 );

    SplitVertically( m_pRightTreeWin, m_pEmptyWin, 140 );
    m_pRightTreeWin->Show( true );

}

//ok
CRightDownSplitter::~CRightDownSplitter()
{

    //切换到Splitter中用于显示的窗口可以不管，但没显示的窗口需要删除
    //将来右边的窗口种类增加时，这个要变
    ReplaceWindow( GetWindow2(), m_pEmptyWin );
    m_pShowTableWin->Destroy();
    m_pThreadInfoWin->Destroy();
    m_pTaskDetailWin->Destroy();

}

//上层调用这个函数来在右边显示任务的详细信息，如果这时右边的显示模式不是
//任务的详细信息模式，那么报告一个不一致的错误
void CRightDownSplitter::ShowTaskDetailReport( _TaskAttr& task )
{
    m_pTaskDetailWin->ShowTaskDetail( task );
}

//当右边的树控件选择了一个条目时，把条目中的数据传到这里来处理，
//可能需要右边进行相应的显示变化
void CRightDownSplitter::OnRightTreeSelectItem( int nodedata )
{

    //0是根，右边切换空白
    //-1是信息提取器
    //-2是文件管理器
    //-3是图表，右边切换成图表
    //1-19是线程信息
    //20是详细信息
    //21是日志记录


    if ( !nodedata )  //root
    {

        GetWindow2() ->Show( false );
        ReplaceWindow( GetWindow2(), m_pEmptyWin );
        m_pEmptyWin->Show();
        //下面两句作用不明？？
        MainFrame* pmainframe = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
        pmainframe->OnShowModeSwitch( nodedata );
        return ;
    }

    //一个是信息提取，一个是文件管理，都用线程信息显示
    if ( -1 == nodedata || -2 == nodedata )
    {
        GetWindow2() ->Show( false );
        ReplaceWindow( GetWindow2(), m_pThreadInfoWin );
        m_pThreadInfoWin->DeleteAllItems(); //??
        m_pThreadInfoWin->Show( true );
        //向上层通报
        MainFrame* pmainframe = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
        pmainframe->OnShowModeSwitch( nodedata );
        return ;
    }

    if ( -3 == nodedata )
    {
        GetWindow2() ->Show( false );
        ReplaceWindow( GetWindow2(), m_pShowTableWin );
        m_pShowTableWin->Show( true );
        MainFrame* pmainframe = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
        pmainframe->OnShowModeSwitch( nodedata );
        return ;
    }

    if ( nodedata >= 1 && nodedata <= 19 )
    {

        GetWindow2() ->Show( false );

        ReplaceWindow( GetWindow2(), m_pThreadInfoWin );
        m_pThreadInfoWin->DeleteAllItems();  //??
        m_pThreadInfoWin->Show( true );
        MainFrame* pmainframe = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
        pmainframe->OnShowModeSwitch( nodedata );

        return ;
    }

    if ( nodedata == 20 )
    {
        GetWindow2() ->Show( false );
        ReplaceWindow( GetWindow2(), m_pTaskDetailWin );
        m_pTaskDetailWin->Show( true );

        //找到当前选择的任务，显示任务详细信息
        MainFrame* pmainframe = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
        _TaskAttr* curtask = pmainframe->GetCurrentTask();

        ShowTaskDetailReport( *curtask );
        pmainframe->OnShowModeSwitch( nodedata );
        return ;
    }

    if ( nodedata == 21 )  //log info
    {
        GetWindow2() ->Show( false );
        ReplaceWindow( GetWindow2(), m_pThreadInfoWin );

        m_pThreadInfoWin->DeleteAllItems(); //??
        m_pThreadInfoWin->Show( true );

        //添加日志数据
        MainFrame* pmainframe = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
        _TaskAttr* curtask = pmainframe->GetCurrentTask();

        //提取curtask->nID日志并显示
        std::vector<_tasklog>::const_iterator it;

        for ( it = gTaskLog.begin();it != gTaskLog.end();it++ )
        {
            if ( it->ntaskid == curtask->nID )
            {
                m_pThreadInfoWin->AddInfo( it->ntype, it->info, it->ntime );
            }
        }

        pmainframe->OnShowModeSwitch( nodedata );
        return ;
    }
}



void CRightDownSplitter::DynamicLang()
{
    m_pRightTreeWin->DynamicLang();
    m_pTaskDetailWin->DynamicLang();
}

