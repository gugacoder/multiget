/***************************************************************************
*  mgstring.h
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

/*
	this file for all string in multiget
	I make this file for dynamic language change
	I am not sure if wxLocale can do that
	so I choose my way
*/

#ifndef _MGSTRING_H
#define _MGSTRING_H

#include <string>
#include <wx/wx.h>
#include <wx/string.h>
#include "common.h"



#define _NOL 	6   //number of language
#define _MAXNUM 263 //limit number of string, more than sizeof _MGSTRID

enum _MGSTRID {  //every identical string have it's id
    _S_APP_NAME,
    _S_APP_DUPRUN,
    _S_MENU_FILE,
    _S_MENU_FILE_QUIT,
    _S_MENU_TASK,
    _S_MENU_TASK_NEW,
    _S_MENU_TASK_START,
    _S_MENU_TASK_PAUSE,
    _S_MENU_TASK_DELETE,
    _S_MENU_TASK_MOVEUP,
    _S_MENU_TASK_MOVEDOWN,
    _S_MENU_TASK_ADDTHREAD,
    _S_MENU_TASK_SUBTHREAD,
    _S_MENU_TASK_RELOAD,
    _S_MENU_OPTION_CONFIG,
    _S_MENU_OPTION_PROXYCONFIG,
    _S_MENU_OPTION,
    _S_MENU_OPTION_PROPERTY,
    _S_MENU_HELP,
    _S_MENU_HELP_ABOUT,
    _S_FINISH,
    _S_RUNNINGTASK,
    _S_FAILEDTASK,
    _S_FINISHTASK,
    _S_PAUSETASK,
    _S_SOFTWARE,
    _S_PACKET,
    _S_MUSIC,
    _S_IMAGE,
    _S_MOVIE,
    _S_PUBLISH,
    _S_PICTURE,
    _S_UNKNOW,
    _S_DELETED,
    _S_FILENAME,
    _S_SIZE	,
    _S_PROGRESS,
    _S_SPEED,
    _S_LEFTTIME,
    _S_RETRY,
    _S_URL,
    _S_INFORMATION,
    _S_PIONEER	,
    _S_FILEINFO,
    _S_THREAD,
    _S_DETAIL,
    _S_VALUE,
    _S_NAME,
    _S_TASKID,
    _S_TASKSTATUS,
    _S_THREADNUM,
    _S_AUTOCHECKMD5,
    _S_ERRORTIME,
    _S_RUNNING,
    _S_WAITING,
    _S_WRONG,
    _S_SPEEDMODE ,
    _S_OPTION,
    _S_VERSION ,
    _S_CANCEL,
    _S_OK ,
    _S_COPYRIGHT,
    _S_ABOUT_MULTIGET,
    _S_SYSCONFIG,

    _S_INFO_JOBRUNNING,
    _S_INFO_RESOLVING,
    _S_INFO_SERVERIP,
    _S_INFO_SENDQUITFAIL,
    _S_INFO_SENDTYPEAFAIL,
    _S_INFO_SENDTYPEIFAIL,
    _S_INFO_SENDRESTFAIL,
    _S_INFO_INVALIDLINE,
    _S_INFO_REFUSEUSER,
    _S_INFO_NOPASSNEED,
    _S_INFO_INVALIDUSERORPASS,
    _S_INFO_CTRLCONNECTFAIL ,
    _S_INFO_CREATEINGSOCKET,
    _S_INFO_CONNECTFAIL,
    _S_INFO_CONNECTOK,

    _S_NEW_CHOOSEPATH,
    _S_NEW_INPUTTASKINFO,
    _S_NEW_SAVETO,
    _S_NEW_URL,
    _S_NEW_MIRRORURL,
    _S_NEW_RENAME,
    _S_NEW_THREADNUM,
    _S_NEW_RUNNOW,
    _S_NEW_YES,
    _S_NEW_NO,
    _S_NEW_TASK,
    _S_NEW_BASIC,
    _S_NEW_PROXY,
    _S_NEW_LOGIN,
    _S_NEW_USER,
    _S_NEW_PASS,
    _S_NEW_NOSOCKSPROXY,
    _S_NEW_NOFTPPROXY,
    _S_NEW_NOHTTPPROXY,
    _S_NEW_NONE,
    _S_NEW_RETRY,
    _S_NEW_RETRYWAIT,
    _S_NEW_REFER,
    _S_NEW_REFERADDR,

