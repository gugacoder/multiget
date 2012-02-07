/***************************************************************************
*            proxydlg.cpp
*
*  Sat Sep 16 20:53:44 2006
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


#include "proxydlg.h"
#include "mainframe.h"
#include "addproxydlg.h"
#include "mgapp.h"
#include <wx/sizer.h>


DEFINE_EVENT_TYPE( mgID_DELETE_PROXY )
DEFINE_EVENT_TYPE( mgID_ADD_PROXY )

BEGIN_EVENT_TABLE( CProxyDlg, wxDialog )
EVT_BUTTON( mgID_ADD_PROXY, CProxyDlg::OnAddProxy )
EVT_BUTTON( mgID_DELETE_PROXY, CProxyDlg::OnDeleteProxy )
EVT_CUSTOM( wxEVT_SIZE, wxID_ANY, CProxyDlg::OnSize )
END_EVENT_TABLE()

CProxyDlg::CProxyDlg( MainFrame* parent )
        : wxDialog( parent, wxID_ANY, _("Proxy Administrator"), wxDefaultPosition, wxSize( 500, 300 ), wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE  )
{
    m_pParent = parent;
    proxylist = NULL;
    Init();
}

CProxyDlg::~CProxyDlg()
{}

void CProxyDlg::Init()
{

    if ( proxylist != NULL )
        delete proxylist;

    wxBoxSizer* all = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* lis = new wxBoxSizer( wxHORIZONTAL );

    proxylist =
        new wxListView( this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );

    //add head to list
    wxListItem itemCol;

    itemCol.SetText( _("Name") );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );

    proxylist->InsertColumn( 0, itemCol );

    proxylist->SetColumnWidth( 0, 100 );

    itemCol.SetText( _("Type") );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );

    proxylist->InsertColumn( 1, itemCol );

    proxylist->SetColumnWidth( 1, 100 );

    itemCol.SetText( _("Server") );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );

    proxylist->InsertColumn( 2, itemCol );

    proxylist->SetColumnWidth( 2, 150 );

    itemCol.SetText( _("Port") );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );

    proxylist->InsertColumn( 3, itemCol );

    proxylist->SetColumnWidth( 3, 50 );

    itemCol.SetText( _("User") );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );

    proxylist->InsertColumn( 4, itemCol );

    proxylist->SetColumnWidth( 4, 100 );

    itemCol.SetText( _("Password") );

    itemCol.SetAlign( wxLIST_FORMAT_LEFT );

    proxylist->InsertColumn( 5, itemCol );

    proxylist->SetColumnWidth( 5, 100 );

    //add data to list
    LoadData();

    lis->Add( proxylist,
              1,
              wxEXPAND );

    all->Add( lis,
              1,
              wxEXPAND | wxALL,
              5
            );

    wxBoxSizer* but = new wxBoxSizer( wxHORIZONTAL );


    but->Add( new wxButton( this, mgID_ADD_PROXY, _("Add Proxy") ), 0, wxALL, 5 );

    but->Add( new wxButton( this, mgID_DELETE_PROXY, _("Del Proxy") ), 0, wxALL, 5 );

    but->Add( new wxButton( this, wxID_OK, _( "OK" ) ), 0, wxALL, 5 );

    //	but->Add(new wxButton(this,wxID_CANCEL,_("Cancel")),0,wxALL,5);
    all->Add( but, 0, wxALIGN_RIGHT );

    SetSizer( all );
}

//增加一个信息行
int CProxyDlg::AddInfo( wxListCtrl *proxylist, wxString name, wxString type, wxString server,
                        wxString port, wxString user, wxString pass )
{
    wxListItem item;

    int id;
    id = proxylist->InsertItem( proxylist->GetItemCount(), item );
    proxylist->SetItem( id, 0, name );
    proxylist->SetItem( id, 1, type );
    proxylist->SetItem( id, 2, server );
    proxylist->SetItem( id, 3, port );
    proxylist->SetItem( id, 4, user );
    proxylist->SetItem( id, 5, pass );
    proxylist->SetItemBackgroundColour( id, id % 2 ? wxColour( 210, 210, 210 ) : wxColour( 230, 230, 230 ) );
    return id;
}

void CProxyDlg::OnAddProxy( wxCommandEvent& event )
{
    CAddProxy dlg( this );

    if ( wxID_OK == dlg.ShowModal() )
    {
        //get the args and if legal,add to list;

        if ( dlg.m_name.IsEmpty() || dlg.m_server.IsEmpty() || dlg.m_port <= 0 || dlg.m_port >= 65536 )
        {
            wxMessageBox( _( "args error." ) );
            return ;
        }
        else
        {
            if ( !m_pParent->AddProxy( dlg.m_type, dlg.m_name, dlg.m_server, dlg.m_port, dlg.m_user, dlg.m_pass ) )
            {
                wxMessageBox( _( "add proxy fail,maybe have same name." ) );
                return ;
            }
            else
            {
                LoadData();
            }
        }
    }
}

//seems ok
void CProxyDlg::OnDeleteProxy( wxCommandEvent& event )
{
    long pos = proxylist->GetFirstSelected();

    if ( pos == -1 )
    {
        wxMessageBox( _( "no select item" ) );
        return ;
    }

    std::vector<long> selist;
    selist.push_back( pos );

    while ( -1 != ( pos = proxylist->GetNextSelected( pos ) ) )
    {
        selist.push_back( pos );
    }

    wxString namelist;
    std::vector<wxString> namelist2;
    //get the select proxy name
    std::vector<long>::iterator it;

    for ( it = selist.begin();it != selist.end();it++ )
    {
        namelist += proxylist->GetItemText( *it );
        namelist2.push_back( proxylist->GetItemText( *it ) );
        namelist += _( "," );
    }

    namelist = _( "Delete below?\n" ) + namelist;
    wxMessageDialog dlg( this, namelist, _( "cap" ), wxOK | wxCANCEL );

    if ( wxID_OK == dlg.ShowModal() )
    {
        //先在父类删除，再刷新？
        std::vector<wxString>::iterator it2;

        for ( it2 = namelist2.begin();it2 != namelist2.end();it2++ )
            m_pParent->RemoveProxy( *it2 );

        LoadData();
    }

}


void CProxyDlg::LoadData()
{
    if ( proxylist == NULL )
        return ;

    proxylist->DeleteAllItems();

    int items = m_pParent->GetProxyCount();

    for ( int i = 0;i < items;i++ )
    {
        _ProxyAttr px = m_pParent->GetProxy( i );
        wxString x1, x2, x3, x4, x5, x6;
        //x1 = wxString( px.sName.c_str() );
			x1 = wxString( px.sName.c_str(), wxConvLocal );  //unicode patch
        switch ( px.nType )
        {

            case 0:
            x2 = _( "socks auto" );
            break;

            case 1:
            x2 = _( "socks v4" );
            break;

            case 2:
            x2 = _( "socks v4a" );
            break;

            case 3:
            x2 = _( "socks v5" );
            break;

            case 4:
            x2 = _( "ftp proxy" );
            break;

            case 5:
            x2 = _( "http proxy" );
            break;

            default:
            x2 = _( "unknow" );
            break;
        }

        //x3 = wxString( px.sServer.c_str() );
			x3 = wxString( px.sServer.c_str(), wxConvLocal ); //unicode patch
        x4.Printf( _( "%d" ), px.nPort );
        //x5 = wxString( px.sUser );
        //x6 = wxString( px.sPass );
			x5=wxString::FromAscii(px.sUser.c_str());//unicode patch
			x6=wxString::FromAscii(px.sPass.c_str());//unicode patch
        AddInfo( proxylist, x1, x2, x3, x4, x5, x6 );

    }
}

void CProxyDlg::OnSize( wxEvent& event )
{

    wxSize vs = GetClientSize();

    int c2 = proxylist->GetColumnWidth( 1 );
    int c3 = proxylist->GetColumnWidth( 2 );
    int c4 = proxylist->GetColumnWidth( 3 );
    int c5 = proxylist->GetColumnWidth( 4 );
    int c6 = proxylist->GetColumnWidth( 5 );

    if ( vs.x - c2 - c3 - c4 - c5 - c6 > 100 )
    {
        proxylist->SetColumnWidth( 0, vs.x - c2 - c3 - c4 - c5 - c6 - 1 );
    }
    else
    {
        proxylist->SetColumnWidth( 0, 100 );
    }

    event.Skip();
}

