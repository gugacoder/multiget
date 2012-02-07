/***************************************************************************
*  floatwin.h
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
#ifndef _FLOAT_WIN_H
#define _FLOAT_WIN_H

#ifdef WIN32
#include <wx/minifram.h>
#endif
#include <wx/dialog.h>
#include <wx/frame.h>
#include "common.h"
#include <string>
#include <vector>

class MainFrame;

class CDropUrl;

#ifdef WIN32
class CFloatWin : public wxMiniFrame
#else
class CFloatWin : public wxDialog
#endif
{

public:
    CFloatWin( MainFrame* parent );
    virtual ~CFloatWin();
    void OnDropURL( wxString url );

protected:
    void OnMouseMove( wxMouseEvent& evt );
    void OnPaint( wxPaintEvent& event );
    void OnLeftDown( wxMouseEvent& evt );
    void OnLeftUp( wxMouseEvent& WXUNUSED( evt ) );

private:
    wxPoint m_delta;
    MainFrame *m_pParent;
    CDropUrl	*m_DropUrl;

    DECLARE_EVENT_TABLE()
};

#endif

