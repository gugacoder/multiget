/***************************************************************************
*  mgfilemanager.cpp
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
 
       filename	:	mgfilemanager.cpp
       create	:	2006/09/09
       author	:	liubin
       EMAIL	:	multiget@gmail.com
       version	:	0.5
	   license	:	GPL
 
 
|***|***|***|***|***|***|***|***|***|***|***|***|***|***|***|***|***|***|
|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|
 
	***change log***
	***修订历史***
	2006/07/06 : 
		修正GetNextTask()一些细节
	2006/07/09 : 
		增加了写索引时防止中断
	2006/07/10 : 
		修订了文件下载完后索引不一致的问题
	2006/07/31 : 
		修改文件后缀，部分下载末尾加.mg，索引加.mgidx
		系统整体修改，支持大文件处理
	2006/08/07 :
		修改GetNextTask，避免分配文件末尾给线程下载。
	2006/08/11 :
		修改GetNextTask，在分配新任务前取消原任务标记。
	2006/08/12
		读取索引时过滤长度为0的索引节点，否则可能索引太长了。
	2006/08/13
		增加HeartBeat，速度和完成等参数在本地执行
	2006/09/07
		为兼容VC6做一些修改，取消构造中运行
	2006/09/09
		为兼容VC6做一些修改	
	2006/10/02
		为不能取得文件长度的任务做数据提交的完善
	2006/10/06
		multi url support
	2006/10/22
		some translation
	2006/11/14
		modify for mac and bsd compile
*/

#include "mgfilemanager.h"
#include "showtablewindow.h"
#include "mgsingletask.h"
#include "mainframe.h"
#include "mgapp.h"
#include "speedctrl.h"
#include "mirroradmin.h"
#include "mgapp.h"
#include <wx/intl.h>

#ifdef WIN32
const static int _RECVBUF=16384;  //seems lost the define in common.h in win32
#endif

extern CSpeedCtrl gSpeedCtrl;
//extern CMirrorAdmin gMirrorAdmin;
using namespace std;

CMgFileManager::CMgFileManager(
    CMgSingleTask *parent,
    std::string	mainurl,
    std::string sname,
    std::string spath,
    llong nlen,  //-1 for unknow length
    std::string	mainrefer //only for http
)
{
#ifdef WIN32
    m_sPathFileName = spath + std::string( "\\" ) + sname;
#else
    m_sPathFileName = spath + std::string( "/" ) + sname;
#endif
    m_sFileName = sname;
    m_nFileLen = nlen;
    m_bStop = false;
    m_pParent = parent;
    m_bUpdateUI = parent->UpdateUI();
    m_pShowWin = parent->GetShowTableWin();
    m_nOriginBytes = 0;
    m_nSumBytes = 0;
    m_nSpeed = 0;
    m_nTimeToFinish = -1;
    m_fRatio = 0;

    //add the main url to list
    _ul tt;

    tt.prior = 10000;
    tt.url = mainurl;
    tt.refer = mainrefer;
    tt.bused = false;
    m_UrlList.push_back( tt );

    memset( m_AntSpeed, 0, 10 * sizeof( int ) );
    memset( m_AntId2UrlIndex, 0, 10 * sizeof( int ) );
    m_SumPack = 0;
    m_nSwap = -1;
    pthread_mutex_init ( &m_IndexMutex, NULL );
    pthread_mutex_init ( &m_UListMutex, NULL );

}

/**************************************************
 * Function to conver a wxString to std::string   *
 * In UNICODE mode, wxString::c_str() return a    *
 * wchar_t * type, this type cannot passed to     *
 * std::string.                                   *
 *************************************************/
