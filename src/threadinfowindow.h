
/*
        filename	:	threadinfowindow.hh
        create date	:	2006/07/12
        author		:	liubin
        email		:	multiget@gmail.com
        version		:	0.3
 
		***修订历史***
 
*/

#ifndef _THREADINFO_WINDOW_H
#define _THREADINFO_WINDOW_H

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/listctrl.h>
#include <string>

#include "common.h"

class CThreadInfoWindow : public wxListView
{

public:
    CThreadInfoWindow( wxWindow* parent );
    void AddInfo( _MSGTYPE infotype, wxString info );
    void AddInfo( _MSGTYPE infotype, std::string info );
    void AddInfo( _MSGTYPE infotype, wxString info , int ntime );
    void AddInfo( _MSGTYPE infotype, std::string info , int ntime );

protected:
    void OnSize( wxEvent& event );
//    void OnErase( wxEraseEvent& event );

private:
    DECLARE_EVENT_TABLE()
};

#endif

