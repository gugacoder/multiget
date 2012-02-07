/***************************************************************************
*  common.h
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
#ifndef _COMMON_H
#define _COMMON_H
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string>
#include <vector>
#include <list>

#ifndef WIN32
#include <assert.h>
#endif

#ifdef WIN32
#include <wx/setup.h>
#include <windows.h>
#endif

#include <wx/string.h>

#ifdef WIN32
typedef __int64 llong;
typedef unsigned __int64 ullong;
#else
typedef long long int llong ;
typedef unsigned long long int ullong ;
#endif

#define RECVBUF 16384


#define DBGOUT(str) std::cout<<__FILE__<<":"<<__LINE__<<":"<<str<<std::endl;


struct bindex
{
    llong start;
    llong end;
    int mark;
};

class CMgFileManager;

struct btask
{
    int ntaskid;
    int priority;
    int status;
    CMgFileManager *fm;
};

enum _MGLANG {
    EN_US_UTF_8 = 0,
    ZH_CN_UTF_8 = 1,
    ZH_TW_UTF_8 = 2,
    DE_DE_UTF_8 = 3,
	PT_BR_UTF_8 = 4,
    ZH_TW_BIG5,
    ZH_CN_18030,
    //add your language here

};

enum _SOCKVER
{
	AUTOVERSION, 
	V4, 
	V4A, 
	V5
};

enum _SPEED_MODE
{
    _SPEED_UNLIMIT = 0,
    _SPEED_5M = 1,
    _SPEED_2M = 2,
    _SPEED_1M = 3,
    _SPEED_700K = 4,
    _SPEED_500K = 5,
    _SPEED_400K = 6,
    _SPEED_300K = 7,
    _SPEED_200K = 8,
    _SPEED_100K = 9,
    _SPEED_50K = 10,
    _SPEED_20K = 11,
    _SPEED_10K = 12,
};

enum _SINGLE_TASK_STATUS
{
    _TASK_NONE,
    _TASK_WAITTING,
    _TASK_RUNNING,
    _TASK_ERROR,
    _TASK_FINISH,
};

enum _TASK_SHOWTYPE{
    _SHOW_NONE,
    _SHOW_RUNNING,
    _SHOW_WAITING,
    _SHOW_PAUSE,
    _SHOW_WRONG,
    _SHOW_FINISH,
    _SHOW_DELETE,
};

enum _PROTYPE{
    UNKNOW_PROTOCOL = 0,
    FTP_PROTOCOL = 1,
    HTTP_PROTOCOL = 2,
    SFTP_PROTOCOL = 3,
    SHTTP_PROTOCOL = 4,
    RTSP_PROTOCOL = 5,
    MMS_PROTOCOL = 6,
    RSYNC_PROTOCOL = 7,
};


enum _MSGTYPE {
    MSG_ERROR,
    MSG_WARNNING,
    MSG_INFO,
    MSG_SUCCESS,
    MSG_OUT,
    MSG_IN,
};

enum _TASK_TYPE{
    _STATUS_ROOT,
    _STATUS_RUNNING,
    _STATUS_WAITING,
    _STATUS_PAUSE,
    _STATUS_WRONG,
    _STATUS_FINISH,
    _STATUS_FINISH_SOFT,
    _STATUS_FINISH_PACK,
    _STATUS_FINISH_MUSIC,
    _STATUS_FINISH_ISO,
    _STATUS_FINISH_MOVIE,
    _STATUS_FINISH_PUBLISH,
    _STATUS_FINISH_PICTURE,
    _STATUS_FINISH_UNKNOW,
    _STATUS_DELETE,
};


struct _TaskAttr
{
    int nID; //
    _TASK_TYPE nStatus; //
    int nThread; //
    //   int nPriority; //
    bool bAutoMd5; //10/20
    std::string sName; ////
    llong nLen; //
    float fRatio; //
    llong nFinish; //
    int nSpeed; //average speed
    int nTime; //time to finish
    int nError; //error times
    int nRetry; //09/29
    int nRetryWait; //09/29
    //	bool bAutoMd5;	//10/20
    std::string sURL; //mainURL
    std::string sRefer; //2006/10/20
    std::vector<std::string> sMirrorURL; //mirrorURL 10/12
    std::string sSavePath;
    std::string sSockProxy;
    std::string sFtpProxy;
    std::string sHttpProxy;
    std::string sMD5;
};

struct _ProxyAttr
{
    int nType; //socks(0),socksv4(1),socksv4a(2),socksv5(3),ftp(4),http(5) ...
    std::string sName; //unique custom name,
    std::string sServer; //proxy server name
    int nPort;
    std::string sUser;
    std::string sPass;
};



struct _syslog
{
    int ntime;
    std::string info;
};



struct _tasklog
{
    int ntaskid;
    int ntime;
    _MSGTYPE ntype;
    std::string info;
};



//int getserverip( const char *server, char *sip, int buflen );
bool GetUserHome( std::string& homedir );
#ifdef WIN32
bool CheckForRegisterValue(
    HKEY syskey, std::string key, std::string keyname, std::string &value );
#endif
bool GetTempDir( std::string& tempdir );
void Trim( std::string& str );
std::string c_str(const wxString &str);

class CSpeedCtrl;

#endif
