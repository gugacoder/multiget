
/***************************************************************************
 *  mainframe.h
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



#ifndef _MAINFRAME_H
#define _MAINFRAME_H
#include "common.h"

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/msgdlg.h>
#include <wx/statusbr.h>
#include <wx/sound.h>
#include <pthread.h>
DECLARE_EVENT_TYPE( mgID_TIMER, -1 )
DECLARE_EVENT_TYPE( mgEVT_TASK_FINISH, -1 )
DECLARE_EVENT_TYPE( mgEVT_URL_NOTICE, -1 )
DECLARE_EVENT_TYPE( mgEVT_CMD_NOTICE, -1 )  //cmdline task
DECLARE_EVENT_TYPE( mgID_NEW_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_START_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_STOP_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_CANCEL_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_MOVEUP_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_MOVEDOWN_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_OPTION_CONFIG, -1 )
//DECLARE_EVENT_TYPE( mgEVT_TASK_FINISH, -1 )
//DECLARE_EVENT_TYPE( mgID_NEW_TASK, -1 )
DECLARE_EVENT_TYPE( mgID_SUBTHREAD, -1 );
DECLARE_EVENT_TYPE( mgID_ADDTHREAD, -1 );
DECLARE_EVENT_TYPE( mgID_RELOAD, -1 );
/*2008-4-15 Disable the Language menu, Wing Sun */
/*
DECLARE_EVENT_TYPE( mgID_VIEW_LANG, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_EN_UTF8, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_CN_UTF8, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_GB18030, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_TW_BIG5, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_TW_UTF8, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_DE_UTF8, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_LANG_PT_UTF8, -1 )
*/
DECLARE_EVENT_TYPE( mgID_VIEW_TOOLBAR, -1 );
DECLARE_EVENT_TYPE( mgID_VIEW_TOOLBAR_SIZE16, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_TOOLBAR_SIZE24, -1 )
DECLARE_EVENT_TYPE( mgID_VIEW_TOOLBAR_SIZE32, -1 )

DECLARE_EVENT_TYPE( mgID_VIEW_STATUSBAR, -1 );
DECLARE_EVENT_TYPE( mgID_OPTION_PROXY, -1 );
DECLARE_EVENT_TYPE( mgID_OPTION_PROPERTY, -1 );

DECLARE_EVENT_TYPE( mgID_FILE_SAVECONFIG, -1 );
DECLARE_EVENT_TYPE( mgID_FILE_SAVETASK, -1 );
DECLARE_EVENT_TYPE( mgID_FILE_SAVEPROXY, -1 );
DECLARE_EVENT_TYPE( mgID_HELP_VISITHOME, -1 );
//

class CRightDownSplitter;

class CTaskManager;

class CLeftTree;

class CTaskList;

class CMeterToolBar;

class CMeterCtrl;

class CFloatWin;

class CSysTray;

class CHideWin;

class CMgConfig;

class CNewTaskDlg;

class CPropertyDlg;
//#ifndef __WXX11__
//class CDropUrl;
//#endif
static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE ;
//static long TOOLBAR_SIZE  = 16;
//wxTB_HORIZONTAL|wxTB_FLAT|wxTB_TEXT
//wxTB_FLAT | wxTB_HORZ_TEXT
//wxTB_HORIZONTAL|wxTB_FLAT

class MainFrame : public wxFrame
{

    friend class CSysTray;

    friend class CHideWin;

    friend class CTaskList;

    friend class CPropertyDlg;

public:
    // Constructor
    MainFrame(const wxString& title, wxLocale& m_locale);
    virtual ~MainFrame();

    void OnSelectTask( int taskid );
    void OnSelectType( _TASK_TYPE type );
    int AddNewTask( std::string url, int threadnum = 0, bool autorun = true );
    int StartTask( int ntask );
    _TaskAttr* GetCurrentTask();
    void OnShowModeSwitch( int mode );
    void OnDropURL( wxString url );
    CFloatWin* GetFloatWin()
    {
        return m_FloatWin;
    };

