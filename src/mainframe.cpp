/***************************************************************************
*  mainframe.cpp
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


#include "mgapp.h"
#include "mainframe.h"
#include "listenpipe.h"
#include <list>
#include <wx/image.h>
#include <locale.h>
#include <wx/aboutdlg.h>
#include <wx/intl.h>
#include "rightdownsplitter.h"
#include "taskmanager.h"
#include "lefttree.h"
#include "tasklist.h"
#include "mgnewtaskdlg.h"
#include "dropurl.h"
#include "mgurlparser.h"
#include "filemd5.h"

#include "meterctrl.h"
#include "systray.h"
#include "floatwin.h"
#include "hidewin.h"
#include "proxydlg.h"
#include "configwin.h"
#include "mirroradmin.h"
#include "propertydlg.h"
#include "batch.h"

#include "./newicons/16/new.xpm"
#include "./newicons/16/run.xpm"
#include "./newicons/16/stop.xpm"
#include "./newicons/16/delete.xpm"
#include "./newicons/16/up.xpm"
#include "./newicons/16/down.xpm"
#include "./newicons/16/plus.xpm"
#include "./newicons/16/sub.xpm"
#include "./newicons/16/reload.xpm"
#include "./newicons/16/option.xpm"
#include "./newicons/16/quit.xpm"
#include "./icons/about.xpm"
#include "./newicons/16/proxy.xpm"
#include "./newicons/16/save.xpm"
#include "./newicons/16/msgsystem_16.xpm"

#include "./newicons/32/tool_new.xpm"
#include "./newicons/32/tool_run.xpm"
#include "./newicons/32/tool_pause.xpm"
#include "./newicons/32/tool_delete.xpm"
#include "./newicons/32/tool_up.xpm"
#include "./newicons/32/tool_down.xpm"
#include "./newicons/32/tool_option.xpm"
#include "./newicons/32/tool_sub.xpm"
#include "./newicons/32/tool_plus.xpm"
#include "./newicons/32/tool_reload.xpm"
#include "./newicons/32/tool_proxy.xpm"
#include "./newicons/32/tool_property.xpm"
#include "./newicons/baby.xpm"
#include "./newicons/16/logo_16.xpm" 
//#include "./newicons/24/logo_24.xpm"
#include "./newicons/32/logo_32.xpm"
#include "./newicons/48/logo_48.xpm"
#include "./newicons/24/logo_22.xpm"

#include <fstream>

using namespace std;

DEFINE_EVENT_TYPE( mgID_TIMER )
DEFINE_EVENT_TYPE( mgEVT_TASK_FINISH )
DEFINE_EVENT_TYPE( mgEVT_URL_NOTICE )
DEFINE_EVENT_TYPE( mgEVT_CMD_NOTICE )
DEFINE_EVENT_TYPE( mgID_NEW_TASK )
DEFINE_EVENT_TYPE( mgID_START_TASK )
DEFINE_EVENT_TYPE( mgID_STOP_TASK )
DEFINE_EVENT_TYPE( mgID_CANCEL_TASK )
DEFINE_EVENT_TYPE( mgID_MOVEUP_TASK )
DEFINE_EVENT_TYPE( mgID_MOVEDOWN_TASK )
DEFINE_EVENT_TYPE( mgID_OPTION_CONFIG )
DEFINE_EVENT_TYPE( mgID_SUBTHREAD )
DEFINE_EVENT_TYPE( mgID_ADDTHREAD )
DEFINE_EVENT_TYPE( mgID_RELOAD )

DEFINE_EVENT_TYPE( mgID_VIEW_LANG )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_EN_UTF8 )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_CN_UTF8 )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_GB18030 )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_TW_UTF8 )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_TW_BIG5 )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_DE_UTF8 )
DEFINE_EVENT_TYPE( mgID_VIEW_LANG_PT_UTF8 )

DEFINE_EVENT_TYPE( mgID_VIEW_TOOLBAR )
DEFINE_EVENT_TYPE( mgID_VIEW_TOOLBAR_SIZE16 )
DEFINE_EVENT_TYPE( mgID_VIEW_TOOLBAR_SIZE24 )
DEFINE_EVENT_TYPE( mgID_VIEW_TOOLBAR_SIZE32 )
DEFINE_EVENT_TYPE( mgID_VIEW_STATUSBAR )
DEFINE_EVENT_TYPE( mgID_OPTION_PROXY )
DEFINE_EVENT_TYPE( mgID_OPTION_PROPERTY )

DEFINE_EVENT_TYPE( mgID_FILE_SAVECONFIG );
DEFINE_EVENT_TYPE( mgID_FILE_SAVETASK );
DEFINE_EVENT_TYPE( mgID_FILE_SAVEPROXY );
DEFINE_EVENT_TYPE( mgID_HELP_VISITHOME );

// Event table for MainFrame
BEGIN_EVENT_TABLE( MainFrame, wxFrame )
EVT_MENU( wxID_ABOUT, MainFrame::OnAbout )
EVT_MENU( wxID_EXIT, MainFrame::OnQuit )
EVT_TIMER( mgID_TIMER, MainFrame::OnTimer )
EVT_SIZE( MainFrame::OnSize )
EVT_CLOSE( MainFrame::OnClose )
//EVT_WINDOW_DESTROY( MainFrame::OnDestroy )
EVT_COMMAND( wxID_ANY, mgEVT_TASK_FINISH, MainFrame::OnTaskFinish )
EVT_COMMAND( wxID_ANY, mgEVT_URL_NOTICE, MainFrame::OnUrlNotice )
EVT_COMMAND( wxID_ANY, mgEVT_CMD_NOTICE, MainFrame::OnCmdNotice )
EVT_ICONIZE( MainFrame::OnIconize )
EVT_MENU( mgID_NEW_TASK, MainFrame::OnNewTask )
EVT_MENU( mgID_START_TASK, MainFrame::OnStartTask )
EVT_MENU( mgID_STOP_TASK, MainFrame::OnStopTask )
EVT_MENU( mgID_CANCEL_TASK, MainFrame::OnCancelTask )
EVT_MENU( mgID_SUBTHREAD, MainFrame::OnDecreaseThread )
EVT_MENU( mgID_ADDTHREAD, MainFrame::OnIncreaseThread )
EVT_MENU( mgID_RELOAD, MainFrame::OnReloadFile )
EVT_MENU( mgID_OPTION_CONFIG, MainFrame::OnConfig )
EVT_MENU( mgID_MOVEDOWN_TASK, MainFrame::OnMoveDown )
EVT_MENU( mgID_MOVEUP_TASK, MainFrame::OnMoveUp )
/*2008-4-15 Disable the Language menu, Wing Sun */
/*
EVT_MENU( mgID_VIEW_LANG_EN_UTF8, MainFrame::OnLangEnUtf8 )
EVT_MENU( mgID_VIEW_LANG_CN_UTF8, MainFrame::OnLangCnUtf8 )
EVT_MENU( mgID_VIEW_LANG_GB18030, MainFrame::OnLangCn18030 )
EVT_MENU( mgID_VIEW_LANG_TW_UTF8, MainFrame::OnLangTwUtf8 )
EVT_MENU( mgID_VIEW_LANG_TW_BIG5, MainFrame::OnLangTwBig5 )
EVT_MENU( mgID_VIEW_LANG_DE_UTF8, MainFrame::OnLangDeUtf8 )
EVT_MENU( mgID_VIEW_LANG_PT_UTF8, MainFrame::OnLangPtUtf8 )
*/
EVT_MENU( mgID_VIEW_STATUSBAR, MainFrame::OnStatusBar )
EVT_MENU( mgID_VIEW_TOOLBAR_SIZE16, MainFrame::OnToolbarSize16 )
EVT_MENU( mgID_VIEW_TOOLBAR_SIZE24, MainFrame::OnToolbarSize24 )
EVT_MENU( mgID_VIEW_TOOLBAR_SIZE32, MainFrame::OnToolbarSize32 )
EVT_MENU( mgID_OPTION_PROXY, MainFrame::OnProxySetting )
EVT_MENU( mgID_OPTION_PROPERTY, MainFrame::OnProperty )

EVT_MENU( mgID_FILE_SAVECONFIG, MainFrame::OnSaveConfig )
EVT_MENU( mgID_FILE_SAVEPROXY, MainFrame::OnSaveProxy )
EVT_MENU( mgID_FILE_SAVETASK, MainFrame::OnSaveTask )
EVT_MENU( mgID_HELP_VISITHOME, MainFrame::OnVisitHome )

EVT_WINDOW_CREATE( MainFrame::OnWindowCreate )
EVT_MAXIMIZE( MainFrame::OnMaximize )
//EVT_SET_FOCUS( MainFrame::OnFocus )
//EVT_KILL_FOCUS( MainFrame::OnLostFocus )
END_EVENT_TABLE()


extern int gRunTaskNum;
extern int gConnectTimeOut;
extern int gReceiveTimeOut;
extern int gTaskDefThread;
extern int gRetryTime;
extern int gRetryWait;
extern std::list<std::string> gSavePathHistory;
extern std::string gDefSavePath, gDefFtpPass, gsMonitorExt, gsIgnoreExt;

extern bool gbIgnoreExt;
extern bool gbMonitorExt;
extern bool gbShowDropWin;
extern bool gbMonitorClip;
extern bool gbSoundTip;
extern bool gbAutoMd5;
extern int gDropWinX;
extern int gDropWinY;
extern int gSpeedMode;

//a simple mirror admin,because filemanager need it.
//extern CMirrorAdmin gMirrorAdmin;

void MainFrame::OnIconize( wxIconizeEvent& event )
{
    if ( event.Iconized() )
    {
        Iconize( true );
        Hide();
    }
}

void MainFrame::OnWindowCreate( wxWindowCreateEvent& event )
{
#ifndef WIN32
//in windows ,this is called before ctor. so only use in linux
    if ( m_FloatWin )
    {
		m_FloatWin->Show(); 
	}

    if ( m_SysTray )
    {
		m_SysTray->SetIcon( wxIcon( logo_22_xpm ), _("MultiGet file downloader"));
	}
#else
	event.Skip();
#endif
}

MainFrame::MainFrame( const wxString& title, wxLocale& locale)
        : wxFrame( NULL, wxID_ANY, title, wxDefaultPosition, wxSize( 800, 600 ),
                   wxRESIZE_BORDER | wxDEFAULT_FRAME_STYLE | wxMAXIMIZE | wxFULL_REPAINT_ON_RESIZE ),
		m_locale(locale)
{


    m_nCurrentShowType = ( _STATUS_ROOT );
    m_nTaskID = 0;
    m_nCurrentSelectTaskID = -1;
    m_lefttree = NULL;
    m_tasklist = NULL;
    m_vspliter = NULL;
    m_hspliter = NULL;
    m_vsubspliter = NULL;
    m_ToolBar = NULL;
    m_MenuBar = NULL;
	m_MeterBar = NULL;
    m_HideWin = NULL;
    m_NewTaskDlg = NULL;
    m_PropertyDlg = NULL;
	m_lang = NULL;
    //m_bMonitorClip = true;
    m_bShowStatusBar = true;
    TOOLBAR_SIZE = 32;
    m_nWinSizeW = 800;
    m_nWinSizeH = 600;
    pthread_mutex_init ( &m_TaskMutex, NULL );
    //change to SetIcons
    wxIconBundle bund;
    bund.AddIcon( wxIcon( logo_16_xpm ) );
    bund.AddIcon( wxIcon( logo_22_xpm ) );
    bund.AddIcon( wxIcon( logo_32_xpm ) );
    bund.AddIcon( wxIcon( logo_48_xpm ) );

    SetIcons( bund );


    m_TaskManager = new CTaskManager( this );


    ReadTaskDB();

    ReadProxyDB();

    if ( !ReadConfig() )
    {
        GetUserHome( gDefSavePath );
    }

    InitFileTypeList();

    InitMeterBar(); //call before onsize
    InitSpliter();	//call before onsize
    InitMenuBar(); //make menu generate a size event in win32
	
    InitToolBar();
    InitStatusBar();



    m_Timer.SetOwner( this, mgID_TIMER );
    m_Timer.Start( 1000 );
	
    if ( gbShowDropWin )
        m_FloatWin = new CFloatWin( this );
    else
        m_FloatWin = NULL;

    m_SysTray = new CSysTray( this );
	
#ifdef WIN32
//in windows 
    if ( m_FloatWin )
    {//float win have trouble currently
		m_FloatWin->Show(); 
	}

    if ( m_SysTray )
    {
		m_SysTray->SetIcon( wxIcon( logo_22_xpm ), _("MultiGet file downloader") );
	}
#endif
	
    m_HideWin = new CHideWin( this );

    m_HideWin->MonitorClip( gbMonitorClip );

    UpdateToolBar();

    ScheduleTask();

#ifndef WIN32
    CListenPipe *tpi = new CListenPipe( this );

    tpi->Create();

    tpi->Run();
	
#endif
}


MainFrame::~MainFrame()
{
    delete m_TaskManager;

    if ( m_FloatWin )
        m_FloatWin->Destroy();

    m_HideWin->Destroy();
}


void MainFrame::InitMenuBar()
{


    wxMenu *fileMenu = new wxMenu;

    wxMenu *taskMenu = new wxMenu;

    wxMenu *viewMenu = new wxMenu;

    wxMenu *setMenu = new wxMenu;

    wxMenu *helpMenu = new wxMenu;

    wxMenuItem *visit =
        new wxMenuItem( helpMenu, mgID_HELP_VISITHOME, _("Visit homepage..."), _( "Visit homepage" ) );
    visit->SetBitmap( wxBitmap( msgsystem_16_xpm ) );
    helpMenu->Append( visit );
	
    wxMenuItem *about =
        new wxMenuItem( helpMenu, wxID_ABOUT, _("About\tCtrl+O"), _( "About program" ) );
    about->SetBitmap( wxBitmap( about_xpm ) );
    helpMenu->Append( about );

    wxMenuItem *option =
        new wxMenuItem( setMenu, mgID_OPTION_CONFIG, _("Settings\tCtrl+S"), _( "sys setting" ) );
    option->SetBitmap( wxBitmap( option_xpm ) );
    setMenu->Append( option );

    wxMenuItem *proxy =
        new wxMenuItem( setMenu, mgID_OPTION_PROXY, _("Proxy Admin\tCtrl+T"), _( "proxy setting" ) );
    proxy->SetBitmap( wxBitmap( proxy_xpm ) );
    setMenu->Append( proxy );

	
    wxMenuItem *saveconfig =
        new wxMenuItem( fileMenu, mgID_FILE_SAVECONFIG, _("Save Config"), _( "Save Config" ) );
    saveconfig->SetBitmap( wxBitmap( save_xpm ) );
    fileMenu->Append( saveconfig );	

    wxMenuItem *savetask =
        new wxMenuItem( fileMenu, mgID_FILE_SAVETASK, _("Save task list"), _( "Save Task" ) );
    savetask->SetBitmap( wxBitmap( save_xpm ) );
    fileMenu->Append( savetask );	
	
    wxMenuItem *saveproxy =
        new wxMenuItem( fileMenu, mgID_FILE_SAVEPROXY, _("Save proxy list"), _( "Save Proxy" ) );
    saveproxy->SetBitmap( wxBitmap( save_xpm ) );
    fileMenu->Append( saveproxy );		

	fileMenu->AppendSeparator();
	
    wxMenuItem *quit =
        new wxMenuItem( fileMenu, wxID_EXIT, _("Quit\tCtrl+Q"), _( "Quit program" ) );
    quit->SetBitmap( wxBitmap( quit_xpm ) );
    fileMenu->Append( quit );	
	
    //view

/*2008-4-15 Disable the Language menu, Wing Sun */
/*
    wxMenu *lang = new wxMenu;
    lang->AppendRadioItem( mgID_VIEW_LANG_EN_UTF8, _( "English UTF-8" ), _( "change to english." ) );
    lang->AppendSeparator();
    lang->AppendRadioItem( mgID_VIEW_LANG_CN_UTF8, _( "Chinese UTF-8" ), _( "change to chinese" ) );
    lang->AppendRadioItem( mgID_VIEW_LANG_GB18030, _( "Chinese GB18030/GB2312/GBK" ), _( "change to chinese" ) );
    lang->AppendSeparator();
    lang->AppendRadioItem( mgID_VIEW_LANG_TW_UTF8, _( "Chinese Traditional zh_TW UTF-8" ), _( "Change to Chinese Traditional zh_TW UTF-8" ) );
    lang->AppendRadioItem( mgID_VIEW_LANG_TW_BIG5, _( "Chinese Traditional zh_TW BIG5" ), _( "Change to Chinese Traditional zh_TW BIG5" ) );
    lang->AppendSeparator();
    lang->AppendRadioItem( mgID_VIEW_LANG_DE_UTF8, _( "German UTF-8" ), _( "change to German" ) );
    lang->AppendSeparator();
    lang->AppendRadioItem( mgID_VIEW_LANG_PT_UTF8, _( "Portuguese UTF-8" ), _( "change to Portuguese" ) );
	m_lang=lang;
	CheckLangMenu( wxGetApp().GetLang() );

    viewMenu->Append( mgID_VIEW_LANG, _("Language"), lang );
    viewMenu->AppendSeparator();
*/

    wxMenu *tool = new wxMenu;
    tool->AppendRadioItem( mgID_VIEW_TOOLBAR_SIZE16, _("16*16 icon"), _( "size 16" ) );
    tool->AppendRadioItem( mgID_VIEW_TOOLBAR_SIZE24, _("24*24 icon"), _( "size 24" ) );
    tool->AppendRadioItem( mgID_VIEW_TOOLBAR_SIZE32, _("32*32 icon"), _( "size 32" ) );
    viewMenu->Append( mgID_VIEW_TOOLBAR, _("ToolBar"), tool );

    switch ( TOOLBAR_SIZE )
    {

        case 16:
        tool->Check( mgID_VIEW_TOOLBAR_SIZE16, true );
        break;

        case 24:
        tool->Check( mgID_VIEW_TOOLBAR_SIZE24, true );
        break;

        case 32:
        tool->Check( mgID_VIEW_TOOLBAR_SIZE32, true );
        break;
    }

    viewMenu->AppendCheckItem( mgID_VIEW_STATUSBAR, _("StatusBar"), _( "StatusBar" ) );
    viewMenu->Check( mgID_VIEW_STATUSBAR, m_bShowStatusBar );

    wxMenuItem *newtask =
        new wxMenuItem( taskMenu, mgID_NEW_TASK, _("New\tCtrl+N"), _( "create a new task." ) );
    newtask->SetBitmap( wxBitmap( new_xpm ) );
    wxMenuItem *starttask =
        new wxMenuItem( taskMenu, mgID_START_TASK, _("Start\tCtrl+G"), _( "start current task." ) );
    starttask->SetBitmap( wxBitmap( run_xpm ) );
    wxMenuItem *pausetask =
        new wxMenuItem( taskMenu, mgID_STOP_TASK, _("Pause\tCtrl+P"), _( "pause current task." ) );
    pausetask->SetBitmap( wxBitmap( stop_xpm ) );
    wxMenuItem *deletetask =
        new wxMenuItem( taskMenu, mgID_CANCEL_TASK, _("Delete\tDel"), _( "delete current task." ) );
    deletetask->SetBitmap( wxBitmap( delete_xpm ) );

    wxMenuItem *moveup =
        new wxMenuItem( taskMenu, mgID_MOVEUP_TASK, _("MoveUp\tCtrl+U"), _( "moveup current task." ) );
    moveup->SetBitmap( wxBitmap( up_xpm ) );
    wxMenuItem *movedown =
        new wxMenuItem( taskMenu, mgID_MOVEDOWN_TASK, _("MoveDown\tCtrl+D"), _( "movedown current task." ) );
    movedown->SetBitmap( wxBitmap( down_xpm ) );

    wxMenuItem *addant =
        new wxMenuItem( taskMenu, mgID_ADDTHREAD, _("+Thread\tCtrl++"), _( "increase a working thread." ) );
    addant->SetBitmap( wxBitmap( plus_xpm ) );
    wxMenuItem *subant =
        new wxMenuItem( taskMenu, mgID_SUBTHREAD, _("-Thread\tCtrl+-"), _( "decrease a working thread." ) );
    subant->SetBitmap( wxBitmap( sub_xpm ) );

    wxMenuItem *reload =
        new wxMenuItem( taskMenu, mgID_RELOAD, _("Redownload"), _( "redownload the task" ) );
    reload->SetBitmap( wxBitmap( reload_xpm ) );

    taskMenu->Append( newtask );
    taskMenu->Append( starttask );
    taskMenu->Append( pausetask );
    taskMenu->Append( deletetask );
    taskMenu->AppendSeparator( );
    taskMenu->Append( moveup );
    taskMenu->Append( movedown );
    taskMenu->AppendSeparator( );
    taskMenu->Append( addant );
    taskMenu->Append( subant );
    taskMenu->AppendSeparator( );
    taskMenu->Append( reload );
    //root menu
    wxMenuBar* menubar = new wxMenuBar( wxMB_DOCKABLE );
    assert( menubar != NULL );
    menubar->Append( fileMenu, _("File(&F)") );
    menubar->Append( taskMenu, _("Task(&T)") );
    menubar->Append( viewMenu, _("View(&V)") );
    menubar->Append( setMenu, _("Option(&P)") );
    menubar->Append( helpMenu, _("Help(&H)") );

	//Under Windows, a size event is generated, so be sure to initialize data members properly before calling SetMenuBar.
    SetMenuBar( menubar );
}

