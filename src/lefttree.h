/***************************************************************************
*  lefttree.h
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

#ifndef _LEFT_TREE_H
#define _LEFT_TREE_H

#include <wx/wx.h>
#include <wx/treectrl.h>

class CLeftTree : public wxTreeCtrl
{

public:
    CLeftTree( wxWindow* parent );
    virtual ~CLeftTree()
    { }

    void DynamicLang();

protected:
    void OnSelectChanged( wxTreeEvent& event );
    //void OnErase( wxEraseEvent& event );
    //void OnSize( wxSizeEvent& event );


    wxTreeItemId m_rid ;
    wxTreeItemId m_dl ;
    wxTreeItemId m_pu ;
    wxTreeItemId m_er ;
    wxTreeItemId m_fi ;
    wxTreeItemId m_de ; //

    wxTreeItemId m_fi_soft;
    wxTreeItemId m_fi_packet;
    wxTreeItemId m_fi_music;
    wxTreeItemId m_fi_image;
    wxTreeItemId m_fi_movie;
    wxTreeItemId m_fi_publish;
    wxTreeItemId m_fi_picture;
    wxTreeItemId m_fi_unknow;

private:
    DECLARE_EVENT_TABLE()
};

#endif