/*
std::string CMgFileManager::c_str(const wxString &str)
{
	const wxWX2MBbuf tmp_buf= wxConvCurrent->cWX2MB(str);
	const char * tmp_str = (const char *) tmp_buf;
	std::string result=tmp_str;
	return tmp_str;
}
*/
//if true, init ok
bool CMgFileManager::CheckFile()
{

    std::string tempmsg;

	OutMsg( c_str(_("Task object:")) + m_sPathFileName , MSG_INFO );

    //get user home directory
    OutMsg( _("Get user temporary directory...:"), MSG_INFO );

    std::string tempdir;

    if ( !GetTempDir( tempdir ) )
    {
        OutMsg( _("Get tempdir fail"), MSG_ERROR );
        return false;
    }

    OutMsg( c_str(_("Temporary directory is:")) + tempdir );
#ifdef WIN32
	tempdir += std::string( "\\" );
#else
    tempdir += std::string( "/" );
#endif
    m_IndexName = tempdir + m_sFileName + std::string( ".mgidx" );
    m_PartName = tempdir + m_sFileName + std::string( ".mg" );

    tempmsg = c_str(_("Temporary file is:")) + m_PartName;
    OutMsg( tempmsg );
    tempmsg = c_str(_("Index file is:")) + m_IndexName;
    OutMsg( tempmsg );

    //check if *.mgidx exists

    bool bCanResume = ( m_nFileLen == -1 ? false : true );

    OutMsg( _("Check temporary file..."), MSG_INFO );
    FILE *fp;

    if ( bCanResume && NULL != ( fp = fopen( m_PartName.c_str(), "r" ) ) )
    {
        fclose( fp );
        OutMsg( _("Temporary file exists."), MSG_INFO );
    }
    else
    {
        bCanResume = false;
        OutMsg( _("Temporary file not exists."), MSG_WARNNING );
    }

    if ( bCanResume )
    {
        OutMsg( _("Check index file..."), MSG_INFO );

        if ( bCanResume && NULL != ( fp = fopen( m_IndexName.c_str(), "r" ) ) )
        {
            fclose( fp );
            OutMsg( _("Index file exists."), MSG_INFO );
        }
        else
        {
            bCanResume = false;
            OutMsg( _("Index file not exists."), MSG_WARNNING );
        }
    }

    if ( bCanResume )
    {
        OutMsg( _("Checking temporary file length...") );

        if ( bCanResume && m_nFileLen == FileSize( m_PartName ) )
        {
            OutMsg( _("File length OK."), MSG_SUCCESS );
        }
        else
        {
            bCanResume = false;
            OutMsg( _("File length check fail."), MSG_WARNNING );
        }
    }

    //if can't resume,create new file
    if ( bCanResume )
    {
        ReadIndex();
        //m_bOldExist = true;
        m_nOriginBytes = SumBytes();
    }
    else
    {
        OutMsg( _("Creating new temp and index file..."), MSG_INFO );
        //m_bOldExist = false;
        FILE *fp;

        if ( NULL != ( fp = fopen( m_PartName.c_str(), "wb" ) ) )
        {
            if ( WriteIndex() < 0 )
            {
                OutMsg( _("Create index file fail."), MSG_ERROR );
                //m_bError = true;
                return false;
            }

            if ( m_nFileLen != -1 )
            {
                //写入文件长度的0到文件,可以对Linux做优化
#if defined(WIN32) || defined(__WINDOWS__)
                char buf[ 1024 ];
                memset( buf, 0, 1024 );
                llong count = m_nFileLen;

                while ( count >= 1024 )
                {
                    fwrite( buf, 1, 1024, fp );
                    count -= 1024;
                };

                if ( count > 0 )
                    fwrite( buf, 1, count, fp );

#else
				//对Linux的优化分配,2006/07/05
#if defined(__BSD__)|| defined(__DARWIN__)	//it's wx preprocessor
                //bsd and mac haven't ftruncate64
                if ( 0 != ftruncate( fileno( fp ), m_nFileLen ) )
#else
                if ( 0 != ftruncate64( fileno( fp ), m_nFileLen ) )
#endif
                {
                    OutMsg( _("ftruncate fail,quit.\n"), MSG_ERROR );
                    return false;
                }

#endif

            } //if(m_nFileLen!=-1)

            OutMsg( _("Creating new temp and index file..."), MSG_SUCCESS ); //msg should change

            fclose( fp );

        }
        else
        {
            tempmsg = std::string( "Create " ) + m_PartName + string( " Fail, quit." );
            OutMsg( tempmsg, MSG_ERROR );
            return false;
        }
    }

    if ( m_bUpdateUI && m_pShowWin != NULL )
    {

        struct bindex data[ 20 ];
        pthread_mutex_lock( &m_IndexMutex );
        int nsize = m_Index.size();

        for ( int i = 0;i < nsize && i < 20;i++ )
        {
            data[ i ] = m_Index[ i ];
        }

        pthread_mutex_unlock( &m_IndexMutex );

        m_pShowWin->SetRange( m_nFileLen, 0, nsize, data );
    }

    return true;

}


