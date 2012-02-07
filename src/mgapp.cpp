/***************************************************************************
*  mgapp.cpp
*
*  Wed Sep  6 22:19:52 2006
*  Copyright  2006  liubin,China
*  Email multiget@gamil.com
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

#include "mgapp.h"
#include <pthread.h>
#include "mainframe.h"
#include "common.h"
#include <wx/dialog.h> 
#include <wx/dir.h>
#include <wx/wx.h>
#include <locale.h>
//#include <iostream>

//using namespace std;
// Give wxWidgets the means to create a MgApp object
IMPLEMENT_APP( MgApp )
// Initialize the application


extern std::vector<_syslog> gSysLog;
extern std::vector<_tasklog> gTaskLog;
extern pthread_mutex_t gTaskLogMutex;

void gfTaskLog( int ntaskid, std::string info, _MSGTYPE ntype, int ntime )
{

    _tasklog log;
    log.ntaskid = ntaskid;
    log.ntime = ntime;
    log.info = info;
    log.ntype = ntype;
    pthread_mutex_lock( &gTaskLogMutex );
    gTaskLog.push_back( log );
    pthread_mutex_unlock( &gTaskLogMutex );

}

void gfSysLog( std::string info )
{
    _syslog log;
    log.ntime = time( NULL );
    log.info = info;
    gSysLog.push_back( log );
}

BEGIN_EVENT_TABLE( MgApp, wxApp )
EVT_END_SESSION( MgApp::OnEndSession )
END_EVENT_TABLE()

bool MgApp::OnInit()
{
    m_Checker = NULL;
    m_pipe = NULL;

    CheckTempDir();

#ifndef WIN32 //no single check in win32	
    //single check
    //long pid=getpid();
    //check if lock file exists
    m_Checker = new CSingleCheck();

    if ( m_Checker->IsOldExists() )
    {
        //notice popup
        //if have args

        if ( argc <= 1 )
        {
            //tip?
            return false;
        }

        char buf[ 2048 ] = {0};

        for ( int i = 1;i < argc;i++ )
        {
            char tmp[ 512 ];
#ifndef _UNICODE
            sprintf( tmp, "%s^", argv[ i ] );//
#else
			wxString temps = argv[ i ];
			sprintf( tmp, "%s^", temps.mb_str( wxConvLocal).data() );
#endif
            strcat( buf, tmp );
        }

        std::string temp;
        GetTempDir( temp );
        temp += "/cmdline.pipe";

        int fd = open ( temp.c_str(), O_WRONLY );

        write ( fd, buf, strlen( buf ) + 1 );

        close ( fd );

        return false;

    }

#endif //no single check in win32

#ifndef WIN32
    ::wxHandleFatalExceptions( true );
#endif

#ifdef WIN32
    m_MultiString.SetLang( EN_US_UTF_8 );
#else
#ifndef OLD_CODE
	//initialize localization object for applicatoin..
    m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_CONV_ENCODING); 
	//Add localization catalog file lookup directory .
	wxLocale::AddCatalogLookupPathPrefix(wxT(PACKAGE_LOCALE_DIR));
	//set localization catalog file name.
    m_locale.AddCatalog(wxT(GETTEXT_PACKAGE));
#else
    //char* lang = getenv( "LANG" );
    char *lang;
    lang = setlocale (LC_ALL, NULL);

    if ( strcasecmp( lang, "zh_CN.UTF-8" ) == 0 || 
		 strcasecmp( lang, "zh_CN.UTF8" ) == 0 )
    {
        m_MultiString.SetLang( ZH_CN_UTF_8 );
    }

    else if ( strcasecmp( lang, "zh_CN.GB2312" ) == 0 ||
              strcasecmp( lang, "zh_CN.GBK" ) == 0 ||
              strcasecmp( lang, "zh_CN.GB18030" ) == 0 )
    {
        m_MultiString.SetLang( ZH_CN_18030 );
    }
    else if ( strcasecmp( lang, "zh_TW.UTF-8" ) == 0 ||
              strcasecmp( lang, "zh_TW.UTF8" ) == 0 )
    {
        m_MultiString.SetLang( ZH_TW_UTF_8 );
    }
    else if ( strcasecmp( lang, "zh_TW.BIG5" ) == 0 ||
              strcasecmp( lang, "zh_TW" ) == 0 )
    {
        m_MultiString.SetLang( ZH_TW_BIG5 );
    }
    else if ( strcasecmp( lang, "de_DE.UTF8" ) == 0 ||
              strcasecmp( lang, "de_DE" ) == 0 )
    {
        m_MultiString.SetLang( DE_DE_UTF_8 );
    }
	else if ( strcasecmp( lang, "pt_BR.UTF8" ) == 0 ||
			  strcasecmp( lang, "pt_BR" ) == 0 ||
			  strcasecmp( lang, "pt_PT.UTF8" ) == 0||
			  strcasecmp( lang, "pt_PT" ) == 0)
	{
		m_MultiString.SetLang( PT_BR_UTF_8 );
	}
    else
    {
        m_MultiString.SetLang( EN_US_UTF_8 );
    }

#endif

#endif
	
    SetAppName( _("MultiGet file downloader") );

#ifdef __WXGTK__

    SetUseBestVisual( true ); //GTK only

#endif

	
    m_frame = new MainFrame( _("MultiGet file downloader"),m_locale);
	
    assert( m_frame != NULL );

    SetTopWindow( m_frame );

    m_frame->Show( true );

#ifndef WIN32
    //run with args?
    //if have args
    if ( argc > 1 )
    {

        char buf[ 2048 ] = {0};

        for ( int i = 1;i < argc;i++ )
        {
            char tmp[ 512 ];
#ifndef _UNICODE
            sprintf( tmp, "%s^", argv[ i ] );//
#else
			wxString temps = argv[ i ];
			sprintf( tmp, "%s^", temps.mb_str( wxConvLocal ).data() );
#endif
            strcat( buf, tmp );
        }

        std::string temp;
        GetTempDir( temp );
        temp += "/cmdline.pipe";

        int fd = open ( temp.c_str(), O_WRONLY );

        write ( fd, buf, strlen( buf ) + 1 );

        close ( fd );
    }

#endif //no args check in win32
    return true;

}

void MgApp::OnFatalException()
{
    wxMessageBox( _( "Fatal Exception! QUIT!" ) );
}

int MgApp::OnExit()
{
    if ( m_Checker )
        delete m_Checker;

    if ( m_pipe )
        pclose( m_pipe );

    return 0;
}
/*
void MgApp::SetLang( _MGLANG lang )
{
    m_MultiString.SetLang( lang );
}

_MGLANG MgApp::GetLang()
{
    return m_MultiString.GetLang();
}

std::string MgApp::GetStr( _MGSTRID id )
{
    return m_MultiString.GetStr( id );
};

wxString MgApp::GetWxStr( _MGSTRID id )
{
    return m_MultiString.GetWxStr( id );
};
*/
void MgApp::CheckTempDir()
{
    std::string tmpdir;

    if ( !GetTempDir( tmpdir ) )
        return ; //fail
	
    //check if temp dir exists
	if(!wxDir::Exists(wxString(tmpdir.c_str(),wxConvLocal)))
	{//try to create dir
		
#ifdef	WIN32
		CreateDirectory(tmpdir.c_str(),NULL);
#else
        	std::string cmd = std::string( "mkdir " ) + tmpdir;
        	system( cmd.c_str() );
#endif
		
    }

}

void MgApp::OnEndSession( wxCloseEvent& event )
{//seems can't catch this event?
    wxCloseEvent post;
    post.SetCanVeto( false );
    m_frame->AddPendingEvent( post );
}
