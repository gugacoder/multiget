/***************************************************************************
*  configwin.cpp
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
#include "configwin.h"
#include "mgapp.h"

#include "mainframe.h"
#include <wx/valgen.h>

DEFINE_EVENT_TYPE( cfID_PICKPATH )
DEFINE_EVENT_TYPE( cfID_SPINTHREAD )
DEFINE_EVENT_TYPE( cfID_SPINRETRY )
DEFINE_EVENT_TYPE( cfID_SPINMAXTASK )

DEFINE_EVENT_TYPE( cfID_SPINCONNECT )
DEFINE_EVENT_TYPE( cfID_SPINRW )
DEFINE_EVENT_TYPE( cfID_SPINWAIT )

BEGIN_EVENT_TABLE( CConfigWindow, wxDialog )

EVT_BUTTON( cfID_PICKPATH, CConfigWindow::OnPickPath )
EVT_SPINCTRL( cfID_SPINTHREAD, CConfigWindow::OnSpinThread )
EVT_SPINCTRL( cfID_SPINRETRY, CConfigWindow::OnSpinRetry )
EVT_SPINCTRL( cfID_SPINMAXTASK, CConfigWindow::OnSpinTask )

EVT_SPINCTRL( cfID_SPINCONNECT, CConfigWindow::OnSpinConnect )
EVT_SPINCTRL( cfID_SPINRW, CConfigWindow::OnSpinReadWrite )
EVT_SPINCTRL( cfID_SPINWAIT, CConfigWindow::OnSpinWait )

END_EVENT_TABLE()

extern int gRunTaskNum;
extern int gConnectTimeOut;
extern int gReceiveTimeOut;
extern int gTaskDefThread;
extern int gRetryTime;
extern int gRetryWait;
extern bool gbIgnoreExt;
extern bool gbMonitorExt;
extern bool gbSoundTip;
extern bool gbAutoMd5;
extern _SPEED_MODE gSpeedMode;
extern std::string gDefSavePath, gDefFtpPass, gsMonitorExt, gsIgnoreExt;

CConfigWindow::CConfigWindow( MainFrame* parent )
        : wxDialog( parent, -1, _("System Configure"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    m_nTask = gRunTaskNum;
    m_nThread = gTaskDefThread;
    m_nRetry = gRetryTime;
    m_nConnectTimeOut = gConnectTimeOut;
    m_nTimeOut = gReceiveTimeOut;
    m_nRetryWait = gRetryWait;
    m_bSound = gbSoundTip;
    m_bAutoMd5 = gbAutoMd5;
    m_bIgnoreExt = gbIgnoreExt;
    m_bMonitorExt = gbMonitorExt;
    m_sMonitorExt = wxString( gsMonitorExt.c_str(), wxConvLocal );
    m_sIgnoreExt = wxString( gsIgnoreExt.c_str(), wxConvLocal );
    m_sSavePath = wxString( gDefSavePath.c_str(), wxConvLocal );
    m_sPass = wxString( gDefFtpPass.c_str(), wxConvLocal );

    m_SpeedMode = ( int ) gSpeedMode;

    m_SpinTask = NULL;
    m_SpinThread = NULL;
    m_SpinRetry = NULL;
    m_SpinConnectTimeOut = NULL;
    m_SpinRetryWait = NULL;
    m_SpinTimeOut = NULL;
    Init();
}

CConfigWindow::~CConfigWindow()
{}

void CConfigWindow::Init()
{
    //for border
    wxBoxSizer * top = new wxBoxSizer( wxVERTICAL );
    //old top
    wxBoxSizer *all = new wxBoxSizer( wxVERTICAL );

    //part 1
    wxBoxSizer *part1 = new wxStaticBoxSizer( wxVERTICAL, this, _("Task Arguments") );
    wxBoxSizer *line1 = new wxBoxSizer( wxHORIZONTAL ); //max run task
    line1->Add( new wxStaticText( this, -1, _("Max Running Tasks[1-10]:") , wxDefaultPosition, wxSize( 150, 25 ) ), 1, wxALIGN_LEFT | wxEXPAND );
    m_SpinTask = new wxSpinCtrl( this, cfID_SPINMAXTASK, _( "" ), wxDefaultPosition, wxSize( 60, 25 ) ); //
    m_SpinTask->SetRange( 1, 10 );
    m_SpinTask->SetValue( m_nTask );
    line1->Add( m_SpinTask, 0, wxALIGN_LEFT );

    wxBoxSizer *line2 = new wxBoxSizer( wxHORIZONTAL ); //default thread
    line2->Add( new wxStaticText( this, -1, _("Default Sessions[1-10]:"), wxDefaultPosition, wxSize( 150, 25 ) ), 1, wxALIGN_LEFT | wxEXPAND );
    m_SpinThread = new wxSpinCtrl( this, cfID_SPINTHREAD, _( "" ), wxDefaultPosition, wxSize( 60, 25 ) ); //
    m_SpinThread->SetRange( 1, 10 );
    m_SpinThread->SetValue( m_nThread );
    line2->Add( m_SpinThread, 0, wxALIGN_LEFT );

    wxBoxSizer *line3 = new wxBoxSizer( wxHORIZONTAL ); //default save path
    line3->Add( new wxStaticText(
                    this,
                    -1,
                    _("Default Save Path:"),
                    wxDefaultPosition,
                    wxSize( 160, 25 ) ), 0, wxEXPAND );
    line3->Add( new wxTextCtrl(
                    this,
                    -1,
                    _( "" ),
                    wxDefaultPosition,
                    wxSize( 100, 25 ),
                    1,
                    wxGenericValidator( &m_sSavePath ) ), 1, wxEXPAND );

    line3->Add( new wxButton( this, cfID_PICKPATH, _( "..." ), wxDefaultPosition, wxSize( 35, 25 ) ), 0, wxALIGN_RIGHT );
    //connect timeout
    //part 2
    wxBoxSizer *part2 = new wxStaticBoxSizer( wxVERTICAL, this, _("Network Arguments") );
    wxBoxSizer *line4 = new wxBoxSizer( wxHORIZONTAL );
    line4->Add( new wxStaticText( this, -1, _("Connect Timeout[10-300]:"), wxDefaultPosition, wxSize( 150, 25 ) ), 1, wxEXPAND );
    m_SpinConnectTimeOut = new wxSpinCtrl( this, cfID_SPINCONNECT, _( "" ), wxDefaultPosition, wxSize( 60, 25 ) ); //
    m_SpinConnectTimeOut->SetRange( 10, 300 );
    m_SpinConnectTimeOut->SetValue( m_nConnectTimeOut );
    line4->Add( m_SpinConnectTimeOut, 0, wxALIGN_LEFT );

    wxBoxSizer *line5 = new wxBoxSizer( wxHORIZONTAL );
    line5->Add( new wxStaticText( this, -1, _("Send/Receive Timeout[10-300]:"), wxDefaultPosition, wxSize( 150, 25 ) ), 1, wxEXPAND );
    m_SpinTimeOut = new wxSpinCtrl( this, cfID_SPINRW, _( "" ), wxDefaultPosition, wxSize( 60, 25 ) ); //
    m_SpinTimeOut->SetRange( 10, 300 );
    m_SpinTimeOut->SetValue( m_nTimeOut );
    line5->Add( m_SpinTimeOut, 0, wxALIGN_LEFT );
    //retry,retry delay
    wxBoxSizer *line6 = new wxBoxSizer( wxHORIZONTAL );
    line6->Add( new wxStaticText( this, -1, _("Default Retry/Sessions[1-99999]:"), wxDefaultPosition, wxSize( 150, 25 ) ), 1, wxEXPAND );
    m_SpinRetry = new wxSpinCtrl( this, cfID_SPINRETRY, _( "" ), wxDefaultPosition, wxSize( 60, 25 ) ); //
    m_SpinRetry->SetRange( 1, 99999 );
    m_SpinRetry->SetValue( m_nRetry );
    line6->Add( m_SpinRetry, 0, wxALIGN_LEFT );

    wxBoxSizer *line7 = new wxBoxSizer( wxHORIZONTAL );
    line7->Add( new wxStaticText( this, -1, _("Default Retry Delay[5-3000]:"), wxDefaultPosition, wxSize( 150, 25 ) ), 1, wxEXPAND );
    m_SpinRetryWait = new wxSpinCtrl( this, cfID_SPINWAIT, _( "" ), wxDefaultPosition, wxSize( 60, 25 ) ); //
    m_SpinRetryWait->SetRange( 5, 3000 );
    m_SpinRetryWait->SetValue( m_nRetryWait );
    line7->Add( m_SpinRetryWait, 0, wxALIGN_LEFT );
    //FTP pass

    //part3 soundtip,FTP pass,speedmode
    wxBoxSizer *part3 = new wxStaticBoxSizer( wxVERTICAL, this, _("Other") );
    wxBoxSizer *line8 = new wxBoxSizer( wxHORIZONTAL );

    line8->Add( new wxStaticText(
                    this,
                    -1,
                    _("FTP Anon Pass:"),
                    wxDefaultPosition,
                    wxSize( 110, 25 ) ), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
    line8->Add( new wxTextCtrl(
                    this,
                    -1,
                    _( "" ),
                    wxDefaultPosition,
                    wxSize( 150, 25 ),
                    1,
                    wxGenericValidator( &m_sPass ) ), 1, wxEXPAND | wxALIGN_CENTER_VERTICAL );

    line8->AddSpacer( 10 );
    line8->Add( new wxStaticText(
                    this,
                    -1,
                    _("Speed Limit"),
                    wxDefaultPosition,
                    wxSize( 90, 25 ) ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );

    wxArrayString choices;
    choices.Add( _("No Limit") );
    choices.Add( _( "5 M/s" ) );
    choices.Add( _( "2 M/s" ) );
    choices.Add( _( "1 M/s" ) );
    choices.Add( _( "700 KB/s" ) );
    choices.Add( _( "500 KB/s" ) );
    choices.Add( _( "400 KB/s" ) );
    choices.Add( _( "300 KB/s" ) );
    choices.Add( _( "200 KB/s" ) );
    choices.Add( _( "100 KB/s" ) );
    choices.Add( _( "50 KB/s" ) );
    choices.Add( _( "20 KB/s" ) );
    choices.Add( _( "10 KB/s" ) );

    line8->Add( new wxChoice(
                    this,
                    -1,
                    wxDefaultPosition,
                    wxSize( 100, 25 ),
                    choices,
                    0,
                    wxGenericValidator( &m_SpeedMode ) ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
    //float win

    wxBoxSizer *monitor = new wxStaticBoxSizer( wxVERTICAL, this, _("Monitor Clipboard") );
    wxBoxSizer *line9 = new wxBoxSizer( wxHORIZONTAL );
    line9->Add( new wxCheckBox(
                    this,
                    -1,
                    _("Ignore Extentions:"),
                    wxDefaultPosition,
                    wxSize( 160, 25 ),
                    wxCHK_2STATE,
                    wxGenericValidator( &m_bIgnoreExt ) ), 0, wxALIGN_LEFT | wxALIGN_TOP );

    line9->Add( new wxTextCtrl(
                    this,
                    -1,
                    _( "" ),
                    wxDefaultPosition,
                    wxSize( 200, 50 ),
                    wxTE_BESTWRAP | wxTE_MULTILINE,
                    wxGenericValidator( &m_sIgnoreExt ) ), 1, wxEXPAND );


    wxBoxSizer *line10 = new wxBoxSizer( wxHORIZONTAL );
    line10->Add( new wxCheckBox(
                     this,
                     -1,
                     _("Capture Extentions:"),
                     wxDefaultPosition,
                     wxSize( 160, 25 ),
                     wxCHK_2STATE,
                     wxGenericValidator( &m_bMonitorExt ) ), 0, wxALIGN_LEFT | wxALIGN_TOP );

    //wxTE_BESTWRAP|wxTE_MULTILINE
    line10->Add( new wxTextCtrl(
                     this,
                     -1,
                     _( "" ),
                     wxDefaultPosition,
                     wxSize( 200, 50 ),
                     wxTE_BESTWRAP | wxTE_MULTILINE,
                     wxGenericValidator( &m_sMonitorExt ) ), 1, wxEXPAND );

    wxBoxSizer *line12 = new wxBoxSizer( wxHORIZONTAL );
    line12->Add( new wxCheckBox(
                     this,
                     -1,
                     _("Sound tip when task finish (Experimental)"),
                     wxDefaultPosition,
                     wxSize( 250, 25 ),
                     wxCHK_2STATE,
                     wxGenericValidator( &m_bSound ) ), 1, wxALIGN_LEFT | wxEXPAND );

    line12->Add( new wxCheckBox(
                     this,
                     -1,
                     _("Auto md5 check"),
                     wxDefaultPosition,
                     wxSize( 150, 25 ),
                     wxCHK_2STATE,
                     wxGenericValidator( &m_bAutoMd5 ) ), 1, wxALIGN_LEFT | wxEXPAND );

    line8->AddSpacer( 5 );
    part1->Add( line1, 1, wxEXPAND );
    part1->Add( line2, 1, wxEXPAND );
    part1->Add( line3, 1, wxEXPAND );
    all->Add( part1, 0, wxEXPAND );

    part2->Add( line4, 1, wxEXPAND );
    part2->Add( line5, 1, wxEXPAND );
    part2->Add( line6, 1, wxEXPAND );
    part2->Add( line7, 1, wxEXPAND );
    all->Add( part2, 0, wxEXPAND );

    //part3->AddSpacer(5);
    //part3->Add(line11,0,wxEXPAND);

    monitor->Add( line9, 1, wxEXPAND );
    monitor->AddSpacer( 10 );
    monitor->Add( line10, 1, wxEXPAND );
    all->Add( monitor, 1, wxEXPAND );

    part3->Add( line8, 1, wxEXPAND );
    part3->Add( line12, 1, wxEXPAND );
    all->Add( part3, 0, wxEXPAND );

    wxBoxSizer* but = new wxBoxSizer( wxHORIZONTAL );
    but->Add( new wxButton( this, wxID_CANCEL, _( "Cancel" ) ), 0, wxALL, 5 );
    but->Add( new wxButton( this, wxID_OK, _( "OK" ) ), 0, wxALL, 5 );
    all->Add( but, 0, wxALIGN_RIGHT | wxALIGN_BOTTOM );

    top->Add( all, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 5 );
    //top->SetMinSize(420,300);
    top->Fit( this );
    SetSizer( top );
    top->SetSizeHints( this );
}


void CConfigWindow::OnPickPath( wxCommandEvent& event )
{
    wxDirDialog dlg( this, _("Choose a directory to save file"), m_sSavePath, wxDD_NEW_DIR_BUTTON );

    if ( dlg.ShowModal() == wxID_OK )
    {
        m_sSavePath = dlg.GetPath();
        TransferDataToWindow();
    }
}

void CConfigWindow::OnSpinThread( wxSpinEvent& event )
{
    m_nThread = m_SpinThread->GetValue();
}

void CConfigWindow::OnSpinRetry( wxSpinEvent& event )
{
    m_nRetry = m_SpinRetry->GetValue();
}

void CConfigWindow::OnSpinTask( wxSpinEvent& event )
{
    m_nTask = m_SpinTask->GetValue();
}

void CConfigWindow::OnSpinConnect( wxSpinEvent& event )
{
    m_nConnectTimeOut = m_SpinConnectTimeOut->GetValue();
}

void CConfigWindow::OnSpinReadWrite( wxSpinEvent& event )
{
    m_nTimeOut = m_SpinTimeOut->GetValue();
}

void CConfigWindow::OnSpinWait( wxSpinEvent& event )
{
    m_nRetryWait = m_SpinRetryWait->GetValue();
}