CMgFileManager::~CMgFileManager()
{}

//read the idx file
//maybe more than 10 indexs
int CMgFileManager::ReadIndex()
{
    m_Index.clear();

    ifstream idxfile( m_IndexName.c_str() );

    if ( idxfile.bad() )
    {
		OutMsg( _("Read index fail!"),MSG_ERROR);
		return 0;
	}

    char buf[ 256 ];

    while ( idxfile.good() )
    {
        llong from, to;
        idxfile.getline( buf, 256 );

#ifndef WIN32		
        if ( 2 != sscanf( buf, "(%lld,%lld)", &from, &to ) )
#else
		if ( 2 != sscanf( buf, "(%I64d,%I64d)", &from, &to ) )
#endif
        {
            break;
        }
        else
        {
            if ( to <= from )
                continue; //invalid data

            if ( to < 0 || from < 0 )
                continue; //invalid data

            bindex node;

            node.start = from;

            node.end = to;

            node.mark = 0;

            m_Index.push_back( node );
        }
    }

    idxfile.close();
    //check index

    if ( m_nFileLen == -1 )
    {
        m_Index.clear();
    }
    else
    {
        vector<bindex>::iterator it;

        for ( it = m_Index.begin();it != m_Index.end();it++ )
        {
            if ( it->start < 0 || it->start > it->end || it->start > m_nFileLen )
            {
                m_Index.clear();
                break;
            }

            if ( it->end < 0 || it->end < it->start || it->end > m_nFileLen )
            {
                m_Index.clear();
                break;
            }
        }
    }

	if(m_Index.size()>=1) OutMsg(_("get index ok"));
    return m_Index.size();
}

//ok tid is 1 based
void CMgFileManager::ThreadQuit( int tid )
{

    pthread_mutex_lock( &m_IndexMutex );

    vector<bindex>::iterator it;

    for ( it = m_Index.begin();it != m_Index.end();it++ )
    {
        if ( it->mark == tid )
        {
            it->mark = 0;

            break;
        }
    }

    MergeIndex(); //clear some nodes

    pthread_mutex_unlock( &m_IndexMutex );
}

//write index to file
int CMgFileManager::WriteIndex()
{

    /*/
    FILE *fp;
    fp=fopen( m_IndexName.c_str(),"r");
    if(fp==NULL) return -1;

    char buf[128];
    vector<bindex>::iterator it;
    for ( it = m_Index.begin();it != m_Index.end();it++ )
    {
    sprintf(buf,"(%lld,%lld)\n",it->start,it->end);
    fwrite(buf,strlen(buf),1,fp);

    }

    fclose(fp);
    return m_Index.size();

    /*/
    ofstream idxfile( m_IndexName.c_str(), ios::out | ios::trunc );

    if ( idxfile.bad() )
        return -1;

    vector<bindex>::iterator it;

    for ( it = m_Index.begin();it != m_Index.end();it++ )
    {
        idxfile << "(" << it->start << "," << it->end << ")" << endl;
    }

    idxfile.close();
    return m_Index.size();

    //*/

}

//get file length,check if the length ok
llong CMgFileManager::FileSize( string sFileName )
{

    ifstream f;
    f.open( sFileName.c_str(), ios::binary | ios::in );

    if ( !f.good() || f.eof() || !f.is_open() )
    {
        return 0;
    }

    f.seekg( 0, ios::beg );
    ifstream::pos_type begin_pos = f.tellg();
    f.seekg( 0, std::ios::end );
    llong len = static_cast<llong>( f.tellg() - begin_pos );
    f.close();
    return len;

}


struct SAscendingDateSort
{
    bool operator() ( bindex a, bindex b )
    {
        return a.start < b.start;
    }
};

//order the index
void CMgFileManager::OrderIndex()
{ //按起点顺序从小到大排序

    if ( m_Index.size() <= 1 )
        return ;

    sort( m_Index.begin(), m_Index.end(), SAscendingDateSort() );
}

//lastsize returned for sometime thread can't get filesize ahead till task finish
bool CMgFileManager::IsTaskFinish( llong& lastsize )
{

    if ( m_Index.size() == 1 &&
            m_Index[ 0 ].start == 0 &&
            m_Index[ 0 ].end == m_nFileLen
       )
    {

        lastsize = m_nFileLen;
        return true;
    }
    else
    {
		if ( m_nFileLen == 0 ) 
		{ 
			lastsize=0; return true; 
		} else {
        	lastsize = m_nFileLen;
        	return false;
		}
    }

}