void MainFrame::InitToolBar()
{

    // delete and recreate the toolbar
    long style;

    if ( GetToolBar() != NULL )
    {
        style = GetToolBar() ->GetWindowStyle();
        delete GetToolBar();
        SetToolBar( NULL );

    }
    else
    {
        style = TOOLBAR_STYLE;
    }

    wxToolBar* toolbar = CreateToolBar( style, -1 );


    //tool icon is 32*32
    wxBitmap bmpNew( tool_new_xpm );
    wxBitmap bmpStart( tool_run_xpm );
    wxBitmap bmpStop( tool_pause_xpm );
    wxBitmap bmpCancel( tool_delete_xpm );
    wxBitmap bmpMoveup( tool_up_xpm );
    wxBitmap bmpMovedown( tool_down_xpm );
    wxBitmap bmpAddAnts( tool_plus_xpm );
    wxBitmap bmpSubAnts( tool_sub_xpm );
    wxBitmap bmpReload( tool_reload_xpm );
    wxBitmap bmpOption( tool_option_xpm );
    wxBitmap bmpProxy( tool_proxy_xpm );
    wxBitmap bmpProperty( tool_property_xpm );


    if ( TOOLBAR_SIZE != 32 )
    {
        bmpNew = wxBitmap( bmpNew.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpStart = wxBitmap( bmpStart.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpStop = wxBitmap( bmpStop.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpCancel = wxBitmap( bmpCancel.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpMoveup = wxBitmap( bmpMoveup.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpMovedown = wxBitmap( bmpMovedown.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpAddAnts = wxBitmap( bmpAddAnts.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpSubAnts = wxBitmap( bmpSubAnts.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpReload = wxBitmap( bmpReload.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpOption = wxBitmap( bmpOption.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpProxy = wxBitmap( bmpProxy.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );
        bmpProperty = wxBitmap( bmpProperty.ConvertToImage().Scale( TOOLBAR_SIZE, TOOLBAR_SIZE ) );

    }

    /*
    	_S_TIP_NEWTASK,
    	_S_TIP_RUNTASK,
    	_S_TIP_STOPTASK,
    	_S_TIP_DELETETASK,
    	_S_TIP_MOVEUPTASK,
    	_S_TIP_MOVEDOWNTASK,
    	_S_TIP_ADDTASKTHREAD,
    	_S_TIP_SUBTASKTHREAD,
    	_S_TIP_RELOADTASK,
    	_S_TIP_SYSCONFIG,
    	_S_TIP_PROXYADMIN,
    */
    toolbar->SetMargins( 0, 0 );
	toolbar->SetToolBitmapSize(wxSize(TOOLBAR_SIZE, TOOLBAR_SIZE));

    toolbar->AddTool( mgID_NEW_TASK,
                      _("New\tCtrl+N"),
                      bmpNew,
                      _("New task") );

    toolbar->AddTool( mgID_START_TASK,
                      _("Start\tCtrl+G"),
                      bmpStart,
                      _("Run task") );

    toolbar->AddTool( mgID_STOP_TASK,
                      _("Pause\tCtrl+P"),
                      bmpStop,
                      _("Stop task") );

    toolbar->AddTool( mgID_CANCEL_TASK,
                      _("Delete\tDel"),
                      bmpCancel,
                      _("Delete task") );

    toolbar->AddSeparator();

    toolbar->AddTool( mgID_MOVEUP_TASK,
                      _("MoveUp\tCtrl+U"),
                      bmpMoveup,
                      _("Move up task") );

    toolbar->AddTool( mgID_MOVEDOWN_TASK,
                      _("MoveDown\tCtrl+D"),
                      bmpMovedown,
                      _("Move down task") );

    toolbar->AddSeparator();

    toolbar->AddTool( mgID_ADDTHREAD,
                      _("+Thread\tCtrl++"),
                      bmpAddAnts,
                      _("Increase a session") );

    toolbar->AddTool( mgID_SUBTHREAD,
                      _("-Thread\tCtrl+-"),
                      bmpSubAnts,
                      _("Reduce a session") );

    toolbar->AddSeparator();

    toolbar->AddTool( mgID_RELOAD,
                      _("Redownload"),
                      bmpReload,
                      _("Redownload task") );

    toolbar->AddSeparator();

    toolbar->AddTool( mgID_OPTION_CONFIG,
                      _("Settings\tCtrl+S"),
                      bmpOption,
                      _("System configure") );

    toolbar->AddTool( mgID_OPTION_PROXY,
                      _("Proxy Admin\tCtrl+T"),
                      bmpProxy,
                      _("Proxy Administrator") );

    toolbar->AddSeparator();

    toolbar->AddTool( mgID_OPTION_PROPERTY,
                      _("Properties"),
                      bmpProperty,
                      _("Show/Modify task properties") );

    toolbar->Realize();

    SetToolBar( toolbar );
}

void MainFrame::InitSpliter()
{

    m_vspliter = new wxSplitterWindow(
                     this,
                     wxID_ANY,
                     wxDefaultPosition,
                     wxDefaultSize,
#ifdef WIN32
					wxNO_BORDER,
#else
					wxSP_LIVE_UPDATE | wxSP_BORDER,
#endif
                    _( "ms" )
                 );
    assert( m_vspliter != NULL );
    m_vspliter->SetMinimumPaneSize( 150 );

    m_hspliter = new wxSplitterWindow(
                     m_vspliter,
                     wxID_ANY,
                     wxDefaultPosition,
                     wxDefaultSize,
#ifdef WIN32
					wxNO_BORDER,
#else
                     wxSP_LIVE_UPDATE | wxSP_BORDER /*|wxSP_PERMIT_UNSPLIT*/,
#endif
                     _( "rs" )
                 );


    assert( m_hspliter != NULL );
    m_hspliter->SetMinimumPaneSize( 100 );
    m_vsubspliter = new CRightDownSplitter( m_hspliter );


    assert( m_vsubspliter != NULL );
    m_lefttree = new CLeftTree( m_vspliter );


    assert( m_lefttree != NULL );
    m_tasklist = new CTaskList( m_hspliter );


    assert( m_tasklist != NULL );

    m_hspliter->SetSashGravity( 0.5 );
    m_vspliter->SetSashGravity( 0.1 );

	
    m_vspliter->SplitVertically( m_lefttree, m_hspliter, 150 );
    m_hspliter->SplitHorizontally( m_tasklist, m_vsubspliter, 200 );
}

void MainFrame::OnNewTask( wxCommandEvent& event )
{
    DoNewTask( _( "" ) );
}

void MainFrame::OnStartTask( wxCommandEvent& event )
{

    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {

        if ( it->nID == m_nCurrentSelectTaskID )
        {
            it->nStatus = _STATUS_WAITING;
            ScheduleTask();
            UpdateToolBar();
            UpdateTaskList();
            break ;
        }
    }

}

void MainFrame::OnDecreaseThread( wxCommandEvent& event )
{
    if ( m_nCurrentSelectTaskID < 0 )
        return ;

    _TaskAttr* pcur = GetCurrentTask();

    if ( pcur == NULL )
        return ;

    if ( pcur->nStatus == _STATUS_RUNNING )
    {
        if ( m_TaskManager->GetTaskStatus( m_nCurrentSelectTaskID ) != _TASK_NONE )
        {
            if ( m_TaskManager->ThreadDown( m_nCurrentSelectTaskID ) )
            {
                if ( pcur->nThread > 1 )
                {
                    pcur->nThread--;
                    UpdateTaskList();
                    UpdateRightDown();
                }
            }
        }
    }
    else if (
        pcur->nStatus == _STATUS_WAITING ||
        pcur->nStatus == _STATUS_WRONG ||
        pcur->nStatus == _STATUS_PAUSE )
    {
        if ( pcur->nThread > 1 )
        {
            pcur->nThread--;
            UpdateTaskList();
            UpdateRightDown();
        }
    }
    else if ( pcur->nStatus == _STATUS_DELETE )
    {
        if ( pcur->nThread > 1 )
        {
            pcur->nThread--;
            UpdateTaskList();
            UpdateRightDown();
        }
    }
    else //finish or delete
        return ;
}

void MainFrame::OnIncreaseThread( wxCommandEvent& event )
{

    _TaskAttr* pcur = GetCurrentTask();

    if ( pcur == NULL )
        return ;

    if ( pcur->nStatus == _STATUS_RUNNING )
    {
        if ( m_TaskManager->GetTaskStatus( m_nCurrentSelectTaskID ) != _TASK_NONE )
        {
            if ( m_TaskManager->ThreadUp( m_nCurrentSelectTaskID ) )
            {
                if ( pcur->nThread < 10 )
                {
					pthread_mutex_lock( &m_TaskMutex );
                    pcur->nThread++;

                    UpdateTaskList();
					pthread_mutex_unlock( &m_TaskMutex );

                    UpdateRightDown();


                }
            }
        }
    }
    else if (
        pcur->nStatus == _STATUS_WAITING ||
        pcur->nStatus == _STATUS_WRONG ||
        pcur->nStatus == _STATUS_PAUSE )
    {
        if ( pcur->nThread < 10 )
        {
			pthread_mutex_lock( &m_TaskMutex );
            pcur->nThread++;
            UpdateTaskList();
			pthread_mutex_unlock( &m_TaskMutex );
            UpdateRightDown();

        }
    }

    else if ( pcur->nStatus == _STATUS_DELETE )
    {
        if ( pcur->nThread < 10 )
        {
			pthread_mutex_lock( &m_TaskMutex );
            pcur->nThread++;
            UpdateTaskList();
			pthread_mutex_unlock( &m_TaskMutex );
            UpdateRightDown();

        }
    }
    else //finish
    {
        return ;
    }
}

void MainFrame::OnCancelTask( wxCommandEvent& event )
{


    if ( m_nCurrentSelectTaskID < 0 )
    {
        return ;
    }

    _TaskAttr* pcur = GetCurrentTask();

    if ( pcur == NULL )
    {
        return ;
    }


    if ( pcur->nStatus == _STATUS_DELETE )
    { //delete all file


        /* comfirm
              wxString msg;
              wxString url( pcur->sURL.c_str(), wxConvLocal );
              msg = _MGSTR(_S_MAINFRAME_DELETESELECT) + _("\n") + url;
              wxMessageDialog dlg( this, msg, _MGSTR( _S_MAINFRAME_DELETEWARNNING ),
                                   wxICON_QUESTION | wxYES_NO | wxSTAY_ON_TOP | wxNO_DEFAULT );

              if ( wxID_NO == dlg.ShowModal() )
              {
                  return ;
              }
		*/


        std::string tempdir;

        if ( GetTempDir( tempdir ) )
        {
            std::string tempfile = tempdir;
            tempfile += "/";
            tempfile += pcur->sName;
            tempfile += ".mg";
            remove( tempfile.c_str() );
            tempfile += "idx";
            remove( tempfile.c_str() );
        }


        std::string destfile = pcur->sSavePath;
        destfile += "/";
        destfile += pcur->sName;
        remove( destfile.c_str() );

        DeleteTaskFromTaskList( m_nCurrentSelectTaskID );

        m_nCurrentSelectTaskID = -1;

        UpdateTaskList();

        UpdateToolBar();

        UpdateRightDown();

    }
    else
    {

        StopTask( m_nCurrentSelectTaskID );
        pcur->nStatus = _STATUS_DELETE;
        UpdateTaskList();
        UpdateToolBar();
        UpdateRightDown();
    }

}

void MainFrame::OnConfig( wxCommandEvent& event )
{

    CConfigWindow conf( this );
    m_SysTray->DlgShow( true );
    int nret = conf.ShowModal();
    m_SysTray->DlgShow( false );

    if ( wxID_OK == nret )
    {

        gRunTaskNum = conf.m_nTask;
        gConnectTimeOut = conf.m_nConnectTimeOut;
        gReceiveTimeOut = conf.m_nTimeOut;
        gTaskDefThread = conf.m_nThread;
        gRetryTime = conf.m_nRetry;
        gRetryWait = conf.m_nRetryWait;
        gDefSavePath = conf.m_sSavePath.mb_str(); //
        gDefFtpPass = conf.m_sPass.mb_str(); //
        gsIgnoreExt = conf.m_sIgnoreExt.mb_str(); //
        gsMonitorExt = conf.m_sMonitorExt.mb_str(); //
        gbIgnoreExt = conf.m_bIgnoreExt; //
        gbMonitorExt = conf.m_bMonitorExt;
        gSpeedMode = ( _SPEED_MODE ) ( conf.m_SpeedMode );
        gbSoundTip = conf.m_bSound;
        gbAutoMd5 = conf.m_bAutoMd5;

    }

}


void MainFrame::StopTask( int taskid )
{
    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {

        if ( it->nID == taskid )
        {

            if ( it->nStatus == _STATUS_WAITING )
            {

                it->nStatus = _STATUS_PAUSE;
                return ;
            }
            else if ( it->nStatus == _STATUS_RUNNING )
            {

                if ( m_TaskManager->GetTaskStatus( taskid ) != _TASK_NONE )
                {
                    m_TaskManager->CloseUpdateUI( taskid );
                    m_TaskManager->RemoveTask( taskid );

                }

                it->nStatus = _STATUS_PAUSE;
            }
        }
    }
}

void MainFrame::OnStopTask( wxCommandEvent& event )
{
    StopTask( m_nCurrentSelectTaskID );
    ScheduleTask();
    UpdateRightDown();
    UpdateToolBar();
}

void MainFrame::DeleteTaskFromTaskList( int taskid )
{
    vector<_TaskAttr>::iterator it;
    pthread_mutex_lock( &m_TaskMutex );

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nID == taskid )
        {

            m_AllTaskList.erase( it );

            break;
        }
    }

    pthread_mutex_unlock( &m_TaskMutex );
}


void MainFrame::UpdateToolBar()
{
    wxToolBar * toolbar = GetToolBar();

    if ( toolbar == NULL )
        return ;

    wxMenuBar* menubar = GetMenuBar();

    if ( menubar == NULL )
        return ;

    if ( m_nCurrentSelectTaskID < 0 )
    {

        toolbar->EnableTool( mgID_NEW_TASK, true );
        toolbar->EnableTool( mgID_START_TASK, false );
        toolbar->EnableTool( mgID_STOP_TASK, false );
        toolbar->EnableTool( mgID_CANCEL_TASK, false );
        toolbar->EnableTool( mgID_MOVEUP_TASK, false );
        toolbar->EnableTool( mgID_MOVEDOWN_TASK, false );
        toolbar->EnableTool( mgID_SUBTHREAD, false );
        toolbar->EnableTool( mgID_ADDTHREAD, false );
        toolbar->EnableTool( mgID_RELOAD, false );
        toolbar->EnableTool( mgID_OPTION_PROPERTY, false );

        menubar->Enable( mgID_NEW_TASK, true );
        menubar->Enable( mgID_START_TASK, false );
        menubar->Enable( mgID_STOP_TASK, false );
        menubar->Enable( mgID_CANCEL_TASK, false );
        menubar->Enable( mgID_MOVEUP_TASK, false );
        menubar->Enable( mgID_MOVEDOWN_TASK, false );
        menubar->Enable( mgID_SUBTHREAD, false );
        menubar->Enable( mgID_ADDTHREAD, false );
        menubar->Enable( mgID_RELOAD, false );

    }
    else
    {

        vector<_TaskAttr>::iterator it;

        for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
        {
            if ( it->nID == m_nCurrentSelectTaskID )
            {

                switch ( it->nStatus )
                {

                    case _STATUS_RUNNING:

                    case _STATUS_WAITING:
                    toolbar->EnableTool( mgID_NEW_TASK, true );
                    toolbar->EnableTool( mgID_START_TASK, false );
                    toolbar->EnableTool( mgID_STOP_TASK, true );
                    toolbar->EnableTool( mgID_CANCEL_TASK, true );
                    toolbar->EnableTool( mgID_SUBTHREAD, true );
                    toolbar->EnableTool( mgID_ADDTHREAD, true );
                    toolbar->EnableTool( mgID_RELOAD, false );

                    menubar->Enable( mgID_NEW_TASK, true );
                    menubar->Enable( mgID_START_TASK, false );
                    menubar->Enable( mgID_STOP_TASK, true );
                    menubar->Enable( mgID_CANCEL_TASK, true );
                    menubar->Enable( mgID_SUBTHREAD, true );
                    menubar->Enable( mgID_ADDTHREAD, true );
                    menubar->Enable( mgID_RELOAD, false );

                    bool canup, candown;
                    m_tasklist->IsTaskCanMove( m_nCurrentSelectTaskID, canup, candown );
                    toolbar->EnableTool( mgID_MOVEUP_TASK, canup );
                    toolbar->EnableTool( mgID_MOVEDOWN_TASK, candown );

                    menubar->Enable( mgID_MOVEUP_TASK, canup );
                    menubar->Enable( mgID_MOVEDOWN_TASK, candown );
                    toolbar->EnableTool( mgID_OPTION_PROPERTY, true );
                    break;

                    case _STATUS_PAUSE:

                    case _STATUS_WRONG:
                    toolbar->EnableTool( mgID_NEW_TASK, true );
                    toolbar->EnableTool( mgID_START_TASK, true );
                    toolbar->EnableTool( mgID_STOP_TASK, false );
                    toolbar->EnableTool( mgID_CANCEL_TASK, true );
                    toolbar->EnableTool( mgID_SUBTHREAD, true );
                    toolbar->EnableTool( mgID_ADDTHREAD, true );
                    toolbar->EnableTool( mgID_MOVEUP_TASK, false );
                    toolbar->EnableTool( mgID_MOVEDOWN_TASK, false );
                    toolbar->EnableTool( mgID_RELOAD, true );
                    toolbar->EnableTool( mgID_OPTION_PROPERTY, true );

                    menubar->Enable( mgID_NEW_TASK, true );
                    menubar->Enable( mgID_START_TASK, true );
                    menubar->Enable( mgID_STOP_TASK, false );
                    menubar->Enable( mgID_CANCEL_TASK, true );
                    menubar->Enable( mgID_SUBTHREAD, true );
                    menubar->Enable( mgID_ADDTHREAD, true );
                    menubar->Enable( mgID_MOVEUP_TASK, false );
                    menubar->Enable( mgID_MOVEDOWN_TASK, false );
                    menubar->Enable( mgID_RELOAD, true );

                    break;

                    case _STATUS_FINISH_SOFT:

                    case _STATUS_FINISH_PACK:

                    case _STATUS_FINISH_MUSIC:

                    case _STATUS_FINISH_ISO:

                    case _STATUS_FINISH_MOVIE:

                    case _STATUS_FINISH_PUBLISH:

                    case _STATUS_FINISH_PICTURE:

                    case _STATUS_FINISH_UNKNOW:
                    toolbar->EnableTool( mgID_NEW_TASK, true );
                    toolbar->EnableTool( mgID_START_TASK, false );
                    toolbar->EnableTool( mgID_STOP_TASK, false );
                    toolbar->EnableTool( mgID_CANCEL_TASK, true );
                    toolbar->EnableTool( mgID_SUBTHREAD, false );
                    toolbar->EnableTool( mgID_ADDTHREAD, false );
                    toolbar->EnableTool( mgID_MOVEUP_TASK, false );
                    toolbar->EnableTool( mgID_MOVEDOWN_TASK, false );
                    toolbar->EnableTool( mgID_RELOAD, true );

                    menubar->Enable( mgID_NEW_TASK, true );
                    menubar->Enable( mgID_START_TASK, false );
                    menubar->Enable( mgID_STOP_TASK, false );
                    menubar->Enable( mgID_CANCEL_TASK, true );
                    menubar->Enable( mgID_SUBTHREAD, false );
                    menubar->Enable( mgID_ADDTHREAD, false );
                    menubar->Enable( mgID_MOVEUP_TASK, false );
                    menubar->Enable( mgID_MOVEDOWN_TASK, false );
                    menubar->Enable( mgID_RELOAD, true );

                    toolbar->EnableTool( mgID_OPTION_PROPERTY, true );
                    break;

                    case _STATUS_DELETE:

                    default:
                    toolbar->EnableTool( mgID_NEW_TASK, true );
                    toolbar->EnableTool( mgID_START_TASK, true );
                    toolbar->EnableTool( mgID_STOP_TASK, false );
                    toolbar->EnableTool( mgID_CANCEL_TASK, true );
                    toolbar->EnableTool( mgID_SUBTHREAD, true );
                    toolbar->EnableTool( mgID_ADDTHREAD, true );
                    toolbar->EnableTool( mgID_MOVEUP_TASK, false );
                    toolbar->EnableTool( mgID_MOVEDOWN_TASK, false );
                    toolbar->EnableTool( mgID_RELOAD, false );

                    menubar->Enable( mgID_NEW_TASK, true );
                    menubar->Enable( mgID_START_TASK, true );
                    menubar->Enable( mgID_STOP_TASK, false );
                    menubar->Enable( mgID_CANCEL_TASK, true );
                    menubar->Enable( mgID_SUBTHREAD, true );
                    menubar->Enable( mgID_ADDTHREAD, true );
                    menubar->Enable( mgID_MOVEUP_TASK, false );
                    menubar->Enable( mgID_MOVEDOWN_TASK, false );
                    menubar->Enable( mgID_RELOAD, false );
                    toolbar->EnableTool( mgID_OPTION_PROPERTY, true );

                    break;

                } //switch

                break;
            } //if
        } //for
    } //else

}

int MainFrame::GetNewTaskID()
{
    return ++m_nTaskID;
}

void MainFrame::OnAbout( wxCommandEvent& event )
{
/*
    wxString msg;
    msg = _MGSTR( _S_INFO_WELCOME );
    msg += _( "\n\n" );
    msg += _MGSTR( _S_COPYRIGHT );
    msg += _( "\n\n" );
    msg += _MGSTR( _S_VERSION );
    msg += _( "\n\n" );
    msg += _MGSTR( _S_ABOUT_BUGREPORT );
    msg += _( "\n\n" );
    msg += _MGSTR( _S_ABOUT_MAINPUBLISH );
    msg += _( "\n\n" );
	msg += _( "----------------------------------------\n");
	msg += _MGSTR( _S_THANKS_CJACKER );
    msg += _( "\n" );
	msg += _MGSTR( _S_THANKS_GUOHONGTAO );
    msg += _( "\n" );
	msg += _MGSTR( _S_THANKS_LANGUE );
    msg += _( "\n" );
	msg += _MGSTR( _S_THANKS_MATTHE );
    msg += _( "\n" );
	msg += _MGSTR( _S_THANKS_ABEL );
    msg += _( "\n" );
	msg += _MGSTR( _S_THANKS_ISJFK );
    msg += _( "\n" );
	msg += _MGSTR( _S_THANKS_BRYAN );
    msg += _( "\n" );	
	msg += _MGSTR( _S_THANKS_MICHAEL_FRASE );
    msg += _( "\n" );	
	msg += _MGSTR( _S_THANKS_JOSE_OSWALDO );
    msg += _( "\n" );		
	msg += _( "----------------------------------------\n\n");


    //msg += _MGSTR( _S_ABOUT_ANNOUNCE );

    m_SysTray->DlgShow( true );
    ::wxMessageBox( msg, _MGSTR( _S_ABOUT_MULTIGET ),
                    wxOK | wxICON_INFORMATION, this );

    m_SysTray->DlgShow( false );
*/
	wxAboutDialogInfo info;
	info.SetName(_("multiget"));
	info.AddDeveloper(_("liubin (multiget@gmail.com)"));
	info.SetDescription(_("This is a multi-thread http/ftp file downloader."));
	info.SetVersion(_("v1.2.0"));
	info.SetLicence(_("GPLv2"));
	info.SetWebSite(_("http://multiget.sourceforge.net"));

//sorry! I can't remember all the translators.
	info.AddTranslator(_("Jose Oswaldo"));
	info.AddTranslator(_("Michael Frase"));
	info.AddTranslator(_("abel"));

	//wxBitmap baby(baby_xpm);
	wxIcon bicon;
	bicon.CopyFromBitmap(wxBitmap(baby_xpm));
	info.SetIcon(bicon);
	info.SetCopyright(_("liubin, China"));
	::wxAboutBox(info);
}

void MainFrame::OnQuit( wxCommandEvent& event )
{
//really quit?
	Hide();
    Close();
    return ;

}


void MainFrame::OnSize( wxSizeEvent& event )
{
    int w, h;
    GetClientSize( &w, &h );

	if(m_MeterBar)    m_MeterBar->SetSize( 0, 0, w, 30 );
    if(m_vspliter)	  m_vspliter->SetSize( 0, 31, w, h - 30 );

//    m_nWinSizeW = w;
//    m_nWinSizeH = h;
}


void MainFrame::OnSelectType( _TASK_TYPE type )
{
    switch ( type )
    {

        case _STATUS_ROOT:
        m_nCurrentShowType = ( _STATUS_ROOT );
        break;

        case _STATUS_RUNNING:
        m_nCurrentShowType = ( _STATUS_RUNNING );
        break;

        case _STATUS_PAUSE:
        m_nCurrentShowType = ( _STATUS_PAUSE );
        break;

        case _STATUS_WRONG:
        m_nCurrentShowType = ( _STATUS_WRONG );
        break;

        case _STATUS_FINISH:
        m_nCurrentShowType = ( _STATUS_FINISH );
        break;

        case _STATUS_FINISH_SOFT:
        m_nCurrentShowType = ( _STATUS_FINISH_SOFT );
        break;

        case _STATUS_FINISH_PACK:
        m_nCurrentShowType = ( _STATUS_FINISH_PACK );
        break;

        case _STATUS_FINISH_MUSIC:
        m_nCurrentShowType = ( _STATUS_FINISH_MUSIC );
        break;

        case _STATUS_FINISH_ISO:
        m_nCurrentShowType = ( _STATUS_FINISH_ISO );
        break;

        case _STATUS_FINISH_MOVIE:
        m_nCurrentShowType = ( _STATUS_FINISH_MOVIE );
        break;

        case _STATUS_FINISH_PUBLISH:
        m_nCurrentShowType = ( _STATUS_FINISH_PUBLISH );
        break;

        case _STATUS_FINISH_PICTURE:
        m_nCurrentShowType = ( _STATUS_FINISH_PICTURE );
        break;

        case _STATUS_FINISH_UNKNOW:
        m_nCurrentShowType = ( _STATUS_FINISH_UNKNOW );
        break;

        case _STATUS_DELETE:
        m_nCurrentShowType = ( _STATUS_DELETE );
        break;

        default:
        m_nCurrentShowType = ( _STATUS_ROOT );
        break;
    }

    UpdateTaskList();
}


void MainFrame::OnSelectTask( int taskid )
{

    m_TaskManager->CloseUpdateUI( m_nCurrentSelectTaskID );
    m_nCurrentSelectTaskID = taskid;

    UpdateRightDown();
    UpdateToolBar();


    _TaskAttr *task = GetCurrentTask();

    if ( task != NULL && task->nStatus == _STATUS_RUNNING )
    {
        m_TaskManager->UpdateRunningTable( task->nID, m_vsubspliter->GetShowTableWin() );
    }

    return ;
}


void MainFrame::UpdateRightDown()
{
    m_vsubspliter->ShowTask( GetCurrentTask() );
}


void MainFrame::UpdateTaskList()
{

    m_tasklist->DeleteAllItems();

    if ( m_nCurrentShowType == _STATUS_ROOT )
    {
        return ;
    }


    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( m_nCurrentShowType == _STATUS_FINISH )
        {

            if (
                it->nStatus == _STATUS_FINISH_SOFT ||
                it->nStatus == _STATUS_FINISH_PACK ||
                it->nStatus == _STATUS_FINISH_MUSIC ||
                it->nStatus == _STATUS_FINISH_ISO	||
                it->nStatus == _STATUS_FINISH_MOVIE ||
                it->nStatus == _STATUS_FINISH_PUBLISH ||
                it->nStatus == _STATUS_FINISH_PICTURE ||
                it->nStatus == _STATUS_FINISH_UNKNOW
            )
            {
                m_tasklist->AddTaskList( *it );
            }

        }
        else if ( m_nCurrentShowType == _STATUS_RUNNING )
        {

            if ( it->nStatus == _STATUS_RUNNING || it->nStatus == _STATUS_WAITING )
            {
                m_tasklist->AddTaskList( *it );
            }

        }
        else
        {
            if ( it->nStatus == m_nCurrentShowType )
            {
                m_tasklist->AddTaskList( *it );
            }
        }
    }


    int stask = m_tasklist->SelectTask( m_nCurrentSelectTaskID );

    if ( stask != m_nCurrentSelectTaskID )
    {
        m_nCurrentSelectTaskID = stask;
        UpdateRightDown();
    }
}


void MainFrame::ScheduleTask()
{
    vector<_TaskAttr>::iterator it;


    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nStatus == _STATUS_WAITING )
        {


            if ( _TASK_NONE == m_TaskManager->GetTaskStatus( it->nID ) &&
                    m_TaskManager->GetTaskNumber() < gRunTaskNum )

            {

                it->nStatus = _STATUS_RUNNING;
                m_TaskManager->AddTask( *it );
            }
        }
    }


    UpdateTaskList();

    UpdateStatusBar();

}

_TaskAttr* MainFrame::GetCurrentTask()
{
    return GetTask( m_nCurrentSelectTaskID );
}

_TaskAttr* MainFrame::GetTask( int taskid )
{
    if ( taskid < 0 )
        return NULL;

    for ( int i = 0;i < int( m_AllTaskList.size() );i++ )
    {
        if ( m_AllTaskList[ i ].nID == taskid )
            return & m_AllTaskList[ i ];
    }

    return NULL;
}


int MainFrame::AddNewTask( string url, int threadnum, bool autorun )
{

    if ( IsUrlExist( url ) )
    {
        m_SysTray->DlgShow( true );
        //wxMessageBox( "url exists" );
		wxMessageBox(wxString::FromAscii("url exists")); //unicode patch
        m_SysTray->DlgShow( false );
		return 0;
    }


    _TaskAttr atask;
    atask.nID = GetNewTaskID();
    atask.nThread = ( ( threadnum <= 0 || threadnum > 10 ) ? gTaskDefThread : threadnum );

    if ( !autorun )
    {
        atask.nStatus = _STATUS_PAUSE;
    }
    else
    {
        atask.nStatus = _STATUS_WAITING;
    }

    atask.sName = GetTempFileName( url );
    atask.sURL = url;
    pthread_mutex_lock( &m_TaskMutex );
    m_AllTaskList.push_back( atask );
    pthread_mutex_unlock( &m_TaskMutex );

    ScheduleTask();
    return atask.nID;
}

string MainFrame::GetTempFileName( string url )
{
    //no path file name
    char fn[ 512 ];
    strcpy( fn, url.c_str() );
    int i = strlen( fn );

    while ( fn[ i ] != '/' && i > 0 )
        i--;

    return string( fn + i + 1 );

}


void MainFrame::UpdateTaskList( int ntask )
{
    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nID == ntask )
        {
            m_tasklist->UpdateTaskList( *it );
            break;
        }
    }

}

//should change to static
void MainFrame::InitFileTypeList()
{
    //soft
    //.exe;.bin;.bat;.com;.dll;.so;.scr;.ttf;.ap;.app;.drv;
    m_FileTypeList[ 0 ].push_back( string( ".exe" ) );
    m_FileTypeList[ 0 ].push_back( string( ".bin" ) );
    m_FileTypeList[ 0 ].push_back( string( ".bat" ) );
    m_FileTypeList[ 0 ].push_back( string( ".com" ) );
    m_FileTypeList[ 0 ].push_back( string( ".dll" ) );
    m_FileTypeList[ 0 ].push_back( string( ".so" ) );
    m_FileTypeList[ 0 ].push_back( string( ".scr" ) );
    m_FileTypeList[ 0 ].push_back( string( ".ttf" ) );
    m_FileTypeList[ 0 ].push_back( string( ".ap" ) );
    m_FileTypeList[ 0 ].push_back( string( ".app" ) );
    m_FileTypeList[ 0 ].push_back( string( ".drv" ) );

    m_FileTypeList[ 0 ].resize( m_FileTypeList[ 0 ].size() );
    //pack
    //.gz;.tar;.zip;.rar;.bz2;.cab;.arj;.ace;.arc;.jar;.tgz;.lzh;
    m_FileTypeList[ 1 ].push_back( string( ".gz" ) );
    m_FileTypeList[ 1 ].push_back( string( ".tar" ) );
    m_FileTypeList[ 1 ].push_back( string( ".zip" ) );
    m_FileTypeList[ 1 ].push_back( string( ".rar" ) );
    m_FileTypeList[ 1 ].push_back( string( ".bz2" ) );
    m_FileTypeList[ 1 ].push_back( string( ".cab" ) );
    m_FileTypeList[ 1 ].push_back( string( ".arj" ) );
    m_FileTypeList[ 1 ].push_back( string( ".ace" ) );
    m_FileTypeList[ 1 ].push_back( string( ".arc" ) );
    m_FileTypeList[ 1 ].push_back( string( ".jar" ) );
    m_FileTypeList[ 1 ].push_back( string( ".tgz" ) );
    m_FileTypeList[ 1 ].push_back( string( ".lzh" ) );
    m_FileTypeList[ 1 ].push_back( string( ".7z" ) );
    m_FileTypeList[ 1 ].push_back( string( ".rpm" ) );
    m_FileTypeList[ 1 ].push_back( string( ".deb" ) );
    m_FileTypeList[ 1 ].resize( m_FileTypeList[ 1 ].size() );
    //MUSIC
    //.wav;.mp3;.wma;.ogg;.mid;.aif;.ra;.ram;.snd;.aiff;.aif;.svx;.voc;
    m_FileTypeList[ 2 ].push_back( string( ".wav" ) );
    m_FileTypeList[ 2 ].push_back( string( ".mp3" ) );
    m_FileTypeList[ 2 ].push_back( string( ".wma" ) );
    m_FileTypeList[ 2 ].push_back( string( ".ogg" ) );
    m_FileTypeList[ 2 ].push_back( string( ".mid" ) );
    m_FileTypeList[ 2 ].push_back( string( ".aif" ) );
    m_FileTypeList[ 2 ].push_back( string( ".ra" ) );
    m_FileTypeList[ 2 ].push_back( string( ".ram" ) );
    m_FileTypeList[ 2 ].push_back( string( ".snd" ) );
    m_FileTypeList[ 2 ].push_back( string( ".aiff" ) );
    m_FileTypeList[ 2 ].push_back( string( ".aif" ) );
    m_FileTypeList[ 2 ].push_back( string( ".svx" ) );
    m_FileTypeList[ 2 ].push_back( string( ".voc" ) );

    m_FileTypeList[ 2 ].resize( m_FileTypeList[ 2 ].size() );
    //ISO
    m_FileTypeList[ 3 ].push_back( string( ".iso" ) );
    m_FileTypeList[ 3 ].push_back( string( ".img" ) );
    m_FileTypeList[ 3 ].push_back( string( ".nrg" ) );
    m_FileTypeList[ 3 ].push_back( string( ".cue" ) );

    m_FileTypeList[ 3 ].resize( m_FileTypeList[ 3 ].size() );
    //MOVIE
    //.rm;.rmvb;.avi;.mov;.mpg;.asf;.wmv;.vod;.mpeg;.qt;.swf;
    m_FileTypeList[ 4 ].push_back( string( ".rm" ) );
    m_FileTypeList[ 4 ].push_back( string( ".rmvb" ) );
    m_FileTypeList[ 4 ].push_back( string( ".avi" ) );
    m_FileTypeList[ 4 ].push_back( string( ".mov" ) );
    m_FileTypeList[ 4 ].push_back( string( ".mpg" ) );
    m_FileTypeList[ 4 ].push_back( string( ".asf" ) );
    m_FileTypeList[ 4 ].push_back( string( ".wmv" ) );
    m_FileTypeList[ 4 ].push_back( string( ".vod" ) );
    m_FileTypeList[ 4 ].push_back( string( ".mpeg" ) );
    m_FileTypeList[ 4 ].push_back( string( ".qt" ) );
    m_FileTypeList[ 4 ].push_back( string( ".swf" ) );

    m_FileTypeList[ 4 ].resize( m_FileTypeList[ 4 ].size() );
    //PUBLISH
    //.pdf;.ps;.chm;.doc;.xls;.txt;.cad;.cpp;.c;.cxx;.h;.hxx;.dwg;.hlp;.rtf;.htm;
    m_FileTypeList[ 5 ].push_back( string( ".pdf" ) );
    m_FileTypeList[ 5 ].push_back( string( ".ps" ) );
    m_FileTypeList[ 5 ].push_back( string( ".chm" ) );
    m_FileTypeList[ 5 ].push_back( string( ".doc" ) );
    m_FileTypeList[ 5 ].push_back( string( ".xls" ) );
    m_FileTypeList[ 5 ].push_back( string( ".txt" ) );
    m_FileTypeList[ 5 ].push_back( string( ".cad" ) );
    m_FileTypeList[ 5 ].push_back( string( ".cpp" ) );
    m_FileTypeList[ 5 ].push_back( string( ".c" ) );
    m_FileTypeList[ 5 ].push_back( string( ".cxx" ) );
    m_FileTypeList[ 5 ].push_back( string( ".h" ) );
    m_FileTypeList[ 5 ].push_back( string( ".hxx" ) );
    m_FileTypeList[ 5 ].push_back( string( ".dwg" ) );
    m_FileTypeList[ 5 ].push_back( string( ".hlp" ) );
    m_FileTypeList[ 5 ].push_back( string( ".rtf" ) );
    m_FileTypeList[ 5 ].push_back( string( ".htm" ) );

    m_FileTypeList[ 5 ].resize( m_FileTypeList[ 5 ].size() );
    //PICTURE
    //.jpg;.jpeg;.png;.gif;.bmp;.xcf;.pix;.fli;.flc;.gcm;.g3;.xjt;.jpe;.ico;
    //.wmf;.psd;.psp;.tga;.tif;.tiff;.xpm;.xbm;.icon;.pcx;
    m_FileTypeList[ 6 ].push_back( string( ".jpg" ) );
    m_FileTypeList[ 6 ].push_back( string( ".jpeg" ) );
    m_FileTypeList[ 6 ].push_back( string( ".png" ) );
    m_FileTypeList[ 6 ].push_back( string( ".gif" ) );
    m_FileTypeList[ 6 ].push_back( string( ".bmp" ) );
    m_FileTypeList[ 6 ].push_back( string( ".xcf" ) );
    m_FileTypeList[ 6 ].push_back( string( ".pix" ) );
    m_FileTypeList[ 6 ].push_back( string( ".fli" ) );
    m_FileTypeList[ 6 ].push_back( string( ".flc" ) );
    m_FileTypeList[ 6 ].push_back( string( ".gcm" ) );
    m_FileTypeList[ 6 ].push_back( string( ".g3" ) );
    m_FileTypeList[ 6 ].push_back( string( ".xjt" ) );
    m_FileTypeList[ 6 ].push_back( string( ".jpe" ) );
    m_FileTypeList[ 6 ].push_back( string( ".ico" ) );
    m_FileTypeList[ 6 ].push_back( string( ".wmf" ) );
    m_FileTypeList[ 6 ].push_back( string( ".psd" ) );
    m_FileTypeList[ 6 ].push_back( string( ".psp" ) );
    m_FileTypeList[ 6 ].push_back( string( ".tga" ) );
    m_FileTypeList[ 6 ].push_back( string( ".tif" ) );
    m_FileTypeList[ 6 ].push_back( string( ".tiff" ) );
    m_FileTypeList[ 6 ].push_back( string( ".xpm" ) );
    m_FileTypeList[ 6 ].push_back( string( ".xbm" ) );
    m_FileTypeList[ 6 ].push_back( string( ".pcx" ) );
    m_FileTypeList[ 6 ].push_back( string( ".icon" ) );


    m_FileTypeList[ 6 ].resize( m_FileTypeList[ 6 ].size() );
}


int MainFrame::GetFileType( std::string sname )
{
    for ( int i = 0;i < 7;i++ )
    {
        vector<string>::iterator it;

        for ( it = m_FileTypeList[ i ].begin();it != m_FileTypeList[ i ].end();it++ )
        {
            if ( CompareExtName( sname, *it ) )
                return i + 1;
        }
    }

    return 0;
}

bool MainFrame::CompareExtName( string sname, string ext )
{
    if ( sname.length() == 0 || ext.length() == 0 )
        return false;

    if ( sname.length() < ext.length() )
        return false;

    string sub = sname.substr( sname.length() - ext.length(), ext.length() );

    return strcasecmp( sub.c_str(), ext.c_str() ) == 0;

}

_TASK_TYPE MainFrame::GetStatusByFileType( int type )
{
    switch ( type )
    {

        case 0:
        return _STATUS_FINISH_UNKNOW;
        break;

        case 1:
        return _STATUS_FINISH_SOFT;
        break;

        case 2:
        return _STATUS_FINISH_PACK;
        break;

        case 3:
        return _STATUS_FINISH_MUSIC;
        break;

        case 4:
        return _STATUS_FINISH_ISO;
        break;

        case 5:
        return _STATUS_FINISH_MOVIE;
        break;

        case 6:
        return _STATUS_FINISH_PUBLISH;
        break;

        case 7:
        return _STATUS_FINISH_PICTURE;
        break;

        default:
        return _STATUS_FINISH_UNKNOW;

    }
}

void MainFrame::OnTimer( wxTimerEvent& event )
{

    //send a beat so args will recalc
    m_TaskManager->HeartBeat();

    int TotalSpeed = 0;

    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nStatus == _STATUS_RUNNING )
        {
            if ( m_TaskManager->GetTaskStatus( it->nID ) != _TASK_RUNNING )
            {
                continue;
            }

            bool bret =
                m_TaskManager->GetRunningArgs( it->nID, it->fRatio, it->nFinish, it->nSpeed, it->nTime );

            if ( !bret )
                continue;

            TotalSpeed += it->nSpeed;


            it->sName = m_TaskManager->GetTaskFilename( it->nID );

            it->nLen = m_TaskManager->GetTaskSize( it->nID );

            it->nThread = m_TaskManager->GetTaskThreadNum( it->nID );

            it->nError = m_TaskManager->GetErrorTime( it->nID );

            UpdateTaskList( it->nID );

        }
    } //for

    if ( GetStatusBar() )
    {
        wxString sp;
        sp.Printf( _( "%d KB/S" ) , TotalSpeed / 1024 );
        GetStatusBar() ->SetStatusText( _("Speed:") + sp, 3 );
    }

    m_MeterBar->PutSpeedData( TotalSpeed );

}

void MainFrame::OnClose( wxCloseEvent& event )
{

    if ( event.CanVeto() )
    {

        if ( IsShown() )
        { //hide
            Hide();
            event.Veto();
            return ;
        }

        if ( m_TaskManager->GetTaskNumber() > 0 )
        {
            wxMessageDialog * dlg = new wxMessageDialog( this,
                                    _("There are tasks running,close window will stop all running task,QUIT?"),
                                    _("Warnning"),
                                    wxICON_QUESTION | wxYES_NO | wxSTAY_ON_TOP | wxNO_DEFAULT );

            m_SysTray->DlgShow( true );
            int dlgret = dlg->ShowModal();
            m_SysTray->DlgShow( false );

            if ( wxID_NO == dlgret )
            {
                event.Veto();
                dlg->Destroy();
                return ;
            }
            else
            {
                dlg->Destroy();
                StopAllTask();
            }
        }

    }
    else
    { //can't veto
        //DBGOUT( "can't veto" );
    }


    SaveTaskDB();
    SaveProxyDB();
    SaveConfig();

    delete m_SysTray;
    m_SysTray = NULL;

    this->Destroy();
}

void MainFrame::OnTaskFinish( wxCommandEvent& event )
{

    int ntask = event.GetId();


    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nID == ntask )
        {

            int status = m_TaskManager->GetTaskStatus( ntask );

            if ( status == _TASK_FINISH )
            {
                int ntype = GetFileType( m_TaskManager->GetTaskFilename( ntask ) );

                switch ( ntype )
                {

                    case 0:
                    it->nStatus = _STATUS_FINISH_UNKNOW;
                    break;

                    case 1:
                    it->nStatus = _STATUS_FINISH_SOFT;
                    break;

                    case 2:
                    it->nStatus = _STATUS_FINISH_PACK;
                    break;

                    case 3:
                    it->nStatus = _STATUS_FINISH_MUSIC;
                    break;

                    case 4:
                    it->nStatus = _STATUS_FINISH_ISO;
                    break;

                    case 5:
                    it->nStatus = _STATUS_FINISH_MOVIE;
                    break;

                    case 6:
                    it->nStatus = _STATUS_FINISH_PUBLISH;
                    break;

                    case 7:
                    it->nStatus = _STATUS_FINISH_PICTURE;
                    break;

                    default:
                    it->nStatus = _STATUS_FINISH_UNKNOW;
                    break;
                }


                it->sName = m_TaskManager->GetTaskFilename( it->nID );

                it->nLen = m_TaskManager->GetTaskSize( ntask );

                it->nFinish = it->nLen;

                it->nSpeed = 0;

                it->nError = m_TaskManager->GetErrorTime( it->nID );

                it->fRatio = 1.0f;


                m_TaskManager->RemoveTask( ntask );


                std::string sRename;

                if ( !MoveFile( it->sName, it->sSavePath, sRename ) )
                {
                    m_SysTray->DlgShow( true );
                    wxMessageBox( _("file move fail! \nfind it in /home/yourname/.MultiGet/ and move it out.") );
                    m_SysTray->DlgShow( false );
                }

                it->sName = sRename; //

                ScheduleTask();


                if ( ntask == m_nCurrentSelectTaskID )
                {
                    UpdateRightDown();
                }

                PlaySound( true );
                //notice finish
                m_MeterBar->SetTip( c_str(_("Task Finish:")) + it->sName );

                if ( it->bAutoMd5 && gbAutoMd5 )
                    FileMd5( *it );

                break;
            }
            else if ( status == _TASK_ERROR )
            {

                it->nStatus = _STATUS_WRONG;

                it->sName = m_TaskManager->GetTaskFilename( it->nID );
                it->nLen = m_TaskManager->GetTaskSize( ntask );

                int speed;
                it->nFinish = m_TaskManager->GetFinishByte( ntask, speed );

                it->nSpeed = speed;
                it->nError = m_TaskManager->GetErrorTime( it->nID );

                if ( it->nLen <= 0 )
                    it->fRatio = 0.0f;

                else
                    it->fRatio = ( float( it->nFinish ) / it->nLen );

                m_TaskManager->RemoveTask( ntask );

                ScheduleTask();

                if ( ntask == m_nCurrentSelectTaskID )
                {

                    UpdateRightDown();
                }

                PlaySound( false );
                //notice
                m_MeterBar->SetTip( c_str(_("Task Fail:")) + it->sName );
                break;
            }
            else
            {
                break;
            }
        }
    }

}