    bool IsMonitorClip();
    void MonitorClip( bool mo );
    void StartAllTask();
    void StopAllTask();
    void OnAbout( wxCommandEvent& event );
    void SwitchShowDropWin();
    int	GetProxyCount();
    _ProxyAttr& GetProxy( int seq );
    void RemoveProxy( wxString name );
    bool AddProxy( int type, wxString name, wxString server, int port, wxString user, wxString pass );
    bool GetProxyByName( std::string name, int& type, std::string& server, int& port, std::string& user, std::string& pass );

protected:
    void SysTrayExitNotice();
    void OnQuit( wxCommandEvent& event );
    void OnConfig( wxCommandEvent& event );
    void OnTimer( wxTimerEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnClose( wxCloseEvent& event );
    void OnMaximize( wxMaximizeEvent& event );
//    void OnDestroy( wxWindowDestroyEvent& );
//    void OnFocus( wxFocusEvent& );
//    void OnLostFocus( wxFocusEvent& );
    void OnTaskFinish( wxCommandEvent& event );
    void OnUrlNotice( wxCommandEvent& event );
    void OnCmdNotice( wxCommandEvent& event );
    void OnNewTask( wxCommandEvent& event );
    void OnStartTask( wxCommandEvent& event );
    void OnCancelTask( wxCommandEvent& event );
    void OnStopTask( wxCommandEvent& event );
    void OnProperty( wxCommandEvent& event );
    void OnIncreaseThread( wxCommandEvent& event );
    void OnDecreaseThread( wxCommandEvent& event );
    void OnMoveDown( wxCommandEvent& event );
    void OnMoveUp( wxCommandEvent& event );
    void OnReloadFile( wxCommandEvent& event );
    void OnWindowCreate( wxWindowCreateEvent& event );
/*2008-4-15 Disable the Language menu, Wing Sun */
/*
    void OnLangEnUtf8( wxCommandEvent& );
    void OnLangCnUtf8( wxCommandEvent& );
    void OnLangCn18030( wxCommandEvent& );
    void OnLangTwBig5( wxCommandEvent& );
    void OnLangTwUtf8( wxCommandEvent& );
    void OnLangDeUtf8(wxCommandEvent& );
	void OnLangPtUtf8( wxCommandEvent& );
    void UpdateMenuText();
    void DynamicLang();
*/
    void UpdateTaskList();
    void UpdateRightDown();
    void OnIconize( wxIconizeEvent& event );
    void ScheduleTask();
    void OnStatusBar( wxCommandEvent& event );
    void OnToolbarSize16( wxCommandEvent& );
    void OnToolbarSize24( wxCommandEvent& );
    void OnToolbarSize32( wxCommandEvent& );
    void OnProxySetting( wxCommandEvent& );
	
	void OnSaveConfig(wxCommandEvent& event);
	void OnSaveProxy(wxCommandEvent& event);
	void OnSaveTask(wxCommandEvent& event);
	void OnVisitHome(wxCommandEvent& event);
	
    void UpdateStatusBar();
    void InitStatusBar();
    void PlaySound( bool success );
    void CreateSound( wxSound& snd, bool success );
    void FileMd5( _TaskAttr& task );
    std::string GetTempFileName( std::string url );
    void UpdateTaskList( int ntask );
    int GetFileType( std::string sname );
    bool IsUrlExist( std::string url );
    _TASK_TYPE GetStatusByFileType( int type );
    _TaskAttr* GetTask( int taskid );
    void AddBatchTask(
        std::string mailurl,
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
        std::string mir[] );

private:
    bool ReadTaskDB();
    bool SaveTaskDB();
    bool SaveProxyDB();
    bool ReadProxyDB();
    bool ReadConfig();
    bool SaveConfig();
    int GetNewTaskID();

    void InitFileTypeList();
    bool CompareExtName( std::string sname, std::string ext );
    void UpdateToolBar();
    void UpdateToolText();
    void StopTask( int taskid );
    void RecordSavePath( std::string path );
    void DeleteTaskFromTaskList( int taskid );
    bool MoveFile( std::string mgfile, std::string destpath , std::string& renamestr );
    void SwapTaskTable( int task1, int task2 );
    void InitToolBar();
    void InitMenuBar();
    void InitSpliter();
    void InitMeterBar();
/*2008-4-15 Disable the Language menu, Wing Sun */
/*
	bool CheckLocale(_MGLANG lang);
	void CheckLangMenu(_MGLANG lang);
	void NoLangSupport();
*/
    //void CheckTempDir();
    void DoNewTask( wxString url, wxString refer = wxT( "" ), wxString savepath = wxT( "" ), wxString savefile = wxT( "" ) );

    _TASK_TYPE m_nCurrentShowType;


    wxSplitterWindow	*m_vspliter;
    wxSplitterWindow	*m_hspliter;
    CRightDownSplitter	*m_vsubspliter;

    CTaskList	*m_tasklist;
    CLeftTree *m_lefttree;
    CTaskManager *m_TaskManager;

    CFloatWin	*m_FloatWin;
    CSysTray	*m_SysTray;
    CHideWin	*m_HideWin;
    CNewTaskDlg *m_NewTaskDlg;
    CPropertyDlg *m_PropertyDlg;
    //	bool		m_bMonitorClip;
    //#ifndef __WXX11__
    //    CDropUrl	*m_DropUrl;
    //#endif

    wxToolBar *m_ToolBar;	//main toolbar

    CMeterCtrl *m_MeterBar; //speed bar

    wxMenuBar *m_MenuBar;	//main menu
	wxMenu 		*m_lang;	//lang menu
    //all task list
    std::vector<_TaskAttr> m_AllTaskList;
    //proxy list
    std::vector<_ProxyAttr> m_AllProxyList;

    //filename ext
    std::vector<std::string> m_FileTypeList[ 10 ];



    int m_nTaskID;
    int m_nCurrentSelectTaskID;
    wxTimer m_Timer;

    bool m_bShowStatusBar;
    long TOOLBAR_SIZE ;

    int m_nWinSizeW;
    int m_nWinSizeH;

    pthread_mutex_t m_TaskMutex;

    DECLARE_EVENT_TABLE()
	wxLocale& m_locale; //Localization object.

};



#endif