//对索引合并，应该加锁使用
//对没有作标记的0长度索引，删除
void CMgFileManager::MergeIndex()
{
    if ( m_Index.empty() )
        return ;

    if ( m_Index.size() == 1 )
    {
        if ( !m_Index.front().mark && m_Index.front().start == m_Index.front().end )
        {
            m_Index.clear();
            return ;
        }
    }

    vector<bindex> icopy;

    vector<bindex>::iterator it;

    bindex lastindex = m_Index.front();

    for ( it = m_Index.begin() + 1;it != m_Index.end();it++ )
    {
        if ( lastindex.end >= it->start )
        {
            if ( lastindex.end < it->end )
            {
                lastindex.end = it->end;
                lastindex.mark = it->mark;
            }
            else
            {
                continue;
            }
        }
        else
        {
            if ( !lastindex.mark && lastindex.start == lastindex.end )
            {
            }
            else
            {
                icopy.push_back( lastindex );
            }

            lastindex = *it;
        }
    }

    icopy.push_back( lastindex );

    m_Index.swap( icopy );

}

//thread give his data by calling this function
//if >0 returned thread should continue to get data
//if =0 returned thread should stop.
//if <0 returned ,means something error with write data,thread should stop.
//tid is 1 based
int CMgFileManager::FileData( int tid, llong offset, int len, void *data, bool end, int& us )
{

    //if posible ,check file length

    if ( end )
    {

        llong flen = offset + len;

        if ( m_nFileLen != -1 )
        { //have size

            if ( m_nFileLen != flen )
            { //socket close maybe not indicate the file end.
                //sometime overload server will close the connection
                //so we don't use this info if we alread have file size
                OutMsg( _("Server closed socket not at file end, maybe file invalid."), MSG_WARNNING );
            }
            else
            {
                OutMsg( _("File length have been confirmed."), MSG_SUCCESS );
            }
        }
        else
        { //no filesize,asume the flen is filesize,because server closed the connection
            //no function convert long long to string
            char lenstr[ 24 ];
            sprintf( lenstr, "%lld", flen );
            m_nFileLen = flen;
            OutMsg( c_str(_("Final file length:")) +  lenstr );
        }
    }

    pthread_mutex_lock( &m_IndexMutex );

    us = gSpeedCtrl.DataIn( len ); //speed control

    bool combine;
    //write data to file
    int wret = WriteData( offset, len, data, combine );

    if ( wret >= 0 )
    {
        WriteIndex();	//write index

        if ( combine )
        {
            m_nSumBytes = SumBytes() - m_nOriginBytes;
        }
        else
        {
            m_nSumBytes += len;	//record data to calcu speed
        }
    }
    else
    {
        goto quit;
        //pthread_mutex_unlock(&m_IndexMutex);
        //return wret;
    }

    //OK,update ui data
    if ( m_bUpdateUI && m_pShowWin )
    {

        struct bindex data[ 20 ];

        for ( int i = 0;i < int( m_Index.size() ) && i < 20;i++ )
        {
            data[ i ] = m_Index[ i ];
        }

        m_pShowWin->SetData( m_Index.size(), data );

    }

quit:

    m_UrlList[ m_AntId2UrlIndex[ tid - 1 ] ].prior++; //multi url prior
    m_AntSpeed[ tid - 1 ] ++;
    m_SumPack++;


    if ( m_UrlList.size() > 1 && m_nSwap > 0 )
    {
        if ( m_nSwap == tid )
        {

            if ( wret > 0 )
                wret = -3;

            memset( m_AntSpeed, 0, 10 * sizeof( int ) );

            m_SumPack = 0;

            m_nSwap = -1;

        }

        pthread_mutex_unlock( &m_IndexMutex );
        return wret;

    }

    //10M check
    if ( m_SumPack * RECVBUF > 10 * 1024 * 1024 && m_UrlList.size() > 1 )
    {

        //检查我们这个线程的速度和最大速度的差异，如果>=2就试图切换
        //本线程速度＝m_AntSpeed[tid-1];
        //找最大速度
        int maxspeed = -1;

        for ( int i = 0;i < 10;i++ )
        {
            if ( m_AntSpeed[ i ] > maxspeed )
                maxspeed = m_AntSpeed[ i ];
        }

        int minspeed = 1000000;
        int minpos = -1;
        //找最小速度

        for ( int i = 0;i < 10;i++ )
        {
            if ( m_AntSpeed[ i ] > 0 && m_AntSpeed[ i ] < minspeed )
            {
                minspeed = m_AntSpeed[ i ];
                minpos = i;
            }
        }

        //check if have new mirror.
        bool havenew = false;

        pthread_mutex_lock( &m_UListMutex );

        std::vector<_ul>::const_iterator itm;

        for ( itm = m_UrlList.begin();itm != m_UrlList.end();itm++ )
        {
            if ( !itm->bused )
            {
                havenew = true;
                break;
            }
        }

        pthread_mutex_unlock( &m_UListMutex );

        if ( havenew )
        {
            m_nSwap = minpos + 1;

            if ( m_nSwap == tid )
            {
                if ( wret > 0 )
                    wret = -3;

                memset( m_AntSpeed, 0, 10 * sizeof( int ) );

                m_SumPack = 0;

                m_nSwap = -1;
            }
        }

        else if ( float( maxspeed ) / minspeed >= 2.0 )
        {

            m_nSwap = minpos + 1;

            if ( m_nSwap == tid )
            {

                if ( wret > 0 )
                    wret = -3;

                memset( m_AntSpeed, 0, 10 * sizeof( int ) );

                m_SumPack = 0;

                m_nSwap = -1;
            }

        }
        else
        { //wait for next
            memset( m_AntSpeed, 0, 10 * sizeof( int ) );
            m_SumPack = 0;
            m_nSwap = -1;
        }
    }

    pthread_mutex_unlock( &m_IndexMutex );
    //if socks closed the end this session
    //2006/10/09 comment out 2 lines
    //if(end && wret<0) return wret;
    //else if(end && wret>=0) return 0;
    return wret;
}