void MainFrame::OnShowModeSwitch( int mode )
{

	_TaskAttr* curtask = GetCurrentTask();
	if( curtask == NULL ) return;

                if ( m_TaskManager->GetTaskStatus( curtask->nID ) != _TASK_NONE )
                {

                    if ( mode == -2 || mode == -1 || mode > 0 )
                    {

                        m_TaskManager->UpdateThreadInfo(
                            curtask->nID,
                            m_vsubspliter->GetThreadInfoWin(),
                            mode );

                    }
                    else if ( mode == -3 )
                    {
                        m_TaskManager->UpdateRunningTable(
                            curtask->nID,
                            m_vsubspliter->GetShowTableWin() );
                    }
                    else if ( mode == 0 )
                    {
                        m_TaskManager->CloseUpdateUI( curtask->nID );
                    }
                    else
                    {
                        return ;
                    }
                }
                else
                {
                    return ;
                }

/*
    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {

        if ( it->nID == m_nCurrentSelectTaskID )
        {
            if ( it->nStatus == _STATUS_RUNNING )
            {

                if ( m_TaskManager->GetTaskStatus( m_nCurrentSelectTaskID ) != _TASK_NONE )
                {

                    if ( mode == -2 || mode == -1 || mode > 0 )
                    {
						DBGOUT("before updatethreadinfo");
                        m_TaskManager->UpdateThreadInfo(
                            m_nCurrentSelectTaskID,
                            m_vsubspliter->GetThreadInfoWin(),
                            mode );
						DBGOUT("after updatethreadinfo");
                    }
                    else if ( mode == -3 )
                    {
                        m_TaskManager->UpdateRunningTable(
                            m_nCurrentSelectTaskID,
                            m_vsubspliter->GetShowTableWin() );
                    }
                    else if ( mode == 0 )
                    {
                        m_TaskManager->CloseUpdateUI( m_nCurrentSelectTaskID );
                    }
                    else
                    {
                        return ;
                    }
                }
                else
                {
                    return ;
                }
            }
            else
            {
                return ;
            }
        }
    }
*/
}

bool MainFrame::SaveTaskDB()
{
    //1.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]
    //文件尾：0xFFFF[4]

    //2.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]
    //文件尾：0xFFFF[4]

    //3.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //nRetry[4]+nRetryWait[4]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]
    //文件尾：0xFFFF[4]

    //4.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //nRetry[4]+nRetryWait[4]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]+nMC[4]+
    //sMirrorURL[0][v]+....
    //文件尾：0xFFFF[4]


    //v5.0 data format
    //binary record
    //filehead+data+filetail
    //filehead:ver[4]+headlen[4]+datalen[4]+items[4]
    //items:nStatus[4]+nThread[4]+bAutoMd5[4]+sName[v]+nLen[8]+nFinish[8]+
    //nRetry[4]+nRetryWait[4]+
    //sURL[v]+sRefer[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]+nMC[4]+
    //sMirrorURL[0][v]+....
    //filetail:0xFFFF[4]

    FILE * fp;

    std::string tempdb;

    if ( !GetTempDir( tempdb ) )
    {
        return false;
    }

    tempdb += "/task.db";

    fp = fopen( tempdb.c_str(), "wb" );

    if ( fp == NULL )
    {
        return false;
    }

    struct _fh
    {
        int ver;
        int headlen; //4*4=16
        int datalen;
        int count; //=m_AllTaskList.size()
    }fh;

    fh.ver = 5;
    fh.headlen = sizeof( fh );
    fh.datalen = 0;
    fh.count = m_AllTaskList.size();

    if ( 1 != fwrite( &fh, sizeof( fh ), 1, fp ) )
    {
        fclose( fp );
        return false;
    }

    int dl = 0;
    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {

        fwrite( &( it->nStatus ), sizeof( it->nStatus ), 1, fp );
        dl += sizeof( it->nStatus );
        fwrite( &( it->nThread ), sizeof( it->nThread ), 1, fp );
        dl += sizeof( it->nThread );
        fwrite( &( it->bAutoMd5 ), sizeof( it->bAutoMd5 ), 1, fp );
        dl += sizeof( it->bAutoMd5 );
        fwrite( it->sName.c_str(), it->sName.length() + 1, 1, fp );
        dl += it->sName.length() + 1;
        fwrite( &( it->nLen ), sizeof( it->nLen ), 1, fp );
        dl += sizeof( it->nLen );
        fwrite( &( it->nFinish ), sizeof( it->nFinish ), 1, fp );
        dl += sizeof( it->nFinish );
        fwrite( &( it->nRetry ), sizeof( it->nRetry ), 1, fp );
        dl += sizeof( it->nRetry );
        fwrite( &( it->nRetryWait ), sizeof( it->nRetryWait ), 1, fp );
        dl += sizeof( it->nRetryWait );
        fwrite( it->sURL.c_str(), it->sURL.length() + 1, 1, fp );
        dl += it->sURL.length() + 1;
        fwrite( it->sRefer.c_str(), it->sRefer.length() + 1, 1, fp );
        dl += it->sRefer.length() + 1;
        fwrite( it->sSavePath.c_str(), it->sSavePath.length() + 1, 1, fp );
        dl += it->sSavePath.length() + 1;
        fwrite( it->sSockProxy.c_str(), it->sSockProxy.length() + 1, 1, fp );
        dl += it->sSockProxy.length() + 1;
        fwrite( it->sFtpProxy.c_str(), it->sFtpProxy.length() + 1, 1, fp );
        dl += it->sFtpProxy.length() + 1;
        fwrite( it->sHttpProxy.c_str(), it->sHttpProxy.length() + 1, 1, fp );
        dl += it->sHttpProxy.length() + 1;


        fwrite( it->sMD5.c_str(), it->sMD5.length() + 1, 1, fp );
        dl += it->sMD5.length() + 1;

        //ver 4.0 add it
        //mirrorcount;
        int msize = it->sMirrorURL.size();
        fwrite( &msize, sizeof( int ), 1, fp );
        dl += sizeof( int );
        //mirrors

        for ( int i = 0;i < msize;i++ )
        {
            fwrite( it->sMirrorURL[ i ].c_str(), it->sMirrorURL[ i ].length() + 1, 1, fp );
            dl += it->sMirrorURL[ i ].length() + 1;
        }

    }

    fh.datalen = dl;
    int tail = 0x00FF;
    fwrite( &tail, sizeof( int ), 1, fp );
    fseek( fp, 0, SEEK_SET );
    fwrite( &fh, fh.headlen, 1, fp );
    fclose( fp );

    return true;

}


