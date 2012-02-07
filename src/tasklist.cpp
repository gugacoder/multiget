
/*
      	文件名	:	tasklist.cpp
        建立日期	:	2006/07/25
        作者		:	liubin,China
        EMAIL	:	multiget@gmail.com
        版本		:	0.5
		
		处理右上部的任务列表显示，当有选择时反馈选择的任务到上层处理
		
*/

#include "tasklist.h"
#include "./icons/info.xpm"

#include "./newicons/16/run.xpm"
#include "./newicons/16/wait.xpm"
#include "./newicons/16/stop.xpm"
#include "./newicons/16/finish.xpm"
#include "./newicons/16/delete.xpm"
#include "./newicons/16/property.xpm"
#include "./newicons/16/reload.xpm"

#include "mainframe.h" //任务被选择时，调用这里面的函数
#include "mgapp.h"
#include <wx/imaglist.h>
#include <iostream>

using namespace std;


DEFINE_EVENT_TYPE( mgID_PRUN )
DEFINE_EVENT_TYPE( mgID_PSTOP )
DEFINE_EVENT_TYPE( mgID_PDELETE )
DEFINE_EVENT_TYPE( mgID_PROPERTY )
DEFINE_EVENT_TYPE( mgID_PRELOAD )

BEGIN_EVENT_TABLE( CTaskList, wxListView )

EVT_LIST_ITEM_SELECTED( -1, CTaskList::OnSelect )
EVT_ERASE_BACKGROUND( CTaskList::OnErase )
EVT_LIST_ITEM_RIGHT_CLICK( -1, CTaskList::OnRightClick )

EVT_MENU( mgID_PRUN, CTaskList::OnRun )
EVT_MENU( mgID_PSTOP, CTaskList::OnStop )
EVT_MENU( mgID_PDELETE, CTaskList::OnDelete )
EVT_MENU( mgID_PROPERTY, CTaskList::OnProperty )
EVT_MENU( mgID_PRELOAD, CTaskList::OnReload )

END_EVENT_TABLE()

CTaskList::CTaskList( wxWindow* parent )
        : wxListView( parent, -1, wxDefaultPosition, wxDefaultSize,
                      wxLC_REPORT | wxLC_SINGLE_SEL | wxFULL_REPAINT_ON_RESIZE  )
{


    wxImageList * imlist = new wxImageList( 16, 16, true, 6 );
    imlist->Add( wxIcon( info_xpm ) );
    imlist->Add( wxIcon( run_xpm ) );
    imlist->Add( wxIcon( wait_xpm ) );
    imlist->Add( wxIcon( stop_xpm ) );
    imlist->Add( wxIcon( delete_xpm ) );
    imlist->Add( wxIcon( finish_xpm ) );
    AssignImageList( imlist, wxIMAGE_LIST_SMALL );

    wxListItem itemCol;
    itemCol.SetText( _( "" ) );
    //itemCol.SetImage( 0 );
    itemCol.SetAlign( wxLIST_FORMAT_CENTER );
    InsertColumn( 0, itemCol );
    SetColumnWidth( 0, 20 );
    itemCol.SetImage( -1 );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );
    itemCol.SetText( _("File Name") );
    InsertColumn( 1, itemCol );
    SetColumnWidth( 1, 200 );
    itemCol.SetText( _("Size") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTRE );
    InsertColumn( 2, itemCol );
    SetColumnWidth( 2, 100 );
    itemCol.SetText( _("Completed") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTER );
    InsertColumn( 3, itemCol );
    SetColumnWidth( 3, 100 );
    itemCol.SetText( _("Progress") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTER );
    InsertColumn( 4, itemCol );
    SetColumnWidth( 4, 80 );
    itemCol.SetText( _("Speed") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTER );
    InsertColumn( 5, itemCol );
    SetColumnWidth( 5, 100 );
    itemCol.SetText( _("Time Left") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTER );
    InsertColumn( 6, itemCol );
    SetColumnWidth( 6, 100 );
    itemCol.SetText( _("Retry") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTER );
    InsertColumn( 7, itemCol );
    SetColumnWidth( 7, 50 );
    itemCol.SetText( _("Main URL") );
    itemCol.SetAlign( wxLIST_FORMAT_LEFT );
    InsertColumn( 8, itemCol );
    SetColumnWidth( 8, 700 );


    //	SetBackgroundColour(wxColour(255,255,0));
    //	SetItemBackgroundColour(0,wxColour(255,255,0));
}