//write data to file
//return:
// <0 error
// =0 merged with other block,stop
// >0 continue

int CMgFileManager::WriteData( llong offset, int len, void *data, bool& combine )
{

    FILE * fp;

    combine = false;

//use wxwidgets preprocesser ,mac and bsd only have fopen
#if defined(WIN32) || defined(__BSD__) || defined(__WINDOWS__) || defined(__DARWIN__)  

    if ( NULL != ( fp = fopen( m_PartName.c_str(), "rb+" ) ) )
#else 

    if ( NULL != ( fp = fopen64( m_PartName.c_str(), "r+" ) ) )
#endif

    {

#ifdef WIN32

        if ( 0 != fseek( fp, offset, SEEK_SET ) )
#else

        if ( 0 != fseeko( fp, offset, SEEK_SET ) )
#endif

        {
            OutMsg( _("fseeko fail."), MSG_ERROR );
            return -1;
        }

        //似乎要在这里防止数据过载，否则文件末尾会变化

        if ( m_nFileLen != -1 )
        { //know filelen

            if ( offset + ( llong ) len <= m_nFileLen )
            {
                if ( len != int( fwrite( data, 1, len, fp ) ) )
                {
                    OutMsg( _("fwrite fail."), MSG_ERROR );
                    return -1;
                }
            }
            else if ( m_nFileLen - offset > 0 )
            {
                OutMsg( _("data overload"), MSG_ERROR );

                if ( m_nFileLen - offset != int( fwrite( data, 1, m_nFileLen - offset, fp ) ) )
                {
                    OutMsg( _("fwrite fail."), MSG_ERROR );
                    return -1;
                }
            }
            else
            {
                OutMsg( _("data overload"), MSG_ERROR );
            }
        }
        else //unknow filelen
        {
            if ( len != int( fwrite( data, 1, len, fp ) ) )
            {
                OutMsg( _("fwrite fail."), MSG_ERROR );
                return -1;
            }
        }

        fclose( fp );
        //是否继续下载要判断是否这段数据已经连接到了下一个已下载的数据段
        //首先找到这个数据段在索引中的项目，然后查找下一个索引的起点位置，如果能够重合，就停止本线程下载
        //bool found=false;

        for ( int i = 0;i < int( m_Index.size() );i++ )
        {
            if ( m_Index[ i ].end == offset )
            {
                //found

                if ( i == int( m_Index.size() - 1 ) )
                { //last block

                    m_Index[ i ].end += len;

                    if ( m_nFileLen != -1 )
                    { //have filelen,we can judge if data end

                        if ( m_Index[ i ].end > m_nFileLen )
                        {
                            OutMsg( _("Data overload, maybe file invalid!"), MSG_WARNNING );
                            m_Index[ i ].end = m_nFileLen;
                            combine = true;
                        }

                        return m_Index[ i ].end == m_nFileLen ? 0 : 1;
                    }
                    else //2006/10/02 add
                    { //no filelen,we can't judge if data end,just continue
                        return 1;
                    }
                }
                else if ( m_Index[ i + 1 ].start <= offset + len )
                { //block merge with next block
                    //是否让本线程退出取决于数据是否超越了下一个块的末尾，如果超越了下个快的末尾
                    //那么应该让本线程继续，否则应该退出本线程，继续选择下一个下载点。
                    int nret = m_Index[ i + 1 ].end > offset + len ? 0 : 1;
                    m_Index[ i ].end += len;
                    combine = true;
                    MergeIndex();
                    return nret;
                }
                else
                { //no merge with other,continue
                    m_Index[ i ].end += len;
                    return 1;
                }
            }
        }

        //not found the data head,maybe this block is overlapped by others,break.
        combine = true;

        return 0;

    }

    OutMsg( _("open datafile error when write !!"), MSG_ERROR );
    return -1;
}


