
/*
      	文件名	:	taskdetailreport.cpp
        建立日期	:	2006/07/25
        作者		:	liubin
        EMAIL	:	multiget@gmail.com
        版本		:	0.5	
		
  		这个类用于在右下显示已经停止的任务的信息
		
		2006/07/29	新增OnSize，OnColumnResize函数处理窗口变化
 
*/
#include "taskdetailreport.h"
#include "mgapp.h"

//#include <iostream>

using namespace std;

BEGIN_EVENT_TABLE( CTaskDetailReport, wxListView )
EVT_CUSTOM( wxEVT_SIZE, wxID_ANY, CTaskDetailReport::OnSize )
EVT_LIST_COL_END_DRAG( wxID_ANY, CTaskDetailReport::OnColumnResize )
EVT_ERASE_BACKGROUND( CTaskDetailReport::OnErase )
END_EVENT_TABLE()

CTaskDetailReport::CTaskDetailReport( wxWindow* parent )
        : wxListView( parent, wxID_ANY, wxDefaultPosition,
                      wxSize( 0, 0 ), wxLC_REPORT | wxLC_SINGLE_SEL )
{

    SetBackgroundStyle( wxBG_STYLE_COLOUR );
    wxListItem itemCol;
    itemCol.SetText( _("Name") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTRE );
    InsertColumn( 0, itemCol );
    SetColumnWidth( 0, 200 );
    itemCol.SetText( _("Value") );
    itemCol.SetAlign( wxLIST_FORMAT_LEFT );
    InsertColumn( 1, itemCol );
    SetColumnWidth( 1, 500 );

    SetBackgroundColour( wxColour( 255, 255, 255 ) );
}

/*
struct _TaskAttr
{
    int nID; //系统中分配的唯一任务号
    _TASK_TYPE nStatus; //任务状态，
    int nThread; //当前线程数，只对运行状态中的任务有效
    int nPriority; //任务优先级，只对运行中的任务有效
    std::string sName; //最终解析的文件名
    llong nLen; //文件长度
    float fRatio; //完成度，如果是已完成文件则不参考这个值
    llong nFinish; //完成的字节数
    int nSpeed; //平均速度
    int nTime; //估计剩余时间
    int nError; //发生错误的次数
    std::string sURL; //原始URL
	//std::string sMirrorURL[9]; //9个mirrorURL
    std::string sSavePath; //最后存放的位置
	std::string sSockProxy;
	std::string sFtpProxy;
	std::string sHttpProxy;
	std::string sMD5;
};
*/
void CTaskDetailReport::ShowTaskDetail( _TaskAttr& tsk )
{
    DeleteAllItems();

    wxString name, value;

    name = _("Task ID");
    value.Printf( _( "%d" ), tsk.nID );
    AddInfo( name, value );

    name = _("Main URL");
    wxString url( tsk.sURL.c_str(), wxConvLocal );
    AddInfo( name, url );

	name = _("Number of mirror URL"); //number of mirror
    value.Printf( _( "%d" ), tsk.sMirrorURL.size() );
    AddInfo( name, value );
	
    name = _("Task Status");

    switch ( tsk.nStatus )
    {

        case _STATUS_RUNNING:
        value = _("Running");
        break;

        case _STATUS_PAUSE:
        value = _("Pause");
        break;

        case _STATUS_WAITING:
        value = _("Waiting");
        break;

        case _STATUS_WRONG:
        value = _("Wrong");
        break;

        case _STATUS_FINISH_SOFT:

        case _STATUS_FINISH_PACK:

        case _STATUS_FINISH_MUSIC:

        case _STATUS_FINISH_ISO:

        case _STATUS_FINISH_MOVIE:

        case _STATUS_FINISH_PUBLISH:

        case _STATUS_FINISH_PICTURE:

        case _STATUS_FINISH_UNKNOW:
        value = _("Completed");
        break;

        case _STATUS_DELETE:
        value = _("Trash");
        break;

        default:
        value = _("Unknown");
        break;
    }

    AddInfo( name, value );

    name = _("Thread Numbers");
    value.Printf( _( "%d" ), tsk.nThread );
    AddInfo( name, value );

    name = _("Save Path");
    wxString spath( tsk.sSavePath.c_str(), wxConvLocal );
    AddInfo( name, spath );

    name = _("File Name");
    wxString wxFn( tsk.sName.c_str(), wxConvLocal );
    AddInfo( name, wxFn );

    name = _("Size");
    value.Printf( _( "%lld" ), tsk.nLen );
    AddInfo( name, value );

    name = _("Completed");
    value.Printf( _( "%lld" ), tsk.nFinish );
    AddInfo( name, value );

    name = _("Progress");
    value.Printf( _( "%0.4f%%" ), tsk.fRatio * 100 );
    AddInfo( name, value );

    name = _("Speed");
    value.Printf( _( "%d bytes/s" ), tsk.nSpeed );
    AddInfo( name, value );

    name = _("Error times");
    value.Printf( _( "%d" ), tsk.nError );
    AddInfo( name, value );


    name = _("SOCKS Proxy");
    wxString socks( tsk.sSockProxy.c_str(), wxConvLocal );

    if ( socks == _( "" ) )
        socks = _( "none" );

    AddInfo( name, socks );

    name = _("FTP Proxy");

    wxString ftp( tsk.sFtpProxy.c_str(), wxConvLocal );

    if ( ftp == _( "" ) )
        ftp = _( "none" );

    AddInfo( name, ftp );

    name = _("HTTP Proxy");

    wxString http( tsk.sHttpProxy.c_str(), wxConvLocal );

    if ( http == _( "" ) )
        http = _( "none" );

    AddInfo( name, http );


    name = _("MD5 sum auto check");
    value = tsk.bAutoMd5 ? _( "YES" ) : _( "NO" );
    AddInfo( name, value );

    name = _("MD5 sum");

    wxString md5( tsk.sMD5.c_str(), wxConvLocal );

    if ( md5 == _( "" ) )
        md5 = _( "none" );

    AddInfo( name, md5 );
}

