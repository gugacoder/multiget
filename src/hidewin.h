/***************************************************************************
*  hidewin.h
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

#ifndef _HIDEWIN_H
#define _HIDEWIN_H

#include <wx/wx.h>


DECLARE_EVENT_TYPE( mgCLIP_TIMER, -1 )

class MainFrame;

class CHideWin : public wxFrame
{

public:
    CHideWin( MainFrame *parent );
    wxTimer m_Timer;
    wxString m_LastClip;
    void MonitorClip( bool mo )
    {
        m_LastClip.Clear();
        m_bMonitor = mo;
    }

    bool IgnoreExtention( wxString url );
    bool MonitorExtention( wxString url );

protected:
    bool m_bMonitor;
    void OnTimer( wxTimerEvent& event );
    bool HaveExt( wxString str, wxString ext );
    MainFrame *m_pParent;

    DECLARE_EVENT_TABLE()
};

#endif