//2006/07/06修正一些细节
//这个函数会对任务结束做出一个判断
//tid is 1 based
//返回1成功，返回0失败
bool CMgFileManager::GetTask( int tid, llong &taskpos )
{
    //提取一个任务片的起点
    //算法：先搜现有块的末尾，如果有空的分配出去，如果没空的，搜索一个间隔最大的空白区域，在中间插入起点。

    if ( m_bStop )
    {
        OutMsg( _("task canceled, no more work."), MSG_INFO );
        return false;
    }

	if ( m_nFileLen == 0 ) {
		OutMsg( _("zero length task, no work") , MSG_INFO );
		return false;  //a zero length task
	}

    pthread_mutex_lock( &m_IndexMutex );
    //寻找原来的点然后注销标记
    vector<bindex>::iterator it;

    for ( it = m_Index.begin();it != m_Index.end();it++ )
    {
        if ( it->mark == tid )
        {
            it->mark = 0;
            break;
        }
    }

    OrderIndex();
    MergeIndex();

    if ( m_Index.empty() )
    {
        AddNewIndex( tid, 0 );
        taskpos = 0L;
        goto quit1;
    }
    else if ( m_Index.size() == 1 )
    {
        if ( m_Index[ 0 ].start == 0 && m_Index[ 0 ].end == m_nFileLen )
        { //in fact,task finish
            goto quit0;
        }
        else if ( m_Index[ 0 ].start != 0 )
        {	//没从起点开始分配，加一个起点
            AddNewIndex( tid, 0 );
            taskpos = 0L;
            goto quit1;
        }
        else
        {
            if ( !m_Index[ 0 ].mark )
            { //唯一的索引还没分配出去
                taskpos = m_Index[ 0 ].end;
                m_Index[ 0 ].mark = tid ;
                goto quit1;
            }
            else
            { //唯一的索引已经分配出去了

                if ( m_nFileLen - m_Index[ 0 ].end < RECVBUF )
                { //to little data to split
                    goto quit0;
                }
                else
                {
                    llong newpos = ( m_Index[ 0 ].end + m_nFileLen ) / 2 - 1;
                    AddNewIndex( tid, newpos );
                    taskpos = newpos;
                    goto quit1;
                }
            }
        }
    }
    else
    { //item>1
        //looking for a index not delivered

        vector<bindex>::iterator it;

        for ( it = m_Index.begin();it != m_Index.end();it++ )
        {

            if ( !it->mark && it->end < m_nFileLen )
            { //文件的末尾不要再分配线程下载08/07
                it->mark = tid ;
                taskpos = it->end;
                goto quit1;
            }
        }

        //没有索引可用，找最大的空白插入
        llong maxpos = 0;

        llong maxempty = 0;

        if ( m_Index[ 0 ].start != 0 )
        { //起始空白
            maxpos = 0;
            maxempty = m_Index[ 0 ].start;
        }

        if ( m_nFileLen - m_Index.back().end > maxempty )
        { //末尾空白
            maxpos = m_Index.back().end;
            maxempty = m_nFileLen - m_Index.back().end;
        }

        //中间段空白
        for ( int i = 1;i < int( m_Index.size() );i++ )
        {
            if ( m_Index[ i ].start - m_Index[ i - 1 ].end > maxempty )
            {
                maxpos = m_Index[ i - 1 ].end;
                maxempty = m_Index[ i ].start - m_Index[ i - 1 ].end;
            }
        }

        //找到了最大的空白。
        if ( maxempty < RECVBUF )
        {
            //WriteIndex();
            goto quit0;
        }
        else
        {
            taskpos = maxpos + maxempty / 2 - 1 ;
            AddNewIndex( tid, taskpos );
            goto quit1;
        }
    }

quit1:
    pthread_mutex_unlock( &m_IndexMutex );
    return true;

quit0:
    pthread_mutex_unlock( &m_IndexMutex );
    return false;
}

