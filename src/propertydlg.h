/***************************************************************************
*            propertydlg.h
*
*  Fri Oct 13 22:38:08 2006
*  Copyright  2006  liubin, China
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


#ifndef _PROPERTY_DLG_H
#define _PROPERTY_DLG_H

#include "common.h"
#include <iostream>
#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include <wx/clipbrd.h>

class MainFrame;

DECLARE_EVENT_TYPE( prID_PICKPATH, -1 )
DECLARE_EVENT_TYPE( prID_SPINTHREAD, -1 )
DECLARE_EVENT_TYPE( prID_SPINRETRY, -1 )
DECLARE_EVENT_TYPE( prID_SPINRETRYWAIT, -1 )
//DECLARE_EVENT_TYPE( prID_URLTEXT, -1 )
//DECLARE_EVENT_TYPE( prEVT_URL_CHANGE,-1)

class CPropertyDlg : public wxDialog
{

public:
    CPropertyDlg( MainFrame* parent, int taskid );
    virtual ~CPropertyDlg();
    void GetProxyName( std::string& socks, std::string& ftp, std::string& http );
    std::string GetUrl();
    void AddMirrorUrl( wxString url );
    int GetMirrorUrl( int num, std::string mu[] );
    std::string GetSelectFileName();
    std::string GetSocksProxyName();
    std::string GetHttpProxyName();
    std::string GetFtpProxyName();
    //data
    int	m_nRunNow; //2
    int	m_nThreadNum; //2
    int	m_nRetry;
    int	m_nRetryWait;
    int m_nSocksProxy; //3
    int m_nFtpProxy; //4
    int m_nHttpProxy; //5

    wxString m_sUrl;  //1
    wxArrayString m_sMirrorUrl;
    wxString m_sSavePath; //1
    wxString m_sRename;  //1

    wxString m_sUser; //6
    wxString m_sPass; //6
    wxString m_sRefer;
    //proxy list

    struct _PL
    {
        std::string name; //proxy name
        wxString showname; //show name
    };

    std::vector<_PL> m_SocksProxyList;
    std::vector<_PL> m_FtpProxyList;
    std::vector<_PL> m_HttpProxyList;

protected:
    void Init( _TASK_TYPE ttype );
    void OnPickPath( wxCommandEvent& event );
    void OnThSpin( wxSpinEvent& event );
    void OnReSpin( wxSpinEvent& event );
    void OnWaSpin( wxSpinEvent& event );
    void OnFileSpin( wxSpinEvent& event );
    void OnUrlText( wxCommandEvent& event );
    bool SelectFile( int step );
    void InitProxyList( std::string socks, std::string ftp, std::string http );
    MainFrame* m_pParent;
    wxSpinCtrl	*m_ThSpin, *m_ThRetry, *m_ThRetryWait;
    wxListBox	*m_pMirrorBox;
    wxTextCtrl	*m_pTextCtrl;
    wxSpinButton *m_FileSpin;
    int m_nFileSpin;
    bool m_bOpenUrlKeyEvent;
    DECLARE_EVENT_TABLE()
};

#endif