//增加一个信息行
int CTaskDetailReport::AddInfo( wxString name, wxString value )
{
    wxListItem item;

    int id;
    id = InsertItem( GetItemCount(), item );
    SetItem( id, 0, name );
    SetItem( id, 1, value );

    SetItemBackgroundColour( id, id % 2 ? wxColour( 210, 210, 210 ) : wxColour( 230, 230, 230 ) );
    return id;
}

void CTaskDetailReport::OnSize( wxEvent& event )
{
    //wxSize vs=GetVirtualSize();
    wxSize vs = GetClientSize();
    int fw = GetColumnWidth( 0 );

    if ( vs.x >= fw + 300 )
    {
        SetColumnWidth( 1, vs.x - fw );
    }
    else
    {
        SetColumnWidth( 1, 300 );
    }

    event.Skip();

}

void CTaskDetailReport::OnColumnResize( wxListEvent& event )
{
    int ncol = event.GetColumn();

    if ( ncol == 0 )
    { //第一列被调整

        wxSize vs = GetClientSize();
        int fw = GetColumnWidth( 0 );

        if ( vs.x >= fw + 300 )
        {
            SetColumnWidth( 1, vs.x - fw );
        }
        else
        {
            SetColumnWidth( 1, 300 );
        }
    }

    event.Skip();
}

void CTaskDetailReport::DynamicLang()
{

    wxListItem itemCol;
    itemCol.SetText( _("Name") );
    itemCol.SetAlign( wxLIST_FORMAT_CENTRE );
    SetColumn( 0, itemCol );

    itemCol.SetText( _("Value") );
    itemCol.SetAlign( wxLIST_FORMAT_LEFT );
    SetColumn( 1, itemCol );

    SetItemText(  0, _("Task ID") );
    SetItemText(  1, _("Main URL") );
	SetItemText(  2, _("Number of mirror URL") );
    SetItemText(  3, _("Task Status") );
    SetItemText(  4, _("Thread Numbers") );
    SetItemText(  5, _("Save Path") );
    SetItemText(  6, _("File Name") );
    SetItemText(  7, _("Size") );
    SetItemText(  8, _("Completed") );
    SetItemText(  9, _("Progress") );
    SetItemText( 10, _("Speed") );
    SetItemText( 11, _("Error times") );
    SetItemText( 12, _("SOCKS Proxy") );
    SetItemText( 13, _("FTP Proxy") );
    SetItemText( 14, _("HTTP Proxy") );
    SetItemText( 15, _("MD5 sum auto check") );
    SetItemText( 16, _("MD5 sum") );

    wxSize vs = GetClientSize();
    int fw = GetColumnWidth( 0 );

    if ( vs.x >= fw + 300 )
    {
        SetColumnWidth( 1, vs.x - fw );
    }
    else
    {
        SetColumnWidth( 1, 300 );
    }


}

void CTaskDetailReport::OnErase( wxEraseEvent& event )
{ //KDE下，白色会变灰色底
	event.Skip();
	/*
    wxPaintDC pdc( this );
    pdc.SetBackground( *wxWHITE_BRUSH );
    pdc.Clear();
	*/
}

