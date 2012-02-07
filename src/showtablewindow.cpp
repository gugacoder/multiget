/*
      文件名	:	showtablewindow.cpp
      建立日期	:	2006/07/12
      作者		:	liubin
      EMAIL	:	multiget@gmail.com
      版本		:	0.7
   软件协议	:	GPL

在右下边显示下载进度的图表

***修订历史***
2006/08/04 新增弹出菜单设置块的大小
2006/08/06 修正GetBlockColor整数溢出
2006/08/07 弹出菜单限制太小的选择（刷新速度问题）
*/

#include "showtablewindow.h"
#include "./icons/gray_block.xpm"
#include "./icons/green_block.xpm"

#include "./icons/yellow_1_block.xpm"
#include "./icons/yellow_2_block.xpm"
#include "./icons/yellow_3_block.xpm"
#include "./icons/yellow_4_block.xpm"
#include "./icons/yellow_5_block.xpm"
#include "./icons/yellow_6_block.xpm"
#include "./icons/yellow_7_block.xpm"
#include "./icons/yellow_8_block.xpm"
#include "./icons/yellow_9_block.xpm"
#include "./icons/yellow_10_block.xpm"

#include "mgapp.h"
#ifndef WIN32
#include <sys/time.h>
#endif
#include <wx/dcbuffer.h>

using namespace std;

#define _BLOCK_SIZE 10	//每个数据块在屏幕上的像素宽度
#define _BLOCK_GAP	1	//相临块的间隔
#define _BLOCK_DIST (_BLOCK_SIZE+_BLOCK_GAP)  //相临块的距离 
//边距定义
#define _TOP_EDGE	5
#define _LEFT_EDGE	5
#define _RIGHT_EDGE 5
#define _BOTTOM_EDGE 10

DEFINE_EVENT_TYPE( mgID_8K )
DEFINE_EVENT_TYPE( mgID_16K )
DEFINE_EVENT_TYPE( mgID_32K )
DEFINE_EVENT_TYPE( mgID_64K )
DEFINE_EVENT_TYPE( mgID_128K )
DEFINE_EVENT_TYPE( mgID_256K )
DEFINE_EVENT_TYPE( mgID_512K )
DEFINE_EVENT_TYPE( mgID_1M )
DEFINE_EVENT_TYPE( mgID_2M )
DEFINE_EVENT_TYPE( mgID_4M )

BEGIN_EVENT_TABLE( CShowTableWindow, wxScrolledWindow )
EVT_RIGHT_UP( CShowTableWindow::OnMouseEvent )
EVT_SIZE( CShowTableWindow::OnSize )
EVT_MENU( mgID_8K, CShowTableWindow::On8K )
EVT_MENU( mgID_16K, CShowTableWindow::On16K )
EVT_MENU( mgID_32K, CShowTableWindow::On32K )
EVT_MENU( mgID_64K, CShowTableWindow::On64K )
EVT_MENU( mgID_128K, CShowTableWindow::On128K )
EVT_MENU( mgID_256K, CShowTableWindow::On256K )
EVT_MENU( mgID_512K, CShowTableWindow::On512K )
EVT_MENU( mgID_1M, CShowTableWindow::On1M )
EVT_MENU( mgID_2M, CShowTableWindow::On2M )
EVT_MENU( mgID_4M, CShowTableWindow::On4M )
END_EVENT_TABLE()

void CShowTableWindow::On8K( wxCommandEvent& )
{
    SetUnit( 8 );
}

void CShowTableWindow::On16K( wxCommandEvent& )
{
    SetUnit( 16 );
}

void CShowTableWindow::On32K( wxCommandEvent& )
{
    SetUnit( 32 );
}

void CShowTableWindow::On64K( wxCommandEvent& )
{
    SetUnit( 64 );
}

void CShowTableWindow::On128K( wxCommandEvent& )
{
    SetUnit( 128 );
}

void CShowTableWindow::On256K( wxCommandEvent& )
{
    SetUnit( 256 );
}

void CShowTableWindow::On512K( wxCommandEvent& )
{
    SetUnit( 512 );
}

