
/*
        filename	:	threadinfowindow.cpp
        create date	:	2006/07/12
        author		:	liubin
        email		:	multiget@gmail.com
        version		:	0.3
 
		***修订历史***
		2006/08/04 增加按窗口大小变化列的宽度OnSize
*/

#include "threadinfowindow.h"
#include "./icons/up_info.xpm"
#include "./icons/down_info.xpm"
#include "./newicons/16/info.xpm"
#include "./newicons/16/error_info.xpm"
#include "./newicons/16/warnning.xpm"
#include "./newicons/16/success.xpm"
#include <wx/datetime.h>
#include <wx/imaglist.h>
#include <iostream>


using namespace std;

BEGIN_EVENT_TABLE( CThreadInfoWindow, wxListView )
EVT_CUSTOM( wxEVT_SIZE, wxID_ANY, CThreadInfoWindow::OnSize )
//EVT_ERASE_BACKGROUND( CThreadInfoWindow::OnErase )
END_EVENT_TABLE()

void CThreadInfoWindow::OnSize( wxEvent& event )
{
    wxSize vs = GetClientSize();

    if ( vs.x >= 300 )
        SetColumnWidth( 0, vs.x );
    else
        SetColumnWidth( 0, 300 );

    event.Skip();
}

CThreadInfoWindow::CThreadInfoWindow( wxWindow* parent )
        : wxListView( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL/*wxALWAYS_SHOW_SB*/)
{

    wxListItem itemCol;

    itemCol.SetText( _( "Send/Recv message" ) );
    itemCol.SetAlign( wxLIST_FORMAT_LEFT );
    InsertColumn( 0, itemCol );
    SetColumnWidth( 0, 600 );

    wxImageList *imlist = new wxImageList( 16, 16, true, 6 );
    imlist->Add( wxIcon( error_info_xpm ) ); //MSG_ERROR
    imlist->Add( wxIcon( up_info_xpm ) );  //MSG_OUT
    imlist->Add( wxIcon( down_info_xpm ) ); //MSG_IN
    imlist->Add( wxIcon( info_xpm ) );  //MSG_INFO
    imlist->Add( wxIcon( success_xpm ) ); //MSG_SUCCESS
    imlist->Add( wxIcon( warnning_xpm ) );	 //MSG_WARNNING

    AssignImageList( imlist, wxIMAGE_LIST_SMALL );

}

//infotype 0:error 1:send 2:receive 3:info
void CThreadInfoWindow::AddInfo( _MSGTYPE infotype, wxString info )
{

    int ncount = GetItemCount();
    wxListItem it;
    it.SetId( ncount );

    switch ( infotype )
    {

        case MSG_ERROR:
        it.SetImage( 0 );
        break;

        case MSG_OUT:
        it.SetImage( 1 );
        break;

        case MSG_IN:
        it.SetImage( 2 );
        break;

        case MSG_WARNNING:
        it.SetImage( 5 );
        break;

        case MSG_SUCCESS:
        it.SetImage( 4 );
        break;

        default:

        case MSG_INFO:
        it.SetImage( 3 );
        break;

    }

    it.SetText( info );
    InsertItem( it );

}

//infotype 0:error 1:send 2:receive 3:info
void CThreadInfoWindow::AddInfo( _MSGTYPE infotype, wxString info, int ntime )
{

	wxDateTime stm;
	stm.Set((time_t)ntime);
	
#ifdef WIN32	
    wxString wts = stm.Format(_("%H:%M:%S "));
#else
    wxString wts = stm.Format(_("%T "));
#endif

    int ncount = GetItemCount();
    wxListItem it;
    it.SetId( ncount );

    switch ( infotype )
    {

        case MSG_ERROR:
        it.SetImage( 0 );
        break;

        case MSG_OUT:
        it.SetImage( 1 );
        break;

        case MSG_IN:
        it.SetImage( 2 );
        break;

        case MSG_WARNNING:
        it.SetImage( 5 );
        break;

        case MSG_SUCCESS:
        it.SetImage( 4 );
        break;

        default:

        case MSG_INFO:
        it.SetImage( 3 );
        break;

    }

    it.SetText( wts + info );
    InsertItem( it );

}

void CThreadInfoWindow::AddInfo( _MSGTYPE infotype, std::string info )
{

    wxString wxinfo( info.c_str(), wxConvLocal );
    AddInfo( infotype, wxinfo );
}

void CThreadInfoWindow::AddInfo( _MSGTYPE infotype, std::string info, int ntime )
{

	wxDateTime stm;
	stm.Set((time_t)ntime);
	
#ifdef WIN32	
    wxString wts = stm.Format(_("%H:%M:%S "));
#else
    wxString wts = stm.Format(_("%T "));
#endif

    wts += wxString(info.c_str(),wxConvLocal);

    AddInfo( infotype, wts );
}