//因为是内部函数，控制锁不在这里面加
//添加索引必须要分配出去
//tid is 1 based
void CMgFileManager::AddNewIndex( int tid, llong pos )
{
    bindex bi;
    bi.start = bi.end = pos;
    //mark is 1 based!
    bi.mark = tid ;
    m_Index.push_back( bi );
    OrderIndex();
}

//num should enough big
int CMgFileManager::GetTaskIndex( bindex data[], int num )
{
    pthread_mutex_lock( &m_IndexMutex );
    int size = m_Index.size();

    if ( size > num )
        return 0;

    for ( int i = 0;i < size ;i++ )
        data[ i ] = m_Index[ i ];

    pthread_mutex_unlock( &m_IndexMutex );

    return size;
}

void CMgFileManager::UpdateRunningTable( CShowTableWindow* pwin )
{

    struct bindex data[ 20 ];
    pthread_mutex_lock( &m_IndexMutex );
    int nsize = m_Index.size();

    for ( int i = 0;i < nsize && i < 20;i++ )
    {
        data[ i ] = m_Index[ i ];
    }

    pthread_mutex_unlock( &m_IndexMutex );
    pwin->SetRange( m_nFileLen, 0, nsize, data );
    m_pShowWin = pwin;
    m_bUpdateUI = true;
}

void CMgFileManager::CloseUpdateUI()
{
    m_bUpdateUI = false;
    m_pShowWin = NULL;
}

void CMgFileManager::Stop()
{
    m_bUpdateUI = false;
    m_pShowWin = NULL;
    m_bStop = true;

}

void CMgFileManager::OutMsg( const wxChar* str, _MSGTYPE type )
{
    OutMsg(c_str(str), type );
}

void CMgFileManager::OutMsg( const std::string& str , _MSGTYPE type )
{
    m_pParent->OutMsg( -2, str, type );
}

void CMgFileManager::HeartBeat()
{

    if ( m_ByteSeq.size() < 10 )
    {
        m_ByteSeq.push_back( m_nSumBytes );
    }
    else
    {
        m_ByteSeq.pop_front();
        m_ByteSeq.push_back( m_nSumBytes );
    }

    int beats = m_ByteSeq.size();
    int diff = m_nSumBytes - m_ByteSeq.front();
    m_nSpeed = diff / beats;

    if ( m_nSpeed != 0 && m_nFileLen != -1 && m_nFileLen != 0 )
    {
        m_nTimeToFinish = ( m_nFileLen - m_nSumBytes - m_nOriginBytes ) / m_nSpeed;
    }
    else
    {
        m_nTimeToFinish = -1;
    }

    if ( m_nFileLen != -1 && m_nFileLen != 0 )
    {
        m_fRatio = float( m_nSumBytes + m_nOriginBytes ) / m_nFileLen;
    }
}

//no lock for this func
llong CMgFileManager::SumBytes()
{
    vector<bindex>::const_iterator it;

    llong sum = 0;

    for ( it = m_Index.begin(); it != m_Index.end(); it++ )
    {
        sum += it->end - it->start;
    }

    return sum;
}

void CMgFileManager::GetRunningArgs( float& fratio, llong& finish, int& speed, int& ttf )
{
    fratio = m_fRatio;
    finish = m_nSumBytes + m_nOriginBytes;
    speed = m_nSpeed;
    ttf = m_nTimeToFinish;
}