bool MainFrame::ReadTaskDB()
{

    m_AllTaskList.clear();
    //1.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]
    //文件尾：0xFFFF[4]

    //2.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]
    //文件尾：0xFFFF[4]

    //3.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //nRetry[4]+nRetryWait[4]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]
    //文件尾：0xFFFF[4]

    //4.0版数据记录格式
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据项目：nStatus[4]+nThread[4]+nPriority[4]+sName[v]+nLen[8]+nFinish[8]+
    //nRetry[4]+nRetryWait[4]+
    //sURL[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]+nMC[4]+
    //sMirrorURL[0][v]+....
    //文件尾：0xFFFF[4]

    //v5.0 data format
    //binary record
    //filehead+data+filetail
    //filehead:ver[4]+headlen[4]+datalen[4]+items[4]
    //items:nStatus[4]+nThread[4]+bAutoMd5[4]+sName[v]+nLen[8]+nFinish[8]+
    //nRetry[4]+nRetryWait[4]+
    //sURL[v]+sRefer[v]+sSavePath[v]+sSockProxy[v]+sFtpProxy[v]+sHttpProxy[v]+sMD5[v]+nMC[4]+
    //sMirrorURL[0][v]+....
    //filetail:0xFFFF[4]

    FILE *fp;

    std::string tempdb;

    if ( !GetTempDir( tempdb ) )
    {
        return false;
    }

    tempdb += "/task.db";

    fp = fopen( tempdb.c_str(), "rb" );

    if ( fp == NULL )
    {
        return false;
    }

    struct _fh
    {
        int ver;
        int headlen; //4*4=16
        int datalen;
        int count; //=m_AllTaskList.size()
    }fh;

    fseek( fp, 0, SEEK_SET );

    if ( 1 != fread( &fh, sizeof( fh ), 1, fp ) )
    {
        fclose( fp );
        return false;
    }

    if ( fh.headlen != sizeof( fh ) )
    {
        fclose( fp );
        return false;
    }

    if ( fh.ver < 0 || fh.ver > 5 )
    {
        fclose( fp );
        return false;
    }

    if ( fh.count == 0 || fh.datalen == 0 )
    {
        fclose( fp );
        return true;
    }

	if ( fh.datalen < 0 )
	{
		fclose( fp );
		return false;
	}

    fseek( fp, fh.headlen, SEEK_SET ); //moveto data
    char *buf = new char[ fh.datalen ];

    if ( 1 != fread( buf, fh.datalen, 1, fp ) )
    {
        delete[] buf;
        return false;
    }

    fclose( fp );

    _TaskAttr pa;
    pa.nSpeed = 0;
    pa.nTime = 0;
    pa.nError = 0;


    int movepos = 0;
    char strbuf[ 512 ];

    for ( int i = 0;i < fh.count;i++ )
    { //read dataitem
        pa.sMirrorURL.clear();

        memcpy( &pa.nStatus, buf + movepos, sizeof( pa.nStatus ) );
        movepos += sizeof( pa.nStatus );

        memcpy( &pa.nThread, buf + movepos, sizeof( pa.nThread ) );
        movepos += sizeof( pa.nThread );

        if ( fh.ver >= 5 )
        {
            memcpy( &pa.bAutoMd5, buf + movepos, sizeof( pa.bAutoMd5 ) );
            movepos += sizeof( pa.bAutoMd5 );
        }
        else
        {
            int nouse;
            memcpy( &nouse, buf + movepos, sizeof( nouse ) );
            movepos += sizeof( nouse );
            pa.bAutoMd5 = true;
        }

        strcpy( strbuf, buf + movepos );
        pa.sName = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        memcpy( &pa.nLen, buf + movepos, sizeof( pa.nLen ) );
        movepos += sizeof( pa.nLen );
        memcpy( &pa.nFinish, buf + movepos, sizeof( pa.nFinish ) );
        movepos += sizeof( pa.nFinish );

        if ( fh.ver >= 3 )
        {
            memcpy( &pa.nRetry, buf + movepos, sizeof( pa.nRetry ) );
            movepos += sizeof( pa.nRetry );
            memcpy( &pa.nRetryWait, buf + movepos, sizeof( pa.nRetryWait ) );
            movepos += sizeof( pa.nRetryWait );
        }
        else
        {
            pa.nRetry = 3;
            pa.nRetryWait = 30;
        }

        strcpy( strbuf, buf + movepos );
        pa.sURL = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;

        if ( fh.ver >= 5 )
        { //refer
            strcpy( strbuf, buf + movepos );
            pa.sRefer = std::string( strbuf );
            movepos += strlen( strbuf ) + 1;
        }
        else
        {
            pa.sRefer.clear();
        }

        strcpy( strbuf, buf + movepos );
        pa.sSavePath = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        strcpy( strbuf, buf + movepos );
        pa.sSockProxy = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        strcpy( strbuf, buf + movepos );
        pa.sFtpProxy = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        strcpy( strbuf, buf + movepos );
        pa.sHttpProxy = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;

        if ( fh.ver >= 2 )
        { //md5
            strcpy( strbuf, buf + movepos );
            pa.sMD5 = std::string( strbuf );

            if ( pa.sMD5.length() != 32 )
                pa.sMD5.clear();

            movepos += strlen( strbuf ) + 1;
        }
        else
        {
            pa.sMD5.clear();
        }

        if ( fh.ver >= 4 )
        { //mirror url
            int msize;
            memcpy( &msize, buf + movepos, sizeof( int ) );
            movepos += sizeof( int );

            for ( int i = 0;i < msize;i++ )
            {
                strcpy( strbuf, buf + movepos );
                movepos += strlen( strbuf ) + 1;

                pa.sMirrorURL.push_back( std::string( strbuf ) );
            }
        }

        if ( pa.nLen != 0 && pa.nLen != -1 )
            pa.fRatio = ( float ) pa.nFinish / pa.nLen;
        else
            pa.fRatio = 0;

        pa.nID = GetNewTaskID();

        m_AllTaskList.push_back( pa );
    }


    delete[] buf;
    return true;

}

bool MainFrame::MoveFile( string mgfile, string destpath, std::string& renamestr )
{
    if ( mgfile.length() <= 3 )
        return false;

    std::string tempdir;

    if ( !GetTempDir( tempdir ) )
        return false;

    tempdir += "/";

    tempdir += mgfile;

    tempdir += ".mg";


    string destfile = destpath;

    if ( destfile[ destfile.length() - 1 ] != '/' )
        destfile += "/";

    destfile += mgfile;


    ifstream fcheck;

    fcheck.open( destfile.c_str(), ios::binary | ios::in );

    if ( fcheck.is_open() )
    {

        //file exists,make a new filename
        fcheck.close();

        int i = 0;

        while ( ++i )
        {


            char buf[ 512 ];
            sprintf( buf, "%s[%d]", destfile.c_str(), i );

            fcheck.open( buf, ios::binary | ios::in );

            if ( fcheck.is_open() )
            {
                fcheck.close();
                continue;
            }
            else
            {

                destfile = std::string( buf );
                renamestr = mgfile;
                sprintf( buf, "[%d]", i );
                renamestr += buf;
                fcheck.close();
                break;
            }
        }
    }
    else
    {

        renamestr = mgfile;
    }


	return wxRenameFile( wxString(tempdir.c_str(),wxConvLocal), wxString(destfile.c_str(),wxConvLocal) );

}


void MainFrame::OnMoveDown( wxCommandEvent& event )
{
    int downid;
    m_tasklist->MoveDown( m_nCurrentSelectTaskID, downid );

    if ( downid < 0 )
    {
        return ;
    }

    _TaskAttr *pcur, *pdown;
    pcur = GetTask( m_nCurrentSelectTaskID );
    pdown = GetTask( downid );

    if ( pcur == NULL || pdown == NULL )
    {
        return ;
    }

    if ( pcur->nStatus == _STATUS_RUNNING && pdown->nStatus == _STATUS_WAITING )
    {
        if ( !m_TaskManager->RemoveTask( pcur->nID ) )
        {
            //DBGOUT( "delete task fail" );
        }

        pcur->nStatus = _STATUS_WAITING;
        pdown->nStatus = _STATUS_RUNNING;
        m_TaskManager->AddTask( *pdown );
    }

    SwapTaskTable( pcur->nID, pdown->nID );

    UpdateTaskList();

}


void MainFrame::OnMoveUp( wxCommandEvent& event )
{
    int upid;
    m_tasklist->MoveUp( m_nCurrentSelectTaskID, upid );

    if ( upid < 0 )
    {
        return ;
    }

    _TaskAttr	*pcur, *pup;
    pcur = GetTask( m_nCurrentSelectTaskID );
    pup = GetTask( upid );

    if ( pcur == NULL || pup == NULL )
    {
        //event.Skip();
        //DBGOUT( "pcur==NULL||pup==NULL" );
        return ;
    }

    if ( pcur->nStatus == _STATUS_WAITING && pup->nStatus == _STATUS_RUNNING )
    {
        if ( !m_TaskManager->RemoveTask( pup->nID ) )
        {
            //DBGOUT( "delete task fail" );
        }

        pcur->nStatus = _STATUS_RUNNING;
        pup->nStatus = _STATUS_WAITING;
        m_TaskManager->AddTask( *pcur );
    }

    SwapTaskTable( pcur->nID, pup->nID );

    UpdateTaskList();

}

void MainFrame::SwapTaskTable( int task1, int task2 )
{
    _TaskAttr swap;
    vector<_TaskAttr>::iterator it1, it2;

    for ( it1 = m_AllTaskList.begin();it1 != m_AllTaskList.end();it1++ )
    {
        if ( it1->nID == task1 )
        {
            break;
        }
    }

    for ( it2 = m_AllTaskList.begin();it2 != m_AllTaskList.end();it2++ )
    {
        if ( it2->nID == task2 )
        {
            break;
        }
    }

    swap = *it1;
    *it1 = *it2;
    *it2 = swap;

}

void MainFrame::OnDropURL( wxString url )
{

    CUrlParser upar;

    if ( !upar.SetUrl( string( url.mb_str( wxConvLocal ) ) ) )  //?????
    {
        m_SysTray->DlgShow( true );
        wxMessageBox( _("Not a supported url file address") );
        m_SysTray->DlgShow( false );
        return ;
    }

    if ( IsUrlExist( std::string( url.mb_str( wxConvLocal ) ) ) )
    {
        m_SysTray->DlgShow( true );
        //wxMessageBox( "url exists" );  //maybe add some more code
		wxMessageBox(wxString::FromAscii("url exists")); //unicode patch
        m_SysTray->DlgShow( false );
        return ;
    }


    DoNewTask( url );
}

void MainFrame::InitMeterBar()
{
    m_MeterBar = new CMeterCtrl( this );
}

void MainFrame::SysTrayExitNotice()
{
    this->Hide();
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED,
                          wxID_EXIT );
    AddPendingEvent( event );
    return ;
}

void MainFrame::StopAllTask()
{

    vector<_TaskAttr>::iterator it;
    bool got = false;

    if ( m_TaskManager->RemoveAllTask() )
        got = true;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nStatus == _STATUS_RUNNING || it->nStatus == _STATUS_WAITING )
        {
            it->nStatus = _STATUS_PAUSE;
            got = true;
        }
    }

    if ( got )
        ScheduleTask();
}

void MainFrame::StartAllTask()
{
    vector<_TaskAttr>::iterator it;
    bool got = false;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->nStatus == _STATUS_PAUSE )
        {
            it->nStatus = _STATUS_WAITING;
            got = true;
        }
    }

    if ( got )
        ScheduleTask();
}

void MainFrame::SwitchShowDropWin()
{
    if ( m_FloatWin )
    {
        m_FloatWin->Hide();
        m_FloatWin->Destroy();
        m_FloatWin = NULL;
        gbShowDropWin = false;
    }
    else
    {
        m_FloatWin = new CFloatWin( this );
        m_FloatWin->Show();
        gbShowDropWin = true;
    }
	return;
}
/*2008-4-15 Disable the Language menu, Wing Sun */
/*
void MainFrame::OnLangEnUtf8( wxCommandEvent& )
{
	if( CheckLocale ( EN_US_UTF_8 ) )
	{
    	wxGetApp().SetLang( EN_US_UTF_8 );
    	DynamicLang();
	}
	else
	{
		CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}
}

void MainFrame::OnLangCnUtf8( wxCommandEvent& )
{
	if( CheckLocale( ZH_CN_UTF_8 ) )
	{
    	wxGetApp().SetLang( ZH_CN_UTF_8 );
    	DynamicLang();
	}
	else
	{
		CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}
}

void MainFrame::OnLangCn18030( wxCommandEvent& )
{
	if( CheckLocale( ZH_CN_18030 ) )
	{
    	wxGetApp().SetLang( ZH_CN_18030 );
    	DynamicLang();
	}
	else
	{
    	CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}
}

void MainFrame::OnLangTwBig5( wxCommandEvent& )
{
	if( CheckLocale(ZH_TW_BIG5))
	{
	    wxGetApp().SetLang( ZH_TW_BIG5 );
    	DynamicLang();
	}
	else
	{
		CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}

}

void MainFrame::OnLangTwUtf8( wxCommandEvent& )
{
	if( CheckLocale(ZH_TW_UTF_8) )
	{
    	wxGetApp().SetLang( ZH_TW_UTF_8 );
    	DynamicLang();
	}
	else
	{
		CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}
}

void MainFrame::OnLangDeUtf8( wxCommandEvent& )
{
	if( CheckLocale ( DE_DE_UTF_8 ) )
	{
    	wxGetApp().SetLang( DE_DE_UTF_8 );
    	DynamicLang();
	}
	else
	{
		CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}
}
void MainFrame::OnLangPtUtf8( wxCommandEvent& )
{
	if( CheckLocale ( PT_BR_UTF_8 ) )
	{
    	wxGetApp().SetLang( PT_BR_UTF_8 );
    	DynamicLang();
	}
	else
	{
		CheckLangMenu ( wxGetApp().GetLang() );
		NoLangSupport();
	}
}

void MainFrame::UpdateMenuText()
{
    wxMenuBar * menubar = GetMenuBar();

    if ( menubar == NULL )
        return ;

    //update root
    menubar->SetLabelTop( 0, _("File(&F)") );

    menubar->SetLabelTop( 1, _("Task(&T)") );

    menubar->SetLabelTop( 2, _("View(&V)") );

    menubar->SetLabelTop( 3, _("Option(&P)") );

    menubar->SetLabelTop( 4, _("Help(&H)") );

    //update other
    //file
    menubar->SetLabel( wxID_EXIT, _("Quit\tCtrl+Q") );
	
	menubar->SetLabel( mgID_FILE_SAVECONFIG, _("Save Config") );
	
	menubar->SetLabel( mgID_FILE_SAVEPROXY, _("Save proxy list") );
	
	menubar->SetLabel( mgID_FILE_SAVETASK, _("Save task list") );

    //task
    menubar->SetLabel( mgID_NEW_TASK, _("New\tCtrl+N") );

    menubar->SetLabel( mgID_START_TASK, _("Start\tCtrl+G") );

    menubar->SetLabel( mgID_STOP_TASK, _("Pause\tCtrl+P") );

    menubar->SetLabel( mgID_CANCEL_TASK, _("Delete\tDel") );

    menubar->SetLabel( mgID_MOVEUP_TASK, _("MoveUp\tCtrl+U") );

    menubar->SetLabel( mgID_MOVEDOWN_TASK, _("MoveDown\tCtrl+D") );

    menubar->SetLabel( mgID_ADDTHREAD, _("+Thread\tCtrl++") );

    menubar->SetLabel( mgID_SUBTHREAD, _("-Thread\tCtrl+-") );

    menubar->SetLabel( mgID_RELOAD, _("Redownload") );

    //view
    menubar->SetLabel( mgID_VIEW_LANG, _("Language") );

    menubar->SetLabel( mgID_VIEW_TOOLBAR, _("ToolBar") );

    menubar->SetLabel( mgID_VIEW_TOOLBAR_SIZE16, _("16*16 icon") );

    menubar->SetLabel( mgID_VIEW_TOOLBAR_SIZE24, _("24*24 icon") );

    menubar->SetLabel( mgID_VIEW_TOOLBAR_SIZE32, _("32*32 icon") );

    menubar->SetLabel( mgID_VIEW_STATUSBAR, _("StatusBar") );

    //Setting
    menubar->SetLabel( mgID_OPTION_CONFIG, _("Settings\tCtrl+S") );

    menubar->SetLabel( mgID_OPTION_PROXY, _("Proxy Admin\tCtrl+T") );

    //help
	menubar->SetLabel( mgID_HELP_VISITHOME, _("Visit homepage...") );
	
    menubar->SetLabel( wxID_ABOUT, _("About\tCtrl+O") );
}

void MainFrame::UpdateToolText()
{
    InitToolBar();
    UpdateToolBar();
}

void MainFrame::DynamicLang()
{
    SetTitle( _("MultiGet file downloader") );
    UpdateMenuText();
    UpdateToolText();
	UpdateStatusBar();
    m_tasklist->DynamicLang();
    m_lefttree->DynamicLang();
    m_vsubspliter->DynamicLang();
    m_SysTray->SetIcon( wxIcon( logo_22_xpm ), _("MultiGet file downloader") );
}
*/
//the only new task dlg
void MainFrame::DoNewTask( wxString url, wxString refer, wxString savepath, wxString savefile )
{

    if ( m_NewTaskDlg == NULL )
    {
	
        CUrlParser parser;

        if ( !url.IsNull() )
        {
#ifndef _UNICODE
            if ( !parser.SetUrl( url.mb_str( wxConvLocal ) ) )
#else
            if ( !parser.SetUrl( url.mb_str( wxConvLocal ).data() ) )
#endif
                return ;
        }

		
        m_NewTaskDlg = new CNewTaskDlg( this, url, refer, savepath );

        m_SysTray->DlgShow( true );
        int nret = m_NewTaskDlg->ShowModal();
        m_SysTray->DlgShow( false );

        if ( wxID_OK == nret )
        {

            //the way wxString->string,wxString::mb_str(wxConvLocal)

            std::string surl, ssavepath, srename;

            surl = m_NewTaskDlg->GetUrl();

            CBatch batch( surl );

            if ( batch.GetNum() == 1 )
            {
                surl = batch.Get( 0 );
            }
            else
            {

                int tnum = batch.GetNum();
                //circle add task
                //prepare all args
#ifndef _UNICODE
                std::string refer = m_NewTaskDlg->m_sRefer.mb_str( wxConvLocal );
                std::string savepath = m_NewTaskDlg->m_sSavePath.mb_str( wxConvLocal );
                std::string rename = m_NewTaskDlg->m_sRename.mb_str( wxConvLocal );
#else
                std::string refer = m_NewTaskDlg->m_sRefer.mb_str( wxConvLocal ).data();
                std::string savepath = m_NewTaskDlg->m_sSavePath.mb_str( wxConvLocal ).data();
                std::string rename = m_NewTaskDlg->m_sRename.mb_str( wxConvLocal ).data();
#endif
                std::string sock;
                std::string ftp;
                std::string http;
                m_NewTaskDlg->GetProxyName( sock, ftp, http );


                int threadnum = m_NewTaskDlg->m_nThreadNum;
                bool runnow = m_NewTaskDlg->m_nRunNow == 0;
                int retry = m_NewTaskDlg->m_nRetry;
                int retrywait = m_NewTaskDlg->m_nRetryWait;
                bool automd5 = gbAutoMd5;



                //record path this time used;
                RecordSavePath( savepath );
                CBatch renamebatch( rename );
                std::string selectfilename = m_NewTaskDlg->GetSelectFileName();


                for ( int tasks = 0;tasks < tnum;tasks++ )
                {

                    std::string mir[ 100 ];
                    int nmir = m_NewTaskDlg->GetMirrorUrl( 100, mir );

                    if ( nmir > 0 )
                    {

                        for ( int j = 0;j < nmir;j++ )
                        {

                            std::string temp = mir[ j ];
                            Trim( temp );
                            CBatch mirbatch( selectfilename );

                            if ( temp[ temp.length() - 1 ] == '/' )
                            {

                                temp += mirbatch.Get( tasks );

                                mir[ j ] = temp;
                            }
                        }
                    }


                    AddBatchTask(
                        batch.Get( tasks ),
                        refer,
                        savepath,
                        renamebatch.Get( tasks ),
                        threadnum,
                        runnow,
                        retry,
                        retrywait,
                        sock,
                        ftp,
                        http,
                        automd5,
                        nmir,
                        mir );
                }

                m_NewTaskDlg->Destroy();
                m_NewTaskDlg = NULL;
                //going task
                ScheduleTask();
                UpdateRightDown();
                return ;
            }

            //single task treatment
            ssavepath = m_NewTaskDlg->m_sSavePath.mb_str( wxConvLocal );

            srename = m_NewTaskDlg->m_sRename.mb_str( wxConvLocal );

            if ( parser.SetUrl( surl ) )
            {

                _TaskAttr atask;
                atask.nID = GetNewTaskID();
                atask.sURL = surl;
                //add refer
                atask.sRefer = m_NewTaskDlg->m_sRefer.mb_str( wxConvLocal );
                Trim( atask.sRefer );
                atask.sSavePath = ssavepath;
                //check the path valid? create it if not exists

                atask.nThread = m_NewTaskDlg->m_nThreadNum;
                atask.nStatus = m_NewTaskDlg->m_nRunNow == 0 ? _STATUS_WAITING : _STATUS_PAUSE;


                std::string finame = srename.empty() ? parser.GetFileName() : srename;

                if ( finame.empty() )
                    atask.sName = "index.html";
                else
                    atask.sName = finame;

                //atask.sName = srename.empty()?parser.GetFileName():srename;

                atask.nRetry = m_NewTaskDlg->m_nRetry;

                atask.nRetryWait = m_NewTaskDlg->m_nRetryWait;

                atask.nFinish = 0;

                atask.fRatio = 0.0;

                atask.nError = 0;

                atask.nSpeed = 0;

                atask.bAutoMd5 = gbAutoMd5;

                atask.nLen = 0;

                atask.nTime = 0;

                m_NewTaskDlg->GetProxyName( atask.sSockProxy, atask.sFtpProxy, atask.sHttpProxy );


                //get Mirror url and main url,make a group

                std::string mir[ 100 ];

                int nmir = m_NewTaskDlg->GetMirrorUrl( 100, mir );

                if ( nmir > 0 )
                {

                    for ( int j = 0;j < nmir;j++ )
                    {
                        std::string temp = mir[ j ];

                        if ( temp[ temp.length() - 1 ] == '/' )
                        {
                            temp += m_NewTaskDlg->GetSelectFileName();
                        }

                        atask.sMirrorURL.push_back( temp );
                    }
                }


                //delete dlg;
                m_NewTaskDlg->Destroy();

                m_NewTaskDlg = NULL;

                //record path this time used;
                RecordSavePath( atask.sSavePath );

                //going task
                m_AllTaskList.push_back( atask );

                ScheduleTask();

                UpdateRightDown();

            }
            else
            {
                m_NewTaskDlg->Destroy();
                m_NewTaskDlg = NULL;
                m_SysTray->DlgShow( true );

                wxMessageBox( _("Not a supported url file address"),
                              _("Wrong"), wxICON_INFORMATION );
                m_SysTray->DlgShow( false );

                return ;
            }


        } //if ( wxID_OK == nret )
        else
        { //Cancel task;

            m_NewTaskDlg->Destroy();
            m_NewTaskDlg = NULL;
        }

    } //if(m_NewTaskDlg==NULL)
    else
    {
        CUrlParser parser;
#ifndef _UNICODE
        if ( parser.SetUrl( url.mb_str( wxConvLocal ) ) )
#else
		if ( parser.SetUrl( url.mb_str( wxConvLocal ).data() ) )
#endif
        {
            m_NewTaskDlg->AddMirrorUrl( url );
        }
        else if ( parser.IsValidMirror() )
        {
            m_NewTaskDlg->AddMirrorUrl( url );
        }

    }

}

bool MainFrame::IsUrlExist( string url )
{
    //check url in task list first
    vector<_TaskAttr>::iterator it;

    for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
    {
        if ( it->sURL == url )
        {
            return true;
        }
    }

    return false;
}

void MainFrame::MonitorClip( bool mo )
{
    m_HideWin->MonitorClip( mo );
    gbMonitorClip = mo;
}

void MainFrame::OnStatusBar( wxCommandEvent& event )
{
    if ( m_bShowStatusBar )
    {
        wxStatusBar * bar = GetStatusBar();
        assert( bar != NULL );
        SetStatusBar( NULL );
        delete bar;
        m_bShowStatusBar = false;
    }
    else
    {
        wxStatusBar* bar = new wxStatusBar( this, wxID_ANY );
        int statusw[ 5 ] = { -1, 100, 100, 140 };
        bar->SetFieldsCount( 5, statusw );
        bar->SetStatusText( _("Welcome to use MultiGet file downloader!"), 0 );
        SetStatusBar( bar );
        m_bShowStatusBar = true;
    }

    UpdateStatusBar();

}

void MainFrame::UpdateStatusBar()
{
    if ( GetStatusBar() )
    {
        int nrun = 0, nwait = 0;
        vector<_TaskAttr>::const_iterator it;

        for ( it = m_AllTaskList.begin();it != m_AllTaskList.end();it++ )
        {
            if ( it->nStatus == _STATUS_RUNNING )
                nrun++;
            else if ( it->nStatus == _STATUS_WAITING )
                nwait++;
            else
                continue;
        }

        wxString num;
        num.Printf( _( "%d" ), nrun );
        GetStatusBar() ->SetStatusText( _("Running:") + num, 1 );
        num.Printf( _( "%d" ), nwait );
        GetStatusBar() ->SetStatusText( _("Waiting:") + num, 2 );

    }
}

void MainFrame::InitStatusBar()
{
    if ( m_bShowStatusBar )
    {
        wxStatusBar * bar = new wxStatusBar( this, wxID_ANY );
        int statusw[ 5 ] = { -1, 100, 100, 140 };
        bar->SetFieldsCount( 5, statusw );
        bar->SetStatusText( _("Welcome to use MultiGet file downloader!" ), 0 );
        SetStatusBar( bar );
    }
}

void MainFrame::OnToolbarSize16( wxCommandEvent& )
{
    wxToolBar * toolbar = GetToolBar();

    if ( TOOLBAR_SIZE == 16 )
        return ;

    TOOLBAR_SIZE = 16;

    SetToolBar( NULL );

    delete toolbar;

    InitToolBar();

    UpdateToolBar();
}

void MainFrame::OnToolbarSize24( wxCommandEvent& )
{
    wxToolBar * toolbar = GetToolBar();

    if ( TOOLBAR_SIZE == 24 )
        return ;

    TOOLBAR_SIZE = 24;

    SetToolBar( NULL );

    delete toolbar;

    InitToolBar();

    UpdateToolBar();
}

void MainFrame::OnToolbarSize32( wxCommandEvent& )
{
    wxToolBar * toolbar = GetToolBar();

    if ( TOOLBAR_SIZE == 32 )
        return ;

    TOOLBAR_SIZE = 32;

    SetToolBar( NULL );

    delete toolbar;

    InitToolBar();

    UpdateToolBar();
}


