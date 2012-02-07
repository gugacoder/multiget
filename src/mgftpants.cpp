/***************************************************************************
*  mgftpants.cpp
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
#include "mgftpants.h"
#include "mgfilemanager.h"
#include "mgsingletask.h"
#include "mgurlparser.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <pthread.h>

using namespace std;

CMgFtpAnts::CMgFtpAnts(
    CMgSingleTask	*parent,
    CMgFileManager *fm,
    std::string url,  //fullurl
    int aid,  ////上层分配的编号
    int retry,
    int retrywait,
    llong from,
    llong tsize
)

{
    m_pParent = parent;
    m_pFM = fm;

    m_nAntId = aid;

    m_nRetry = retry;
    m_nRetryWait = retrywait;
    m_from = from;
    m_nFileSize = tsize;

    CUrlParser par;
    par.SetUrl( url );

    m_file = par.GetEscFilePathName();
    m_user = par.GetUser();
    m_pass = par.GetPass();
    m_Server = par.GetServer();
    m_Port = par.GetPort();
    m_sURL = par.GetRawUrl();
    m_nTotalByte = ( llong ) 0;
}


CMgFtpAnts::~CMgFtpAnts()
{
}

void CMgFtpAnts::OutMsg( const wxChar* str, _MSGTYPE type )
{
    OutMsg(c_str(str), type );
}

//消息输出函数
void CMgFtpAnts::OutMsg( const string& str, _MSGTYPE type )
{
    m_pParent->OutMsg( m_nAntId, str, type );
}

//return value
// <0  : something wrong ,task fail
// =0  : redirect to new url
// >0  : finish piece
int CMgFtpAnts::Go()
{

    int nret;
    int dport;		//数据服务器端口
    char dip[ 24 ];		//数据服务器地址

    CMgFtpSocket DataSock;

again:

    Close();
    DataSock.Close();

    OutMsg( c_str(_("Connecting ")) + m_Server + std::string( "..." ) );

    //make connection

    if ( !MakeCtrlConn ( m_Server.c_str(), m_Port ) )
    {
        OutMsg( _("Connect fail."), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -1;
        }
    }


    //login
    if ( !Login( m_user.c_str(), m_pass.c_str() ) )
    {
        OutMsg( _("Login fail."), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -2;
        }

    }

    //send PASV command to enter pasv mode
    if ( ! EnterPasv( dip, &dport ) )
    {
        Logout();

        OutMsg( _("Enter passive mode fail."), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -5;
        }

    }

    //pasv ok,we then make data connection
    //logmsg("获得数据服务器=%s，数据端口＝%d\n",dip,dport);

    //set same proxy as control
    DataSock.SetProxy( *this );

    //DataSock.SetFtpProxy(*this);

    if ( !DataSock.Connect( dip, dport ) )
    {
        Logout();

        OutMsg( _("Data connection fail"), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -6;
        }

    }

    OutMsg( _("Data connection Ok"), MSG_SUCCESS );


    if ( !EnterBinaryMode( ) )
    {
        Logout();

        OutMsg( _("Enter binary mode fail"), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -3;
        }

    }

    //set resume point
    if ( ! SetResume( m_from + m_nTotalByte ) )
    {
        if ( m_from + m_nTotalByte != 0 )
        {
            Logout();

            OutMsg( _("Set resume point fail"), MSG_WARNNING );

            if ( Retry() )
            {
                goto again;
            }
            else
            {
            	OutMsg( _("No more retry, quit."), MSG_ERROR );
                return -4;
            }

        }
    }


    if ( !GetFile( m_file.c_str() ) )
    {
        Logout();

        OutMsg( _("Fail to get file"), MSG_WARNNING );

        if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return -7;
        }

    }

    char buf[ 100 ];
    sprintf( buf, (c_str(_("Receiving file data from %lld ..."))).c_str(), m_from + m_nTotalByte );
    OutMsg( buf );

    llong ndata = 0;	//本次GetData接收到的数量

    nret = GetData( DataSock, m_from + m_nTotalByte, ndata );

    if ( nret == 0 )
    { //ok
        OutMsg( _("Finish task session normally, quit.") , MSG_SUCCESS);
        return 1;
    }
    else
    { //error

        switch ( nret )
        {

            case -1:
            OutMsg( _("Write file error."), MSG_WARNNING );
            break;

            case - 2:
            OutMsg( _("Network error."), MSG_WARNNING );
            break;

            case - 3:
            OutMsg( _("change a URL" ), MSG_WARNNING );
            break;

            default:
            OutMsg( _("Other error"), MSG_WARNNING );
            break;
        }

        m_nTotalByte += ndata;

        if ( nret == -3 )
        {
            return -3;
        }
        else if ( Retry() )
        {
            goto again;
        }
        else
        {
            OutMsg( _("No more retry, quit."), MSG_ERROR );
            return nret;
        }
    }
}


//控制连接上发送请求文件命令
bool CMgFtpAnts::GetFile( const char* filename )
{
    int nret;

    char buffer[ 512 ];
    sprintf( buffer, "RETR %s\r\n", filename );

    if ( !Send( buffer, strlen( buffer ) ) )
    {
        return false; //write data error
    }

    sprintf( buffer, "RETR %s", filename );
    OutMsg( buffer );

    nret = GetRetCode( );

    if ( nret == 150 )
        return true;
    else
        return false;
}

//获取数据
//返回0停止接收数据，返回<0错误，返回>0 继续接收数据
// -1 数据写文件错，-2网络错误但还需要继续接收数据
//bytes最终返回本次写到文件中的数据量，不代表从网络读的数据量
//如果没有错误发生，则从网络读的数据量就等于写到文件的数量
int CMgFtpAnts::GetData( CMgFtpSocket& sock, llong spos, llong& bytes )
{

    int movepos = 0;
    int nret;
    llong total = 0;	//本次已经接收的数据,最后返回给bytes
    unsigned char buffer[ RECVBUF ];

    while ( 1 )
    {
        nret = sock.Read( buffer + movepos, RECVBUF - movepos );

        if ( nret < 0 )
        { //net error
            /*/
            			bytes=total;
            			return -2;
            /*/

            if ( movepos > 0 )
            {

                int wret = WriteData( spos + total, movepos, buffer );
                //写文件也失败，本次写的不算数

                if ( wret < 0 && wret != -3 )
                {
                    bytes = total;
                    return -1;
                }
                else if ( wret < 0 && wret == -3 )
                {
                    bytes = total;
                    return -3;
                }

                //写文件正确，刚好也可以停止本线程了
                else if ( wret == 0 )
                {
                    bytes = total + movepos;
                    return 0;
                }

                //网络错误，但写文件正确，还需要继续接收数据
                else
                {
                    bytes = total + movepos;
                    return -2;
                }

            }
            else
            {
                bytes = total;
                return -2; //网络错误，但还需要继续接收数据
            }

            //*/

        }
        else if ( nret == 0 )
        {	//数据传送正常关闭，传输完毕
            //写剩余数据
            int wret = WriteData( spos + total, movepos, buffer, true );

            if ( wret < 0 && wret != -3 )
            {
                //				DBGOUT("写数据失败");
                bytes = total;
                return -1;
            }
            else if ( wret < 0 && wret == -3 )
            {
                bytes = total;
                return -3;
            }
            else if ( wret == 0 )
            { //写文件正常，返回也要求本线程停止下载，很好
                bytes = total + movepos;
                return 0;
            }
            else
            {
                //DBGOUT("文件长度存在疑问");
                //OutMsg( "file length doubt!!", MSG_WARNNING );
                bytes = total + movepos;
                return 0;	//对方都主动关闭连接了，我们还是不继续吧。
            }

        }
        else //nret>0 网络接收正常
        {
            movepos += nret;

            if ( movepos >= RECVBUF - 1024 )
            {
                //buffer full , write out
                int wret = WriteData( spos + total, movepos, buffer );

                if ( wret < 0 && wret != -3 )
                {
                    bytes = total;
                    return -1;  //写数据错
                }
                else if ( wret < 0 && wret == -3 )
                {
                    bytes = total;
                    return -3;
                }
                else if ( wret == 0 )
                {
                    total += movepos;
                    movepos = 0;
                    bytes = total;
                    return 0;  //管理器要求结束这个线程
                }
                else
                { //一切正常，要继续下载

                    total += movepos;
                    movepos = 0;
                    continue;
                }
            }
            else
            { //接收的数据量还不够写出去
                continue;
            }
        } //if
    } //while(1)

    return 0;

}

int CMgFtpAnts::WriteData( llong offset, int len, unsigned char *buf, bool end )
{
    int OldState, nret, us;
    pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &OldState );
    nret = m_pFM->FileData( m_nAntId , offset, len, buf, end, us );
    pthread_setcancelstate ( OldState, NULL );

    if ( us > 0 )
#ifdef WIN32		
        Sleep( us*1000 );
#else
		usleep( us );
#endif

    return nret;
}

bool CMgFtpAnts::Retry()
{
    if ( --m_nRetry > 0 )
    {
        char buf[ 50 ];

        sprintf( buf, (c_str(_("Wait %d seconds to retry..."))).c_str(), m_nRetryWait );

        OutMsg( buf );

        m_pParent->m_nError++;
#ifdef WIN32
        Sleep( 1000*m_nRetryWait );
#else
        sleep( m_nRetryWait );
#endif

        return true;
    }

    return false;
}

