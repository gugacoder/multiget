/***************************************************************************
*            propertydlg.cpp
*
*  Fri Oct 13 22:37:45 2006
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


#include "propertydlg.h"
#include "mainframe.h"
#include "mgapp.h"
#include "mgurlparser.h"
#include "common.h"
#include <wx/valgen.h>

DEFINE_EVENT_TYPE( prID_PICKPATH )
DEFINE_EVENT_TYPE( prID_SPINTHREAD )
DEFINE_EVENT_TYPE( prID_SPINRETRY )
DEFINE_EVENT_TYPE( prID_SPINRETRYWAIT )
DEFINE_EVENT_TYPE( prID_SPINFILE )
//DEFINE_EVENT_TYPE( prID_URLTEXT)
//DEFINE_EVENT_TYPE( prEVT_URL_CHANGE)
BEGIN_EVENT_TABLE( CPropertyDlg, wxDialog )
EVT_BUTTON( prID_PICKPATH, CPropertyDlg::OnPickPath )
EVT_SPINCTRL( prID_SPINTHREAD, CPropertyDlg::OnThSpin )
EVT_SPINCTRL( prID_SPINRETRY, CPropertyDlg::OnReSpin )
EVT_SPINCTRL( prID_SPINRETRYWAIT, CPropertyDlg::OnWaSpin )
EVT_SPIN( prID_SPINFILE, CPropertyDlg::OnFileSpin )
//EVT_TEXT( prID_URLTEXT, CPropertyDlg::OnUrlText )
END_EVENT_TABLE()

extern std::list<std::string> gSavePathHistory; //保存目录
extern int gTaskDefThread, gRetryTime, gRetryWait;
extern std::string gDefFtpPass;

CPropertyDlg::CPropertyDlg( MainFrame* parent, int taskid )
        : wxDialog( parent, -1, _("Task properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE| wxRESIZE_BORDER )
{
    m_pParent = parent;

    _TaskAttr *tsk = m_pParent->GetTask( taskid );

    if ( tsk == NULL )
        return ;


    //m_sUrl = wxString( tsk->sURL.c_str() );
	m_sUrl=wxString( tsk->sURL.c_str(), wxConvLocal );//unicode patch

    //m_sRefer = wxString( tsk->sRefer.c_str() );
	m_sRefer=wxString( tsk->sRefer.c_str(), wxConvLocal ); //unicode patch

    //对这个url分离用户名和密码
    if ( !m_sUrl.IsEmpty() )
    {
        CUrlParser par;
        std::string m_swap;
#ifndef _UNICODE
        if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ) ) )
#else
			if( par.SetUrl( m_sUrl.mb_str(wxConvLocal).data() ) ) //unicode patch
#endif
        {
            if ( par.GetUrlType() == FTP_PROTOCOL )
            {
                m_sUser = wxString( par.GetUser().c_str(), wxConvLocal );
                m_sPass = wxString( par.GetPass().c_str(), wxConvLocal );
                m_sUrl = wxString( par.GetRawUrl().c_str(), wxConvLocal );
            }
            else
            {
                m_sUrl = wxString( par.GetRawUrl().c_str(), wxConvLocal );
            }

            if ( par.GetFileName().empty() )
            {
                m_sRename = _( "index.html" );
            }
            else
            {
                //m_sRename = par.GetFileName();
					m_sRename=wxString(par.GetFileName().c_str(),wxConvLocal); //unicode patch
            }
        }
    }

    //m_sSavePath = wxString( tsk->sSavePath.c_str() );
	m_sSavePath=wxString( tsk->sSavePath.c_str(), wxConvLocal ); //unicode patch
    m_nThreadNum = tsk->nThread;
    m_nRetry = tsk->nRetry;
    m_nRetryWait = tsk->nRetryWait;

    m_nSocksProxy = 0;
    m_nFtpProxy = 0;
    m_nHttpProxy = 0;
    m_nRunNow = 0;
    m_nFileSpin = 0;

    m_pMirrorBox = NULL;
    m_ThSpin = NULL;
    m_ThRetry = NULL;
    m_ThRetryWait = NULL;
    m_FileSpin = NULL;
    m_pTextCtrl = NULL;
    m_bOpenUrlKeyEvent = false;
    InitProxyList( tsk->sSockProxy, tsk->sFtpProxy, tsk->sHttpProxy );

    Init( tsk->nStatus );

    //add mirror string

    for ( int i = 0;i < int( tsk->sMirrorURL.size() );i++ )
    {
        AddMirrorUrl( wxString( tsk->sMirrorURL[ i ].c_str(), wxConvLocal ) );
    }
}

CPropertyDlg::~CPropertyDlg()
{
}

void CPropertyDlg::Init( _TASK_TYPE ttype )
{

    //for border
    wxBoxSizer * top = new wxBoxSizer( wxVERTICAL );
    //old top
    wxBoxSizer *all = new wxBoxSizer( wxVERTICAL );
    //part1 url/mirror/savepath/rename
    wxBoxSizer *part1 = new wxStaticBoxSizer( wxVERTICAL, this, _("Basic Info") );
    //part2 run now/thread
    wxBoxSizer *part2 = new wxBoxSizer( wxHORIZONTAL );
    //part3 proxy
    wxBoxSizer *part3 = new wxBoxSizer( wxVERTICAL );
    //part4 user/pass
    wxBoxSizer *part4 = new wxBoxSizer( wxVERTICAL );
    //part5 button
    wxBoxSizer *part5 = new wxBoxSizer( wxVERTICAL );
    //part6 refer
    wxBoxSizer *part6 = new wxBoxSizer( wxVERTICAL );

    //part1
    wxBoxSizer *url = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *mirrorurl = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *savepath = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *rename = new wxBoxSizer( wxHORIZONTAL );

    url->Add( new wxStaticText(
                  this,
                  -1,
                  _("MAIN URL:"),
                  wxDefaultPosition,
                  wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );

    m_pTextCtrl = new wxTextCtrl(
                      this,
                      -1,
                      _( "" ),
                      wxDefaultPosition,
                      wxSize( 400, 25 ),
                      0,
                      wxGenericValidator( &m_sUrl ) );

    if ( ttype == _STATUS_RUNNING )
        m_pTextCtrl->Enable( false );

    url->Add( m_pTextCtrl, 1, wxEXPAND );

    m_FileSpin = new wxSpinButton(
                     this,
                     prID_SPINFILE,
                     wxDefaultPosition,
                     wxSize( 25, 25 ),
                     wxSP_VERTICAL ); //wxSP_HORIZONTAL not support by GTK!

    m_FileSpin->SetRange( 1, 5 );

    m_FileSpin->SetValue( 1 );

    //if(ttype==_STATUS_RUNNING) m_FileSpin->Enable(false);

    url->Add( m_FileSpin, 0, wxEXPAND );

    mirrorurl->Add( new wxStaticText(
                        this,
                        -1,
                        _("Mirrors:"),
                        wxDefaultPosition,
                        wxSize( 70, 25 ) ), 0, wxALIGN_LEFT | wxALIGN_TOP );



    m_pMirrorBox = new wxListBox(
                       this,
                       -1,
                       wxDefaultPosition,
                       wxSize( 400, 75 ),
                       m_sMirrorUrl,
                       wxLB_SINGLE | wxLB_NEEDED_SB  );

    mirrorurl->Add( m_pMirrorBox, 1, wxEXPAND );


    savepath->Add( new wxStaticText(
                       this,
                       -1,
                       _("Save To:"),
                       wxDefaultPosition,
                       wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );


    //DBGOUT("history path size="<<gSavePathHistory.size());
    wxArrayString pchoice;

    std::list<std::string>::const_iterator it;

    for ( it = gSavePathHistory.begin();it != gSavePathHistory.end();it++ )
    {
        if ( it->empty() )
        {
            continue;
        }
        else
        {
            wxString temp( it->c_str(), wxConvLocal );
            pchoice.Add( temp );
        }
    }

    savepath->Add(
        new wxComboBox(
            this,
            -1,
            _( "" ),
            wxDefaultPosition,
            wxSize( 200, 25 ),
            pchoice,
            1,
            wxGenericValidator( &m_sSavePath )
        ), 1, wxEXPAND );

    savepath->Add( new wxButton( this, prID_PICKPATH, _( "..." ), wxDefaultPosition, wxSize( 35, 25 ) ), 0, wxALIGN_RIGHT );
    rename->Add( new wxStaticText( this, -1, _("Rename:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );
    rename->Add( new wxTextCtrl(
                     this,
                     -1,
                     _( "" ),
                     wxDefaultPosition,
                     wxSize( 200, 25 ),
                     1,
                     wxGenericValidator( &m_sRename ) ), 1, wxEXPAND );

    //add part1 to global
    part1->Add( url, 0, wxEXPAND );
    part1->AddSpacer( 3 );
    part1->Add( mirrorurl, 1, wxEXPAND );
    part1->AddSpacer( 3 );
    part1->Add( savepath, 0, wxEXPAND );
    part1->AddSpacer( 3 );
    part1->Add( rename, 0, wxEXPAND );

    //part2

    wxBoxSizer *threads = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Threads") );
    m_ThSpin = new wxSpinCtrl( this, prID_SPINTHREAD, _( "" ), wxDefaultPosition, wxSize( 20, 25 ) ); //
    m_ThSpin->SetRange( 1, 10 );
    m_ThSpin->SetValue( m_nThreadNum );

    if ( ttype == _STATUS_RUNNING )
        m_ThSpin->Enable( false );

    threads->AddSpacer( 10 );

    threads->Add( m_ThSpin, 1, wxEXPAND );

    threads->AddSpacer( 10 );

    part2->Add( threads, 1, wxEXPAND );

    part2->AddSpacer( 10 );

    wxBoxSizer *retry = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Retry") );

    m_ThRetry = new wxSpinCtrl( this, prID_SPINRETRY, _( "" ), wxDefaultPosition, wxSize( 20, 25 ) ); //

    m_ThRetry->SetRange( 0, 999 );

    m_ThRetry->SetValue( m_nRetry );

    if ( ttype == _STATUS_RUNNING )
        m_ThRetry->Enable( false );

    retry->AddSpacer( 10 );

    retry->Add( m_ThRetry, 1, wxEXPAND );

    retry->AddSpacer( 10 );

    part2->Add( retry, 1, wxEXPAND );

    part2->AddSpacer( 10 );

    wxBoxSizer *retrywait = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Retry Wait [s]") );

    m_ThRetryWait = new wxSpinCtrl( this, prID_SPINRETRYWAIT, _( "" ), wxDefaultPosition, wxSize( 20, 25 ) ); //

    m_ThRetryWait->SetRange( 5, 3000 );

    m_ThRetryWait->SetValue( m_nRetryWait );

    if ( ttype == _STATUS_RUNNING )
        m_ThRetryWait->Enable( false );

    retrywait->AddSpacer( 10 );

    retrywait->Add( m_ThRetryWait, 1, wxEXPAND );

    retrywait->AddSpacer( 10 );

    part2->Add( retrywait, 1, wxEXPAND );


    //part3 proxy

    wxStaticBoxSizer* proxy = new wxStaticBoxSizer( wxVERTICAL, this, _("Proxys[option]") );

    wxBoxSizer* socks = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* ftp = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* http = new wxBoxSizer( wxHORIZONTAL );

    socks->Add( new wxStaticText( this, -1, _( "SOCKS:" ), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );

    wxArrayString socksarray; //socks list

    //copy string
    for ( unsigned int i = 0;i < m_SocksProxyList.size();i++ )
    {
        socksarray.Add( m_SocksProxyList[ i ].showname );
    }

    wxChoice *tmp3 = new wxChoice(
                         this,
                         -1,
                         wxDefaultPosition,
                         wxSize( 210, 28 ),
                         socksarray,
                         0,
                         wxGenericValidator( &m_nSocksProxy ) );
    socks->Add( tmp3, 1, wxEXPAND );

    if ( ttype == _STATUS_RUNNING || ttype == _STATUS_WAITING )
        tmp3->Enable( false );

    proxy->Add( socks, 1, wxEXPAND );

    ftp->Add( new wxStaticText( this, -1, _( "FTP:" ), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );

    wxArrayString ftparray; //socks list

    //copy string
    for ( unsigned int i = 0;i < m_FtpProxyList.size();i++ )
    {
        ftparray.Add( m_FtpProxyList[ i ].showname );
    }

    wxChoice *tmp4 = new wxChoice(
                         this,
                         -1,
                         wxDefaultPosition,
                         wxSize( 210, 28 ),
                         ftparray,
                         0,
                         wxGenericValidator( &m_nFtpProxy ) );

    ftp->Add( tmp4, 1, wxEXPAND );

    if ( ttype == _STATUS_RUNNING || ttype == _STATUS_WAITING )
        tmp4->Enable( false );

    proxy->Add( ftp, 1, wxEXPAND );

    http->Add( new wxStaticText( this, -1, _( "HTTP:" ), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );

    wxArrayString httparray; //socks list

    //copy string
    for ( unsigned int i = 0;i < m_HttpProxyList.size();i++ )
    {
        httparray.Add( m_HttpProxyList[ i ].showname );
    }

    wxChoice *tmp5 = new wxChoice(
                         this,
                         -1,
                         wxDefaultPosition,
                         wxSize( 210, 28 ),
                         httparray,
                         0,
                         wxGenericValidator( &m_nHttpProxy ) );

    http->Add( tmp5, 1, wxEXPAND );

    if ( ttype == _STATUS_RUNNING || ttype == _STATUS_WAITING )
        tmp5->Enable( false );

    proxy->Add( http, 1, wxEXPAND );

    part3->Add( proxy, 1, wxEXPAND );

    //part4

    wxStaticBoxSizer* userpass = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Login[option]") );

    userpass->Add( new wxStaticText(
                       this,
                       -1,
                       _("User:"),
                       wxDefaultPosition,
                       wxSize( 70, 25 ) ), 0, wxALIGN_RIGHT );

    wxTextCtrl *tmp1 = new wxTextCtrl(
                           this,
                           -1,
                           _( "" ),
                           wxDefaultPosition,
                           wxSize( 60, 25 ),
                           0,
                           wxGenericValidator( &m_sUser ) );

    userpass->Add( tmp1, 1, wxEXPAND );

    if ( ttype == _STATUS_RUNNING )
        tmp1->Enable( false );

    userpass->AddSpacer( 10 );

    userpass->Add( new wxStaticText(
                       this,
                       -1,
                       _("Password:"),
                       wxDefaultPosition,
                       wxSize( 70, 25 ) ), 0, wxALIGN_RIGHT );

    wxTextCtrl *tmp2 = new wxTextCtrl(
                           this,
                           -1,
                           _( "" ),
                           wxDefaultPosition,
                           wxSize( 80, 25 ),
                           0,
                           wxGenericValidator( &m_sPass ) );

    userpass->Add( tmp2, 1, wxEXPAND );

    if ( ttype == _STATUS_RUNNING )
        tmp2->Enable( false );

    part4->Add( userpass, 1, wxEXPAND );

    //part5

    wxBoxSizer* but = new wxBoxSizer( wxHORIZONTAL );

    but->Add( new wxButton( this, wxID_CANCEL, _( "Cancel" ) ), 0, wxALL, 5 );

    but->Add( new wxButton( this, wxID_OK, _( "OK" ) ), 0, wxALL, 5 );

    part5->Add( but, 0, wxALIGN_RIGHT );

    //part6
    wxStaticBoxSizer* refer = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Reference[option]") );

    refer->Add( new wxStaticText( this, -1, _("Refer:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_RIGHT );

    wxTextCtrl *tmp6 = new wxTextCtrl(
                           this,
                           -1,
                           _( "" ),
                           wxDefaultPosition,
                           wxSize( 80, 25 ),
                           0,
                           wxGenericValidator( &m_sRefer ) );

    refer->Add( tmp6, 1, wxEXPAND );

    if ( ttype == _STATUS_RUNNING || ttype == _STATUS_WAITING )
        tmp6->Enable( false );

    part6->Add( refer, 1, wxEXPAND );

    all->Add( part1, 1, wxEXPAND );

    all->AddSpacer( 10 );

    all->Add( part2, 0, wxEXPAND );

    all->AddSpacer( 10 );

    all->Add( part3, 0, wxEXPAND );

    all->AddSpacer( 10 );

    all->Add( part4, 0, wxEXPAND );

    all->AddSpacer( 10 );

    all->Add( part6, 0, wxEXPAND );

    all->AddSpacer( 10 );

    all->Add( part5, 0, wxALIGN_RIGHT );

    top->Add( all, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 5 );

    top->SetMinSize( 420, 300 );

    TransferDataToWindow();

    top->Fit( this );

    SetSizer( top );

    top->SetSizeHints( this );

    m_bOpenUrlKeyEvent = true;

    //select file if posible
    //SelectFile(1);
}

void CPropertyDlg::OnPickPath( wxCommandEvent& event )
{
    wxDirDialog dlg( this, _("Choose a directory to save file"), m_sSavePath, wxDD_NEW_DIR_BUTTON );

    if ( dlg.ShowModal() == wxID_OK )
    {
        TransferDataFromWindow();
        m_sSavePath = dlg.GetPath();
        TransferDataToWindow();
    }
}

void CPropertyDlg::OnThSpin( wxSpinEvent& event )
{
    m_nThreadNum = m_ThSpin->GetValue();
}

void CPropertyDlg::OnReSpin( wxSpinEvent& event )
{
    m_nRetry = m_ThRetry->GetValue();
}

void CPropertyDlg::OnWaSpin( wxSpinEvent& event )
{
    m_nRetryWait = m_ThRetryWait->GetValue();
}

void CPropertyDlg::OnFileSpin( wxSpinEvent& event )
{

    int tempvalue = m_FileSpin->GetValue();
    //m_nFileSpin = m_FileSpin->GetValue();
    //check if this value is valid for main url.
    //if not a valid , set old value to spin.
    TransferDataFromWindow(); //get url to m_sUrl

    if ( SelectFile( tempvalue ) )
    {
        m_nFileSpin = tempvalue;
    }
    else
    {
        m_FileSpin->SetValue( m_nFileSpin );
    }
}

bool CPropertyDlg::SelectFile( int step )
{
    int len = m_sUrl.Length();
    int pos = len;
    bool found = false;

    if ( step <= 0 )
        return false;

    for ( int i = 0; i < step; i++ )
    {
        found = false;

        for ( int j = pos - 1; j > 1; j-- )
        {
            if ( wxChar( '/' ) == m_sUrl[ j ] )
            {
                found = true;

                if ( wxChar( '/' ) != m_sUrl[ j - 1 ] )
                {
                    //valid /
                    pos = j;
                    break;
                }
                else
                {
                    //not valid /
                    goto fail;
                }
            }
        }

        if ( found )
            continue;
        else
            goto fail;

        //from end to search '/'
    }



    //select it
    //pos point to /
    //select from pos+1 to end;

    m_pTextCtrl->SetSelection( pos, len );

    return true;

fail:

    m_FileSpin->SetValue( m_nFileSpin );

    return false;
}

void CPropertyDlg::InitProxyList( std::string socks, std::string ftp, std::string http )
{
    m_SocksProxyList.clear();
    m_FtpProxyList.clear();
    m_HttpProxyList.clear();

    _PL temp;
    temp.name = "";
    temp.showname = _("None");

    m_SocksProxyList.push_back( temp );
    m_FtpProxyList.push_back( temp );
    m_HttpProxyList.push_back( temp );

    wxString tstr;
    //get proxy from parent
    int count = m_pParent->GetProxyCount();

    for ( int i = 0;i < count;i++ )
    {
        _ProxyAttr pa = m_pParent->GetProxy( i );

        switch ( pa.nType )
        {

            case 0:
            {
                temp.name = pa.sName;
#ifndef _UNICODE
                tstr.Printf( _("%s[%s:%d][auto]"), pa.sName.c_str(), pa.sServer.c_str(), pa.nPort );
#else
				//use Printf in UNICODE will error
				wxString port;
				port.Printf(_("%d"), pa.nPort);

				tstr = 	wxString ( pa.sName.c_str(), wxConvLocal ) + 
						_("[") + wxString( pa.sServer.c_str(), wxConvLocal ) + 
						_(":") + port + _("][auto]");
#endif
                temp.showname = tstr;
                m_SocksProxyList.push_back( temp );
            }

            break;

            case 1:
            {
                temp.name = pa.sName;
#ifndef _UNICODE

                tstr.Printf( _( "%s[%s:%d][v4]" ), pa.sName.c_str(), pa.sServer.c_str(), pa.nPort );
#else
				//use Printf in UNICODE will error
				wxString port;
				port.Printf(_("%d"), pa.nPort);

				tstr = 	wxString ( pa.sName.c_str(), wxConvLocal ) + 
						_("[") + wxString( pa.sServer.c_str(), wxConvLocal ) + 
						_(":") + port + _("][v4]");

#endif
                temp.showname = tstr;
                m_SocksProxyList.push_back( temp );
            }

            break;

            case 2:
            {
                temp.name = pa.sName;
#ifndef _UNICODE

                tstr.Printf( _( "%s[%s:%d][v4a]" ), pa.sName.c_str(), pa.sServer.c_str(), pa.nPort );
#else
				//use Printf in UNICODE will error
				wxString port;
				port.Printf(_("%d"), pa.nPort);

				tstr = 	wxString ( pa.sName.c_str(), wxConvLocal ) + 
						_("[") + wxString( pa.sServer.c_str(), wxConvLocal ) + 
						_(":") + port + _("][v4a]");
#endif
                temp.showname = tstr;
                m_SocksProxyList.push_back( temp );
            }

            break;

            case 3:
            {

                temp.name = pa.sName;
#ifndef _UNICODE

                tstr.Printf( _( "%s[%s:%d][v5]" ), pa.sName.c_str(), pa.sServer.c_str(), pa.nPort );
#else
				//use Printf in UNICODE will error
				wxString port;
				port.Printf(_("%d"), pa.nPort);

				tstr = 	wxString ( pa.sName.c_str(), wxConvLocal ) + 
						_("[") + wxString( pa.sServer.c_str(), wxConvLocal ) + 
						_(":") + port + _("][v5]");
#endif
                temp.showname = tstr;
                m_SocksProxyList.push_back( temp );
            }

            break;

            case 4:
            {
                temp.name = pa.sName;
#ifndef _UNICODE
                tstr.Printf( _( "%s[%s:%d]" ), pa.sName.c_str(), pa.sServer.c_str(), pa.nPort );
#else
				//use Printf in UNICODE will error
				wxString port;
				port.Printf(_("%d"), pa.nPort);

				tstr = 	wxString ( pa.sName.c_str(), wxConvLocal ) + 
						_("[") + wxString( pa.sServer.c_str(), wxConvLocal ) + 
						_(":") + port + _("]");
#endif
                temp.showname = tstr;
                m_FtpProxyList.push_back( temp );
            }

            break;

            case 5:
            {
                temp.name = pa.sName;
#ifndef _UNICODE
                tstr.Printf( _( "%s[%s:%d]" ), pa.sName.c_str(), pa.sServer.c_str(), pa.nPort );
#else
				//use Printf in UNICODE will error
				wxString port;
				port.Printf(_("%d"), pa.nPort);

				tstr = 	wxString ( pa.sName.c_str(), wxConvLocal ) + 
						_("[") + wxString( pa.sServer.c_str(), wxConvLocal ) + 
						_(":") + port + _("]");
#endif
                temp.showname = tstr;
                m_HttpProxyList.push_back( temp );
            }

            break;

            default:
            break;
        }
    }

    if ( m_SocksProxyList.size() == 1 )
    {
        m_SocksProxyList[ 0 ].showname = _("no socks proxy preseted");
    }

    if ( m_FtpProxyList.size() == 1 )
    {
        m_FtpProxyList[ 0 ].showname = _("no ftp proxy preseted");
    }

    if ( m_HttpProxyList.size() == 1 )
    {
        m_HttpProxyList[ 0 ].showname = _("no http proxy preseted");
    }


}

void CPropertyDlg::GetProxyName( std::string& socks, std::string& ftp, std::string& http )
{
    socks = ( m_SocksProxyList[ m_nSocksProxy ].name );
    ftp = ( m_FtpProxyList[ m_nFtpProxy ].name );
    http = ( m_HttpProxyList[ m_nHttpProxy ].name );
}

//如果user和pass不为空，要改写url返回
std::string CPropertyDlg::GetUrl()
{
    if ( !m_sUser.IsEmpty() && !m_sPass.IsEmpty() )
    { //用户和密码两个都不空
        //但是如果都等于缺省值就不重新构造地址

        if (
            m_sUser.mb_str( wxConvLocal ) != "anonymous" ||
            //m_sPass.mb_str( wxConvLocal ) != gDefFtpPass
				m_sPass.mb_str(wxConvLocal) != wxString::FromAscii( gDefFtpPass.c_str()).mb_str() //unicode patch
        )
        {
            CUrlParser parser;
            std::string rebuild;
            std::string origin = std::string( m_sUrl.mb_str( wxConvLocal ) );

            if ( parser.RebuildUrl( origin,
                                    std::string( m_sUser.mb_str( wxConvLocal ) ),
                                    std::string( m_sPass.mb_str( wxConvLocal ) ),
                                    rebuild ) )
            {
                return rebuild;
            }
        }
    }
#ifndef _UNICODE
    return m_sUrl.mb_str( wxConvLocal );
#else
    return m_sUrl.mb_str( wxConvLocal ).data();
#endif
}

void CPropertyDlg::AddMirrorUrl( wxString url )
{
    if ( m_sMirrorUrl.GetCount() > 80 )
        return ; //too many murl

    m_sMirrorUrl.Add( url );

    m_pMirrorBox->InsertItems( 1, &url, 0 );
}

int CPropertyDlg::GetMirrorUrl( int num, std::string mu[] )
{
    int n = m_sMirrorUrl.GetCount();

    int i;

    for ( i = 0; i < n; i++ )
    {
#ifndef _UNICODE
        std::string murl = m_sMirrorUrl[ i ].mb_str( wxConvLocal );
#else
	std::string murl = m_sMirrorUrl[ i ].mb_str( wxConvLocal ).data();
#endif
        Trim( murl );

        if ( murl.empty() )
            continue;
        else if ( murl[ murl.length() - 1 ] == '/' )
        {
            //put the selected file name at the end of url.
            murl += GetSelectFileName();
        }

        mu[ i ] = murl;

        if ( i >= num )
            break;
    }

    return i;
}

std::string CPropertyDlg::GetSelectFileName()
{
    int len = m_sUrl.Length();
    int pos = len;
    bool found = false;
    int step = m_nFileSpin;
    CUrlParser par;
    wxString ret;

    if ( step <= 0 )
        return std::string( "" );

    for ( int i = 0; i < step; i++ )
    {
        found = false;

        for ( int j = pos - 1; j > 1; j-- )
        {
            if ( wxChar( '/' ) == m_sUrl[ j ] )
            {
                found = true;

                if ( wxChar( '/' ) != m_sUrl[ j - 1 ] )
                {
                    //valid /
                    pos = j;
                    break;
                }
                else
                {
                    //not valid /
                    goto fail;
                }
            }
        }

        if ( found )
            continue;
        else
            goto fail;

        //from end to search '/'
    }



    //select it
    //pos point to /
    //select from pos+1 to end;

    ret = m_sUrl.SubString( pos + 1, len );

#ifndef _UNICODE
    return ret.mb_str( wxConvLocal );
#else
	return ret.mb_str( wxConvLocal ).data();//unicode patch
#endif

fail:

#ifndef _UNICODE
    if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ) ) )
#else
	if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ).data() ) ) //unicode patch
#endif
    {
        return par.GetFileName();
    }

    return std::string( "" );
}

std::string CPropertyDlg::GetSocksProxyName()
{
    return m_SocksProxyList[ m_nSocksProxy ].name;
}

std::string CPropertyDlg::GetHttpProxyName()
{
    return m_HttpProxyList[ m_nHttpProxy ].name;
}

std::string CPropertyDlg::GetFtpProxyName()
{
    return m_FtpProxyList[ m_nFtpProxy ].name;
}

/*
void CPropertyDlg::OnUrlText( wxCommandEvent& event )
{//url text changed
 
	
	if(	!m_bOpenUrlKeyEvent) return;  //too ugly but work!!
		
	TransferDataFromWindow();
 
	CUrlParser par;
	if(!par.SetUrl(m_sUrl.mb_str(wxConvLocal)))
	{
		m_sRename.Clear();
		TransferDataToWindow();
		m_pTextCtrl->SetInsertionPointEnd();
	}
	else
	{
		m_sRename=wxString(par.GetFileName().c_str(),wxConvLocal);
		TransferDataToWindow();
		m_pTextCtrl->SetInsertionPointEnd();
	}
 
}
 
*/