void MainFrame::OnProxySetting( wxCommandEvent& )
{
    CProxyDlg * dlg = new CProxyDlg( this );
    m_SysTray->DlgShow( true );
    dlg->ShowModal();
    m_SysTray->DlgShow( false );
}

int MainFrame::GetProxyCount()
{
    return m_AllProxyList.size();
}

_ProxyAttr& MainFrame::GetProxy( int seq )
{
    return m_AllProxyList[ seq ];
}

void MainFrame::RemoveProxy( wxString name )
{
#ifndef _UNICODE
    std::string na = name.mb_str( wxConvLocal );
#else
	std::string na = name.mb_str( wxConvLocal ).data();
#endif

    std::vector<_ProxyAttr>::iterator it;

    for ( it = m_AllProxyList.begin();it != m_AllProxyList.end();it++ )
    {
        if ( it->sName == na )
        {
            m_AllProxyList.erase( it );
            break;
        }
    }
}

bool MainFrame::AddProxy(
    int type, wxString name, wxString server, int port, wxString user, wxString pass )
{
#ifndef _UNICODE
    std::string na = name.mb_str( wxConvLocal );
#else
    std::string na = name.mb_str( wxConvLocal ).data();
#endif
    std::vector<_ProxyAttr>::iterator it;

    for ( it = m_AllProxyList.begin();it != m_AllProxyList.end();it++ )
    {
        if ( it->sName == na )
        {
            return false;
        }
    }

    _ProxyAttr newnode;
    newnode.nType = type;
    newnode.sName = na;
    newnode.sServer = server.mb_str( wxConvLocal );
    newnode.nPort = port;
    newnode.sUser = user.mb_str( wxConvLocal );
    newnode.sPass = pass.mb_str( wxConvLocal );
    m_AllProxyList.push_back( newnode );
    return true;
}

bool MainFrame::SaveProxyDB()
{

    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据：类型[4]+名称[v]+服务器名[v]+端口[4]+用户名[v]+密码[v]
    //文件尾：0xFFFF[4]

    FILE * fp;

    std::string tempdb;

    if ( !GetTempDir( tempdb ) )
    {
        return false;
    }

    tempdb += "/proxy.db";

    fp = fopen( tempdb.c_str(), "wb" );

    if ( fp == NULL )
    {
        return false;
    }

    struct _fh
    {
        int ver;
        int headlen; //4*4=16
        int datalen;
        int count; //=m_AllProxyList.size()
    }

    fh;

    fh.ver = 1;
    fh.headlen = sizeof( fh );
    fh.datalen = 0;
    fh.count = m_AllProxyList.size();

    if ( 1 != fwrite( &fh, fh.headlen, 1, fp ) )
    {
        fclose( fp );
        return false;
    }

    int dl = 0;

    vector<_ProxyAttr>::iterator it;

    for ( it = m_AllProxyList.begin();it != m_AllProxyList.end();it++ )
    {
        fwrite( &( it->nType ), sizeof( int ), 1, fp );
        dl += sizeof( int );
        fwrite( it->sName.c_str(), it->sName.length() + 1, 1, fp );
        dl += it->sName.length() + 1;
        fwrite( it->sServer.c_str(), it->sServer.length() + 1, 1, fp );
        dl += it->sServer.length() + 1;
        fwrite( &( it->nPort ), sizeof( int ), 1, fp );
        dl += sizeof( int );
        fwrite( it->sUser.c_str(), it->sUser.length() + 1, 1, fp );
        dl += it->sUser.length() + 1;
        fwrite( it->sPass.c_str(), it->sPass.length() + 1, 1, fp );
        dl += it->sPass.length() + 1;
    }

    fh.datalen = dl;

    //file tail
    int tail = 0xFFFF;
    fwrite( &tail, sizeof( int ), 1, fp );
    fseek( fp, 0, SEEK_SET );
    fwrite( &fh, fh.headlen, 1, fp );
    fclose( fp );
    return true;
}

bool MainFrame::ReadProxyDB()
{

    m_AllProxyList.clear();
    //记录格式采用2进制格式
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]+数据项目数[4]
    //数据：类型[4]+名称[v]+服务器名[v]+端口[4]+用户名[v]+密码[v]
    //文件尾：0xFFFF[4]

    FILE *fp;

    std::string tempdb;

    if ( !GetTempDir( tempdb ) )
    {
        return false;
    }

    tempdb += "/proxy.db";

    fp = fopen( tempdb.c_str(), "rb" );

    if ( fp == NULL )
    {
        return false;
    }

    struct _fh
    {
        int ver;
        int headlen; //4*4=16
        int datalen;
        int count; //=m_AllProxyList.size()
    }

    fh;

    fseek( fp, 0, SEEK_SET );

    if ( 1 != fread( &fh, sizeof( fh ), 1, fp ) )
    {
        fclose( fp );
        return false;
    }

    if ( fh.ver != 1 || fh.headlen != sizeof( fh ) )
    {
        fclose( fp );
        return false;
    }

    if ( fh.count == 0 || fh.datalen == 0 )
    {
        fclose( fp );
        return true;
    }

    fseek( fp, fh.headlen, SEEK_SET ); //moveto data
    char *buf = new char[ fh.datalen ];

    if ( 1 != fread( buf, fh.datalen, 1, fp ) )
    {
        delete[] buf;
        return false;
    }

    _ProxyAttr pa;
    int movepos = 0;
    char strbuf[ 256 ];

    for ( int i = 0;i < fh.count;i++ )
    { //read dataitem
        memcpy( &pa.nType, buf + movepos, sizeof( int ) );
        movepos += sizeof( int );
        strcpy( strbuf, buf + movepos );
        pa.sName = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        strcpy( strbuf, buf + movepos );
        pa.sServer = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        memcpy( &pa.nPort, buf + movepos, sizeof( int ) );
        movepos += sizeof( int );
        strcpy( strbuf, buf + movepos );
        pa.sUser = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        strcpy( strbuf, buf + movepos );
        pa.sPass = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;

        m_AllProxyList.push_back( pa );
    }

    fclose( fp );
    delete[] buf;
    return true;

}

bool MainFrame::GetProxyByName(
    std::string name, int& type, std::string& server, int& port, std::string& user, std::string& pass )
{

    vector<_ProxyAttr>::iterator it;

    for ( it = m_AllProxyList.begin();it != m_AllProxyList.end();it++ )
    {

        if ( it->sName == name )
        {
            type = it->nType;
            server = it->sServer;
            port = it->nPort;
            user = it->sUser;
            pass = it->sPass;
            return true;
        }
    }

    return false;
}

void MainFrame::PlaySound( bool success )
{
#if defined(__BSD__)|| defined(__DARWIN__)
	return;
#else
    if ( !gbSoundTip )
        return ;

    wxSound *snd = new wxSound;

    CreateSound( *snd, success );

    if ( snd->IsOk() )
    {
        snd->Play( wxSOUND_ASYNC );
    }
#endif
}

