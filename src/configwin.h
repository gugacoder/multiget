/***************************************************************************
*  configwin.h
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
#ifndef _CONFIG_WIN_H
#define _CONFIG_WIN_H
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include "common.h"

class MainFrame;

DECLARE_EVENT_TYPE( cfID_PICKPATH, -1 )
DECLARE_EVENT_TYPE( cfID_SPINTHREAD, -1 )
DECLARE_EVENT_TYPE( cfID_SPINRETRY, -1 )
DECLARE_EVENT_TYPE( cfID_SPINMAXTASK, -1 )

DECLARE_EVENT_TYPE( cfID_SPINCONNECT, -1 )
DECLARE_EVENT_TYPE( cfID_SPINRW, -1 )
DECLARE_EVENT_TYPE( cfID_SPINRETRY, -1 )
DECLARE_EVENT_TYPE( cfID_SPINWAIT, -1 )

class CConfigWindow : public wxDialog
{

public:
    CConfigWindow( MainFrame* parent );
    virtual ~CConfigWindow();

    int m_nTask;
    int m_nThread;
    int m_nRetry;
    int m_nConnectTimeOut;
    int m_nTimeOut;
    int m_nRetryWait;
    bool m_bIgnoreExt; //ignore extention
    bool m_bMonitorExt;
    bool m_bSound;
    bool m_bAutoMd5;
    wxString m_sSavePath, m_sPass, m_sMonitorExt, m_sIgnoreExt;
    int m_SpeedMode;

protected:
    void OnPickPath( wxCommandEvent& event );
    void OnSpinThread( wxSpinEvent& event );
    void OnSpinRetry( wxSpinEvent& event );
    void OnSpinTask( wxSpinEvent& event );

    void OnSpinConnect( wxSpinEvent& event );
    void OnSpinReadWrite( wxSpinEvent& event );
    void OnSpinWait( wxSpinEvent& event );

    void Init();

private:
    wxSpinCtrl	*m_SpinTask, *m_SpinThread, *m_SpinRetry, *m_SpinConnectTimeOut;
    wxSpinCtrl *m_SpinRetryWait, *m_SpinTimeOut;
    DECLARE_EVENT_TABLE()
};

#endif