void CShowTableWindow::On1M( wxCommandEvent& )
{
    SetUnit( 1024 );
}

void CShowTableWindow::On2M( wxCommandEvent& )
{
    SetUnit( 2048 );
}

void CShowTableWindow::On4M( wxCommandEvent& )
{
    SetUnit( 4096 );
}

void CShowTableWindow::OnSize( wxSizeEvent& )
{
    m_bForceUpdata = true;
    Update(); //似乎也不是立即刷新？
}

CShowTableWindow::CShowTableWindow( wxWindow* parent )
        : wxScrolledWindow( parent, -1, wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE )
{
    m_from = m_to = m_nBlock = 0;
    m_unit = 8;  //缺省的显示单元40KB
    m_bForceUpdata = false;

    m_DataList.clear();
    //EnableScrolling(false,true);
    //背景颜色
    //    SetBackgroundColour( wxColour( 255, 255, 255 ) );
    SetScrollRate( _BLOCK_DIST, _BLOCK_DIST );

    m_GrayBlock = wxBitmap( gray_block_xpm );

    //	m_YellowBlock=yellow;
    m_GreenBlock = wxBitmap( green_block_xpm );


    m_YellowBlock[ 0 ] = wxBitmap( yellow_1_block_xpm );
    m_YellowBlock[ 1 ] = wxBitmap( yellow_2_block_xpm );
    m_YellowBlock[ 2 ] = wxBitmap( yellow_3_block_xpm );
    m_YellowBlock[ 3 ] = wxBitmap( yellow_4_block_xpm );
    m_YellowBlock[ 4 ] = wxBitmap( yellow_5_block_xpm );
    m_YellowBlock[ 5 ] = wxBitmap( yellow_6_block_xpm );
    m_YellowBlock[ 6 ] = wxBitmap( yellow_7_block_xpm );
    m_YellowBlock[ 7 ] = wxBitmap( yellow_8_block_xpm );
    m_YellowBlock[ 8 ] = wxBitmap( yellow_9_block_xpm );
    m_YellowBlock[ 9 ] = wxBitmap( yellow_10_block_xpm );

}

//主要处理右键弹出菜单
void CShowTableWindow::OnMouseEvent( wxMouseEvent& event )
{
    wxPoint pos = GetPosition();
    // 菜单开始
    wxMenu *rootPopMenu = new wxMenu( _( "Block Size" ) );

    rootPopMenu->AppendCheckItem( mgID_8K, _( "8K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_16K, _( "16K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_32K, _( "32K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_64K, _( "64K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_128K, _( "128K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_256K, _( "256K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_512K, _( "512K/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_1M, _( "1M/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_2M, _( "2M/Block" ) );
    rootPopMenu->AppendCheckItem( mgID_4M, _( "4M/Block" ) );


    switch ( m_unit )
    {

        case 8:
        rootPopMenu->Check( mgID_8K, true );
        break;

        case 16:
        rootPopMenu->Check( mgID_16K, true );
        break;

        case 32:
        rootPopMenu->Check( mgID_32K, true );
        break;

        case 64:
        rootPopMenu->Check( mgID_64K, true );
        break;

        case 128:
        rootPopMenu->Check( mgID_128K, true );
        break;

        case 256:
        rootPopMenu->Check( mgID_256K, true );
        break;

        case 512:
        rootPopMenu->Check( mgID_512K, true );
        break;

        case 1024:
        rootPopMenu->Check( mgID_1M, true );
        break;

        case 2048:
        rootPopMenu->Check( mgID_2M, true );
        break;

        case 4096:
        rootPopMenu->Check( mgID_4M, true );
        break;

        default:
        break;
    }

    if ( ( m_to - m_from ) / 8 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_8K, false );
    }

    if ( ( m_to - m_from ) / 16 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_16K, false );
    }

    if ( ( m_to - m_from ) / 32 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_32K, false );
    }

    if ( ( m_to - m_from ) / 64 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_64K, false );
    }

    if ( ( m_to - m_from ) / 128 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_128K, false );
    }

    if ( ( m_to - m_from ) / 256 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_256K, false );
    }

    if ( ( m_to - m_from ) / 512 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_512K, false );
    }

    if ( ( m_to - m_from ) / 1024 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_1M, false );
    }

    if ( ( m_to - m_from ) / 2048 / 1024 > 10000 )
    {
        rootPopMenu->Enable( mgID_2M, false );
    }

    PopupMenu( rootPopMenu );
    delete rootPopMenu;
}