void MainFrame::CreateSound( wxSound& snd, bool success )
{
#if defined(__BSD__)|| defined(__DARWIN__)
	return;
#else
    if ( success )
    { //right wav
        static const unsigned char goodsound[] =
            {

                0x52, 0x49, 0x46, 0x46, 0xf5, 0x11, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
                0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x11, 0x2b, 0x00, 0x00,
                0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0xd1, 0x11, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e, 0x7c, 0x78, 0x79,
                0x7b, 0x7c, 0x7c, 0x84, 0x91, 0x8e, 0x8c, 0x8e, 0x8b, 0x81, 0x7d, 0x77, 0x90, 0x8a, 0x6f, 0x5f,
                0x5c, 0x61, 0x70, 0x71, 0x6f, 0x8d, 0x8f, 0x95, 0x94, 0x9c, 0xb1, 0xa4, 0x68, 0x70, 0x88, 0x9f,
                0x9d, 0x6b, 0x41, 0x44, 0x71, 0x7e, 0x7c, 0x5f, 0x5f, 0x83, 0x99, 0x9c, 0x95, 0x89, 0x89, 0x83,
                0x8d, 0x98, 0x95, 0x8f, 0x92, 0xa1, 0xa9, 0x9f, 0x88, 0x71, 0x5f, 0x53, 0x46, 0x3b, 0x34, 0x3b,
                0x4f, 0x6b, 0x81, 0x8b, 0x93, 0xa3, 0xbb, 0xd0, 0xd6, 0xc8, 0xb0, 0x99, 0x8d, 0x85, 0x76, 0x5c,
                0x40, 0x35, 0x40, 0x5a, 0x75, 0x7f, 0x7b, 0x75, 0x77, 0x81, 0x88, 0x83, 0x77, 0x6f, 0x78, 0x91,
                0xac, 0xbc, 0xbd, 0xb5, 0xad, 0xa6, 0x9b, 0x87, 0x6b, 0x51, 0x42, 0x40, 0x43, 0x46, 0x48, 0x4f,
                0x61, 0x79, 0x92, 0xa1, 0xa9, 0xaf, 0xb7, 0xbe, 0xbd, 0xad, 0x94, 0x7b, 0x6e, 0x6e, 0x71, 0x6f,
                0x69, 0x66, 0x6b, 0x76, 0x7e, 0x7a, 0x69, 0x57, 0x50, 0x59, 0x6a, 0x7a, 0x85, 0x8f, 0x9e, 0xb2,
                0xc1, 0xc7, 0xbf, 0xaa, 0x95, 0x87, 0x7c, 0x6f, 0x5e, 0x4d, 0x43, 0x44, 0x4e, 0x58, 0x60, 0x68,
                0x75, 0x88, 0x9b, 0xa7, 0xa9, 0xa3, 0x9d, 0x99, 0x97, 0x91, 0x88, 0x80, 0x7f, 0x85, 0x8e, 0x90,
                0x87, 0x78, 0x69, 0x60, 0x5b, 0x55, 0x4e, 0x4a, 0x50, 0x63, 0x7e, 0x96, 0xa5, 0xab, 0xaf, 0xb3,
                0xb7, 0xb5, 0xa8, 0x95, 0x82, 0x75, 0x6e, 0x68, 0x60, 0x55, 0x50, 0x53, 0x5e, 0x6c, 0x76, 0x7c,
                0x80, 0x86, 0x8d, 0x92, 0x90, 0x8b, 0x87, 0x8a, 0x92, 0x9b, 0xa0, 0x9f, 0x9c, 0x98, 0x92, 0x87,
                0x76, 0x61, 0x4f, 0x47, 0x49, 0x50, 0x59, 0x62, 0x6d, 0x7e, 0x91, 0xa1, 0xaa, 0xaa, 0xa6, 0xa4,
                0xa3, 0xa0, 0x98, 0x8a, 0x7b, 0x71, 0x6c, 0x6b, 0x6a, 0x67, 0x65, 0x67, 0x6d, 0x75, 0x78, 0x76,
                0x72, 0x71, 0x76, 0x7e, 0x86, 0x8d, 0x94, 0x9d, 0xa8, 0xaf, 0xaf, 0xa6, 0x97, 0x86, 0x78, 0x6b,
                0x60, 0x55, 0x4e, 0x4f, 0x57, 0x63, 0x6f, 0x78, 0x7f, 0x87, 0x91, 0x9a, 0xa0, 0x9f, 0x9b, 0x97,
                0x95, 0x93, 0x8e, 0x87, 0x7e, 0x78, 0x77, 0x78, 0x79, 0x76, 0x70, 0x6a, 0x67, 0x66, 0x66, 0x66,
                0x66, 0x6c, 0x77, 0x87, 0x97, 0xa3, 0xa9, 0xab, 0xab, 0xa7, 0x9f, 0x92, 0x82, 0x73, 0x69, 0x64,
                0x62, 0x60, 0x5f, 0x61, 0x66, 0x6e, 0x77, 0x7d, 0x81, 0x85, 0x8a, 0x90, 0x94, 0x95, 0x93, 0x90,
                0x8f, 0x90, 0x90, 0x8f, 0x8b, 0x86, 0x81, 0x7d, 0x78, 0x70, 0x65, 0x5c, 0x58, 0x5a, 0x60, 0x69,
                0x73, 0x7e, 0x8b, 0x98, 0xa3, 0xa8, 0xa6, 0xa0, 0x99, 0x93, 0x8c, 0x84, 0x7b, 0x73, 0x6e, 0x6c,
                0x6c, 0x6c, 0x6b, 0x6b, 0x6c, 0x70, 0x76, 0x7a, 0x7d, 0x7f, 0x82, 0x87, 0x8c, 0x90, 0x92, 0x94,
                0x96, 0x98, 0x99, 0x97, 0x90, 0x86, 0x7b, 0x71, 0x68, 0x61, 0x5c, 0x59, 0x5c, 0x64, 0x6f, 0x7b,
                0x85, 0x8c, 0x92, 0x97, 0x9a, 0x9a, 0x98, 0x93, 0x8e, 0x8b, 0x88, 0x85, 0x80, 0x7b, 0x76, 0x73,
                0x72, 0x71, 0x6f, 0x6d, 0x6c, 0x6e, 0x71, 0x74, 0x77, 0x7b, 0x7f, 0x86, 0x8e, 0x96, 0x9c, 0x9e,
                0x9e, 0x9b, 0x97, 0x8f, 0x85, 0x79, 0x6e, 0x66, 0x63, 0x62, 0x64, 0x67, 0x6c, 0x73, 0x7a, 0x81,
                0x86, 0x89, 0x8b, 0x8d, 0x8f, 0x91, 0x91, 0x90, 0x8e, 0x8c, 0x8a, 0x88, 0x85, 0x80, 0x7a, 0x76,
                0x73, 0x70, 0x6d, 0x6a, 0x68, 0x69, 0x6c, 0x72, 0x79, 0x80, 0x88, 0x90, 0x97, 0x9d, 0x9f, 0x9d,
                0x97, 0x90, 0x89, 0x81, 0x7a, 0x73, 0x6e, 0x6b, 0x6c, 0x6e, 0x70, 0x72, 0x73, 0x76, 0x79, 0x7d,
                0x80, 0x83, 0x85, 0x88, 0x8c, 0x8f, 0x91, 0x92, 0x91, 0x8f, 0x8d, 0x8a, 0x86, 0x80, 0x7a, 0x73,
                0x6e, 0x6a, 0x68, 0x66, 0x67, 0x6b, 0x71, 0x79, 0x82, 0x8a, 0x90, 0x94, 0x97, 0x97, 0x96, 0x92,
                0x8c, 0x87, 0x82, 0x7f, 0x7c, 0x79, 0x76, 0x74, 0x73, 0x72, 0x72, 0x72, 0x72, 0x73, 0x75, 0x79,
                0x7d, 0x81, 0x85, 0x89, 0x8d, 0x91, 0x93, 0x94, 0x93, 0x90, 0x8c, 0x88, 0x82, 0x7b, 0x74, 0x6e,
                0x69, 0x68, 0x68, 0x6b, 0x6f, 0x74, 0x7b, 0x81, 0x87, 0x8c, 0x8e, 0x8e, 0x8e, 0x8e, 0x8d, 0x8c,
                0x8a, 0x87, 0x85, 0x83, 0x81, 0x7e, 0x7a, 0x76, 0x73, 0x71, 0x70, 0x6f, 0x70, 0x71, 0x74, 0x79,
                0x7e, 0x83, 0x88, 0x8c, 0x90, 0x93, 0x95, 0x94, 0x92, 0x8d, 0x87, 0x81, 0x7b, 0x76, 0x71, 0x6e,
                0x6d, 0x6e, 0x70, 0x74, 0x77, 0x7a, 0x7e, 0x81, 0x83, 0x86, 0x87, 0x89, 0x8a, 0x8b, 0x8d, 0x8d,
                0x8d, 0x8b, 0x88, 0x85, 0x81, 0x7d, 0x78, 0x74, 0x71, 0x6f, 0x6e, 0x6f, 0x70, 0x73, 0x77, 0x7c,
                0x82, 0x87, 0x8c, 0x8f, 0x92, 0x92, 0x92, 0x8f, 0x8b, 0x86, 0x81, 0x7d, 0x79, 0x77, 0x75, 0x74,
                0x74, 0x74, 0x75, 0x76, 0x77, 0x79, 0x7a, 0x7c, 0x7f, 0x83, 0x86, 0x89, 0x8c, 0x8e, 0x8f, 0x8f,
                0x8d, 0x8b, 0x87, 0x83, 0x7e, 0x7a, 0x76, 0x72, 0x70, 0x6e, 0x6f, 0x71, 0x74, 0x77, 0x7c, 0x80,
                0x85, 0x8a, 0x8c, 0x8e, 0x8e, 0x8d, 0x8b, 0x89, 0x87, 0x84, 0x81, 0x7f, 0x7d, 0x7b, 0x7a, 0x78,
                0x76, 0x74, 0x74, 0x74, 0x75, 0x76, 0x79, 0x7c, 0x80, 0x84, 0x88, 0x8b, 0x8d, 0x8f, 0x8f, 0x8e,
                0x8b, 0x88, 0x84, 0x80, 0x7b, 0x78, 0x74, 0x72, 0x71, 0x71, 0x72, 0x75, 0x78, 0x7c, 0x7f, 0x82,
                0x85, 0x87, 0x89, 0x89, 0x89, 0x89, 0x89, 0x88, 0x88, 0x86, 0x84, 0x82, 0x7f, 0x7c, 0x79, 0x76,
                0x74, 0x72, 0x72, 0x73, 0x75, 0x78, 0x7b, 0x7f, 0x83, 0x87, 0x8a, 0x8c, 0x8d, 0x8d, 0x8c, 0x8b,
                0x88, 0x85, 0x81, 0x7d, 0x7a, 0x77, 0x75, 0x75, 0x74, 0x75, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 0x80,
                0x82, 0x84, 0x86, 0x88, 0x89, 0x8a, 0x8a, 0x8a, 0x89, 0x87, 0x84, 0x81, 0x7d, 0x7a, 0x77, 0x75,
                0x73, 0x73, 0x73, 0x75, 0x78, 0x7b, 0x7e, 0x81, 0x84, 0x87, 0x8a, 0x8b, 0x8b, 0x8b, 0x89, 0x87,
                0x85, 0x82, 0x80, 0x7d, 0x7b, 0x7a, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x79, 0x7b, 0x7c, 0x7f,
                0x81, 0x84, 0x87, 0x89, 0x8b, 0x8b, 0x8b, 0x8a, 0x88, 0x85, 0x82, 0x7e, 0x7b, 0x78, 0x76, 0x75,
                0x74, 0x75, 0x76, 0x78, 0x7a, 0x7d, 0x80, 0x82, 0x85, 0x87, 0x88, 0x89, 0x89, 0x88, 0x87, 0x86,
                0x84, 0x83, 0x81, 0x7f, 0x7d, 0x7b, 0x7a, 0x78, 0x77, 0x76, 0x76, 0x77, 0x78, 0x7a, 0x7d, 0x80,
                0x83, 0x86, 0x88, 0x8a, 0x8b, 0x8a, 0x89, 0x87, 0x85, 0x82, 0x80, 0x7d, 0x7a, 0x78, 0x77, 0x76,
                0x76, 0x77, 0x78, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x83, 0x85, 0x86, 0x87, 0x87, 0x87, 0x87, 0x86,
                0x85, 0x84, 0x82, 0x80, 0x7d, 0x7b, 0x79, 0x77, 0x76, 0x76, 0x76, 0x78, 0x7a, 0x7d, 0x7f, 0x82,
                0x85, 0x87, 0x88, 0x89, 0x89, 0x88, 0x87, 0x85, 0x83, 0x81, 0x7f, 0x7d, 0x7b, 0x7a, 0x79, 0x78,
                0x78, 0x79, 0x79, 0x7a, 0x7c, 0x7d, 0x7f, 0x81, 0x82, 0x84, 0x86, 0x87, 0x88, 0x88, 0x88, 0x87,
                0x85, 0x83, 0x81, 0x7e, 0x7b, 0x79, 0x78, 0x77, 0x77, 0x77, 0x78, 0x7a, 0x7c, 0x7e, 0x81, 0x83,
                0x84, 0x86, 0x87, 0x87, 0x87, 0x86, 0x85, 0x84, 0x83, 0x81, 0x7f, 0x7e, 0x7c, 0x7b, 0x7a, 0x79,
                0x79, 0x79, 0x79, 0x7a, 0x7b, 0x7d, 0x7f, 0x81, 0x83, 0x85, 0x87, 0x88, 0x88, 0x88, 0x87, 0x85,
                0x83, 0x81, 0x7f, 0x7d, 0x7b, 0x79, 0x78, 0x78, 0x78, 0x79, 0x7a, 0x7c, 0x94, 0x94, 0x93, 0x90,
                0x8c, 0x88, 0x84, 0x7f, 0x7b, 0x78, 0x76, 0x75, 0x75, 0x76, 0x77, 0x79, 0x7a, 0x7c, 0x7d, 0x7e,
                0x7f, 0x80, 0x81, 0x80, 0x80, 0x7f, 0x7e, 0x7e, 0x7e, 0x7f, 0x80, 0x82, 0x81, 0x80, 0x7c, 0x7e,
                0x81, 0x83, 0x84, 0x8d, 0x9b, 0x97, 0x92, 0x90, 0x88, 0x7b, 0x76, 0x71, 0x92, 0x90, 0x73, 0x58,
                0x43, 0x3e, 0x4d, 0x60, 0x76, 0xa7, 0xa7, 0xa1, 0x8c, 0x8c, 0xb0, 0xbd, 0x93, 0x9f, 0xa5, 0xa7,
                0x94, 0x55, 0x20, 0x16, 0x3f, 0x54, 0x6c, 0x60, 0x64, 0x82, 0x96, 0xa7, 0xb9, 0xc0, 0xc0, 0xa9,
                0x9d, 0x98, 0x8d, 0x81, 0x7a, 0x7d, 0x82, 0x81, 0x7d, 0x79, 0x76, 0x76, 0x78, 0x7a, 0x78, 0x71,
                0x65, 0x57, 0x4b, 0x47, 0x4e, 0x60, 0x7a, 0x97, 0xaf, 0xbb, 0xbb, 0xad, 0x97, 0x7f, 0x6a, 0x5c,
                0x55, 0x54, 0x57, 0x5a, 0x5d, 0x63, 0x6e, 0x81, 0x9c, 0xbc, 0xd8, 0xf0, 0xf9, 0xf1, 0xd9, 0xb8,
                0x8f, 0x67, 0x46, 0x32, 0x24, 0x1e, 0x1e, 0x22, 0x2a, 0x36, 0x44, 0x5a, 0x73, 0x8c, 0xa3, 0xb3,
                0xbd, 0xbf, 0xbc, 0xb4, 0xaa, 0xa1, 0x99, 0x94, 0x91, 0x90, 0x8f, 0x8e, 0x8b, 0x86, 0x7d, 0x73,
                0x67, 0x5c, 0x55, 0x53, 0x56, 0x60, 0x6c, 0x7a, 0x87, 0x91, 0x97, 0x9a, 0x9b, 0x9b, 0x99, 0x95,
                0x8b, 0x7c, 0x68, 0x52, 0x40, 0x33, 0x32, 0x3e, 0x54, 0x74, 0x96, 0xb4, 0xc8, 0xd4, 0xd8, 0xd6,
                0xcf, 0xc4, 0xb9, 0xa7, 0x91, 0x76, 0x59, 0x40, 0x2f, 0x26, 0x29, 0x37, 0x4a, 0x62, 0x78, 0x88,
                0x91, 0x94, 0x93, 0x91, 0x8e, 0x8d, 0x8c, 0x8c, 0x8a, 0x89, 0x87, 0x87, 0x88, 0x8a, 0x8e, 0x90,
                0x90, 0x8d, 0x87, 0x7f, 0x76, 0x6e, 0x68, 0x66, 0x67, 0x6c, 0x76, 0x82, 0x90, 0x9f, 0xac, 0xb3,
                0xb4, 0xad, 0x9e, 0x89, 0x72, 0x5b, 0x48, 0x3d, 0x35, 0x32, 0x36, 0x3e, 0x49, 0x5b, 0x73, 0x8f,
                0xac, 0xc4, 0xd8, 0xe2, 0xe0, 0xd3, 0xc0, 0xa8, 0x8f, 0x7a, 0x6a, 0x61, 0x5b, 0x58, 0x56, 0x56,
                0x58, 0x5e, 0x67, 0x72, 0x7e, 0x88, 0x8d, 0x8c, 0x87, 0x7e, 0x76, 0x71, 0x71, 0x74, 0x7b, 0x82,
                0x87, 0x8a, 0x89, 0x87, 0x83, 0x7f, 0x7c, 0x7b, 0x7a, 0x7a, 0x7c, 0x80, 0x86, 0x8e, 0x98, 0xa2,
                0xac, 0xb2, 0xb4, 0xb0, 0xa6, 0x98, 0x86, 0x71, 0x5c, 0x49, 0x3b, 0x2e, 0x28, 0x2a, 0x34, 0x43,
                0x5a, 0x74, 0x8e, 0xa4, 0xb4, 0xbf, 0xc0, 0xc0, 0xbf, 0xb8, 0xaf, 0xa2, 0x94, 0x84, 0x75, 0x6a,
                0x64, 0x63, 0x68, 0x70, 0x79, 0x80, 0x82, 0x80, 0x7c, 0x76, 0x71, 0x6e, 0x6e, 0x70, 0x73, 0x76,
                0x78, 0x7a, 0x7c, 0x7f, 0x83, 0x87, 0x8b, 0x8c, 0x89, 0x83, 0x7a, 0x71, 0x6b, 0x68, 0x6b, 0x74,
                0x80, 0x8e, 0x9c, 0xa9, 0xb2, 0xb8, 0xb9, 0xb6, 0xae, 0xa0, 0x8e, 0x79, 0x63, 0x4f, 0x40, 0x37,
                0x33, 0x37, 0x40, 0x4e, 0x60, 0x74, 0x88, 0x9a, 0xa9, 0xb4, 0xb8, 0xb7, 0xb0, 0xa4, 0x96, 0x87,
                0x7b, 0x73, 0x6e, 0x6d, 0x6e, 0x71, 0x74, 0x78, 0x7c, 0x81, 0x86, 0x8a, 0x8e, 0x8f, 0x8d, 0x89,
                0x84, 0x7e, 0x7b, 0x7a, 0x7b, 0x7e, 0x81, 0x81, 0x7f, 0x7a, 0x72, 0x6b, 0x64, 0x60, 0x5f, 0x60,
                0x64, 0x69, 0x70, 0x78, 0x83, 0x90, 0x9e, 0xac, 0xb8, 0xbf, 0xc0, 0xbb, 0xaf, 0xa0, 0x8e, 0x7c,
                0x6c, 0x5e, 0x54, 0x4e, 0x4c, 0x4e, 0x55, 0x5e, 0x6a, 0x78, 0x84, 0x8e, 0x94, 0x96, 0x94, 0x8f,
                0x89, 0x82, 0x7c, 0x77, 0x74, 0x72, 0x72, 0x75, 0x79, 0x7f, 0x86, 0x8d, 0x92, 0x95, 0x95, 0x93,
                0x8f, 0x8b, 0x89, 0x87, 0x88, 0x89, 0x8a, 0x8a, 0x88, 0x85, 0x81, 0x7c, 0x77, 0x71, 0x6a, 0x63,
                0x5b, 0x53, 0x4d, 0x4b, 0x4e, 0x56, 0x64, 0x76, 0x89, 0x9c, 0xac, 0xb8, 0xbf, 0xc0, 0xc0, 0xba,
                0xb1, 0xa3, 0x94, 0x82, 0x71, 0x61, 0x56, 0x50, 0x50, 0x55, 0x5e, 0x69, 0x74, 0x7e, 0x86, 0x8c,
                0x8e, 0x8f, 0x8d, 0x89, 0x83, 0x7c, 0x75, 0x6f, 0x6c, 0x6b, 0x6e, 0x73, 0x79, 0x80, 0x86, 0x8b,
                0x8e, 0x90, 0x92, 0x92, 0x92, 0x92, 0x90, 0x8f, 0x8d, 0x8c, 0x8b, 0x8b, 0x8b, 0x8a, 0x88, 0x84,
                0x7c, 0x73, 0x69, 0x60, 0x58, 0x54, 0x53, 0x55, 0x5a, 0x62, 0x6c, 0x77, 0x84, 0x91, 0x9e, 0xa8,
                0xaf, 0xb2, 0xaf, 0xa8, 0x9d, 0x90, 0x83, 0x78, 0x70, 0x6a, 0x67, 0x67, 0x69, 0x6d, 0x73, 0x7a,
                0x81, 0x88, 0x8e, 0x91, 0x91, 0x8f, 0x8a, 0x83, 0x7d, 0x77, 0x73, 0x70, 0x6f, 0x6f, 0x70, 0x71,
                0x72, 0x74, 0x76, 0x79, 0x7c, 0x7e, 0x80, 0x82, 0x84, 0x86, 0x8a, 0x8f, 0x94, 0x9a, 0x9f, 0xa1,
                0xa1, 0x9e, 0x99, 0x91, 0x88, 0x7d, 0x72, 0x68, 0x5e, 0x56, 0x51, 0x50, 0x54, 0x5c, 0x67, 0x74,
                0x81, 0x8d, 0x96, 0x9c, 0x9e, 0x9d, 0x99, 0x93, 0x8c, 0x84, 0x7c, 0x75, 0x6f, 0x6c, 0x6c, 0x70,
                0x77, 0x80, 0x8a, 0x92, 0x98, 0x9b, 0x9c, 0x99, 0x95, 0x8f, 0x89, 0x82, 0x7b, 0x74, 0x6e, 0x69,
                0x65, 0x64, 0x65, 0x67, 0x6a, 0x6d, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7c, 0x81, 0x87, 0x8e, 0x94,
                0x9a, 0x9f, 0xa2, 0xa4, 0xa5, 0xa3, 0x9e, 0x97, 0x8e, 0x83, 0x77, 0x6c, 0x63, 0x5d, 0x5a, 0x5a,
                0x5d, 0x63, 0x6a, 0x72, 0x7a, 0x82, 0x89, 0x8f, 0x92, 0x92, 0x8f, 0x89, 0x83, 0x7c, 0x77, 0x74,
                0x73, 0x74, 0x77, 0x7c, 0x81, 0x86, 0x8b, 0x90, 0x94, 0x97, 0x98, 0x97, 0x94, 0x8f, 0x89, 0x83,
                0x7d, 0x78, 0x75, 0x73, 0x71, 0x70, 0x6f, 0x6d, 0x6c, 0x6a, 0x6a, 0x6a, 0x6c, 0x6e, 0x72, 0x76,
                0x7b, 0x81, 0x88, 0x8f, 0x96, 0x9d, 0xa2, 0xa4, 0xa3, 0x9f, 0x98, 0x90, 0x86, 0x7d, 0x75, 0x6e,
                0x68, 0x65, 0x65, 0x67, 0x6c, 0x72, 0x7a, 0x82, 0x89, 0x8e, 0x91, 0x91, 0x8e, 0x89, 0x83, 0x7c,
                0x76, 0x70, 0x6c, 0x6a, 0x69, 0x6b, 0x6f, 0x74, 0x7c, 0x83, 0x8a, 0x90, 0x94, 0x96, 0x96, 0x95,
                0x92, 0x90, 0x8d, 0x8a, 0x88, 0x85, 0x82, 0x7f, 0x7d, 0x7b, 0x79, 0x77, 0x75, 0x72, 0x70, 0x6d,
                0x6a, 0x69, 0x69, 0x6b, 0x70, 0x76, 0x7c, 0x83, 0x8a, 0x8f, 0x93, 0x96, 0x96, 0x95, 0x92, 0x8e,
                0x88, 0x82, 0x7c, 0x77, 0x74, 0x73, 0x75, 0x78, 0x7d, 0x83, 0x88, 0x8c, 0x8f, 0x90, 0x90, 0x8d,
                0x89, 0x84, 0x7d, 0x75, 0x6e, 0x68, 0x64, 0x62, 0x63, 0x66, 0x6a, 0x70, 0x76, 0x7c, 0x82, 0x87,
                0x8c, 0x91, 0x94, 0x96, 0x97, 0x97, 0x96, 0x94, 0x92, 0x90, 0x8e, 0x8b, 0x88, 0x84, 0x7f, 0x7a,
                0x74, 0x6f, 0x6a, 0x67, 0x66, 0x66, 0x68, 0x6b, 0x6f, 0x74, 0x7a, 0x80, 0x86, 0x8b, 0x8f, 0x91,
                0x90, 0x8e, 0x8a, 0x85, 0x80, 0x7c, 0x7a, 0x78, 0x79, 0x7b, 0x7e, 0x82, 0x87, 0x8b, 0x90, 0x93,
                0x95, 0x94, 0x91, 0x8d, 0x86, 0x7f, 0x78, 0x71, 0x6c, 0x68, 0x66, 0x66, 0x66, 0x65, 0x64, 0x68,
                0x6d, 0x73, 0x78, 0x85, 0x95, 0x98, 0x9a, 0x9e, 0x9c, 0x95, 0x93, 0x91, 0xab, 0xab, 0x98, 0x82,
                0x6d, 0x5d, 0x5c, 0x5b, 0x5f, 0x7c, 0x7a, 0x77, 0x6c, 0x6f, 0x8d, 0x99, 0x7f, 0x91, 0x9f, 0xad,
                0xaa, 0x83, 0x5a, 0x49, 0x57, 0x58, 0x5b, 0x4a, 0x4a, 0x62, 0x76, 0x89, 0x9d, 0xa9, 0xb3, 0xa9,
                0xaa, 0xaf, 0xad, 0xa7, 0xa1, 0x9e, 0x99, 0x8e, 0x81, 0x73, 0x68, 0x62, 0x62, 0x66, 0x6e, 0x76,
                0x7e, 0x82, 0x81, 0x7c, 0x73, 0x68, 0x5d, 0x55, 0x52, 0x53, 0x59, 0x63, 0x6f, 0x7c, 0x88, 0x93,
                0x9c, 0xa3, 0xa7, 0xa8, 0xa7, 0xa3, 0x9c, 0x93, 0x89, 0x7f, 0x75, 0x6d, 0x66, 0x62, 0x61, 0x64,
                0x6a, 0x74, 0x82, 0x91, 0xa1, 0xb0, 0xbb, 0xc0, 0xbe, 0xb4, 0xa3, 0x8d, 0x75, 0x5f, 0x4c, 0x40,
                0x3b, 0x3b, 0x40, 0x46, 0x51, 0x5c, 0x67, 0x72, 0x7d, 0x88, 0x94, 0xa0, 0xab, 0xb4, 0xb8, 0xb8,
                0xb2, 0xa6, 0x97, 0x85, 0x75, 0x69, 0x62, 0x61, 0x66, 0x6f, 0x78, 0x81, 0x88, 0x8b, 0x8b, 0x89,
                0x87, 0x85, 0x85, 0x86, 0x87, 0x87, 0x85, 0x81, 0x7a, 0x72, 0x6a, 0x65, 0x63, 0x65, 0x6b, 0x75,
                0x80, 0x8c, 0x95, 0x9b, 0x9e, 0x9c, 0x96, 0x8c, 0x81, 0x75, 0x6a, 0x60, 0x59, 0x55, 0x55, 0x58,
                0x5f, 0x68, 0x75, 0x83, 0x92, 0xa0, 0xad, 0xb8, 0xbf, 0xc0, 0xc0, 0xbb, 0xb1, 0xa4, 0x93, 0x81,
                0x6f, 0x5e, 0x4f, 0x46, 0x42, 0x43, 0x4a, 0x56, 0x64, 0x71, 0x7d, 0x86, 0x8a, 0x8b, 0x88, 0x85,
                0x81, 0x7f, 0x7e, 0x7f, 0x80, 0x82, 0x82, 0x82, 0x81, 0x80, 0x81, 0x84, 0x88, 0x8e, 0x95, 0x9b,
                0x9e, 0x9e, 0x9a, 0x92, 0x89, 0x80, 0x77, 0x72, 0x70, 0x72, 0x76, 0x7c, 0x82, 0x87, 0x89, 0x88,
                0x86, 0x82, 0x7d, 0x79, 0x75, 0x72, 0x6f, 0x6b, 0x67, 0x62, 0x5d, 0x58, 0x56, 0x56, 0x5b, 0x63,
                0x6e, 0x7c, 0x8b, 0x9b, 0xa8, 0xb3, 0xbc, 0xc0, 0xc0, 0xbf, 0xb9, 0xaf, 0xa3, 0x95, 0x87, 0x78,
                0x6c, 0x61, 0x5a, 0x56, 0x55, 0x56, 0x5a, 0x60, 0x68, 0x70, 0x78, 0x7f, 0x85, 0x89, 0x8b, 0x8a,
                0x85, 0x7f, 0x76, 0x6e, 0x67, 0x63, 0x62, 0x66, 0x6f, 0x7a, 0x87, 0x94, 0x9f, 0xa7, 0xab, 0xab,
                0xa8, 0xa3, 0x9b, 0x94, 0x8c, 0x84, 0x7d, 0x76, 0x6f, 0x6a, 0x66, 0x64, 0x66, 0x6b, 0x74, 0x7e,
                0x89, 0x92, 0x98, 0x9a, 0x97, 0x8f, 0x85, 0x79, 0x6d, 0x63, 0x5c, 0x58, 0x57, 0x58, 0x5c, 0x62,
                0x69, 0x72, 0x7b, 0x86, 0x90, 0x9b, 0xa5, 0xac, 0xb1, 0xb3, 0xb0, 0xaa, 0xa2, 0x97, 0x8b, 0x7f,
                0x74, 0x6b, 0x65, 0x61, 0x5f, 0x60, 0x63, 0x68, 0x6e, 0x75, 0x7c, 0x83, 0x89, 0x8e, 0x90, 0x90,
                0x8f, 0x8c, 0x88, 0x84, 0x7f, 0x7b, 0x78, 0x75, 0x72, 0x71, 0x70, 0x71, 0x74, 0x78, 0x7c, 0x81,
                0x85, 0x88, 0x88, 0x87, 0x83, 0x7e, 0x7a, 0x76, 0x75, 0x77, 0x7b, 0x82, 0x89, 0x91, 0x97, 0x9c,
                0x9f, 0xa0, 0x9e, 0x9c, 0x97, 0x91, 0x8a, 0x82, 0x79, 0x6f, 0x64, 0x5b, 0x53, 0x4f, 0x4e, 0x52,
                0x59, 0x64, 0x71, 0x7e, 0x89, 0x92, 0x97, 0x9a, 0x99, 0x96, 0x92, 0x8d, 0x88, 0x84, 0x80, 0x7d,
                0x7a, 0x79, 0x78, 0x79, 0x7c, 0x80, 0x85, 0x8b, 0x91, 0x95, 0x97, 0x98, 0x96, 0x92, 0x8d, 0x87,
                0x81, 0x7b, 0x75, 0x70, 0x6c, 0x6a, 0x68, 0x69, 0x6a, 0x6d, 0x71, 0x75, 0x79, 0x7c, 0x7e, 0x7f,
                0x7f, 0x7d, 0x7b, 0x79, 0x77, 0x76, 0x76, 0x76, 0x78, 0x7b, 0x7e, 0x83, 0x88, 0x8f, 0x96, 0x9e,
                0xa6, 0xac, 0xb1, 0xb2, 0xaf, 0xa8, 0x9d, 0x90, 0x81, 0x73, 0x65, 0x5a, 0x52, 0x4d, 0x4a, 0x4b,
                0x4e, 0x53, 0x5b, 0x64, 0x6e, 0x7a, 0x85, 0x90, 0x98, 0x9d, 0x9f, 0x9e, 0x99, 0x92, 0x8a, 0x82,
                0x7c, 0x78, 0x76, 0x77, 0x7a, 0x7e, 0x83, 0x88, 0x8c, 0x90, 0x93, 0x94, 0x94, 0x93, 0x91, 0x8e,
                0x89, 0x83, 0x7d, 0x77, 0x72, 0x6d, 0x6b, 0x6a, 0x6b, 0x6e, 0x71, 0x74, 0x77, 0x79, 0x7a, 0x7b,
                0x7b, 0x7b, 0x7b, 0x7a, 0x7a, 0x79, 0x79, 0x79, 0x7a, 0x7c, 0x80, 0x84, 0x8a, 0x8f, 0x94, 0x98,
                0x9b, 0x9c, 0x9b, 0x98, 0x95, 0x91, 0x8c, 0x86, 0x81, 0x7b, 0x75, 0x6f, 0x6a, 0x66, 0x64, 0x64,
                0x66, 0x6a, 0x71, 0x78, 0x7f, 0x86, 0x8b, 0x8e, 0x8f, 0x8e, 0x8c, 0x89, 0x85, 0x81, 0x7e, 0x7a,
                0x77, 0x75, 0x73, 0x71, 0x71, 0x73, 0x77, 0x7b, 0x81, 0x87, 0x8d, 0x90, 0x92, 0x92, 0x8f, 0x8c,
                0x87, 0x83, 0x7f, 0x7d, 0x7c, 0x7c, 0x7d, 0x7f, 0x82, 0x84, 0x87, 0x89, 0x8b, 0x8b, 0x8b, 0x88,
                0x85, 0x7f, 0x79, 0x72, 0x6a, 0x64, 0x5f, 0x5d, 0x5d, 0x60, 0x65, 0x6c, 0x74, 0x7e, 0x87, 0x8f,
                0x97, 0x9c, 0xa0, 0xa2, 0xa1, 0x9e, 0x98, 0x92, 0x8a, 0x82, 0x7a, 0x74, 0x70, 0x6e, 0x6f, 0x71,
                0x75, 0x79, 0x7e, 0x83, 0x87, 0x8a, 0x8d, 0x8f, 0x90, 0x8f, 0x8d, 0x89, 0x83, 0x7d, 0x75, 0x6e,
                0x68, 0x63, 0x61, 0x61, 0x63, 0x66, 0x6b, 0x71, 0x77, 0x7d, 0x82, 0x87, 0x8c, 0x90, 0x93, 0x95,
                0x96, 0x96, 0x95, 0x92, 0x90, 0x8d, 0x8a, 0x88, 0x88, 0x88, 0x89, 0x8a, 0x8b, 0x8a, 0x89, 0x86,
                0x82, 0x7c, 0x76, 0x70, 0x6a, 0x65, 0x61, 0x5f, 0x5d, 0x5e, 0x60, 0x63, 0x69, 0x70, 0x79, 0x81,
                0x8a, 0x91, 0x97, 0x9b, 0x9c, 0x9c, 0x99, 0x95, 0x91, 0x8c, 0x87, 0x83, 0x80, 0x7d, 0x7c, 0x7b,
                0x7c, 0x7e, 0x81, 0x84, 0x86, 0x88, 0x89, 0x89, 0x87, 0x84, 0x80, 0x7c, 0x77, 0x73, 0x70, 0x6e,
                0x6d, 0x6d, 0x6e, 0x6f, 0x71, 0x73, 0x76, 0x79, 0x7c, 0x7f, 0x82, 0x85, 0x87, 0x87, 0x87, 0x87,
                0x85, 0x84, 0x84, 0x84, 0x85, 0x87, 0x89, 0x8c, 0x8e, 0x90, 0x91, 0x90, 0x8f, 0x8d, 0x8b, 0x88,
                0x84, 0x80, 0x7b, 0x76, 0x70, 0x6b, 0x67, 0x64, 0x63, 0x64, 0x67, 0x6c, 0x72, 0x79, 0x80, 0x86,
                0x8c, 0x90, 0x93, 0x94, 0x94, 0x92, 0x8f, 0x8a, 0x85, 0x7f, 0x7a, 0x74, 0x71, 0x6e, 0x6e, 0x70,
                0x73, 0x79, 0x7f, 0x85, 0x8b, 0x90, 0x93, 0x95, 0x95, 0x94, 0x92, 0x8e, 0x89, 0x84, 0x7f, 0x79,
                0x75, 0x71, 0x6f, 0x6e, 0x6f, 0x72, 0x75, 0x78, 0x7b, 0x7e, 0x7f, 0x80, 0x7f, 0x7e, 0x7d, 0x7c,
                0x7a, 0x79, 0x79, 0x79, 0x7a, 0x7b, 0x7d, 0x7f, 0x82, 0x86, 0x89, 0x8d, 0x90, 0x93, 0x93, 0x93,
                0x91, 0x8e, 0x89, 0x85, 0x80, 0x7c, 0x79, 0x77, 0x76, 0x75, 0x75, 0x76, 0x77, 0x79, 0x7b, 0x7d,
                0x80, 0x82, 0x84, 0x85, 0x84, 0x83, 0x80, 0x7d, 0x79, 0x76, 0x72, 0x70, 0x6f, 0x6e, 0x6f, 0x72,
                0x75, 0x78, 0x7d, 0x82, 0x87, 0x8c, 0x90, 0x94, 0x97, 0x99, 0x99, 0x98, 0x95, 0x92, 0x8d, 0x89,
                0x85, 0x82, 0x7f, 0x7c, 0x7a, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e,
                0x6d, 0x6d, 0x6e, 0x6f, 0x72, 0x75, 0x7a, 0x80, 0x86, 0x8c, 0x92, 0x96, 0x99, 0x9a, 0x9a, 0x98,
                0x95, 0x92, 0x8d, 0x88, 0x83, 0x7f, 0x7a, 0x77, 0x74, 0x73, 0x74, 0x75, 0x78, 0x7c, 0x7f, 0x82,
                0x85, 0x86, 0x86, 0x84, 0x82, 0x7f, 0x7b, 0x78, 0x74, 0x71, 0x6f, 0x6d, 0x6d, 0x6d, 0x6f, 0x72,
                0x75, 0x7a, 0x7f, 0x85, 0x8a, 0x8e, 0x92, 0x94, 0x95, 0x95, 0x93, 0x91, 0x8e, 0x8b, 0x88, 0x85,
                0x82, 0x80, 0x7e, 0x7c, 0x7b, 0x7a, 0x7a, 0x7b, 0x7c, 0x7c, 0x7d, 0x7c, 0x7c, 0x7a, 0x78, 0x76,
                0x74, 0x73, 0x72, 0x72, 0x72, 0x74, 0x76, 0x79, 0x7c, 0x80, 0x84, 0x88, 0x8c, 0x8f, 0x91, 0x92,
                0x91, 0x8f, 0x8c, 0x88, 0x83, 0x7f, 0x7b, 0x78, 0x76, 0x75, 0x76, 0x78, 0x7a, 0x7d, 0x80, 0x83,
                0x86, 0x89, 0x8b, 0x8c, 0x8b, 0x8a, 0x88, 0x84, 0x80, 0x7c, 0x77, 0x74, 0x71, 0x6f, 0x6f, 0x70,
                0x72, 0x74, 0x77, 0x7a, 0x7c, 0x7f, 0x81, 0x83, 0x84, 0x86, 0x86, 0x87, 0x86, 0x86, 0x85, 0x84,
                0x84, 0x84, 0x84, 0x85, 0x85, 0x86, 0x87, 0x87, 0x87, 0x86, 0x85, 0x83, 0x82, 0x80, 0x7e, 0x7c,
                0x7a, 0x79, 0x78, 0x77, 0x77, 0x77, 0x77, 0x79, 0x7a, 0x7c, 0x7e, 0x80, 0x81, 0x82, 0x82, 0x81,
                0x7f, 0x7e, 0x7c, 0x7a, 0x79, 0x78, 0x77, 0x78, 0x79, 0x7a, 0x7d, 0x80, 0x84, 0x88, 0x8c, 0x90,
                0x94, 0x96, 0x97, 0x97, 0x94, 0x91, 0x8d, 0x88, 0x82, 0x7d, 0x78, 0x74, 0x70, 0x6e, 0x6c, 0x6b,
                0x6c, 0x6d, 0x6f, 0x72, 0x75, 0x78, 0x7b, 0x7d, 0x7f, 0x7f, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x80,
                0x82, 0x84, 0x86, 0x88, 0x8b, 0x8d, 0x8f, 0x90, 0x90, 0x90, 0x8f, 0x8e, 0x8b, 0x88, 0x84, 0x80,
                0x7b, 0x78, 0x74, 0x72, 0x71, 0x71, 0x72, 0x74, 0x76, 0x78, 0x7b, 0x7d, 0x7e, 0x80, 0x80, 0x81,
                0x80, 0x7f, 0x7e, 0x7c, 0x7b, 0x79, 0x78, 0x77, 0x78, 0x79, 0x7b, 0x7e, 0x81, 0x85, 0x89, 0x8c,
                0x8f, 0x91, 0x92, 0x93, 0x93, 0x91, 0x8f, 0x8c, 0x88, 0x84, 0x80, 0x7b, 0x77, 0x74, 0x71, 0x70,
                0x70, 0x70, 0x71, 0x73, 0x75, 0x76, 0x78, 0x7a, 0x7b, 0x7d, 0x7e, 0x7f, 0x80, 0x80, 0x81, 0x81,
                0x82, 0x82, 0x83, 0x84, 0x85, 0x87, 0x88, 0x89, 0x8a, 0x8a, 0x8a, 0x88, 0x86, 0x84, 0x82, 0x7f,
                0x7d, 0x7c, 0x7a, 0x7a, 0x79, 0x7a, 0x7b, 0x7c, 0x7e, 0x80, 0x81, 0x83, 0x84, 0x85, 0x84, 0x83,
                0x82, 0x7f, 0x7d, 0x7a, 0x77, 0x75, 0x74, 0x74, 0x74, 0x75, 0x77, 0x79, 0x7c, 0x7f, 0x82, 0x85,
                0x87, 0x89, 0x8b, 0x8b, 0x8b, 0x8a, 0x89, 0x87, 0x85, 0x83, 0x81, 0x80, 0x7f, 0x7f, 0x7f, 0x7f,
                0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7e, 0x7d, 0x7b, 0x7a, 0x78, 0x77, 0x77,
                0x77, 0x77, 0x78, 0x7a, 0x7c, 0x7e, 0x7f, 0x81, 0x82, 0x83, 0x84, 0x84, 0x84, 0x84, 0x83, 0x83,
                0x82, 0x81, 0x80, 0x80, 0x80, 0x81, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8b, 0x8c, 0x8c, 0x8a, 0x88,
                0x86, 0x82, 0x7f, 0x7b, 0x77, 0x73, 0x6f, 0x6d, 0x6b, 0x6b, 0x6b, 0x6d, 0x70, 0x74, 0x78, 0x7c,
                0x80, 0x84, 0x87, 0x89, 0x8b, 0x8c, 0x8c, 0x8c, 0x8b, 0x8a, 0x89, 0x87, 0x86, 0x85, 0x84, 0x83,
                0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7c, 0x7a, 0x79, 0x77, 0x76, 0x76,
                0x76, 0x76, 0x76, 0x77, 0x78, 0x79, 0x7b, 0x7c, 0x7e, 0x7f, 0x80, 0x81, 0x81, 0x82, 0x82, 0x81,
                0x81, 0x82, 0x82, 0x83, 0x84, 0x85, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8b, 0x8b, 0x8a, 0x89, 0x88,
                0x85, 0x83, 0x7f, 0x7c, 0x78, 0x75, 0x73, 0x71, 0x70, 0x6f, 0x70, 0x71, 0x73, 0x75, 0x78, 0x7a,
                0x7d, 0x80, 0x82, 0x84, 0x86, 0x87, 0x87, 0x87, 0x87, 0x86, 0x85, 0x84, 0x84, 0x84, 0x84, 0x84,
                0x84, 0x85, 0x85, 0x85, 0x85, 0x84, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7c, 0x7b, 0x79, 0x79, 0x78,
                0x78, 0x79, 0x7a, 0x7c, 0x7d, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x81, 0x80, 0x7e, 0x7d, 0x7c,
                0x7a, 0x7a, 0x79, 0x7a, 0x7b, 0x7c, 0x7e, 0x80, 0x82, 0x85, 0x87, 0x8a, 0x8b, 0x8c, 0x8c, 0x8b,
                0x8a, 0x88, 0x86, 0x83, 0x81, 0x7e, 0x7c, 0x7a, 0x79, 0x78, 0x77, 0x77, 0x78, 0x79, 0x7a, 0x7c,
                0x7e, 0x7f, 0x80, 0x81, 0x81, 0x81, 0x80, 0x7f, 0x7e, 0x7d, 0x7d, 0x7c, 0x7c, 0x7c, 0x7c, 0x7d,
                0x7e, 0x7f, 0x80, 0x82, 0x84, 0x85, 0x86, 0x87, 0x87, 0x87, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82,
                0x82, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x79, 0x78, 0x76,
                0x75, 0x74, 0x74, 0x74, 0x75, 0x77, 0x79, 0x7c, 0x7f, 0x82, 0x85, 0x87, 0x89, 0x8b, 0x8c, 0x8d,
                0x8d, 0x8c, 0x8b, 0x89, 0x87, 0x85, 0x83, 0x81, 0x80, 0x7f, 0x7e, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d,
                0x7d, 0x7c, 0x7c, 0x7c, 0x7b, 0x7a, 0x7a, 0x79, 0x79, 0x78, 0x78, 0x78, 0x79, 0x7a, 0x7b, 0x7c,
                0x7e, 0x7f, 0x81, 0x83, 0x84, 0x85, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x85, 0x85,
                0x85, 0x84, 0x84, 0x84, 0x84, 0x83, 0x83, 0x82, 0x81, 0x80, 0x7f, 0x7d, 0x7a, 0x78, 0x76, 0x74,
                0x73, 0x72, 0x72, 0x72, 0x73, 0x75, 0x77, 0x7a, 0x7d, 0x80, 0x83, 0x86, 0x88, 0x8a, 0x8c, 0x8c,
                0x8c, 0x8b, 0x8a, 0x88, 0x86, 0x84, 0x82, 0x80, 0x7f, 0x7e, 0x7e, 0x7d, 0x7d, 0x7e, 0x7e, 0x7f,
                0x7f, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x7a, 0x7a, 0x7a, 0x7b, 0x7c,
                0x7d, 0x7e, 0x7f, 0x80, 0x80, 0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81,
                0x81, 0x82, 0x83, 0x83, 0x84, 0x85, 0x85, 0x85, 0x85, 0x85, 0x84, 0x83, 0x82, 0x80, 0x7e, 0x7d,
                0x7b, 0x7a, 0x79, 0x78, 0x77, 0x77, 0x78, 0x79, 0x7a, 0x7c, 0x7d, 0x7f, 0x80, 0x81, 0x82, 0x82,
                0x83, 0x83, 0x82, 0x82, 0x81, 0x81, 0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x81, 0x82, 0x83,
                0x84, 0x85, 0x85, 0x85, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80, 0x7f, 0x7e, 0x7d, 0x7c, 0x7c, 0x7b,
                0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7a, 0x7a, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e,
                0x80, 0x81, 0x83, 0x85, 0x87, 0x88, 0x8a, 0x8a, 0x8b, 0x8b, 0x8a, 0x89, 0x87, 0x85, 0x83, 0x81,
                0x7f, 0x7d, 0x7b, 0x7a, 0x79, 0x79, 0x79, 0x79, 0x79, 0x7a, 0x7b, 0x7c, 0x7c, 0x7d, 0x7d, 0x7e,
                0x7e, 0x7d, 0x7d, 0x7d, 0x7c, 0x7c, 0x7c, 0x7d, 0x7d, 0x7e, 0x7f, 0x81, 0x82, 0x84, 0x85, 0x86,
                0x88, 0x88, 0x89, 0x89, 0x89, 0x88, 0x87, 0x86, 0x84, 0x82, 0x81, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b,
                0x7b, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x79, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b, 0x7c,
                0x7e, 0x7f, 0x81, 0x83, 0x85, 0x87, 0x88, 0x89, 0x8a, 0x8a, 0x8a, 0x89, 0x88, 0x87, 0x85, 0x83,
                0x81, 0x7f, 0x7d, 0x7c, 0x7b, 0x7a, 0x7a, 0x7a, 0x7a, 0x7b, 0x7c, 0x7d, 0x7d, 0x7e, 0x7f, 0x7f,
                0x7f, 0x7f, 0x7e, 0x7e, 0x7e, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d, 0x7e, 0x7e, 0x7f, 0x80, 0x81, 0x82,
                0x83, 0x83, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x83, 0x83, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81,
                0x80, 0x80, 0x80, 0x7f, 0x7f, 0x7e, 0x7d, 0x7d, 0x7c, 0x7b, 0x7b, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
                0x7b, 0x7c, 0x7d, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x84, 0x85, 0x84, 0x84, 0x83, 0x83, 0x82,
                0x81, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x81, 0x82, 0x83, 0x83, 0x83, 0x83, 0x82,
                0x82, 0x81, 0x80, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x79, 0x79, 0x7a, 0x7a, 0x7b, 0x7c, 0x7d,
                0x7e, 0x7f, 0x80, 0x80, 0x81, 0x81, 0x82, 0x82, 0x82, 0x83, 0x83, 0x83, 0x84, 0x84, 0x85, 0x85,
                0x86, 0x86, 0x86, 0x86, 0x85, 0x84, 0x83, 0x82, 0x80, 0x7e, 0x7d, 0x7b, 0x7a, 0x79, 0x78, 0x78,
                0x78, 0x78, 0x78, 0x79, 0x7a, 0x7b, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x81, 0x82, 0x82, 0x82, 0x82,
                0x82, 0x81, 0x81, 0x82, 0x82, 0x82, 0x83, 0x83, 0x84, 0x84, 0x85, 0x85, 0x86, 0x85, 0x85, 0x85,
                0x84, 0x82, 0x81, 0x80, 0x7e, 0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x78, 0x78, 0x78, 0x78, 0x79, 0x7a,
                0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x81, 0x82, 0x83, 0x83, 0x84, 0x85, 0x85, 0x86, 0x86,
                0x86, 0x86, 0x86, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7d, 0x7b, 0x7a, 0x7a, 0x79,
                0x79, 0x7a, 0x7a, 0x7b, 0x7c, 0x7d, 0x7d, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f,
                0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x81, 0x81, 0x82, 0x83, 0x84, 0x84, 0x85, 0x85, 0x85, 0x84,
                0x84, 0x83, 0x82, 0x81, 0x80, 0x7f, 0x7f, 0x7e, 0x7d, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
                0x7d, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d, 0x7d, 0x7e, 0x7e, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x81, 0x82,
                0x83, 0x83, 0x84, 0x84, 0x84, 0x84, 0x84, 0x83, 0x83, 0x82, 0x81, 0x80, 0x7f, 0x7f, 0x7e, 0x7e,
                0x7e, 0x7e, 0x7e, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x81, 0x80, 0x80, 0x7f, 0x7f, 0x7e, 0x7d,
                0x7c, 0x7b, 0x7b, 0x7b, 0x7b, 0x7b, 0x7c, 0x7c, 0x7d, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85,
                0x85, 0x85, 0x85, 0x85, 0x85, 0x84, 0x83, 0x83, 0x82, 0x82, 0x81, 0x81, 0x80, 0x80, 0x7f, 0x7f,
                0x7e, 0x7d, 0x7d, 0x7c, 0x7c, 0x7b, 0x7b, 0x7b, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7c, 0x7d, 0x7e,
                0x7f, 0x80, 0x81, 0x82, 0x82, 0x83, 0x83, 0x84, 0x84, 0x84, 0x84, 0x83, 0x83, 0x83, 0x83, 0x82,
                0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x81, 0x81, 0x80, 0x80, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b,
                0x7a, 0x79, 0x79, 0x79, 0x79, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x80, 0x81, 0x82, 0x83, 0x84,
                0x84, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x83, 0x83, 0x83, 0x82, 0x82, 0x81, 0x80,
                0x80, 0x7f, 0x7e, 0x7d, 0x7c, 0x7c, 0x7b, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7b, 0x7c, 0x7c, 0x7d,
                0x7e, 0x7f, 0x80, 0x81, 0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81,
                0x81, 0x81, 0x82, 0x82, 0x82, 0x82, 0x83, 0x83, 0x83, 0x82, 0x82, 0x81, 0x80, 0x80, 0x7f, 0x7e,
                0x7d, 0x7c, 0x7b, 0x7b, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x80, 0x81,
                0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
                0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
            };

        snd.Create( sizeof( goodsound ), goodsound );
    }
    else
    { //error wav
        static const unsigned char badsound[] =
            {

                0x52, 0x49, 0x46, 0x46, 0x83, 0x05, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
                0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1f, 0x00, 0x00, 0x40, 0x1f, 0x00, 0x00,
                0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x05, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80,
                0x85, 0x82, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x77, 0x7e, 0x81, 0x8a, 0x89, 0x67, 0x6c, 0x79,
                0x80, 0x82, 0x87, 0x80, 0x80, 0x97, 0x7c, 0x7f, 0x93, 0xa2, 0xa8, 0xaa, 0x89, 0x68, 0x4c, 0x3f,
                0x3f, 0x3b, 0x3f, 0x6d, 0x97, 0xbe, 0xe5, 0xf1, 0xb8, 0x80, 0x58, 0x33, 0x2b, 0x65, 0x8f, 0xb0,
                0xc1, 0xd1, 0xb0, 0x7a, 0x4e, 0x2f, 0x0f, 0x2f, 0x6f, 0x96, 0xb8, 0xd9, 0xc1, 0x88, 0x5c, 0x37,
                0x13, 0x37, 0x77, 0xa4, 0xc5, 0xe1, 0xc9, 0x8d, 0x60, 0x3f, 0x23, 0x3b, 0x76, 0xa4, 0xc5, 0xe1,
                0xdd, 0x9a, 0x6b, 0x3b, 0x13, 0x3b, 0x79, 0xa6, 0xcd, 0xdd, 0xb4, 0x77, 0x33, 0x1f, 0x48, 0x7d,
                0xa6, 0xcd, 0xd1, 0x9c, 0x6c, 0x44, 0x23, 0x2b, 0x60, 0x92, 0xc5, 0xe1, 0xb6, 0x7c, 0x44, 0x23,
                0x44, 0x7c, 0xa6, 0xc9, 0xd9, 0xa8, 0x78, 0x4e, 0x2b, 0x1f, 0x4a, 0x80, 0xac, 0xd5, 0xd1, 0x96,
                0x6d, 0x48, 0x27, 0x3b, 0x73, 0x98, 0xc5, 0xe5, 0xc1, 0x87, 0x5c, 0x37, 0x1f, 0x48, 0x80, 0xb0,
                0xcd, 0xc1, 0x8f, 0x68, 0x46, 0x37, 0x44, 0x71, 0x9c, 0xc9, 0xcd, 0xb2, 0x85, 0x5c, 0x42, 0x4e,
                0x68, 0x8b, 0xb0, 0xb8, 0xa4, 0x87, 0x65, 0x48, 0x4a, 0x67, 0x85, 0xa2, 0xba, 0xb4, 0x93, 0x74,
                0x54, 0x3f, 0x3f, 0x56, 0x78, 0x95, 0xb2, 0xac, 0x92, 0x7f, 0x68, 0x5c, 0x6a, 0x79, 0x81, 0x90,
                0xa4, 0x90, 0x75, 0x58, 0x5e, 0x80, 0x91, 0x94, 0x81, 0x60, 0x58, 0x69, 0x79, 0x85, 0x92, 0xa6,
                0xa4, 0x80, 0x5c, 0x44, 0x2f, 0x42, 0x76, 0x98, 0xbc, 0xd1, 0xb6, 0x84, 0x5c, 0x44, 0x3f, 0x4c,
                0x78, 0xa0, 0xc1, 0xd1, 0xc9, 0x98, 0x6d, 0x46, 0x33, 0x33, 0x5c, 0x8d, 0xb6, 0xd1, 0xbe, 0x8f,
                0x65, 0x3f, 0x3b, 0x4c, 0x6e, 0x8f, 0xb6, 0xc9, 0xbc, 0x8f, 0x72, 0x50, 0x3f, 0x52, 0x77, 0x8f,
                0xb2, 0xc5, 0xc5, 0x98, 0x70, 0x52, 0x3b, 0x2f, 0x54, 0x7d, 0x97, 0xb8, 0xd1, 0xd1, 0xa8, 0x87,
                0x60, 0x42, 0x3f, 0x50, 0x54, 0x64, 0x90, 0xb4, 0x97, 0xa6, 0xa0, 0x73, 0x6b, 0x80, 0x6e, 0x5e,
                0x7d, 0xa2, 0x8f, 0x91, 0xa8, 0x7c, 0x54, 0x6c, 0x71, 0x6e, 0x7f, 0x98, 0x90, 0x7f, 0x8a, 0x7c,
                0x54, 0x48, 0x3f, 0x50, 0x6d, 0x91, 0xb4, 0xa6, 0x91, 0x7e, 0x66, 0x58, 0x4c, 0x52, 0x7c, 0x9c,
                0xbe, 0xd5, 0xd9, 0xc5, 0x92, 0x6d, 0x46, 0x2f, 0x37, 0x5e, 0x82, 0xac, 0xbc, 0xa4, 0x83, 0x6b,
                0x4a, 0x44, 0x60, 0x82, 0xa2, 0xba, 0xc5, 0xbc, 0x97, 0x78, 0x56, 0x3f, 0x2f, 0x3b, 0x5e, 0x8c,
                0xb2, 0xc9, 0xc9, 0xb6, 0x8a, 0x6a, 0x4e, 0x3b, 0x3f, 0x6c, 0x93, 0xb6, 0xcd, 0xd1, 0xb4, 0x85,
                0x60, 0x48, 0x33, 0x3f, 0x67, 0x8c, 0xaa, 0xc1, 0xb6, 0x8f, 0x70, 0x56, 0x4c, 0x66, 0x86, 0xa4,
                0xae, 0xb4, 0xac, 0x8b, 0x73, 0x52, 0x3f, 0x42, 0x65, 0x86, 0x9c, 0xa6, 0xa4, 0x88, 0x73, 0x64,
                0x54, 0x5c, 0x76, 0x8c, 0xa0, 0xac, 0xa4, 0x96, 0x89, 0x7f, 0x75, 0x77, 0x83, 0x8f, 0x8f, 0x8e,
                0x84, 0x70, 0x5c, 0x56, 0x56, 0x68, 0x7c, 0x93, 0xac, 0xb0, 0xac, 0x8f, 0x75, 0x60, 0x58, 0x65,
                0x76, 0x82, 0x8f, 0x98, 0x93, 0x85, 0x7a, 0x6e, 0x66, 0x73, 0x83, 0x8f, 0x97, 0x9c, 0x8e, 0x7a,
                0x6b, 0x60, 0x5a, 0x6e, 0x82, 0x91, 0xa2, 0xae, 0xa0, 0x83, 0x74, 0x74, 0x73, 0x7e, 0x8d, 0x91,
                0x97, 0x9b, 0x8e, 0x7b, 0x5c, 0x56, 0x6a, 0x7b, 0x90, 0x95, 0x91, 0x8f, 0x89, 0x7e, 0x70, 0x71,
                0x7b, 0x83, 0x90, 0x98, 0xa2, 0x8f, 0x7e, 0x79, 0x76, 0x7a, 0x80, 0x87, 0x92, 0x92, 0x93, 0x90,
                0x7f, 0x73, 0x72, 0x71, 0x6e, 0x7e, 0x85, 0x8f, 0x8e, 0x8c, 0x84, 0x75, 0x73, 0x79, 0x80, 0x8a,
                0x90, 0x9c, 0xa6, 0x98, 0x8e, 0x80, 0x73, 0x6d, 0x67, 0x6c, 0x76, 0x7f, 0x8b, 0x92, 0x8f, 0x92,
                0x91, 0x85, 0x80, 0x7c, 0x76, 0x79, 0x7e, 0x83, 0x86, 0x81, 0x84, 0x80, 0x75, 0x72, 0x6f, 0x71,
                0x77, 0x83, 0x8f, 0x98, 0x98, 0x90, 0x8a, 0x80, 0x7e, 0x80, 0x84, 0x84, 0x8a, 0x8e, 0x8a, 0x82,
                0x77, 0x74, 0x71, 0x78, 0x81, 0x81, 0x83, 0x8a, 0x8f, 0x86, 0x80, 0x7c, 0x75, 0x75, 0x80, 0x87,
                0x89, 0x8d, 0x8f, 0x8b, 0x7e, 0x7f, 0x7e, 0x7e, 0x85, 0x89, 0x8b, 0x8f, 0x8f, 0x8e, 0x86, 0x7e,
                0x75, 0x74, 0x7b, 0x80, 0x7c, 0x7d, 0x80, 0x80, 0x83, 0x8c, 0x8f, 0x8f, 0x91, 0x98, 0x97, 0x92,
                0x8f, 0x8d, 0x8b, 0x8c, 0x8d, 0x84, 0x81, 0x81, 0x7c, 0x73, 0x71, 0x71, 0x72, 0x7f, 0x87, 0x8a,
                0x89, 0x8f, 0x8e, 0x84, 0x80, 0x82, 0x80, 0x82, 0x8c, 0x8c, 0x88, 0x8d, 0x93, 0x88, 0x80, 0x80,
                0x7f, 0x7a, 0x80, 0x85, 0x80, 0x80, 0x87, 0x82, 0x7e, 0x80, 0x80, 0x7d, 0x7d, 0x80, 0x80, 0x80,
                0x88, 0x8b, 0x8b, 0x8e, 0x90, 0x88, 0x84, 0x82, 0x7e, 0x80, 0x81, 0x83, 0x84, 0x82, 0x83, 0x80,
                0x76, 0x75, 0x70, 0x73, 0x80, 0x82, 0x86, 0x8e, 0x8b, 0x86, 0x80, 0x7e, 0x78, 0x75, 0x7d, 0x83,
                0x84, 0x8d, 0x8f, 0x89, 0x84, 0x80, 0x7f, 0x7c, 0x7e, 0x82, 0x85, 0x89, 0x8b, 0x85, 0x7f, 0x7b,
                0x7b, 0x7f, 0x83, 0x8a, 0x8f, 0x8f, 0x8f, 0x8a, 0x80, 0x7c, 0x75, 0x76, 0x7a, 0x80, 0x86, 0x88,
                0x8c, 0x8a, 0x83, 0x80, 0x7f, 0x7c, 0x80, 0x85, 0x87, 0x8a, 0x8e, 0x8e, 0x87, 0x84, 0x83, 0x82,
                0x83, 0x87, 0x8d, 0x8a, 0x8b, 0x89, 0x81, 0x7b, 0x77, 0x7a, 0x7f, 0x83, 0x8e, 0x8f, 0x8d, 0x8c,
                0x89, 0x81, 0x81, 0x81, 0x85, 0x8a, 0x8d, 0x8f, 0x90, 0x8d, 0x8c, 0x87, 0x83, 0x83, 0x82, 0x82,
                0x82, 0x84, 0x82, 0x80, 0x7a, 0x7b, 0x7b, 0x7f, 0x82, 0x88, 0x8b, 0x8f, 0x90, 0x8c, 0x87, 0x80,
                0x7e, 0x7e, 0x80, 0x80, 0x83, 0x85, 0x88, 0x8a, 0x88, 0x85, 0x82, 0x7f, 0x7d, 0x7a, 0x7a, 0x7e,
                0x80, 0x84, 0x88, 0x89, 0x8d, 0x8c, 0x88, 0x84, 0x80, 0x7f, 0x7e, 0x7f, 0x80, 0x84, 0x86, 0x89,
                0x87, 0x85, 0x81, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x80, 0x81, 0x84, 0x88, 0x89, 0x89,
                0x89, 0x85, 0x80, 0x7f, 0x7d, 0x79, 0x76, 0x79, 0x7e, 0x80, 0x81, 0x83, 0x83, 0x82, 0x82, 0x81,
                0x80, 0x82, 0x86, 0x88, 0x87, 0x86, 0x82, 0x80, 0x7d, 0x7e, 0x7c, 0x7e, 0x81, 0x87, 0x89, 0x89,
                0x86, 0x81, 0x7f, 0x7e, 0x7e, 0x7f, 0x81, 0x85, 0x89, 0x8c, 0x8b, 0x87, 0x83, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x82, 0x83, 0x83, 0x83, 0x83, 0x82, 0x84, 0x84, 0x86, 0x87, 0x88, 0x89, 0x87, 0x86,
                0x85, 0x83, 0x82, 0x82, 0x81, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x83, 0x85, 0x88, 0x89,
                0x86, 0x85, 0x84, 0x83, 0x84, 0x86, 0x86, 0x87, 0x88, 0x88, 0x86, 0x83, 0x82, 0x80, 0x80, 0x7f,
                0x7d, 0x7d, 0x7a, 0x78, 0x79, 0x7a, 0x7b, 0x7f, 0x80, 0x83, 0x85, 0x86, 0x88, 0x85, 0x84, 0x84,
                0x82, 0x84, 0x84, 0x83, 0x81, 0x80, 0x7f, 0x7e, 0x7c, 0x7d, 0x7d, 0x7d, 0x7f, 0x80, 0x80, 0x83,
                0x86, 0x84, 0x82, 0x81, 0x80, 0x7c, 0x79, 0x79, 0x76, 0x7a, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x80,
                0x7f, 0x80, 0x80, 0x80, 0x83, 0x84, 0x85, 0x85, 0x85, 0x81, 0x80, 0x80, 0x7e, 0x7d, 0x7d, 0x7f,
                0x7f, 0x7f, 0x80, 0x80, 0x80, 0x81, 0x83, 0x80, 0x80, 0x80, 0x80, 0x7d, 0x7c, 0x7e, 0x7f, 0x80,
                0x84, 0x86, 0x86, 0x85, 0x83, 0x80, 0x7e, 0x7e, 0x7e, 0x7f, 0x80, 0x84, 0x85, 0x85, 0x84, 0x83,
                0x82, 0x81, 0x81, 0x80, 0x82, 0x84, 0x84, 0x83, 0x83, 0x80, 0x80, 0x7f, 0x7e, 0x81, 0x81, 0x84,
                0x88, 0x86, 0x85, 0x81, 0x80, 0x7e, 0x7f, 0x7f, 0x80, 0x84, 0x87, 0x87, 0x87, 0x84, 0x82, 0x80,
                0x80, 0x80, 0x81, 0x83, 0x86, 0x85, 0x84, 0x83, 0x80, 0x7e, 0x7c, 0x7c, 0x7d, 0x80, 0x80, 0x82,
                0x84, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x82, 0x82, 0x80, 0x7f, 0x7e, 0x7a, 0x79, 0x7a, 0x7a,
                0x7b, 0x7f, 0x80, 0x81, 0x81, 0x81, 0x82, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7e,
                0x7e, 0x7c, 0x7c, 0x7c, 0x7d, 0x7f, 0x80, 0x80, 0x81, 0x81, 0x81, 0x80, 0x7e, 0x7d, 0x7a, 0x79,
                0x79, 0x79, 0x7b, 0x7c, 0x7e, 0x80, 0x80, 0x7f, 0x7d, 0x7e, 0x7e, 0x7e, 0x7e, 0x7f, 0x80, 0x80,
                0x7f, 0x7e, 0x7b, 0x7a, 0x7a, 0x7c, 0x7d, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7d, 0x7c, 0x7c, 0x7c,
                0x7d, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x81, 0x83, 0x82, 0x83, 0x81, 0x80, 0x7f, 0x7d, 0x7c, 0x7c, 0x7f, 0x80, 0x82,
                0x85, 0x85, 0x84, 0x82, 0x81, 0x80, 0x80, 0x80, 0x80, 0x81, 0x82, 0x83, 0x84, 0x83, 0x82, 0x81,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x81, 0x81, 0x83, 0x84, 0x84, 0x85, 0x85, 0x84, 0x82,
                0x81, 0x80, 0x80, 0x7f, 0x80, 0x80, 0x80, 0x81, 0x82, 0x83, 0x83, 0x83, 0x81, 0x81, 0x81, 0x81,
                0x81, 0x82, 0x83, 0x83, 0x83, 0x83, 0x82, 0x80, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x82, 0x80, 0x81, 0x82, 0x81, 0x80, 0x80, 0x7f, 0x7f,
                0x7e, 0x7f, 0x7f, 0x80, 0x7f, 0x80, 0x7e, 0x7d, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x7e, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e, 0x7c, 0x7a, 0x79, 0x7a, 0x7c,
                0x7e, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7e, 0x7d, 0x7d, 0x7e, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7e,
                0x7e, 0x7e, 0x7e, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x80, 0x80, 0x7f, 0x7f, 0x80, 0x80, 0x80,
                0x81, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x82, 0x81, 0x81, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x81, 0x81, 0x83, 0x82, 0x83, 0x82, 0x81,
            };

        snd.Create( sizeof( badsound ), badsound );
    }

#endif
}

