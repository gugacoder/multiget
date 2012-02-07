
/***************************************************************************
 *  systray.h
 *
 *  2006/08/21
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

#ifndef _SYS_TRAY_H
#define _SYS_TRAY_H

#include <wx/wx.h>
#include <wx/taskbar.h>

class MainFrame;

DECLARE_EVENT_TYPE( PU_RESTORE, -1 )
DECLARE_EVENT_TYPE( PU_SHOWDROPWIN, -1 )
DECLARE_EVENT_TYPE( PU_MONITORCLIP, -1 )
DECLARE_EVENT_TYPE( PU_STARTALL, -1 )
DECLARE_EVENT_TYPE( PU_STOPALL, -1 )
DECLARE_EVENT_TYPE( PU_NEWTASK, -1 )
DECLARE_EVENT_TYPE( PU_SPEEDMODE, -1 )
DECLARE_EVENT_TYPE( PU_OPTION, -1 )
DECLARE_EVENT_TYPE( PU_ABOUT, -1 )

DECLARE_EVENT_TYPE( PU_EXIT, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_UNLIMIT, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT5M , -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT2M , -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT1M, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT700K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT500K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT400K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT300K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT200K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT100K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT50K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT20K, -1 )
DECLARE_EVENT_TYPE( PU_SPEED_LIMIT10K, -1 )

class CSysTray : public wxTaskBarIcon
{

public:

    CSysTray( MainFrame* pmain );
    void OnLeftButtonClick( wxTaskBarIconEvent& );
    void OnRightButtonClick( wxTaskBarIconEvent& );
    void OnMenuRestore( wxCommandEvent& );
    void OnMenuExit( wxCommandEvent& );
    virtual wxMenu *CreatePopupMenu();
    void DlgShow( bool show );

protected:
    void OnShowDropWin( wxCommandEvent& );
    void OnMonitorClip( wxCommandEvent& );
    void OnStartAll( wxCommandEvent& );
    void OnStopAll( wxCommandEvent& );
    void OnAbout( wxCommandEvent& );
    void OnNewTask( wxCommandEvent& event );
    void OnOption( wxCommandEvent& event );

    void OnUnLimit( wxCommandEvent& event );
    void OnLimit5M( wxCommandEvent& event );
    void OnLimit2M( wxCommandEvent& event );
    void OnLimit1M( wxCommandEvent& event );
    void OnLimit700K( wxCommandEvent& event );
    void OnLimit500K( wxCommandEvent& event );
    void OnLimit400K( wxCommandEvent& event );
    void OnLimit300K( wxCommandEvent& event );
    void OnLimit200K( wxCommandEvent& event );
    void OnLimit100K( wxCommandEvent& event );
    void OnLimit50K( wxCommandEvent& event );
    void OnLimit20K( wxCommandEvent& event );
    void OnLimit10K( wxCommandEvent& event );

    bool m_bDlgOnMain;

    MainFrame *m_pMainwin;

private:
    DECLARE_EVENT_TABLE()
};

#endif

