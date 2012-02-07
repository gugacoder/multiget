/***************************************************************************
*  hidewin.cpp
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

#include "hidewin.h"
#include "mainframe.h"
#include "mgurlparser.h"
#include "mgapp.h"
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <wx/listctrl.h>
#include <iostream>

using namespace std;

DEFINE_EVENT_TYPE( mgCLIP_TIMER )

BEGIN_EVENT_TABLE( CHideWin, wxFrame )
EVT_TIMER( mgCLIP_TIMER, CHideWin::OnTimer )
END_EVENT_TABLE()

extern bool gbIgnoreExt;
extern bool gbMonitorExt;
extern std::string gsMonitorExt;
extern std::string gsIgnoreExt;

CHideWin::CHideWin( MainFrame *parent )
        : wxFrame( NULL, wxID_ANY, _( "" ) )
{
    m_LastClip = _( "" );
    m_bMonitor = true;
    m_pParent = parent;

#ifndef WIN32	
    //pipe
    int mypipe[ 2 ];

    if ( pipe ( mypipe ) )
    {
		return; //pipe fail
    }
#endif
    m_Timer.SetOwner( this, mgCLIP_TIMER );
    m_Timer.Start( 800 );


}

void CHideWin::OnTimer( wxTimerEvent& event )
{

    if ( !m_bMonitor )
    {
        event.Skip();
        return ;
    }

    //clipboard
    if ( wxTheClipboard->Open() )
    {
        if ( wxTheClipboard->IsSupported( wxDF_TEXT ) )
        {
            wxTextDataObject text;

            if ( wxTheClipboard->GetData( text ) )
            {

                wxTheClipboard->Close();

                wxString sUrl = text.GetText();

                if ( sUrl == m_LastClip || sUrl.IsNull() )
                {
                    return ;
                }

                if ( m_pParent->IsUrlExist( string( sUrl.mb_str( wxConvLocal ) ) ) )
                {
                    return ;
                }

                //filter extention?
                if ( IgnoreExtention( sUrl ) )
                {
                    return ;
                }

                //monitor extention?
                if ( !MonitorExtention( sUrl ) )
                {
                    return ;
                }

                //no check anymore
                m_LastClip = sUrl;

                wxCommandEvent event( mgEVT_URL_NOTICE, 2 );

                event.SetString( sUrl );

                m_pParent->AddPendingEvent( event );

            }
            else
            {
                wxTheClipboard->Close();
            }
        }
        else
        {

            wxTheClipboard->Close();
        }

    }
    else
    {
        return; //open clip fail
    }

    //event.Skip();
}

bool CHideWin::IgnoreExtention( wxString url )
{
    if ( !gbIgnoreExt )
        return false;

    if ( gsIgnoreExt.empty() )
        return false;

    //gsIgnoreExt;
    string strset = ",; ";

    size_t start = gsIgnoreExt.find_first_of( strset, 0 );

    if ( start == string::npos )
    { //only one ext
        return HaveExt( url, wxString( gsIgnoreExt.c_str(), wxConvLocal ) );
    }
    else
    {
        string s = gsIgnoreExt.substr( 0, start + 1 );

        if ( HaveExt( url, wxString( s.c_str(), wxConvLocal ) ) )
            return true;

        size_t end;

        while ( ( end = gsIgnoreExt.find_first_of( strset, start + 1 ) ) != string::npos )
        {

            s = gsIgnoreExt.substr( start, end - start + 1 );

            if ( HaveExt( url, wxString( s.c_str(), wxConvLocal ) ) )
                return true;

            start = end;
        }

        //the last extention

        s = gsIgnoreExt.substr( start + 1, gsIgnoreExt.length() - start );

        if ( HaveExt( url, wxString( s.c_str(), wxConvLocal ) ) )
            return true;
    }

    return false;
}

bool CHideWin::MonitorExtention( wxString url )
{
    if ( !gbMonitorExt )
        return true;

    //gsMonitorExt;
    if ( gsMonitorExt.empty() )
        return false;

    string strset = ",; ";

    size_t start = gsMonitorExt.find_first_of( strset, 0 );

    if ( start == string::npos )
    { //only one ext
        return HaveExt( url, wxString( gsMonitorExt.c_str(), wxConvLocal ) );
    }
    else
    {
        string s = gsMonitorExt.substr( 0, start + 1 );

        if ( HaveExt( url, wxString( s.c_str(), wxConvLocal ) ) )
            return true;

        size_t end;

        while ( ( end = gsMonitorExt.find_first_of( strset, start + 1 ) ) != string::npos )
        {
            s = gsMonitorExt.substr( start, end - start + 1 );

            if ( HaveExt( url, wxString( s.c_str(), wxConvLocal ) ) )
                return true;

            start = end;
        }

        //the last extention

        s = gsMonitorExt.substr( start + 1, gsMonitorExt.length() - start );

        if ( HaveExt( url, wxString( s.c_str(), wxConvLocal ) ) )
            return true;
    }

    return false;

}

bool CHideWin::HaveExt( wxString str, wxString ext )
{
    ext.Trim( false );
    ext.Trim( true );

    if ( ext.IsEmpty() )
        return false;

    if ( str.Length() < ext.Length() )
    {
        return false;
    }

    return str.Right( ext.Length() ) == ext;

}

