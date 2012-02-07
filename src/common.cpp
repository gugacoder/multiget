/***************************************************************************
*  common.cpp
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

#include "common.h"
#include "speedctrl.h"
#include "mirroradmin.h"


#include <pthread.h>

#ifdef WIN32
#include <windows.h>

bool CheckForRegisterValue(
    HKEY syskey, std::string key, std::string keyname, std::string &value )
{
    HKEY phkResult;
    ULONG lRet = RegOpenKeyEx( syskey, key.c_str(), 0, KEY_READ, &phkResult );

    if ( ERROR_SUCCESS != lRet )
    {
        return false;
    }
    else
    {
        BYTE valueBuf[ 1024 ] = {0};
        ULONG valuesize = 1024;
        DWORD valtype;
        lRet = RegQueryValueEx( phkResult, keyname.c_str(), NULL, &valtype, valueBuf, &valuesize );

        if ( ERROR_SUCCESS != lRet  )
        {
            RegCloseKey( phkResult );
            return false;
        }
        else if ( REG_SZ != valtype )
        {
            RegCloseKey( phkResult );
            return false;
        }
        else
        {
            value = std::string( ( char* ) valueBuf );
            RegCloseKey( phkResult );
            return true;
        }
    }
}


#endif

bool GetUserHome( std::string& homedir )
{
#ifdef WIN32

    HKEY syskey = HKEY_CURRENT_USER;
    std::string Key =
        ( "Software\\microsoft\\windows\\CurrentVersion\\Explorer\\Shell Folders" );
    std::string KeyName = ( "Personal" );

    if ( !CheckForRegisterValue( syskey, Key, KeyName, homedir ) )
    {
        return false;
    }
    else
    {
        return true;
    }

#else
    char* userroot = getenv( "HOME" );

    if ( userroot == NULL )
        return false;

    homedir = std::string( userroot );

    return true;

#endif
}

bool GetTempDir( std::string& tempdir )
{
    std::string homedir;

    if ( GetUserHome( homedir ) )
    {
#ifdef WIN32
        tempdir = homedir + "\\MultiGetTemp";
#else

        tempdir = homedir + "/.MultiGet";
#endif

        return true;
    }
    else
    {
        return false;
    }
}

//trim space at head and rear
void Trim( std::string& str )
{
    std::string::size_type pos = str.find_last_not_of( ' ' );

    if ( pos != std::string::npos )
    {
        str.erase( pos + 1 );
        pos = str.find_first_not_of( ' ' );

        if ( pos != std::string::npos )
            str.erase( 0, pos );
    }
    else
        str.erase( str.begin(), str.end() );
}
/**************************************************
 * Function to conver a wxString to std::string   *
 * In UNICODE mode, wxString::c_str() return a    *
 * wchar_t * type, this type cannot passed to     *
 * std::string.                                   *
 *************************************************/

std::string c_str(const wxString &str)
{
	const wxWX2MBbuf tmp_buf= wxConvCurrent->cWX2MB(str);
	const char * tmp_str = (const char *) tmp_buf;
	std::string result=tmp_str;
	return tmp_str;
}


std::vector<_syslog> gSysLog;
std::vector<_tasklog> gTaskLog;
pthread_mutex_t gTaskLogMutex = PTHREAD_MUTEX_INITIALIZER;

std::list<std::string> gSavePathHistory;
//config
int gRunTaskNum = 5;
int gConnectTimeOut = 30;
int gReceiveTimeOut = 30;
int gTaskDefThread = 5;
int gRetryTime = 99;
int gRetryWait = 30;
bool gbIgnoreExt = false;
bool gbMonitorExt = false;
bool gbShowDropWin = true;
bool gbMonitorClip = true;
bool gbSoundTip = false;
bool gbAutoMd5 = true;
bool gbComfirmDelete = true; //10/22
int gDropWinX = -1;
int gDropWinY = -1;

_SPEED_MODE gSpeedMode = _SPEED_UNLIMIT;

std::string gDefSavePath;
std::string gDefFtpPass = "multiget@gmail.com";
std::string gsMonitorExt = ".bin,.exe,.rar .ico .jpg .png;.bmp;.gz";
std::string gsIgnoreExt = ".htm .html .shtml";

CSpeedCtrl gSpeedCtrl;
//simple mirror url admin
//CMirrorAdmin gMirrorAdmin;