//选取Url地址采用积分制，积分规则
//1:某地址接收到一个数据包，积分+1
//2:某个地址连接报告正常+500
//3:某个地址连接报告失败-5000，无效-100000
//4:主地址有20000个预设积分
//5:没被使用的地址有10000预设积分，一旦使用了就清零
//6:每次选择积分最高的地址返回
//7:地址分配出去-500
//8:跳转地址＋100

bool CMgFileManager::GetTask(
    int aid,
    std::string& fullurl,
    bool& urlneedcheck,   //这个url是否需要检查正确性
    llong& from,
    std::string& refer
)
{

    if ( !GetTask( aid, from ) )
    {
        OutMsg( _("No more task, quit!"), MSG_INFO );
        return false;
    }

    pthread_mutex_lock( &m_UListMutex );

    int lsize = m_UrlList.size();
    int maxprior = -100000;
    int maxpos = -1;

    for ( int i = 0;i < lsize;i++ )
    {
        if ( m_UrlList[ i ].prior > maxprior )
        {
            maxprior = m_UrlList[ i ].prior;
            maxpos = i;
        }
    }

    if ( maxpos == -1 )
    {
        //list empty or all url invalid
        //return main url,for retry need a task url.
        //OutMsg("no heathy mirror url ,use main url",MSG_WARNNING);
        maxpos = 0;
    }

    if ( !m_UrlList[ maxpos ].bused )
    {
        m_UrlList[ maxpos ].bused = true;
        m_UrlList[ maxpos ].prior = 0;  //first use clear prior
        urlneedcheck = maxpos != 0 ? true : false;	//first time need check if not main.

    }
    else
    {

        urlneedcheck = false;  //not a first time use,don't check
        m_UrlList[ maxpos ].prior -= 500; //minus for use
    }


    fullurl = m_UrlList[ maxpos ].url;
    refer = m_UrlList[ maxpos ].refer;
    m_AntId2UrlIndex[ aid - 1 ] = maxpos;

    memset( m_AntSpeed, 0, 10 * sizeof( int ) ); //clear speed table
    m_SumPack = 0;


    pthread_mutex_unlock( &m_UListMutex );
    return true;

}

//report url good or bad
void CMgFileManager::ReportUrl( int adjust, std::string url )
{
    pthread_mutex_lock( &m_UListMutex );
    int lsize = m_UrlList.size();

    for ( int i = 0; i < lsize; i++ )
    {
        if ( m_UrlList[ i ].url == url )
        {
            m_UrlList[ i ].prior += adjust; //normal +200，temp fail -5000,invalid -100000

            if ( adjust > 0 )
                OutMsg( c_str(_("URL OK:")) + url, MSG_SUCCESS );
            else if ( adjust <= -100000 )
                OutMsg( c_str(_("URL fail:")) + url, MSG_ERROR );
            else
                OutMsg( c_str(_("URL temp fail:")) + url, MSG_WARNNING );

            break;
        }
    }

    pthread_mutex_unlock( &m_UListMutex );
}

void CMgFileManager::ReportRedirect( std::string origin, std::string redirect )
{
    pthread_mutex_lock( &m_UListMutex );
    int lsize = m_UrlList.size();

    for ( int i = 0;i < lsize;i++ )
    {
        if ( m_UrlList[ i ].url == origin )
        {
            m_UrlList[ i ].refer = m_UrlList[ i ].url;
            m_UrlList[ i ].url = redirect;
            m_UrlList[ i ].prior += 100; //an active url.
            OutMsg( origin + c_str(_(" redirect to ")) + redirect, MSG_INFO );
            break;
        }
    }

    pthread_mutex_unlock( &m_UListMutex );
}

//must filter out the same
void CMgFileManager::AddMirrorUrl( const std::string& url )
{

    _ul tt;
    pthread_mutex_lock( &m_UListMutex );

    tt.prior = 10000; //new url have high prior
    tt.url = url;
    tt.bused = false;

    bool bExists = false;

    //check if any same url exists

    std::vector<_ul>::const_iterator it;

    for ( it = m_UrlList.begin();it != m_UrlList.end();it++ )
    {
        if ( url == it->url )
        {
            bExists = true;
            break;
        }
    }

    if ( !bExists )
        m_UrlList.push_back( tt );

    pthread_mutex_unlock( &m_UListMutex );
}
