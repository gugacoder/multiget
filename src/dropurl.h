/***************************************************************************
*  dropurl.h
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
#ifndef _DROP_URL_H
#define _DROP_URL_H

#include "common.h"
#include <wx/wx.h>
#include <wx/dnd.h>
#include <iostream>

class CFloatWin;

#ifndef __WXX11__

#define wxUSE_DRAG_AND_DROP 1


//class CDropUrl : public wxTextDropTarget
//class CDropUrl : public wxFileDropTarget

class CDropUrl : public wxDropTarget
{

public:
    CDropUrl( CFloatWin* parent );

protected:
    //	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
    //	virtual bool OnDrop(long x, long y, const void *data, size_t size);
    //virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames );
    virtual bool OnDrop( wxCoord x, wxCoord y );
    virtual wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def );
    virtual wxDragResult OnData( wxCoord x, wxCoord y, wxDragResult def );
    void OnDropURL( const wxString& text );

private:
    //wxCustomDataObject dataobj;
    //wxURLDataObject m_urlDataObj;
    CFloatWin* m_pParent;
};


#endif

#endif

