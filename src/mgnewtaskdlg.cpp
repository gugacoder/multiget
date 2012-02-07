/***************************************************************************
*            mgnewtaskdlg.cpp
*
*  Mon Sep 18 14:16:13 2006
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

#include "mgnewtaskdlg.h"
#include "mainframe.h"
#include "mgapp.h"
#include "mgurlparser.h"

#include <wx/valgen.h>

DEFINE_EVENT_TYPE( mgID_PICKPATH )
DEFINE_EVENT_TYPE( mgID_SPINTHREAD )
DEFINE_EVENT_TYPE( mgID_SPINRETRY )
DEFINE_EVENT_TYPE( mgID_SPINRETRYWAIT )
DEFINE_EVENT_TYPE( mgID_SPINFILE )
DEFINE_EVENT_TYPE( mgID_URLTEXT )
DEFINE_EVENT_TYPE( mgID_RENAMETEXT )
DEFINE_EVENT_TYPE( mgEVT_URL_CHANGE )
DEFINE_EVENT_TYPE( mgEVT_RENAME_CHANGE )

BEGIN_EVENT_TABLE( CNewTaskDlg, wxDialog )
EVT_BUTTON( mgID_PICKPATH, CNewTaskDlg::OnPickPath )
EVT_SPINCTRL( mgID_SPINTHREAD, CNewTaskDlg::OnThSpin )
EVT_SPINCTRL( mgID_SPINRETRY, CNewTaskDlg::OnReSpin )
EVT_SPINCTRL( mgID_SPINRETRYWAIT, CNewTaskDlg::OnWaSpin )
EVT_SPIN( mgID_SPINFILE, CNewTaskDlg::OnFileSpin )
EVT_TEXT( mgID_URLTEXT, CNewTaskDlg::OnUrlText )
EVT_TEXT( mgID_RENAMETEXT, CNewTaskDlg::OnRenameText)
//EVT_COMMAND( wxID_ANY, mgEVT_URL_CHANGE, CNewTaskDlg::OnUrlChange )
END_EVENT_TABLE()

extern std::list<std::string> gSavePathHistory; //保存目录
extern int gTaskDefThread, gRetryTime, gRetryWait;
extern std::string gDefSavePath;
extern std::string gDefFtpPass;
CNewTaskDlg::CNewTaskDlg( MainFrame* parent, wxString defUrl, wxString refer, wxString defSavePath )
        : wxDialog( parent, -1, _("New task"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    m_pParent = parent;
    m_sUrl = defUrl;
	
    m_sRefer = refer;
    //对这个url分离用户名和密码

    if ( !m_sUrl.IsEmpty() )
    {
        CUrlParser par;
        std::string m_swap;

#ifndef _UNICODE
        if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ) ) )
#else
		if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ).data() ) )
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
				m_sRename=wxString(par.GetFileName().c_str(),wxConvLocal);
            }
        }
    }

    m_sSavePath = defSavePath;

    if ( m_sSavePath.IsEmpty() )
    {
        m_sSavePath = wxString( gDefSavePath.c_str(), wxConvLocal );

        if ( m_sSavePath.IsEmpty() )
        {
            std::string home;
            GetUserHome( home );
            m_sSavePath = wxString( home.c_str(), wxConvLocal );
        }
    }

    m_nThreadNum = gTaskDefThread;
    m_nSocksProxy = 0;
    m_nFtpProxy = 0;
    m_nHttpProxy = 0;
    m_nRunNow = 0; //run
    m_nFileSpin = 1;
    m_nRetry = gRetryTime;
    m_nRetryWait = gRetryWait;
    m_sSavePath = wxString( gDefSavePath.c_str(), wxConvLocal );
    m_pMirrorBox = NULL;
    m_ThSpin = NULL;
    m_ThRetry = NULL;
    m_ThRetryWait = NULL;
    m_FileSpin = NULL;
    m_pTextCtrl = NULL;
	m_RenameCtrl=NULL;
    m_bOpenUrlKeyEvent = false;
    //这里准备一下代理列表
    InitProxyList();
    Init();

}

CNewTaskDlg::~CNewTaskDlg()
{
}

void CNewTaskDlg::Init()
{

    if ( m_sUrl.IsNull() && wxTheClipboard->Open() )
    {

        if ( wxTheClipboard->IsSupported( wxDF_TEXT ) )
        {

            wxTextDataObject text;

            if ( wxTheClipboard->GetData( text ) )
            {
                m_sUrl = text.GetText();
                CUrlParser upar;

                if ( !upar.SetUrl( std::string( m_sUrl.mb_str( wxConvLocal ) ) ) )
                {
                    m_sUrl = _( "" );
                }
                else if ( upar.GetFileName().empty() )
                {
                    m_sUrl = _( "" );
                }
            }
        }

        wxTheClipboard->Close();
    }

    //for border
    wxBoxSizer *top = new wxBoxSizer( wxVERTICAL );

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

    //part6 insert refer
    wxBoxSizer *part6 = new wxBoxSizer( wxVERTICAL );

    //part5 button
    wxBoxSizer *part5 = new wxBoxSizer( wxVERTICAL );

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
                      mgID_URLTEXT,
                      _( "" ),
                      wxDefaultPosition,
                      wxSize( 400, 25 ),
                      0,
                      wxGenericValidator( &m_sUrl ) );

    m_pTextCtrl->SetMaxLength( 256 );

    url->Add( m_pTextCtrl, 1, wxEXPAND );

    m_FileSpin = new wxSpinButton(
                     this,
                     mgID_SPINFILE,
                     wxDefaultPosition,
#ifdef WIN32					 
                     wxSize( 20, 20 ),
                     wxSP_VERTICAL ); 
#else
                     wxSize( 25, 25 ),
                     wxSP_VERTICAL ); //wxSP_HORIZONTAL not support by GTK!
#endif

    m_FileSpin->SetRange( 1, 5 );

    m_FileSpin->SetValue( 1 );

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


    savepath->Add( new wxStaticText( this, -1, _("Save To:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );

    /*	savepath->Add(
    	new wxTextCtrl(this,-1,_(""),wxDefaultPosition,wxSize(285,25),0,wxGenericValidator(&m_sSavePath))
    	,0,wxEXPAND);
    */

    //从历史记录中导出目录
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
            //DBGOUT("add a path to dropdown");
            //DBGOUT(temp);
            pchoice.Add( temp );
        }
    }

	//it seems in win32 m_sSavePath don't show in gui
	m_SavePathCtrl= new wxComboBox(
            this,
            -1,
            _( "" ),
            wxDefaultPosition,
#ifdef WIN32
            wxSize( 200, 22 ),
#else
            wxSize( 200, 25 ),
#endif
            pchoice,
            1,
            wxGenericValidator( &m_sSavePath )
        );
	//it seems in win32 m_sSavePath don't show in gui
