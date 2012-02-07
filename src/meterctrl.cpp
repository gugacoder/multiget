/***************************************************************************
*  lefttreeitemdata.cpp
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
#include "meterctrl.h"
#include "mgapp.h"

#include "common.h"
#include <iostream>

using namespace std;

DEFINE_EVENT_TYPE( wxID_TIMER2 )

DEFINE_EVENT_TYPE( wxID_30K )
DEFINE_EVENT_TYPE( wxID_90K )
DEFINE_EVENT_TYPE( wxID_300K )
DEFINE_EVENT_TYPE( wxID_900K )
DEFINE_EVENT_TYPE( wxID_3M )
DEFINE_EVENT_TYPE( wxID_9M )

BEGIN_EVENT_TABLE( CMeterCtrl, wxWindow )
EVT_PAINT( CMeterCtrl::OnPaint )
EVT_SIZE( CMeterCtrl::OnSize )
EVT_TIMER( wxID_TIMER2, CMeterCtrl::OnTimer )
EVT_RIGHT_UP( CMeterCtrl::OnMouseRightUp )
EVT_MOTION( CMeterCtrl::OnMouseMove )
EVT_MENU( wxID_30K, CMeterCtrl::On30K )
EVT_MENU( wxID_90K, CMeterCtrl::On90K )
EVT_MENU( wxID_300K, CMeterCtrl::On300K )
EVT_MENU( wxID_900K, CMeterCtrl::On900K )
EVT_MENU( wxID_3M, CMeterCtrl::On3M )
EVT_MENU( wxID_9M, CMeterCtrl::On9M )
END_EVENT_TABLE()

CMeterCtrl::CMeterCtrl( wxWindow* parent )
        : wxWindow( parent, wxID_ANY )
{
    SetBackgroundColour( wxColour( 0, 0, 0 ) );
    m_nMaxSpeed = 300; //300K为上界
    m_SpeedList.push_back( 0 );
    m_timer.SetOwner( this, wxID_TIMER2 );
    m_fade = 0;
    //m_tip = wxGetApp().GetWxStr( _S_100_TONGJI );
}

void CMeterCtrl::SetTip( string tip )
{
    m_tip = wxString( tip.c_str(), wxConvLocal );
    m_fade = 0;

    if ( m_tip.empty() && m_timer.IsRunning() )
    {
        m_timer.Stop();
    }

    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
}

void CMeterCtrl::PutSpeedData( int speed )
{

    if ( m_SpeedList.size() < 1000 )
    {
        m_SpeedList.push_front( speed );
    }
    else
    {
        m_SpeedList.pop_back();
        m_SpeedList.push_front( speed );
    }
#ifdef WIN32
	Refresh();
#else
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
#endif
}

void CMeterCtrl::OnSize( wxSizeEvent& event )
{
	event.Skip();
}

void CMeterCtrl::OnPaint( wxPaintEvent& event )
{

    wxCoord w, h;
    GetSize( &w, &h );

    wxBitmap bmpBuf( w, 30 );
    wxMemoryDC MemDC;
    MemDC.SelectObject( bmpBuf );
    MemDC.SetBackground( wxBrush( wxColour( 0, 0, 0 ) ) );
    MemDC.Clear();

    MemDC.SetPen( *( wxGREEN_PEN ) );

    list<int>::iterator it;

    int count = 0;
    int barheight;

    for ( it = m_SpeedList.begin(); it != m_SpeedList.end(); it++ )
    {
        barheight = ( *it ) * 28 / m_nMaxSpeed / 1024;

        if ( barheight > 28 )
            barheight = 28;

        MemDC.DrawLine( count * 2, 28, count * 2, 28 - barheight );

        MemDC.DrawLine( count * 2 + 1, 28, count * 2 + 1, 28 - barheight );

        count++;

    }


    MemDC.SetPen( *( wxGREY_PEN ) );

    MemDC.DrawLine( 0, 10, 2048, 10 );
    MemDC.DrawLine( 0, 20, 2048, 20 );


    if ( !m_tip.IsEmpty() )
    {
        int gray = 255 - m_fade * 10;
        MemDC.SetTextForeground( wxColour( gray, gray, gray ) );
        MemDC.SetTextBackground( wxColour( 0, 0, 0 ) );
        int tw, th, x = 0, y = 0;
        GetTextExtent( m_tip, &tw, &th );

        if ( tw < w )
            x = ( w - tw ) / 2;
        else
            x = 0;

        if ( th < h )
            y = ( h - th ) / 2;
        else
            y = 0;

        MemDC.DrawText( m_tip, x, y );
    }


    wxPaintDC dc( this );
    dc.Blit( 0, 0, w, h, &MemDC, 0, 0 );
}


void CMeterCtrl::OnTimer( wxTimerEvent& event )
{
    m_fade++;

    if ( m_fade > 25 )
    {
        m_timer.Stop();
        m_tip.clear();
        m_fade = 0;
    }
#ifdef WIN32
	Refresh();
#else
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
#endif
}

void CMeterCtrl::On30K( wxCommandEvent& event )
{
    m_nMaxSpeed = 30;
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
}

void CMeterCtrl::On90K( wxCommandEvent& event )
{
    m_nMaxSpeed = 90;
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
}

void CMeterCtrl::On300K( wxCommandEvent& event )
{
    m_nMaxSpeed = 300;
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
}

void CMeterCtrl::On900K( wxCommandEvent& event )
{
    m_nMaxSpeed = 900;
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );

}

void CMeterCtrl::On3M( wxCommandEvent& event )
{
    m_nMaxSpeed = 3 * 1024;
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
}

void CMeterCtrl::On9M( wxCommandEvent& event )
{
    m_nMaxSpeed = 9 * 1024;
    wxPaintEvent refresh;
    wxPostEvent( this, refresh );
}

void CMeterCtrl::OnMouseMove( wxMouseEvent& event )
{
    if ( m_tip.empty() )
        return ;
    else if ( !m_timer.IsRunning() )
        m_timer.Start( 100 );
}

//pop menu
void CMeterCtrl::OnMouseRightUp( wxMouseEvent& event )
{
    wxPoint pos = GetPosition();
    //menu begin
    wxMenu *rootPopMenu = new wxMenu( _("TOP SPEED") );

    wxMenuItem* _30K = rootPopMenu->AppendCheckItem( wxID_30K, _( "30K/S" ) );
    wxMenuItem* _90K = rootPopMenu->AppendCheckItem( wxID_90K, _( "90K/S" ) );
    wxMenuItem* _300K = rootPopMenu->AppendCheckItem( wxID_300K, _( "300K/S" ) );
    wxMenuItem* _900K = rootPopMenu->AppendCheckItem( wxID_900K, _( "900/S" ) );
    wxMenuItem* _3M = rootPopMenu->AppendCheckItem( wxID_3M, _( "3M/S" ) );
    wxMenuItem* _9M = rootPopMenu->AppendCheckItem( wxID_9M, _( "9M/S" ) );

    switch ( m_nMaxSpeed )
    {

        case 30:
        _30K->Check();
        break;

        case 90:
        _90K->Check();
        break;

        case 300:
        _300K->Check();
        break;

        case 900:
        _900K->Check();
        break;

        case 3*1024:
        _3M->Check();
        break;

        case 9*1024:
        _9M->Check();
        break;

        default:
        break;
    }

    PopupMenu( rootPopMenu );
    delete rootPopMenu;
}