    _S_100_TONGJI,
    _S_INFO_TASKFINISH,
    _S_INFO_TASKFAIL,

    _S_INFO_WELCOME,
    _S_INFO_FILEMOVEFAIL,
    _S_INFO_TASKSTILLRUNNING,
    _S_INFO_WARNNING,
    _S_INFO_NOTSUPPORTURL,

    _S_TRAY_HIDERESTORE,
    _S_TRAY_SHOWDROPWIN,
    _S_TRAY_MONITERCLIP,
    _S_TRAY_STARTALL,
    _S_TRAY_STOPALL ,
    _S_TRAY_NOLIMIT,

    //2006/08/25
    _S_TASKLIST_DAY,
    _S_TASKLIST_TIME,
    _S_METER_TOPSPEED,
    _S_ABOUT_ANNOUNCE,
    _S_MENU_VIEW,
    _S_MENU_VIEW_LANG,
    _S_MENU_VIEW_TOOLBAR,
    _S_MENU_VIEW_TOOLBAR_SIZE16,
    _S_MENU_VIEW_TOOLBAR_SIZE24,
    _S_MENU_VIEW_TOOLBAR_SIZE32,
    _S_MENU_VIEW_STATUSBAR,
    _S_STATUS_TOTALSPEED,
    _S_STATUS_RUNNINGTASK,
    _S_STATUS_WAITINGTASK,

    _S_PROXYDLG_ADMIN,
    _S_PROXYDLG_ADDPROXY,
    _S_PROXYDLG_DELPROXY,
    _S_PROXYDLG_NAME,
    _S_PROXYDLG_TYPE,
    _S_PROXYDLG_SERVER,
    _S_PROXYDLG_PORT,
    _S_PROXYDLG_USER,
    _S_PROXYDLG_PASS,

    _S_ADDPROXYDLG_PROXYNAME,
    _S_ADDPROXYDLG_PROXYARGS,
    _S_ADDPROXYDLG_SERVER,
    _S_ADDPROXYDLG_PORT,
    _S_ADDPROXYDLG_USER,
    _S_ADDPROXYDLG_PASS,
    _S_ADDPROXYDLG_PROXYTYPE,

    _S_MAINFRAME_DELETESELECT,
    _S_MAINFRAME_DELETEWARNNING,

    _S_ABOUT_BUGREPORT,
    _S_ABOUT_MAINPUBLISH,

    _S_DETAIL_SAVEPATH,
    _S_DETAIL_SOCKSPROXY,
    _S_DETAIL_FTPPROXY,
    _S_DETAIL_HTTPPROXY,
    _S_DETAIL_MD5,
    _S_DETAIL_PAUSE,
	_S_DETAIL_NOM, //number of mirror
    _S_LOG,
    _S_MAINFRAME_RELOADWARNNING,
    _S_MAINFRAME_RELOADDELETE,

    _S_CONFIGWIN_TASKARGS,
    _S_CONFIGWIN_MAXRUNTASK,
    _S_CONFIGWIN_DEFTHREAD,
    _S_CONFIGWIN_DEFSAVEPATH,
    _S_CONFIGWIN_NETARGS,
    _S_CONFIGWIN_CONNECTTIMEOUT,
    _S_CONFIGWIN_RWTIMEOUT,
    _S_CONFIGWIN_MAXRETRY,
    _S_CONFIGWIN_WAITRETRY,
    _S_CONFIGWIN_OTHER,
    _S_CONFIGWIN_ANONPASS,
    _S_CONFIGWIN_SPEEDLIMIT,
    _S_CONFIGWIN_NOLIMIT,
    _S_CONFIGWIN_CLIPMONI,
    _S_CONFIGWIN_IGNOREEXT,
    _S_CONFIGWIN_ONLYEXT,
    _S_CONFIGWIN_SOUND,
    _S_CONFIGWIN_AUTOMD5,
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
    _S_TIP_PROPERTY,