//file have moved to dest
void MainFrame::FileMd5( _TaskAttr& task )
{
    CFileMd5 * t = new CFileMd5( task );
    t->Create();
    t->SetPriority( 20 );
    t->Run();
}

//
void MainFrame::RecordSavePath( std::string path )
{

    std::list<std::string>::iterator it;

    for ( it = gSavePathHistory.begin();it != gSavePathHistory.end();it++ )
    {
        if ( *it == path )
        {
            std::string s1 = gSavePathHistory.front();
            std::string s2 = *it;
            *it = s1;
            gSavePathHistory.pop_front();
            gSavePathHistory.push_front( s2 );
            return ;
        }
    }

    gSavePathHistory.push_front( path );

    if ( gSavePathHistory.size() > 10 )
        gSavePathHistory.pop_back();

}


bool MainFrame::ReadConfig()
{

    //记录格式采用2进制格式 v3.0
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]
    //数据：gRunTaskNum[4]+gTaskDefThread[4]+gRetryTime[4]+gRetryWait[4]+
    //gConnectTimeOut[4]+gReceiveTimeOut[4]+gbIgnoreExt[4]+gbMonitorExt[4]+gbAutoMd5[4]+
    //gbMonitorClip[4]+gbShowDropWin[4]+gbSoundTip[4]+gDropWinX[4]+gDropWinY[4]+gSpeedMode[4]+hissize[4]+
    //gDefSavePath[v]+gDefFtpPass[v]+gsMonitorExt[v]+gsIgnoreExt[v]+
    //gSavePathHistory[0][v]+gSavePathHistory[1][v]...gSavePathHistory[9][v]
    //文件尾：0x00EE[4]

    FILE * fp;

    std::string temp;

    if ( !GetTempDir( temp ) )
    {
        return false;
    }

    temp += "/config.db";

    fp = fopen( temp.c_str(), "rb" );

    if ( fp == NULL )
    {

        return false;
    }


    struct _fh
    {
        int ver; //当前版本1.0
        int headlen; //4*3=12
        int datalen; //后确定

    }

    fh;

    fseek( fp, 0, SEEK_SET );

    if ( 1 != fread( &fh, sizeof( fh ), 1, fp ) )
    {
        fclose( fp );
        return false;
    }

    if ( fh.headlen != sizeof( fh ) )
    {
        fclose( fp );
        return false;
    }

    if ( fh.ver <= 0 || fh.ver > 3 || fh.datalen <= 0 )
    {
        fclose( fp );
        return false;
    }

    fseek( fp, fh.headlen, SEEK_SET ); //moveto data
    char *buf = new char[ fh.datalen ]; //完整读到内存中处理

    if ( 1 != fread( buf, fh.datalen, 1, fp ) )
    {
        delete[] buf;
        return false;
    }

    fclose( fp );

    int movepos = 0;

    memcpy( &gRunTaskNum, buf + movepos, sizeof( gRunTaskNum ) );
    movepos += sizeof( gRunTaskNum );

    memcpy( &gTaskDefThread, buf + movepos, sizeof( gTaskDefThread ) );
    movepos += sizeof( gTaskDefThread );

    memcpy( &gRetryTime, buf + movepos, sizeof( gRetryTime ) );
    movepos += sizeof( gRetryTime );

    memcpy( &gRetryWait, buf + movepos, sizeof( gRetryWait ) );
    movepos += sizeof( gRetryWait );

    memcpy( &gConnectTimeOut, buf + movepos, sizeof( gConnectTimeOut ) );
    movepos += sizeof( gConnectTimeOut );

    memcpy( &gReceiveTimeOut, buf + movepos, sizeof( gReceiveTimeOut ) );
    movepos += sizeof( gReceiveTimeOut );

    memcpy( &gbIgnoreExt, buf + movepos, sizeof( gbIgnoreExt ) );
    movepos += sizeof( gbIgnoreExt );

    memcpy( &gbMonitorExt, buf + movepos, sizeof( gbMonitorExt ) );
    movepos += sizeof( gbMonitorExt );

    if ( fh.ver >= 3 )
    {
        memcpy( &gbAutoMd5, buf + movepos, sizeof( gbAutoMd5 ) );
        movepos += sizeof( gbAutoMd5 );
    }
    else
    {
        gbAutoMd5 = true;
    }

    memcpy( &gbMonitorClip, buf + movepos, sizeof( gbMonitorClip ) );
    movepos += sizeof( gbMonitorClip );

    memcpy( &gbShowDropWin, buf + movepos, sizeof( gbShowDropWin ) );
    movepos += sizeof( gbShowDropWin );

    if ( fh.ver >= 2 )
    {
        memcpy( &gbSoundTip, buf + movepos, sizeof( gbSoundTip ) );
        movepos += sizeof( gbSoundTip );
    }
    else
    {
        gbSoundTip = true;
    }

    memcpy( &gDropWinX, buf + movepos, sizeof( gDropWinX ) );
    movepos += sizeof( gDropWinX );

    memcpy( &gDropWinY, buf + movepos, sizeof( gDropWinY ) );
    movepos += sizeof( gDropWinY );

    memcpy( &gSpeedMode, buf + movepos, sizeof( gSpeedMode ) );
    movepos += sizeof( gSpeedMode );

    int hissize;
    memcpy( &hissize, buf + movepos, sizeof( int ) );
    movepos += sizeof( int );

    if ( hissize < 0 || hissize > 10 )
    {
        delete[] buf;
        return false;
    }

    //strings
    char strbuf[ 512 ];

    strcpy( strbuf, buf + movepos );

    gDefSavePath = std::string( strbuf );

    movepos += strlen( strbuf ) + 1;

    strcpy( strbuf, buf + movepos );

    gDefFtpPass = std::string( strbuf );

    movepos += strlen( strbuf ) + 1;

    strcpy( strbuf, buf + movepos );

    gsMonitorExt = std::string( strbuf );

    movepos += strlen( strbuf ) + 1;

    strcpy( strbuf, buf + movepos );

    gsIgnoreExt = std::string( strbuf );

    movepos += strlen( strbuf ) + 1;



    for ( int i = 0;i < hissize;i++ )
    {
        strcpy( strbuf, buf + movepos );
        std::string temp = std::string( strbuf );
        movepos += strlen( strbuf ) + 1;
        gSavePathHistory.push_back( temp );
    }

    delete[] buf;
    return true;
}