//做选择任务的标记
int CTaskList::SelectTask( int taskid, bool on )
{
    long id = FindItem( -1, taskid );

    if ( id < 0 )   //没找到？
    {
        //有没有一个列表？

        if ( GetItemCount() == 0 )
        {
            return -1;
        }

        Select( 0, on ); //选择第一个？

        return GetItemData( 0 );
    }
    else
    {
        Select( id, on );
        return taskid;
    }
}

//把任务向下移动
void CTaskList::MoveDown( int taskid, int& downid )
{
    long id = FindItem( -1, taskid );

    if ( id < 0 )
    {
        downid = -1;
        return ;
    }

    downid = GetItemData( id + 1 );
    return ;
}

void CTaskList::MoveUp( int taskid, int& upid )
{
    long id = FindItem( -1, taskid );

    if ( id < 0 )
    {
        upid = -1;
        return ;
    }

    upid = GetItemData( id - 1 );
    return ;
}

//任务是否能被上下移动? 2006/08/11
void CTaskList::IsTaskCanMove( int taskid, bool& canup, bool& candown )
{
    long id = FindItem( -1, taskid );

    if ( id < 0 )
    {
        canup = false;
        candown = false;
        return ;
    }
    else if ( id == 0 )
    {
        if ( GetItemCount() > 1 )
        {
            canup = false;
            candown = true;

        }
        else
        {
            canup = false;
            candown = false;

        }

        return ;
    }
    else //id>0
    {
        if ( id >= GetItemCount() - 1 )
        {
            canup = true;
            candown = false;
        }
        else
        {
            canup = true;
            candown = true;
        }

        return ;
    }

}

//check OK
void CTaskList::OnSelect( wxListEvent& event )
{

    //get task id
    int taskid = GetItemData( event.GetIndex() );

    //find mainwin
    MainFrame* mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    //call func
    mf->OnSelectTask( taskid );
    //skip
    //event.Skip();
}

//更新列表中任务条目
void CTaskList::UpdateTaskList( _TaskAttr& task )
{

    long id = FindItem( -1, task.nID );

    wxListItem item;

    switch ( task.nStatus )
    { //由task的状态来决定这个图像

        case _STATUS_RUNNING:
        item.SetImage( 1 );
        break;

        case _STATUS_WAITING:
        item.SetImage( 2 );  //等待运行的图标暂时用这个
        break;

        case _STATUS_PAUSE:
        item.SetImage( 3 );
        break;

        case _STATUS_WRONG:
        item.SetImage( 4 );
        break;

        case _STATUS_FINISH:

        case _STATUS_FINISH_SOFT:

        case _STATUS_FINISH_PACK:

        case _STATUS_FINISH_MUSIC:

        case _STATUS_FINISH_ISO:

        case _STATUS_FINISH_MOVIE:

        case _STATUS_FINISH_PUBLISH:

        case _STATUS_FINISH_PICTURE:

        case _STATUS_FINISH_UNKNOW:
        item.SetImage( 5 );
        break;

        case _STATUS_DELETE:
        item.SetImage( 4 );
        break;

        case _STATUS_ROOT:

        default:
        break;
    }

    wxString wxsName( task.sName.c_str(), wxConvLocal );

    SetItem( id, 1, wxsName );

    wxString wxs;

    wxs.Printf( _( "%lld" ), task.nLen );

    SetItem( id, 2, wxs );

    wxs.Printf( _( "%lld" ), task.nFinish );

    SetItem( id, 3, wxs );

    wxs.Printf( _( "%0.2f%%" ), 100 * task.fRatio );

    SetItem( id, 4, wxs );

    wxs.Printf( _( "%dKB/S" ), task.nSpeed / 1024 );

    SetItem( id, 5, wxs );

    //wxs.Printf( _( "%d S" ), task.nTime );

    SetItem( id, 6, TimeToString( task.nTime ) );

    wxs.Printf( _( "%d" ), task.nError );

    SetItem( id, 7, wxs );

    wxString wxURL( task.sURL.c_str(), wxConvLocal );

    SetItem( id, 8, wxURL );

}

