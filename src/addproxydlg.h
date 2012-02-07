/***************************************************************************
*            addproxydlg.h
*
*  Wed Oct 25 21:48:58 2006
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
#ifndef _ADDPROXY_DLG_H
#define _ADDPROXY_DLG_H

class CProxyDlg;

#include <wx/wx.h>
#include <wx/dialog.h>

class CAddProxy : public wxDialog
{

public:
    CAddProxy( CProxyDlg* parent );
    virtual ~CAddProxy();
    void Init();

    int m_type;
    wxString m_name;
    wxString m_server;
    int m_port;
    wxString m_user;
    wxString m_pass;

protected:
    CProxyDlg* m_pParent;

    //   DECLARE_EVENT_TABLE()
};


#endif

