/***************************************************************************
*  lefttree.cpp
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


#include "lefttree.h"
#include "mgapp.h" 

#include "./newicons/16/logo_16.xpm"
#include "./newicons/16/run.xpm"
#include "./newicons/16/stop.xpm"
#include "./newicons/16/fail.xpm"
#include "./newicons/16/finish.xpm"
#include "./icons/Software.xpm"
#include "./icons/softpack.xpm"
#include "./icons/music.xpm"
#include "./icons/iso.xpm"
#include "./icons/movie.xpm"
#include "./icons/publish.xpm"
#include "./icons/Image.xpm"
#include "./icons/unknow.xpm"
#include "./icons/Trash.xpm" 
//#include "Trash_Full.xpm"

#include "lefttreeitemdata.h"
#include "mainframe.h"
#include "common.h"
#include <wx/imaglist.h>

BEGIN_EVENT_TABLE( CLeftTree, wxTreeCtrl )
	EVT_TREE_SEL_CHANGED( -1, CLeftTree::OnSelectChanged )
	//EVT_ERASE_BACKGROUND( CLeftTree::OnErase )
	//EVT_SIZE( CLeftTree::OnSize )
END_EVENT_TABLE()

CLeftTree::CLeftTree( wxWindow* parent )
        : wxTreeCtrl( parent, -1, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxFULL_REPAINT_ON_RESIZE  )
{

    //SetBackgroundStyle( wxBG_STYLE_CUSTOM ); //some system is black ground

    wxImageList * leftimlist = new wxImageList( 16, 16, true, 14 );
    leftimlist->Add( wxIcon( logo_16_xpm ) );
    leftimlist->Add( wxIcon( run_xpm ) );
    leftimlist->Add( wxIcon( stop_xpm ) );
    leftimlist->Add( wxIcon( fail_xpm ) );
    leftimlist->Add( wxIcon( finish_xpm ) );

    leftimlist->Add( wxIcon( Software_xpm ) );
    leftimlist->Add( wxIcon( softpack_xpm ) );
    leftimlist->Add( wxIcon( music_xpm ) );
    leftimlist->Add( wxIcon( iso_xpm ) );
    leftimlist->Add( wxIcon( movie_xpm ) );
    leftimlist->Add( wxIcon( publish_xpm ) );
    leftimlist->Add( wxIcon( Image_xpm ) );
    leftimlist->Add( wxIcon( unknow_xpm ) );
    leftimlist->Add( wxIcon( Trash_xpm ) );
    //	leftimlist->Add(wxIcon(Trash_Full_xpm));

    AssignImageList( leftimlist );

    CLeftTreeItemData *idata;
    m_rid = AddRoot( _( "MultiGet" ), 0 );
	SetItemBold ( m_rid, true );
    m_dl = AppendItem( m_rid, _("Running"), 1 );
    m_pu = AppendItem( m_rid, _("Pause"), 2 );
    m_er = AppendItem( m_rid, _("Failed"), 3 );
    m_fi = AppendItem( m_rid, _("Finish"), 4 );
    m_de = AppendItem( m_rid, _("Trash"), 13 ); //


    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_ROOT );
    SetItemData( m_rid, idata );
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_RUNNING );
    SetItemData( m_dl, idata );
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_PAUSE );
    SetItemData( m_pu, idata );
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_WRONG );
    SetItemData( m_er, idata );
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH );
    SetItemData( m_fi, idata );
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_DELETE );
    SetItemData( m_de, idata );



    m_fi_soft = AppendItem( m_fi, _("Software"), 5 ); //exe,bin
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_SOFT );
    SetItemData( m_fi_soft, idata );

    m_fi_packet = AppendItem( m_fi, _("Package"), 6 ); //rar,zip,gzip,tar,bz2
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_PACK );
    SetItemData( m_fi_packet, idata );

    m_fi_music = AppendItem( m_fi, _("Music"), 7 ); //mp3,wav
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_MUSIC );
    SetItemData( m_fi_music, idata );

    m_fi_image = AppendItem( m_fi, _("Images"), 8 ); //iso
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_ISO );
    SetItemData( m_fi_image, idata );

    m_fi_movie = AppendItem( m_fi, _("Movies"), 9 ); //rmvb,avi,mpg
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_MOVIE );
    SetItemData( m_fi_movie, idata );

    m_fi_publish = AppendItem( m_fi, _("Publish"), 10 ); //pdf,chm,ppt,doc,odp
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_PUBLISH );
    SetItemData( m_fi_publish, idata );

    m_fi_picture = AppendItem( m_fi, _("Pictures"), 11 ); //jpg,gif,tiff,png
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_PICTURE );
    SetItemData( m_fi_picture, idata );

    m_fi_unknow = AppendItem( m_fi, _("Unknown"), 12 ); //jpg,gif,tiff,png
    idata = new CLeftTreeItemData;
    idata->SetData( _STATUS_FINISH_UNKNOW );
    SetItemData( m_fi_unknow, idata );

    Expand( m_rid );
    Expand( m_fi );
}


void CLeftTree::OnSelectChanged( wxTreeEvent& event )
{
    wxTreeItemId id = event.GetItem();
    CLeftTreeItemData *pdata = ( CLeftTreeItemData* ) GetItemData( id );
    MainFrame *mf = ( MainFrame * ) ( GetParent() ->GetParent() );
    mf->OnSelectType( pdata->GetData() );
    event.Skip();	//???
}

void CLeftTree::DynamicLang()
{

    SetItemText( m_dl, _("Running") );
    SetItemText( m_pu, _("Pause") );
    SetItemText( m_er, _("Failed") );
    SetItemText( m_fi, _("Finish") );
    SetItemText( m_de, _("Trash") );

    SetItemText( m_fi_soft, _("Software") );
    SetItemText( m_fi_packet, _("Package") );
    SetItemText( m_fi_music, _("Music" ) );
    SetItemText( m_fi_image, _("Images") );
    SetItemText( m_fi_movie, _("Movies" ) );
    SetItemText( m_fi_publish, _("Publish") );
    SetItemText( m_fi_picture, _("Pictures") );
    SetItemText( m_fi_unknow, _("Unknown") );


}

/*
void CLeftTree::OnErase( wxEraseEvent& event )
{
    return ;
}

void CLeftTree::OnSize( wxSizeEvent& event )
{
    return ;
}*/