//添加任务条目到列表
//为了保证运行任务在前面，运行任务采用插入到头，其他任务采用添加到尾的方法。
void CTaskList::AddTaskList( _TaskAttr& task )
{

    wxListItem item;
    long id;

    switch ( task.nStatus )
    { //由task的状态来决定这个图像

        case _STATUS_RUNNING:

        item.SetImage( 1 );
        break;

        case _STATUS_WAITING:
        item.SetImage( 2 );  //等待运行的图标暂时用这个
        break;

        case _STATUS_PAUSE:
        item.SetImage( 3 );
        break;

        case _STATUS_WRONG:
        item.SetImage( 4 );
        break;

        case _STATUS_FINISH:

        case _STATUS_FINISH_SOFT:

        case _STATUS_FINISH_PACK:

        case _STATUS_FINISH_MUSIC:

        case _STATUS_FINISH_ISO:

        case _STATUS_FINISH_MOVIE:

        case _STATUS_FINISH_PUBLISH:

        case _STATUS_FINISH_PICTURE:

        case _STATUS_FINISH_UNKNOW:
        item.SetImage( 5 );
        break;

        case _STATUS_DELETE:
        item.SetImage( 4 );
        break;

        case _STATUS_ROOT:

        default:
        break;
    }

    if ( task.nStatus == _STATUS_RUNNING )
        item.SetId( 0 );
    else
        item.SetId( GetItemCount() );

    id = InsertItem( item );

    SetItemData( id, task.nID );

    wxString wxsName( task.sName.c_str(), wxConvLocal );

    SetItem( id, 1, wxsName );

    wxString wxs;

    wxs.Printf( _( "%lld" ), task.nLen );

    SetItem( id, 2, wxs );

    wxs.Printf( _( "%lld" ), task.nFinish );

    SetItem( id, 3, wxs );

    wxs.Printf( _( "%0.2f%%" ), 100 * task.fRatio );

    SetItem( id, 4, wxs );

    wxs.Printf( _( "%dKB/S" ), task.nSpeed / 1024 );

    SetItem( id, 5, wxs );

    //wxs.Printf( _( "%d S" ), task.nTime );

    SetItem( id, 6, TimeToString( task.nTime ) );

    wxs.Printf( _( "%d" ), task.nError );

    SetItem( id, 7, wxs );

    wxString wxURL( task.sURL.c_str(), wxConvLocal );

    SetItem( id, 8, wxURL );

}

wxString CTaskList::TimeToString( int seconds )
{
    wxString retstr;

    if ( seconds < 0 )
    {
        retstr = _("Unknown");
        return retstr;
    }

    if ( seconds > 86400 )
    {
        retstr.Printf( _("%d day"), seconds / 86400 );

    }
    else
    {
        wxString temp;
        temp.Printf( _("%02dh:%02dm:%02ds"),
                     ( seconds % 86400 ) / 3600, ( seconds % 3600 ) / 60, seconds % 60 );
        retstr += temp;
    }

    return retstr;
}

void CTaskList::DynamicLang()
{

    wxListItem itemCol;
    GetColumn( 1, itemCol ) ;
    itemCol.SetText( _("File Name") );
    SetColumn( 1, itemCol );

    GetColumn( 2, itemCol ) ;
    itemCol.SetText( _("Size") );
    SetColumn( 2, itemCol );

    GetColumn( 3, itemCol ) ;
    itemCol.SetText( _("Completed") );
    SetColumn( 3, itemCol );

    GetColumn( 4, itemCol ) ;
    itemCol.SetText( _("Progress") );
    SetColumn( 4, itemCol );

    GetColumn( 5, itemCol ) ;
    itemCol.SetText( _("Speed") );
    SetColumn( 5, itemCol );

    GetColumn( 6, itemCol ) ;
    itemCol.SetText( _("Time Left") );
    SetColumn( 6, itemCol );

    GetColumn( 7, itemCol ) ;
    itemCol.SetText( _("Retry") );
    SetColumn( 7, itemCol );

    GetColumn( 8, itemCol ) ;
    itemCol.SetText( _("Main URL") );
    SetColumn( 8, itemCol );

}

void CTaskList::OnErase( wxEraseEvent& event )
{ //KDE下，白色会变灰色底，在List里不能通过这个方法修改背景颜色

    event.Skip();
    //	wxCoord w,h;
    //	wxPaintDC pdc(this);
    //	pdc.GetSize(&w,&h);
    //	pdc.Clear();
    //pdc.DrawIcon(*m_tjicon,(w-90)/2,h-100);
    //	return;
}

