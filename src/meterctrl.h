/***************************************************************************
*  meterctrl.h
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

#ifndef _METER_CTRL_H
#define _METER_CTRL_H

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/control.h>
#include <wx/toolbar.h>
#include <list>

DECLARE_EVENT_TYPE( wxID_TIMER2, -1 )
DECLARE_EVENT_TYPE( wxID_30K, -1 )
DECLARE_EVENT_TYPE( wxID_90K, -1 )
DECLARE_EVENT_TYPE( wxID_300K, -1 )
DECLARE_EVENT_TYPE( wxID_900K, -1 )
DECLARE_EVENT_TYPE( wxID_3M, -1 )
DECLARE_EVENT_TYPE( wxID_9M, -1 )

class CMeterCtrl : public wxWindow
{

public:
    CMeterCtrl( wxWindow* parent );
    void PutSpeedData( int speed );
    void SetTip( std::string tip );

protected:
    void OnPaint( wxPaintEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnTimer( wxTimerEvent& event );
    void On30K( wxCommandEvent& event );
    void On90K( wxCommandEvent& event );
    void On300K( wxCommandEvent& event );
    void On900K( wxCommandEvent& event );
    void On3M( wxCommandEvent& event );
    void On9M( wxCommandEvent& event );
    void OnMouseRightUp( wxMouseEvent& event );
    void OnMouseMove( wxMouseEvent& event );
    int m_nMaxSpeed;
    wxTimer m_timer;
    int	m_fade;
    wxString m_tip;
    std::list<int> m_SpeedList;

private:
    DECLARE_EVENT_TABLE()
};

#endif

