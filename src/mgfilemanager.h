/***************************************************************************
*  mgfilemanager.h
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
       文件名	:   mgfilemanager.hh
       建立日期	:   2006/09/09
       作者		:   liubin
       EMAIL	:   multiget@gmail.com
       版本		:   0.3
	   软件协议	:	GPL
 
*/

#ifndef _FILE_MANAGER_H
#define _FILE_MANAGER_H

#include "common.h"
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <wx/string.h>

class CMgSingleTask;

class CShowTableWindow;

class MyApp;

//struct bindex;

class CMgFileManager
{

public:
    //文件名，存放路径，文件长度，文件长度=0时代表不知道文件长度
    CMgFileManager( CMgSingleTask* parent,
                    std::string mainurl,
                    std::string sname,
                    std::string spath,
                    llong nlen,
                    std::string mainrefer = std::string( "" ) );
    bool CheckFile(); //call after ctor
    virtual ~CMgFileManager();
    //数据提交
    int FileData( int tid, llong offset, int len, void *data, bool end, int& us );
    //判断整个初始化是否正常
    //bool 	IsOk();

    //线程退出通知
    void ThreadQuit( int tid );
    //提取任务索引
    int GetTaskIndex( bindex data[], int num ); //提取任务信息
    //任务是否结束
    bool	IsTaskFinish( llong& lastsize );
    //停止任务
    void	Stop();
    //UI函数
    void UpdateRunningTable( CShowTableWindow* pwin );
    void CloseUpdateUI();

    void	HeartBeat();
    void GetRunningArgs( float& fratio, llong& finish, int& speed, int& ttf );
    void ReportUrl( int adjust, std::string url );
    void	ReportRedirect( std::string origin, std::string redirect );
    bool	GetTask(
        int aid,
        std::string& fullurl,
        bool& urlneedcheck,
        llong& from,
        std::string& refer
    );

    void AddMirrorUrl(const std::string& url );

protected:

    int ReadIndex();
    int WriteIndex();
    llong FileSize( std::string sFileName );
    int WriteData( llong offset, int len, void *data, bool& combine );
    void OrderIndex();
    void MergeIndex();
    void AddNewIndex( int tid, llong pos );

    void OutMsg( const std::string& outmsg, _MSGTYPE type = MSG_INFO );
	void OutMsg( const wxChar* str, _MSGTYPE type = MSG_INFO );

    bool GetTask( int tid, llong &taskpos );

    llong	SumBytes();

    bool m_bStop;

	std::string m_sPathFileName;  // full path filename
    std::string m_sFileName;  //only filename
    llong m_nFileLen;
    llong	m_nSumBytes;
    llong	m_nOriginBytes;

    int	m_nSpeed;
    int	m_nTimeToFinish;
    float	m_fRatio;
    std::list<llong>	m_ByteSeq;

private:
    std::string m_PartName;
    std::string m_IndexName;


    pthread_mutex_t m_IndexMutex;
    pthread_mutex_t m_UListMutex;

    std::vector<bindex> m_Index;

    bool m_bUpdateUI;
    CShowTableWindow	*m_pShowWin;
    CMgSingleTask	*m_pParent;


    struct _ul
    {
        int prior;
        bool bused;
        std::string url;  //full mirror url
        std::string refer; //only http need
    };

    std::vector<_ul>	m_UrlList; //the first is main url;

    int m_AntId2UrlIndex[ 10 ]; //it store the url index
    int m_AntSpeed[ 10 ];
    int m_SumPack;
    int	m_nSwap;
private: //private functions.
//	std::string c_str(const wxString &str);
};


#endif