void CTaskList::OnRightClick( wxListEvent& event )
{

    //find mainwin
    MainFrame * mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    _TaskAttr* pcur = mf->GetCurrentTask();

    if ( pcur == NULL )
        return ;

    //pop a menu,run,stop,delete,prop ...
    // 菜单开始
    wxMenu *rootPopMenu = new wxMenu; //( _( "Options" ) );

    wxMenuItem *ti;

    ti =
        new wxMenuItem( rootPopMenu, mgID_PRUN, _("Run task"), _( "help" ) );

    ti->SetBitmap( wxBitmap( run_xpm ) );

    rootPopMenu->Append( ti );

    ti =
        new wxMenuItem( rootPopMenu, mgID_PSTOP, _("Stop task"), _( "help" ) );

    ti->SetBitmap( wxBitmap( stop_xpm ) );

    rootPopMenu->Append( ti );

    ti =
        new wxMenuItem( rootPopMenu, mgID_PDELETE, _("Delete task"), _( "help" ) );

    ti->SetBitmap( wxBitmap( delete_xpm ) );

    rootPopMenu->Append( ti );

    rootPopMenu->AppendSeparator();

    ti =
        new wxMenuItem( rootPopMenu, mgID_PRELOAD, _("Redonwload task"), _( "help" ) );

    ti->SetBitmap( wxBitmap( reload_xpm ) );

    rootPopMenu->Append( ti );

    rootPopMenu->AppendSeparator();

    ti =
        new wxMenuItem( rootPopMenu, mgID_PROPERTY, _("Show/Modify task properties"), _( "help" ) );

    ti->SetBitmap( wxBitmap( property_xpm ) );

    rootPopMenu->Append( ti );

    /*
    //共用工具条提示
    rootPopMenu->Append( mgID_PRUN, _MGSTR(_S_TIP_RUNTASK) );	
    rootPopMenu->Append( mgID_PSTOP, _MGSTR( _S_TIP_STOPTASK ) );	
    rootPopMenu->Append( mgID_PDELETE, _MGSTR( _S_TIP_DELETETASK ) );
    rootPopMenu->Append( mgID_PRELOAD, _MGSTR( _S_TIP_RELOADTASK ) );	
    rootPopMenu->Append( mgID_PROPERTY, _MGSTR( _S_TIP_PROPERTY ) );	
    */


    switch ( pcur->nStatus )
    {

        case _STATUS_RUNNING:

        case _STATUS_WAITING:
        {
            rootPopMenu->Enable( mgID_PRUN, false );
            rootPopMenu->Enable( mgID_PDELETE, true );

        }

        break;

        case _STATUS_PAUSE:
        {

            rootPopMenu->Enable( mgID_PSTOP, false );

        }

        break;

        case _STATUS_WRONG:
        {

            rootPopMenu->Enable( mgID_PSTOP, false );

        }

        break;

        case _STATUS_FINISH_SOFT:

        case _STATUS_FINISH_PACK:

        case _STATUS_FINISH_MUSIC:

        case _STATUS_FINISH_ISO:

        case _STATUS_FINISH_MOVIE:

        case _STATUS_FINISH_PUBLISH:

        case _STATUS_FINISH_PICTURE:

        case _STATUS_FINISH_UNKNOW:
        {
            rootPopMenu->Enable( mgID_PRUN, false );
            rootPopMenu->Enable( mgID_PSTOP, false );

        }

        break;

        case _STATUS_DELETE:
        {

            rootPopMenu->Enable( mgID_PSTOP, false );

        }

        break;

        default:
        break;

    }

    PopupMenu( rootPopMenu );
    delete rootPopMenu;
}

//pop menu
void CTaskList::OnRun( wxCommandEvent& event )
{
    MainFrame * mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    mf->OnStartTask( event );
}

void CTaskList::OnStop( wxCommandEvent& event )
{
    MainFrame * mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    mf->OnStopTask( event );
}

void CTaskList::OnDelete( wxCommandEvent& event )
{
    MainFrame * mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    mf->OnCancelTask( event );
}

void CTaskList::OnProperty( wxCommandEvent& event )
{
    MainFrame * mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    mf->OnProperty( event );
}

void CTaskList::OnReload( wxCommandEvent& event )
{
    MainFrame * mf = ( MainFrame* ) ( GetParent() ->GetParent() ->GetParent() );
    mf->OnReloadFile( event );
}