    _S_FILEMGR_TASKOBJ,
    _S_FILEMGR_GETTEMPDIR,
    _S_FILEMGR_TEMPDIR,
    _S_FILEMGR_TEMPNAME,
    _S_FILEMGR_INDEXNAME,
    _S_FILEMGR_CHECKTEMP,
    _S_FILEMGR_TEMPEXIST,
    _S_FILEMGR_TEMPNOTEXIST,
    _S_FILEMGR_CHECKINDEX,
    _S_FILEMGR_INDEXEXIST,
    _S_FILEMGR_INDEXNOTEXIST,
    _S_FILEMGR_CHECKTEMPLENGTH,
    _S_FILEMGR_LENGTHOK,
    _S_FILEMGR_LENGTHFAIL,
    _S_FILEMGR_CREATEFILE,
    _S_FILEMGR_CREATOK,
    _S_FILEMGR_REDIRECT,
    _S_FILEMGR_URLOK,
    _S_FILEMGR_URLTEMPFAIL,
    _S_FILEMGR_URLFAIL,
    _S_FILEMGR_NOTASK,
    _S_FILEMGR_DATAOVERLOAD,
    _S_FILEMGR_FILELENCONFIRM,
    _S_FILEMGR_ERRORCLOSESOCK,
    _S_FILEMGR_ENDFILELEN,

    _S_PROPERTYDLG_TITLE,

	_S_SINGLETASK_JOBRUN,
	_S_SINGLETASK_ANALYSISURL,
	_S_SINGLETASK_NOTSUPPORTURL,
	_S_SINGLETASK_TASKURLOK,
	_S_SINGLETASK_URLISFTP,//"main url is ftp protocol"
	_S_SINGLETASK_USESOCKSPROXY,//"task will use socks proxy"
	_S_SINGLETASK_USEFTPPROXY,
	_S_SINGLETASK_USEHTTPPROXY,
	_S_SINGLETASK_GETFILEINFOERROR,
	_S_SINGLETASK_NOFILELENGTH,
	_S_SINGLETASK_NOTSUPPORTRESUME,
	_S_SINGLETASK_URLISHTTP,//"main url is http protocol"

	_S_ANTS_CONNECTING,
	_S_ANTS_CONNECTFAIL,
	_S_ANTS_LOGINFAIL,
	_S_ANTS_ENTERPASVFAIL,
	_S_ANTS_DATACONNFAIL, //"data connection error."
	_S_ANTS_DATACONNOK,
	_S_ANTS_BINARYFAIL,//"enter binary mode fail"
	_S_ANTS_SETRESUMEFAIL, //"can't set resume point"
	_S_ANTS_FAILTOGETFILE, //"fail to get file."
	_S_ANTS_RECVFILEDATA, //"receiving file data from %lld ..."
	_S_ANTS_FINISHNORMAL, //"finish task piece normal, quit."
	_S_ANTS_WRITEERROR, //"Write file error."
	_S_ANTS_NETERROR, //"network error"
	_S_ANTS_CHANGEURL, //"change a url"
	_S_ANTS_WAITTORETRY, //"Wait %d seconds retry ..."
	_S_ANTS_NORETRY,

	_S_THANKS_LANGUE,
	_S_THANKS_GUOHONGTAO,
	_S_THANKS_ABEL,
	_S_THANKS_CJACKER,
	_S_THANKS_MATTHE,
	_S_THANKS_ISJFK,
	_S_THANKS_BRYAN,
	_S_THANKS_MICHAEL_FRASE,
	_S_THANKS_JOSE_OSWALDO,
	_S_MENU_FILE_SAVECONFIG,
	_S_MENU_FILE_SAVETASK,
	_S_MENU_FILE_SAVEPROXY,
	_S_MENU_HELP_VISITHOME,
};


class CMgString
{

public:
    CMgString();
    void SetLang( _MGLANG lang );
    _MGLANG GetLang();

    std::string GetStr( _MGSTRID id );
    wxString GetWxStr( _MGSTRID id );

private:

    _MGLANG m_Lang;

    std::string m_SA[ _NOL ][ _MAXNUM ];
};

#endif
