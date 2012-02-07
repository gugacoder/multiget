/***************************************************************************
*  lefttreeitemdata.h
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
#ifndef _LEFTTREE_ITEMDATA_H
#define _LEFTTREE_ITEMDATA_H

#include <wx/wx.h>
#include <wx/treectrl.h>
#include "common.h"

class CLeftTreeItemData : public wxTreeItemData
{

public:
    void SetData( _TASK_TYPE ndata );
    _TASK_TYPE GetData();
    _TASK_TYPE m_nData;
};


#endif