bool MainFrame::SaveConfig()
{

    //记录格式采用2进制格式 v3.0
    //文件头＋数据＋文件尾
    //文件头：版本[4]+文件头长度[4]+数据全长[4]
    //数据：gRunTaskNum[4]+gTaskDefThread[4]+gRetryTime[4]+gRetryWait[4]+
    //gConnectTimeOut[4]+gReceiveTimeOut[4]+gbIgnoreExt[4]+gbMonitorExt[4]+gbAutoMd5[4]+
    //gbMonitorClip[4]+gbShowDropWin[4]+gbSoundTip[4]+gDropWinX[4]+gDropWinY[4]+gSpeedMode[4]+hissize[4]+
    //gDefSavePath[v]+gDefFtpPass[v]+gsMonitorExt[v]+gsIgnoreExt[v]+
    //gSavePathHistory[0][v]+gSavePathHistory[1][v]...gSavePathHistory[9][v]
    //文件尾：0x00EE[4]

    FILE * fp;

    std::string temp;

    if ( !GetTempDir( temp ) )
    {
        return false;
    }

    temp += "/config.db";

    fp = fopen( temp.c_str(), "wb" );

    if ( fp == NULL )
    {

        return false;
    }


    struct _fh
    {
        int ver; //当前版本3.0
        int headlen; //4*3=12
        int datalen; //后确定

    }

    fh;

    fh.ver = 3;
    fh.headlen = sizeof( fh );
    fh.datalen = 0;

    if ( 1 != fwrite( &fh, sizeof( fh ), 1, fp ) )
    {
        fclose( fp );
        return false;
    }


    int dl = 0;
    fwrite( &gRunTaskNum, sizeof( gRunTaskNum ), 1, fp );
    dl += sizeof( gRunTaskNum );
    fwrite( &gTaskDefThread, sizeof( gTaskDefThread ), 1, fp );
    dl += sizeof( gTaskDefThread );
    fwrite( &gRetryTime, sizeof( gRetryTime ), 1, fp );
    dl += sizeof( gRetryTime );
    fwrite( &gRetryWait, sizeof( gRetryWait ), 1, fp );
    dl += sizeof( gRetryWait );
    fwrite( &gConnectTimeOut, sizeof( gConnectTimeOut ), 1, fp );
    dl += sizeof( gConnectTimeOut );
    fwrite( &gReceiveTimeOut, sizeof( gReceiveTimeOut ), 1, fp );
    dl += sizeof( gReceiveTimeOut );
    fwrite( &gbIgnoreExt, sizeof( gbIgnoreExt ), 1, fp );
    dl += sizeof( gbIgnoreExt );
    fwrite( &gbMonitorExt, sizeof( gbMonitorExt ), 1, fp );
    dl += sizeof( gbMonitorExt );
    fwrite( &gbAutoMd5, sizeof( gbAutoMd5 ), 1, fp );
    dl += sizeof( gbAutoMd5 );
    fwrite( &gbMonitorClip, sizeof( gbMonitorClip ), 1, fp );
    dl += sizeof( gbMonitorClip );
    fwrite( &gbShowDropWin, sizeof( gbShowDropWin ), 1, fp );
    dl += sizeof( gbShowDropWin );
    fwrite( &gbSoundTip, sizeof( gbSoundTip ), 1, fp );
    dl += sizeof( gbSoundTip );
    fwrite( &gDropWinX, sizeof( gDropWinX ), 1, fp );
    dl += sizeof( gDropWinX );
    fwrite( &gDropWinY, sizeof( gDropWinY ), 1, fp );
    dl += sizeof( gDropWinY );
    fwrite( &gSpeedMode, sizeof( gSpeedMode ), 1, fp );
    dl += sizeof( gSpeedMode );
    int hissize = gSavePathHistory.size();

    if ( hissize > 10 )
        hissize = 10;

    fwrite( &hissize, sizeof( int ), 1, fp );

    dl += sizeof( int );

    fwrite( gDefSavePath.c_str(), gDefSavePath.length() + 1, 1, fp );

    dl += gDefSavePath.length() + 1;

    fwrite( gDefFtpPass.c_str(), gDefFtpPass.length() + 1, 1, fp );

    dl += gDefFtpPass.length() + 1;

    fwrite( gsMonitorExt.c_str(), gsMonitorExt.length() + 1, 1, fp );

    dl += gsMonitorExt.length() + 1;

    fwrite( gsIgnoreExt.c_str(), gsIgnoreExt.length() + 1, 1, fp );

    dl += gsIgnoreExt.length() + 1;

    // savepath history,

    std::list<std::string>::iterator it;

    int counter = 0;

    for ( it = gSavePathHistory.begin();it != gSavePathHistory.end() && counter < 10;it++, counter++ )
    {
        fwrite( it->c_str(), it->length() + 1, 1, fp );
        dl += it->length() + 1;
    }

    fh.datalen = dl;

    //file tail

    hissize = 0x00EE;
    fwrite( &hissize, 4, 1, fp );

    //write file head
    fseek( fp, 0, SEEK_SET );
    fwrite( &fh, sizeof( fh ), 1, fp );
    fclose( fp );
    return true;

}

void MainFrame::OnReloadFile( wxCommandEvent& event )
{
    _TaskAttr * pcur = GetCurrentTask();

    if ( pcur == NULL )
        return ;

    if ( pcur->nStatus == _STATUS_RUNNING )
        return ;

    if ( pcur->nStatus == _STATUS_DELETE )
        return ;

    wxString msg;

    wxString url( pcur->sURL.c_str(), wxConvLocal );

    msg = wxString(_("Redownload will delete old first:")) + _( "\n" ) + url;

    wxMessageDialog dlg( this, msg, _("delete old?"),
                         wxICON_QUESTION | wxYES_NO | wxSTAY_ON_TOP | wxNO_DEFAULT );

    if ( wxID_NO == dlg.ShowModal() )
    {
        return ;
    }

    std::string tempdir;

    if ( GetTempDir( tempdir ) )
    {
        std::string tempfile = tempdir;
        tempfile += "/";
        tempfile += pcur->sName;
        tempfile += ".mg";

        remove
            ( tempfile.c_str() );

        tempfile += "idx";

        remove
            ( tempfile.c_str() );
    }

    //删除目标文件（如果已完成）
    std::string destfile = pcur->sSavePath;

    destfile += "/";

    destfile += pcur->sName;

    remove
        ( destfile.c_str() );

    //修改任务参数任务
    pcur->nStatus = _STATUS_WAITING;

    pcur->nFinish = 0;

    pcur->nLen = -1;

    pcur->fRatio = 0;

    pcur->nSpeed = 0;

    pcur->nError = 0;

    pcur->sMD5 = string( "" );

    ScheduleTask();

}

bool MainFrame::IsMonitorClip()
{
    return gbMonitorClip;
}
/*
void MainFrame::OnFocus( wxFocusEvent& event )
{
    //DBGOUT( "focus event" );
}

void MainFrame::OnLostFocus( wxFocusEvent& event )
{
    //DBGOUT( "lost event" );
}
*/
void MainFrame::OnUrlNotice( wxCommandEvent& event )
{

    //
    int id = event.GetId();
    wxString url = event.GetString();

    if ( id == 1 )  //dropurl
    {

        if ( m_NewTaskDlg == NULL && m_PropertyDlg == NULL )
        {
            OnDropURL( url );
        }
        else if ( m_NewTaskDlg != NULL && m_PropertyDlg == NULL )
        {
            //DoNewTask(url);
            m_NewTaskDlg->AddMirrorUrl( url );
        }
        else if ( m_NewTaskDlg == NULL && m_PropertyDlg != NULL )
        {
            m_PropertyDlg->AddMirrorUrl( url );
        }
        else
        { // all dlg up?
            //DoNewTask(url);
            m_NewTaskDlg->AddMirrorUrl( url );
            m_PropertyDlg->AddMirrorUrl( url );
        }

    }
    else if ( id == 2 )  //clipbord
    {

        if ( m_NewTaskDlg == NULL && m_PropertyDlg == NULL )
        {
            DoNewTask( url );
        }
        else if ( m_NewTaskDlg != NULL && m_PropertyDlg == NULL )
        {
            //DoNewTask(url);
            m_NewTaskDlg->AddMirrorUrl( url );
        }
        else if ( m_NewTaskDlg == NULL && m_PropertyDlg != NULL )
        {
            m_PropertyDlg->AddMirrorUrl( url );
        }
        else
        { // all dlg up?
            //DoNewTask(url);
            m_NewTaskDlg->AddMirrorUrl( url );
            m_PropertyDlg->AddMirrorUrl( url );
        }

    }
    else
    {
        //DBGOUT( "unknow id give url" );
    }

}

void MainFrame::OnCmdNotice( wxCommandEvent& event )
{
    wxString task = event.GetString();

    if ( event.GetId() == 1 )
    {
        //DBGOUT("task="<<task);
        //解析行，建立新任务
        //url=?,refer=?,mirror=?,
        wxArrayString group;

        if ( task.Length() <= 5 )
            return ;

        int lastpos = 0;

        for ( int i = 0;i < int( task.Length() );i++ )
        {
            if ( task[ i ] == wxT( '^' ) )
            {
                group.Add( task.SubString( lastpos, i - 1 ) );
                lastpos = i + 1;
            }
        }

        //parse ok
        //search url,mirror,refer;
        wxString url, mirror, refer;

        for ( int i = 0;i < int( group.Count() );i++ )
        {
            cout << group[ i ] << endl;

            if ( group[ i ].Left( 4 ) == _( "url=" ) )
            {
                url = ( group[ i ].Right( group[ i ].Length() - 4 ) );
            }
            else if ( group[ i ].Left( 6 ) == _( "refer=" ) )
            {
                refer = ( group[ i ].Right( group[ i ].Length() - 6 ) );
            }
        }

        if ( url.empty() )
            return ; //no url

        //do the task
        DoNewTask( url, refer );

    }
    else
    { //other id don't run
    }
}

void MainFrame::OnProperty( wxCommandEvent& event )
{

    if ( GetCurrentTask() == NULL )
        return ;

    m_PropertyDlg = new CPropertyDlg( this, m_nCurrentSelectTaskID );

    m_SysTray->DlgShow( true );

    int nret = m_PropertyDlg->ShowModal();

    m_SysTray->DlgShow( false );

    if ( nret == wxID_OK )
    {
        //get attr
        //if finish , no change.
        //if running ,only mirror list can change.
        //
        _TaskAttr * tsk = GetCurrentTask();

        tsk->sRefer = m_PropertyDlg->m_sRefer.mb_str( wxConvLocal );
        tsk->sURL = m_PropertyDlg->GetUrl();
        tsk->sSavePath = m_PropertyDlg->m_sSavePath.mb_str( wxConvLocal );
        tsk->sName = m_PropertyDlg->m_sRename.mb_str( wxConvLocal );
        tsk->nThread = m_PropertyDlg->m_nThreadNum;
        tsk->nRetry = m_PropertyDlg->m_nRetry;
        tsk->nRetryWait = m_PropertyDlg->m_nRetryWait;
        tsk->sSockProxy = m_PropertyDlg->GetSocksProxyName();
        tsk->sHttpProxy = m_PropertyDlg->GetHttpProxyName();
        tsk->sFtpProxy = m_PropertyDlg->GetFtpProxyName();

        std::string mu[ 80 ];
        int mn = m_PropertyDlg->GetMirrorUrl( 80, mu );
        tsk->sMirrorURL.clear();

        for ( int i = 0;i < mn;i++ )
        {
            tsk->sMirrorURL.push_back( mu[ i ] );

            if ( tsk->nStatus == _STATUS_RUNNING )
                m_TaskManager->AddTaskMirrorUrl( tsk->nID, mu[ i ] );
        }

    }
    else
    {}

    m_PropertyDlg->Destroy();

    m_PropertyDlg = NULL;
}

void MainFrame::OnMaximize( wxMaximizeEvent& event )
{

    if ( IsIconized() )
        Iconize( false );

    if ( !IsShown() )
        Show( true );

    RequestUserAttention();
}

//it will not call ScheduleTask
void MainFrame::AddBatchTask(
    std::string mainurl,
    std::string refer,
    std::string savepath,
    std::string rename,
    int threadnum,
    bool runnow,
    int retry,
    int retrywait,
    std::string sock,
    std::string ftp,
    std::string http,
    bool automd5,
    int mirrorcount,
    std::string mir[]
)
{
    CUrlParser parser;

    if ( parser.SetUrl( mainurl ) )
    {

        _TaskAttr atask;
        atask.nID = GetNewTaskID();
        atask.sURL = mainurl;
        //add refer
        atask.sRefer = refer;
        Trim( atask.sRefer );
        atask.sSavePath = savepath;

        atask.nThread = threadnum;
        atask.nStatus = runnow ? _STATUS_WAITING : _STATUS_PAUSE;

        std::string finame = rename.empty() ? parser.GetFileName() : rename;

        if ( finame.empty() )
            atask.sName = "index.html";
        else
            atask.sName = finame;

        atask.nRetry = retry;

        atask.nRetryWait = retrywait;

        atask.nFinish = 0;

        atask.fRatio = 0.0;

        atask.nError = 0;

        atask.nSpeed = 0;

        atask.bAutoMd5 = automd5;

        atask.nLen = 0;

        atask.nTime = 0;

        atask.sSockProxy = sock;

        atask.sFtpProxy = ftp;

        atask.sHttpProxy = http;

        //mirror url
        for ( int j = 0;j < mirrorcount;j++ )
        {
            atask.sMirrorURL.push_back( mir[ j ] );
        }

        pthread_mutex_lock( &m_TaskMutex );
        m_AllTaskList.push_back( atask );
        pthread_mutex_unlock( &m_TaskMutex );

    }
    else
    {
        return ;
    }
}
/*2008-4-15 Disable the Language menu, Wing Sun */
/*

bool MainFrame::CheckLocale(_MGLANG lang)
{

    char *old_locale;
    old_locale = setlocale (LC_ALL, NULL);

	
	bool bret=false;

	switch(lang)
	{
		case EN_US_UTF_8:
			{
				if( NULL!=setlocale(LC_ALL,"en_US.utf8")||
					NULL!=setlocale(LC_ALL,"en_AU.utf8")||
					NULL!=setlocale(LC_ALL,"en_BW.utf8")||
					NULL!=setlocale(LC_ALL,"en_CA.utf8")||
					NULL!=setlocale(LC_ALL,"en_DK.utf8")||
					NULL!=setlocale(LC_ALL,"en_GB.utf8")||
					NULL!=setlocale(LC_ALL,"en_HK.utf8")||
					NULL!=setlocale(LC_ALL,"en_IE.utf8")||
					NULL!=setlocale(LC_ALL,"en_IN")	  ||
					NULL!=setlocale(LC_ALL,"en_NZ.utf8")||
					NULL!=setlocale(LC_ALL,"en_SG.utf8")||
					NULL!=setlocale(LC_ALL,"en_PH.utf8")||
					NULL!=setlocale(LC_ALL,"en_ZA.utf8")||
					NULL!=setlocale(LC_ALL,"en_ZW.utf8") )
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}
			
			}
			break;
		case ZH_CN_UTF_8:
			{
				if( NULL!=setlocale(LC_ALL,"zh_CN.utf8") )
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}
			}
			break;
		case ZH_CN_18030:
			{
#ifndef WIN32				
				if( NULL!=setlocale(LC_ALL,"zh_CN.gb18030") ||
					NULL!=setlocale(LC_ALL,"zh_CN.gbk") 	  ||
					NULL!=setlocale(LC_ALL,"zh_CN.gb2312") )
#else
				if( NULL!=setlocale(LC_ALL,".936"))
#endif
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}				
			}
			break;
		case ZH_TW_UTF_8:
			{
				if( NULL!=setlocale(LC_ALL,"zh_TW.utf8") )
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}
			}
			break;
		case ZH_TW_BIG5:
			{
#ifndef WIN32				
				if( NULL!=setlocale(LC_ALL,"zh_TW.big5") )
#else
				if( NULL!=setlocale(LC_ALL,".950") )
#endif
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}
			}
			break;
		case DE_DE_UTF_8:
			{
#ifndef WIN32				
				if( NULL!=setlocale(LC_ALL,"de_DE.utf8")||
					NULL!= setlocale(LC_ALL,"de_AT.utf8")||
					NULL!= setlocale(LC_ALL,"de_BE.utf8")||
					NULL!= setlocale(LC_ALL,"de_CH.utf8")||	
					NULL!= setlocale(LC_ALL,"de_LU.utf8"))
#else
				if( NULL!=setlocale(LC_ALL,".1252") )
#endif
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}
			}
			break;
		case PT_BR_UTF_8:
			{
#ifndef WIN32				
				if( NULL!=setlocale(LC_ALL,"pt_BR.utf8")||
					NULL!= setlocale(LC_ALL,"pt_PT.utf8"))
#else
				if( NULL!=setlocale(LC_ALL,".1252") )
#endif
				{
					setlocale(LC_ALL,old_locale);
					bret=true;
					break;
				}				
			}
	}

	return bret;
}

void MainFrame::CheckLangMenu(_MGLANG lang)
{
    switch ( lang )
    {

        case ZH_CN_UTF_8:
        m_lang->Check( mgID_VIEW_LANG_CN_UTF8, true );
        break;

        case EN_US_UTF_8:
        m_lang->Check( mgID_VIEW_LANG_EN_UTF8, true );
        break;

        case ZH_CN_18030:
        m_lang->Check( mgID_VIEW_LANG_GB18030, true );
        break;

        case ZH_TW_UTF_8:
        m_lang->Check( mgID_VIEW_LANG_TW_UTF8, true );
        break;

        case ZH_TW_BIG5:
        m_lang->Check( mgID_VIEW_LANG_TW_BIG5, true );

        default:
        break;
    }
}

void MainFrame::NoLangSupport()
{
	m_SysTray->DlgShow( true );
	wxMessageDialog dlg(this, 
					wxString(_("Your system not supports this locale/language, please install language package.")),
					wxString(_("Error!")),
					wxOK|wxICON_ERROR|wxSTAY_ON_TOP);
	dlg.ShowModal();
	m_SysTray->DlgShow( false );
}
*/
void MainFrame::OnSaveConfig(wxCommandEvent& event)
{
    SaveConfig();		
}

void MainFrame::OnSaveProxy(wxCommandEvent& event)
{
    SaveProxyDB();
}

void MainFrame::OnSaveTask(wxCommandEvent& event)
{
    SaveTaskDB();
}

void MainFrame::OnVisitHome(wxCommandEvent& event)
{

	::wxLaunchDefaultBrowser(_("http://multiget.sourceforge.net"),wxBROWSER_NEW_WINDOW);


}