void CShowTableWindow::SetRange( llong to, llong from, int nsize, bindex data[] )
{

    if ( to < 0 )
        m_to = 0;
    else
        m_to = to;

    if ( from < 0 )
        m_from = 0;
    else
        m_from = from;

    if ( from > to )
    {
        m_to = 0;
        m_from = 0;
    }

    //设置一个合适的m_unit,尽量把块数设置在1024－2048之间
    llong range = m_to - m_from;

    int blocks = range / m_unit / 1024;

    if ( blocks < 1024 && blocks != 0 )
    {
        while ( m_unit > 8 && blocks < 1024 )
        {
            blocks *= 2;
            m_unit /= 2;
        }
    }
    else if ( blocks > 2048 )
    {
        while ( m_unit < 4096 && blocks > 2048 )
        {
            blocks /= 2;
            m_unit *= 2;
        }
    }

    m_nBlock = range / m_unit / 1024;

    if ( ( range ) % ( m_unit * 1024 ) )
        m_nBlock += 1;

    m_DataList.clear();

    for ( int i = 0;i < nsize;i++ )
    {
        m_DataList.push_back( data[ i ] );
    }

    //直接发送刷新消息
    wxPaintEvent refresh;

    wxPostEvent( this, refresh );

}

void CShowTableWindow::SetUnit( int kbyte )
{
    if ( kbyte <= 0 )
        return ;

    m_unit = kbyte;

    m_nBlock = ( m_to - m_from ) / m_unit / 1024;

    if ( ( m_to - m_from ) % ( m_unit * 1024 ) )
        m_nBlock += 1;

    //	if( !IsShown() ) return;

    wxPaintEvent refresh;

    wxPostEvent( this, refresh );
}


void CShowTableWindow::OnDraw( wxDC& dc )
{

    if ( !IsShown() )
        return ;

    //获得当前窗口大小，计算每行的块数，每块算
    wxCoord w, h;

    dc.GetSize( &w, &h );

	if( w < 40 || h < 40 ) return; 

    //dc.Clear();
    int bmpWidth = w;

    //保证一行最少有两个块的绘图空间我们才绘制
    //if(w < _LEFT_EDGE + _RIGHT_EDGE + 2*_BLOCK_DIST) return;
    //第一个块的左上点
    wxPoint firstblock( _LEFT_EDGE, _TOP_EDGE );

    //计算一行可以绘几个块？ 如果是n个那么需要_BLOCK_DIST*n像素
    int cols = ( w - _LEFT_EDGE - _RIGHT_EDGE ) / _BLOCK_DIST;

    //有多少数据行
    int lines = m_nBlock / cols;

    if ( m_nBlock % cols )
        lines += 1;

    //绘制区域的高度？
    int bmpHeight = lines * _BLOCK_DIST + _TOP_EDGE;

    wxBitmap bmpBuf( bmpWidth, bmpHeight + h );

    wxMemoryDC MemDC;

    MemDC.SelectObject( bmpBuf );

    MemDC.SetBackground( *wxWHITE_BRUSH );

    MemDC.Clear();

    int i;

    for ( i = 0;i < lines - 1;i++ )
    {
        for ( int j = 0;j < cols;j++ )
        {

            DrawBlock( MemDC, firstblock, i, j, GetBlockColor( lines, cols, i + 1, j + 1 ) );
        }
    }

    //最后一行可能不全，单独画
    for ( i = 0;i < m_nBlock % cols;i++ )
        DrawBlock( MemDC, firstblock, lines - 1, i, GetBlockColor( lines, cols, lines, i + 1 ) );

    dc.Blit( 0, 0, bmpWidth, bmpHeight + h, &MemDC, 0, 0 );

    //设置滚动条
    SetVirtualSize( w - _LEFT_EDGE - _RIGHT_EDGE , lines * _BLOCK_DIST + _BOTTOM_EDGE );
}

