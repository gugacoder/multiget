/***************************************************************************
*  floatwin.cpp
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
#include "floatwin.h"
#include "mainframe.h" 
#include "./newicons/32/float.xpm"

#include "dropurl.h"

#include <iostream>

using namespace std;

#ifdef WIN32
BEGIN_EVENT_TABLE( CFloatWin, wxMiniFrame )
#else
BEGIN_EVENT_TABLE( CFloatWin, wxDialog )
#endif
	EVT_MOTION( CFloatWin::OnMouseMove )
	EVT_LEFT_DOWN( CFloatWin::OnLeftDown )
	EVT_LEFT_UP( CFloatWin::OnLeftUp )
	EVT_PAINT( CFloatWin::OnPaint )
END_EVENT_TABLE()

extern int gDropWinX, gDropWinY;

CFloatWin::CFloatWin( MainFrame* parent )
#ifdef WIN32
: wxMiniFrame( NULL, -1, _( "drop" ), wxPoint( gDropWinX, gDropWinY ), wxSize( 32, 32 ),
           wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR | wxNO_BORDER  )
#else
: wxDialog( NULL, -1, _( "drop" ), wxPoint( gDropWinX, gDropWinY ), wxSize( 32, 32 ),
           wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR | wxDIALOG_NO_PARENT | ~wxSYSTEM_MENU | ~wxCAPTION )
#endif
{
    m_pParent = parent;
    m_DropUrl = new CDropUrl( this );
    SetDropTarget( m_DropUrl );

}

CFloatWin::~CFloatWin()
{
}

void CFloatWin::OnDropURL( wxString url )
{

    wxCommandEvent event( mgEVT_URL_NOTICE, 1 );
    event.SetString( url );
    m_pParent->AddPendingEvent( event );

}

void CFloatWin::OnMouseMove( wxMouseEvent& evt )
{
    wxPoint pt = evt.GetPosition();

    if ( evt.Dragging() && evt.LeftIsDown() )
    {
        wxPoint pos = ClientToScreen( pt );
        Move( wxPoint( pos.x - m_delta.x, pos.y - m_delta.y ) );
        gDropWinX = pos.x - m_delta.x;
        gDropWinY = pos.y - m_delta.y;
    }
}

void CFloatWin::OnLeftDown( wxMouseEvent& evt )
{
    CaptureMouse();
    wxPoint pos = ClientToScreen( evt.GetPosition() );
    wxPoint origin = GetPosition();
    int dx = pos.x - origin.x;
    int dy = pos.y - origin.y;
    m_delta = wxPoint( dx, dy );
}

void CFloatWin::OnLeftUp( wxMouseEvent& WXUNUSED( evt ) )
{
    if ( HasCapture() )
    {
        ReleaseMouse();
    }
}

void CFloatWin::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc( this );
    dc.DrawIcon( wxIcon( float_xpm ), 0, 0 );
}

