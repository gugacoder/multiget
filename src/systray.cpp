
/***************************************************************************
 *  systray.cpp
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
#include "systray.h" 
//#include "./icons/mg_32.xpm"
//#include "./icons/mg_24.xpm"
#include "mgapp.h"
#include "mainframe.h"
#include "floatwin.h"

#include "./icons/sysicon_win.xpm"
#include "./newicons/16/new.xpm"
#include "./icons/quit.xpm"
#include "./icons/about.xpm"
#include "./newicons/16/option.xpm"

DEFINE_EVENT_TYPE( PU_RESTORE )
DEFINE_EVENT_TYPE( PU_SHOWDROPWIN )
DEFINE_EVENT_TYPE( PU_MONITORCLIP )
DEFINE_EVENT_TYPE( PU_STARTALL )
DEFINE_EVENT_TYPE( PU_STOPALL )
DEFINE_EVENT_TYPE( PU_NEWTASK )
DEFINE_EVENT_TYPE( PU_SPEEDMODE )
DEFINE_EVENT_TYPE( PU_OPTION )
DEFINE_EVENT_TYPE( PU_ABOUT )

DEFINE_EVENT_TYPE( PU_EXIT )

DEFINE_EVENT_TYPE( PU_SPEED_UNLIMIT )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT5M )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT2M )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT1M )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT700K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT500K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT400K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT300K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT200K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT100K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT50K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT20K )
DEFINE_EVENT_TYPE( PU_SPEED_LIMIT10K )


BEGIN_EVENT_TABLE( CSysTray, wxTaskBarIcon )
EVT_MENU( PU_RESTORE, CSysTray::OnMenuRestore )
EVT_MENU( PU_EXIT, CSysTray::OnMenuExit )
EVT_MENU( PU_SHOWDROPWIN, CSysTray::OnShowDropWin )
EVT_MENU( PU_MONITORCLIP, CSysTray::OnMonitorClip )
EVT_MENU( PU_STARTALL, CSysTray::OnStartAll )
EVT_MENU( PU_STOPALL, CSysTray::OnStopAll )
EVT_MENU( PU_ABOUT, CSysTray::OnAbout )
EVT_MENU( PU_NEWTASK, CSysTray::OnNewTask )
EVT_MENU( PU_OPTION, CSysTray::OnOption )

EVT_MENU( PU_SPEED_UNLIMIT, CSysTray::OnUnLimit )
EVT_MENU( PU_SPEED_LIMIT5M, CSysTray::OnLimit5M )
EVT_MENU( PU_SPEED_LIMIT2M, CSysTray::OnLimit2M )
EVT_MENU( PU_SPEED_LIMIT1M, CSysTray::OnLimit1M )
EVT_MENU( PU_SPEED_LIMIT700K, CSysTray::OnLimit700K )
EVT_MENU( PU_SPEED_LIMIT500K, CSysTray::OnLimit500K )
EVT_MENU( PU_SPEED_LIMIT400K, CSysTray::OnLimit400K )
EVT_MENU( PU_SPEED_LIMIT300K, CSysTray::OnLimit300K )
EVT_MENU( PU_SPEED_LIMIT200K, CSysTray::OnLimit200K )
EVT_MENU( PU_SPEED_LIMIT100K, CSysTray::OnLimit100K )
EVT_MENU( PU_SPEED_LIMIT50K, CSysTray::OnLimit50K )
EVT_MENU( PU_SPEED_LIMIT20K, CSysTray::OnLimit20K )
EVT_MENU( PU_SPEED_LIMIT10K, CSysTray::OnLimit10K )
EVT_TASKBAR_LEFT_DOWN ( CSysTray::OnLeftButtonClick )
EVT_TASKBAR_RIGHT_DOWN( CSysTray::OnRightButtonClick )
END_EVENT_TABLE()

extern _SPEED_MODE gSpeedMode;
extern bool gbShowDropWin, gbMonitorClip;

CSysTray::CSysTray( MainFrame* pmain )
{
    m_pMainwin = pmain;
    m_bDlgOnMain = false;
}

void CSysTray::DlgShow( bool sh )
{
    m_bDlgOnMain = sh;
}

void CSysTray::OnMenuRestore( wxCommandEvent& )
{
    //	m_pMainwin->Show(!m_pMainwin->IsShown());

    if ( m_pMainwin->IsShown() )
    {
        if ( m_pMainwin->IsIconized() )
        { //maximize it
            //DBGOUT("maximize");
            wxMaximizeEvent max( 2 );
            m_pMainwin->AddPendingEvent( max );
        }
        else
        { //hide it
            wxIconizeEvent min( 2, true );
            m_pMainwin->AddPendingEvent( min );
        }

        //show=false;
    }
    else
    {
        //DBGOUT("maximize");
        wxMaximizeEvent max( 2 );
        m_pMainwin->AddPendingEvent( max );
        //show=true;
    }


}

void CSysTray::OnMenuExit( wxCommandEvent& )
{
    //wxWindowDestroyEvent event;
    //m_pMainwin->AddPendingEvent(event);
    m_pMainwin->SysTrayExitNotice();
    return ;

}

wxMenu* CSysTray::CreatePopupMenu()
{
    wxMenu * menu = new wxMenu;
    wxMenuItem *ti;

    ti =
        new wxMenuItem( menu, PU_RESTORE, _("Show/Hide MultiGet"), _( "" ) );
    ti->SetBitmap( wxBitmap( sysicon_win_xpm ) );

    menu->Append( ti );

    menu->AppendSeparator();

    menu->AppendCheckItem( PU_SHOWDROPWIN, _("Show Drop Window") );
    menu->AppendCheckItem( PU_MONITORCLIP, _("Monitor Clipboard") );
    menu->AppendSeparator();
    menu->Append( PU_STARTALL, _("Start All") );
    menu->Append( PU_STOPALL, _("Stop All") );
    menu->AppendSeparator();

    ti =
        new wxMenuItem( menu, PU_NEWTASK, _("New task"), _( "" ) );
    ti->SetBitmap( wxBitmap( new_xpm ) );
    menu->Append( ti );

    //	menu->Append(PU_NEWTASK, _MGSTR(_S_NEW_TASK));


    menu->AppendSeparator();
    //speed mode popup
    //menu->Append(PU_SPEEDMODE, _MGSTR(_S_SPEEDMODE));
    wxMenu *speed = new wxMenu;
    speed->AppendRadioItem( PU_SPEED_UNLIMIT, _("No Limit"), _( "" ) );
    speed->AppendSeparator();
    speed->AppendRadioItem( PU_SPEED_LIMIT5M, _( "5MB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT2M, _( "2MB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT1M, _( "1MB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT700K, _( "700KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT500K, _( "500KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT400K, _( "400KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT300K, _( "300KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT200K, _( "200KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT100K, _( "100KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT50K, _( "50KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT20K, _( "20KB/s" ), _( "" ) );
    speed->AppendRadioItem( PU_SPEED_LIMIT10K, _( "10KB/s" ), _( "" ) );

    menu->Append( PU_SPEEDMODE, _("Speed Mode"), speed );

    menu->AppendSeparator();
    ti =
        new wxMenuItem( menu, PU_OPTION, _("Option(&P)"), _( "" ) );
    ti->SetBitmap( wxBitmap( option_xpm ) );
    menu->Append( ti );
    //menu->Append(PU_OPTION, _MGSTR(_S_MENU_OPTION));



    ti =
        new wxMenuItem( menu, PU_ABOUT, _("About\tCtrl+O"), _( "" ) );
    ti->SetBitmap( wxBitmap( about_xpm ) );
    menu->Append( ti );

    menu->AppendSeparator();

    ti =
        new wxMenuItem( menu, PU_EXIT, _("Quit\tCtrl+Q"), _( "" ) );
    ti->SetBitmap( wxBitmap( quit_xpm ) );
    menu->Append( ti );


    //menu->Enable(PU_SHOWDRAGWIN,false);
    menu->Check( PU_SHOWDROPWIN, gbShowDropWin );
    menu->Check( PU_MONITORCLIP, gbMonitorClip );

    //check speedmode

    switch ( gSpeedMode )
    {

        case _SPEED_UNLIMIT:
        speed->Check( PU_SPEED_UNLIMIT, true );
        break;

        case _SPEED_5M:
        speed->Check( PU_SPEED_LIMIT5M, true );
        break;

        case _SPEED_2M:
        speed->Check( PU_SPEED_LIMIT2M, true );
        break;

        case _SPEED_1M:
        speed->Check( PU_SPEED_LIMIT1M, true );
        break;

        case _SPEED_700K:
        speed->Check( PU_SPEED_LIMIT700K, true );
        break;

        case _SPEED_500K:
        speed->Check( PU_SPEED_LIMIT500K, true );
        break;

        case _SPEED_400K:
        speed->Check( PU_SPEED_LIMIT400K, true );
        break;

        case _SPEED_300K:
        speed->Check( PU_SPEED_LIMIT300K, true );
        break;

        case _SPEED_200K:
        speed->Check( PU_SPEED_LIMIT200K, true );
        break;

        case _SPEED_100K:
        speed->Check( PU_SPEED_LIMIT100K, true );
        break;

        case _SPEED_50K:
        speed->Check( PU_SPEED_LIMIT50K, true );
        break;

        case _SPEED_20K:
        speed->Check( PU_SPEED_LIMIT20K, true );
        break;

        case _SPEED_10K:
        speed->Check( PU_SPEED_LIMIT10K, true );
        break;

        default:
        break;
    }


    return menu;
}

//change to message?
void CSysTray::OnLeftButtonClick( wxTaskBarIconEvent& event )
{

    //   	m_pMainwin->Show(!m_pMainwin->IsShown());

    //static bool show=true;

    if ( m_pMainwin->IsShown() )
    {
        if ( m_pMainwin->IsIconized() )
        { //maximize it
            //DBGOUT( "maximize" ); 
            wxMaximizeEvent max( 2 );
            m_pMainwin->AddPendingEvent( max );
        }
        else
        { //hide it
            wxIconizeEvent min( 2, true );
            m_pMainwin->AddPendingEvent( min );
        }

        //show=false;
    }
    else
    {
        //DBGOUT( "maximize" );
        wxMaximizeEvent max( 2 );
        m_pMainwin->AddPendingEvent( max );
        //show=true;
    }

    //event.Skip();
}

//change to message?
void CSysTray::OnRightButtonClick( wxTaskBarIconEvent& event )
{
    if ( m_bDlgOnMain )
    {
        m_pMainwin->Show( !m_pMainwin->IsShown() );
        return ;
    }

    event.Skip();
}

void CSysTray::OnShowDropWin( wxCommandEvent& )
{
    m_pMainwin->SwitchShowDropWin();

}

void CSysTray::OnMonitorClip( wxCommandEvent& )
{
    if ( m_pMainwin->IsMonitorClip() )
        m_pMainwin->MonitorClip( false );
    else
        m_pMainwin->MonitorClip( true );
}

void CSysTray::OnStartAll( wxCommandEvent& )
{
    m_pMainwin->StartAllTask();
}

void CSysTray::OnStopAll( wxCommandEvent& )
{
    m_pMainwin->StopAllTask();
}

void CSysTray::OnAbout( wxCommandEvent& event )
{
    m_pMainwin->OnAbout( event );
}


void CSysTray::OnNewTask( wxCommandEvent& event )
{
    m_pMainwin->OnNewTask( event );
}

void CSysTray::OnOption( wxCommandEvent& event )
{
    m_pMainwin->OnConfig( event );
}

void CSysTray::OnUnLimit( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_UNLIMIT;
}

void CSysTray::OnLimit5M( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_5M;
}

void CSysTray::OnLimit2M( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_2M;
}

void CSysTray::OnLimit1M( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_1M;
}

void CSysTray::OnLimit700K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_700K;
}

void CSysTray::OnLimit500K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_500K;
}

void CSysTray::OnLimit400K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_400K;
}

void CSysTray::OnLimit300K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_300K;
}

void CSysTray::OnLimit200K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_200K;
}

void CSysTray::OnLimit100K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_100K;
}

void CSysTray::OnLimit50K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_50K;
}

void CSysTray::OnLimit20K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_20K;
}

void CSysTray::OnLimit10K( wxCommandEvent& event )
{
    gSpeedMode = _SPEED_10K;
}