#ifdef WIN32
	m_SavePathCtrl->SetValue(m_sSavePath);
#endif
		
    savepath->Add(m_SavePathCtrl, 1, wxEXPAND );
    savepath->Add( 
			new wxButton( this, mgID_PICKPATH, _( "..." ), wxDefaultPosition, 
#ifdef WIN32
			wxSize( 35, 22 ) ), 
			0, wxEXPAND );			
#else
			wxSize( 35, 25 ) ), 
			0, wxALIGN_RIGHT );
#endif			
    rename->Add( new wxStaticText( this, -1, _("Rename:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );

	m_RenameCtrl=new wxTextCtrl(
                     this,
                     mgID_RENAMETEXT,
                     _( "" ),
                     wxDefaultPosition,
                     wxSize( 200, 25 ),1,wxGenericValidator( &m_sRename ));
					 
    rename->Add( m_RenameCtrl, 1, wxEXPAND );

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
    m_ThSpin = new wxSpinCtrl( this, mgID_SPINTHREAD, _( "" ), wxDefaultPosition, wxSize( 20, 25 ) ); //
    m_ThSpin->SetRange( 1, 10 );
    m_ThSpin->SetValue( m_nThreadNum );
    threads->AddSpacer( 10 );
    threads->Add( m_ThSpin, 1, wxEXPAND );
    threads->AddSpacer( 10 );
    part2->Add( threads, 1, wxEXPAND );
    part2->AddSpacer( 10 );

    wxBoxSizer *retry = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Retry") );
    m_ThRetry = new wxSpinCtrl( this, mgID_SPINRETRY, _( "" ), wxDefaultPosition, wxSize( 20, 25 ) ); //
    m_ThRetry->SetRange( 0, 999 );
    m_ThRetry->SetValue( m_nRetry );
    retry->AddSpacer( 10 );
    retry->Add( m_ThRetry, 1, wxEXPAND );
    retry->AddSpacer( 10 );
    part2->Add( retry, 1, wxEXPAND );
    part2->AddSpacer( 10 );

    wxBoxSizer *retrywait = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Retry Wait [s]") );
    m_ThRetryWait = new wxSpinCtrl( this, mgID_SPINRETRYWAIT, _( "" ), wxDefaultPosition, wxSize( 20, 25 ) ); //
    m_ThRetryWait->SetRange( 5, 3000 );
    m_ThRetryWait->SetValue( m_nRetryWait );
    retrywait->AddSpacer( 10 );
    retrywait->Add( m_ThRetryWait, 1, wxEXPAND );
    retrywait->AddSpacer( 10 );
    part2->Add( retrywait, 1, wxEXPAND );
    part2->AddSpacer( 10 );

    wxString choices[ 2 ] =
        {
            _("yes"),
            _("no"),
        };
    part2->Add(
        new wxRadioBox(
            this,
            -1,
            _("Run now?"),
            wxDefaultPosition,
            wxSize( 50, 25 ),
            2,
            choices,
            0,
            wxRA_SPECIFY_COLS,
            wxGenericValidator( &m_nRunNow ) ),
        1,
        wxEXPAND | wxALIGN_RIGHT );


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

    socks->Add( new wxChoice(
                    this,
                    -1,
                    wxDefaultPosition,
                    wxSize( 210, 28 ),
                    socksarray,
                    0,
                    wxGenericValidator( &m_nSocksProxy ) ), 1, wxEXPAND );

    proxy->Add( socks, 1, wxEXPAND );

    ftp->Add( new wxStaticText( this, -1, _( "FTP:" ), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );
    wxArrayString ftparray; //socks list
    //copy string

    for ( unsigned int i = 0;i < m_FtpProxyList.size();i++ )
    {
        ftparray.Add( m_FtpProxyList[ i ].showname );
    }

    ftp->Add( new wxChoice(
                  this,
                  -1,
                  wxDefaultPosition,
                  wxSize( 210, 28 ),
                  ftparray,
                  0,
                  wxGenericValidator( &m_nFtpProxy ) ), 1, wxEXPAND );
    proxy->Add( ftp, 1, wxEXPAND );

    http->Add( new wxStaticText( this, -1, _( "HTTP:" ), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_LEFT );
    wxArrayString httparray; //socks list
    //copy string

    for ( unsigned int i = 0;i < m_HttpProxyList.size();i++ )
    {
        httparray.Add( m_HttpProxyList[ i ].showname );
    }

    http->Add( new wxChoice(
                   this,
                   -1,
                   wxDefaultPosition,
                   wxSize( 210, 28 ),
                   httparray,
                   0,
                   wxGenericValidator( &m_nHttpProxy ) ), 1, wxEXPAND );
    proxy->Add( http, 1, wxEXPAND );

    part3->Add( proxy, 1, wxEXPAND );



    //part4

    wxStaticBoxSizer* userpass = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Login[option]") );
    userpass->Add( new wxStaticText( this, -1, _("User:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_RIGHT );
    userpass->Add( new wxTextCtrl(
                       this,
                       -1,
                       _( "" ),
                       wxDefaultPosition,
                       wxSize( 60, 25 ),
                       0,
                       wxGenericValidator( &m_sUser ) ), 1, wxEXPAND );
    userpass->AddSpacer( 10 );
    userpass->Add( new wxStaticText( this, -1, _("Password:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_RIGHT );
    userpass->Add( new wxTextCtrl(
                       this,
                       -1,
                       _( "" ),
                       wxDefaultPosition,
                       wxSize( 80, 25 ),
                       0,
                       wxGenericValidator( &m_sPass ) ), 1, wxEXPAND );

    part4->Add( userpass, 1, wxEXPAND );

    //part5

    wxBoxSizer* but = new wxBoxSizer( wxHORIZONTAL );
    but->Add( new wxButton( this, wxID_CANCEL, _( "Cancel" ) ), 0, wxALL, 5 );
    but->Add( new wxButton( this, wxID_OK, _( "OK" ) ), 0, wxALL, 5 );
    part5->Add( but, 0, wxALIGN_RIGHT );


    //part6
    wxStaticBoxSizer* refer = new wxStaticBoxSizer( wxHORIZONTAL, this, _("Reference[option]") );
    refer->Add( new wxStaticText( this, -1, _("Refer:"), wxDefaultPosition, wxSize( 70, 25 ) ), 0, wxALIGN_RIGHT );
    refer->Add( new wxTextCtrl(
                    this,
                    -1,
                    _( "" ),
                    wxDefaultPosition,
                    wxSize( 80, 25 ),
                    0,
                    wxGenericValidator( &m_sRefer ) ), 1, wxEXPAND );

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

    TransferDataToWindow();
    //top->SetMinSize(420,300);
    top->Fit( this );
    SetSizer( top );
    top->SetSizeHints( this );

    m_bOpenUrlKeyEvent = true;

    if ( m_sUrl.IsNull() )
    {
        m_pTextCtrl->SetFocus();
    }
    else
    {
        m_ThSpin->SetFocus();
    }


}

void CNewTaskDlg::OnPickPath( wxCommandEvent& event )
{
	m_sSavePath=m_SavePathCtrl->GetValue();
	
    wxDirDialog dlg( this, _("Choose a diretory to save file"), m_sSavePath, wxDD_NEW_DIR_BUTTON );

    if ( dlg.ShowModal() == wxID_OK )
    {
        //TransferDataFromWindow();
        m_sSavePath = dlg.GetPath();
		m_SavePathCtrl->SetValue(m_sSavePath);
		//MessageBox(NULL,m_sSavePath,"cap",MB_OK);
        //TransferDataToWindow();//??don't work in windows?
    }
}

void CNewTaskDlg::OnThSpin( wxSpinEvent& event )
{
    m_nThreadNum = m_ThSpin->GetValue();
}

void CNewTaskDlg::OnReSpin( wxSpinEvent& event )
{
    m_nRetry = m_ThRetry->GetValue();
}

void CNewTaskDlg::OnWaSpin( wxSpinEvent& event )
{
    m_nRetryWait = m_ThRetryWait->GetValue();
}

void CNewTaskDlg::OnFileSpin( wxSpinEvent& event )
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

bool CNewTaskDlg::SelectFile( int step )
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

    //m_pTextCtrl->SetInsertionPointEnd();
    return true;

fail:

    m_FileSpin->SetValue( m_nFileSpin );

    return false;
}

void CNewTaskDlg::InitProxyList()
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

void CNewTaskDlg::GetProxyName( std::string& socks, std::string& ftp, std::string& http )
{
    socks = ( m_SocksProxyList[ m_nSocksProxy ].name );

    ftp = ( m_FtpProxyList[ m_nFtpProxy ].name );

    http = ( m_HttpProxyList[ m_nHttpProxy ].name );

}

//如果user和pass不为空，要改写url返回
std::string CNewTaskDlg::GetUrl()
{
    if ( !m_sUser.IsEmpty() && !m_sPass.IsEmpty() )
    { //用户和密码两个都不空
        //但是如果都等于缺省值就不重新构造地址

        if (
            m_sUser.mb_str( wxConvLocal ) != "anonymous" ||
//            m_sPass.mb_str( wxConvLocal ) != gDefFtpPass
				m_sPass.mb_str(wxConvLocal) != wxString::FromAscii(gDefFtpPass.c_str()).mb_str() //unicode patch
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

void CNewTaskDlg::AddMirrorUrl( wxString url )
{
    if ( m_sMirrorUrl.GetCount() > 100 )
        return ; //too many murl

    m_sMirrorUrl.Add( url );

    m_pMirrorBox->InsertItems( 1, &url, 0 );
}

int CNewTaskDlg::GetMirrorUrl( int num, std::string mu[] )
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

        //		else if(murl[murl.length()-1]=='/') {
        //put the selected file name at the end of url.
        //			murl+=GetSelectFileName();
        //		}
        mu[ i ] = murl;

        if ( i >= num )
            break;
    }

    return i;
}

std::string CNewTaskDlg::GetSelectFileName()
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
    return ret.mb_str( wxConvLocal ).data();
#endif

fail:

#ifndef _UNICODE
    if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ) ) )
#else
    if ( par.SetUrl( m_sUrl.mb_str( wxConvLocal ).data() ) )
#endif
    {
        return par.GetFileName();
    }

    return std::string( "" );
}

void CNewTaskDlg::OnUrlText( wxCommandEvent& event )
{ //url text changed

#ifdef WIN32
    if ( !m_bOpenUrlKeyEvent )
        return ;  //too ugly but work!!
	
	TransferDataFromWindow();
	
	CUrlParser par;
    if ( !par.SetUrl( m_sUrl.c_str() ) )
	{
		m_sRename.Clear();
		//m_pTextCtrl->SetInsertionPointEnd();
		m_RenameCtrl->SetValue(m_sRename);		
	}
	else
	{
		m_sRename = wxString( par.GetFileName().c_str(), wxConvLocal );
		//m_pTextCtrl->SetInsertionPointEnd();
		m_RenameCtrl->SetValue(m_sRename);
	}
	
#else
    if ( !m_bOpenUrlKeyEvent )
        return ;  //too ugly but work!!

    TransferDataFromWindow();
	
    CUrlParser par;
#ifndef _UNICODE
    if ( !par.SetUrl( m_sUrl.mb_str( wxConvLocal ) ) )
#else
    if ( !par.SetUrl( m_sUrl.mb_str( wxConvLocal ).data() ) )
#endif
    {
        m_sRename.Clear();
        //TransferDataToWindow(); //this in win32 will cause a new Text Event,and then circle and circle!
        //m_pTextCtrl->SetInsertionPointEnd();
		m_RenameCtrl->SetValue(m_sRename);
    }
    else
    {
        m_sRename = wxString( par.GetFileName().c_str(), wxConvLocal );
        //TransferDataToWindow();
        //m_pTextCtrl->SetInsertionPointEnd();
		m_RenameCtrl->SetValue(m_sRename);
    }
#endif
}

void CNewTaskDlg::OnRenameText( wxCommandEvent& event )
{
	event.Skip();
}

