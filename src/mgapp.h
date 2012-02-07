/***************************************************************************
*  mgapp.h
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

/*
2006/07/12
*/
#ifndef _MGAPP_H
#define _MGAPP_H

#include <wx/wx.h>
#include <wx/msgdlg.h> 
//Disable mgstring, we use GNU Gettext for i18n.
//#include "mgstring.h"
#include <vector>
#include <list>
#include <string>

#ifdef WIN32
#include <windows.h>
#endif

#include "singlecheck.h"
#include <wx/intl.h>

class MainFrame;

class MgApp : public wxApp
{

protected:
    // Called on application startup
    virtual bool OnInit();

public:
//    void SetLang( _MGLANG lang );
//    _MGLANG GetLang();
//    std::string GetStr( _MGSTRID id );
//    wxString GetWxStr( _MGSTRID id );

protected:
    virtual void OnFatalException();
    virtual int OnExit();
    void OnEndSession( wxCloseEvent& event );
    void CheckTempDir();
//    CMgString m_MultiString;
    FILE* m_pipe;
    MainFrame *m_frame;
    wxLocale m_locale; // locale we'll be using

    CSingleCheck *m_Checker;
    DECLARE_EVENT_TABLE()
};


DECLARE_APP( MgApp )

#endif