//自然行，自然列
void CShowTableWindow::DrawBlock( wxDC& dc, wxPoint& base, int line , int col, int color )
{
    //现在这里没有考虑画完成块的数据，以后要根据完成情况选颜色画

    wxPoint lu = base;
    lu.x += col * _BLOCK_DIST;
    lu.y += line * _BLOCK_DIST;

    switch ( color )
    {

        case 1:
        dc.DrawBitmap( m_GreenBlock, lu.x, lu.y );
        break;

        case 2:
        dc.DrawBitmap( m_GrayBlock, lu.x, lu.y );
        break;

        case 3:
        //dc.DrawBitmap(m_YellowBlock,lu.x,lu.y);
        break;

        case 4:

        case 5:

        case 6:

        case 7:

        case 8:

        case 9:

        case 10:

        case 11:

        case 12:

        case 13:
        dc.DrawBitmap( m_YellowBlock[ color - 4 ], lu.x, lu.y );
        break;

        default:
        break;
    }

}

//设置数据后要处理数据的显示
void CShowTableWindow::SetData( int segs, bindex data[] )
{

    m_DataList.clear();

    for ( int i = 0;i < segs;i++ )
    {
        m_DataList.push_back( data[ i ] );
    }

    //这个似乎用处不大
    if ( m_bForceUpdata )
    {
        m_bForceUpdata = false;
        wxPaintEvent refresh;
        wxPostEvent( this, refresh );
        return ;
    }

    //通过时间控制刷新频率
#ifdef WIN32
    static LARGE_INTEGER last, thistime, freq;

    static int first = 1;

    if ( first )
    {
        QueryPerformanceFrequency( &freq );
        first = 0;
        QueryPerformanceCounter( &last );
    }

    QueryPerformanceCounter( &thistime );

    double t1 = ( double ) thistime.QuadPart / ( double ) freq.QuadPart;
    double t2 = ( double ) last.QuadPart / ( double ) freq.QuadPart;

#else

    static struct timeval last, thistime; //最后刷新时间

    static struct timezone tz;

    gettimeofday( &thistime, &tz );

    double t1 = ( double ) thistime.tv_sec + ( double ) thistime.tv_usec / ( 1000 * 1000 );

    double t2 = ( double ) last.tv_sec + ( double ) last.tv_usec / ( 1000 * 1000 );
#endif


    if ( t1 - t2 > 0.5 )   //间隔在0.5秒以上才刷新一次，否则可能太多刷新了。
    {
        last = thistime;
        //直接发送刷新消息
        wxPaintEvent refresh;
        wxPostEvent( this, refresh );
    }

}

//获得块的颜色索引,绿1代表完成，蓝2代表没完成,3前锋
//参数都是自然数
int CShowTableWindow::GetBlockColor( int totalline, int totalcol, int line, int col )
{
    if ( m_to == 0 )
    { //没有长度，应该不能断点续传
        return 1;
    }

    //本块的起点位置
    llong bytepos =
        ( ( llong ) totalcol * ( ( llong ) line - 1 ) + ( llong ) col - 1 ) * ( llong ) m_unit * 1024;

    //	llong bytepos=(tl*(li-1)+co-1)*((llong)m_unit)*1024; //溢出
    llong byteend = bytepos + m_unit * 1024; //本块的终点

    for ( int i = 0;i < int( m_DataList.size() );i++ )
    {

        if ( m_DataList[ i ].mark &&
                bytepos <= m_DataList[ i ].end &&
                byteend > m_DataList[ i ].end )
        {
            //前锋
            return m_DataList[ i ].mark + 3; //从4开始
        }
        else if ( bytepos >= m_DataList[ i ].start &&
                  byteend <= m_DataList[ i ].end )
        {
            //已完成
            return 1;
        }
        else if ( m_DataList[ i ].end < byteend && m_DataList[ i ].end > bytepos )
        {
            return 1; //半完成
        }
    }

    //没完成
    return 2;
}

