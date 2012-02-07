/***************************************************************************
*            proxydlg.h
*
*  Sat Sep 16 20:55:00 2006
*  Copyright  2006  liubin,china
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

#ifndef _PROXY_DLG_H
#define _PROXY_DLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h> //mac need it

class MainFrame;

DECLARE_EVENT_TYPE( mgID_DELETE_PROXY, -1 );
DECLARE_EVENT_TYPE( mgID_ADD_PROXY, -1 );

class CProxyDlg : public wxDialog
{

public:
    CProxyDlg( MainFrame* parent );
    virtual ~CProxyDlg();

protected:
    wxListView *proxylist;
    void Init();
    void OnAddProxy( wxCommandEvent& event );
    void OnDeleteProxy( wxCommandEvent & event );
    int AddInfo( wxListCtrl *proxylist, wxString name, wxString type, wxString server,
                 wxString port, wxString user, wxString pass );
    void LoadData();
    void OnSize( wxEvent& event );
    MainFrame* m_pParent;

    DECLARE_EVENT_TABLE()
};

#endif

