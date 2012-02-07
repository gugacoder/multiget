/***************************************************************************
*  dropurl.cpp
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
2006/08/05
*/

#include "dropurl.h"
#include "mainframe.h"
#include "floatwin.h"


using namespace std;
CDropUrl::CDropUrl( CFloatWin* parent )
{
    m_pParent = parent;

#ifdef WIN32
	SetDataObject(new wxURLDataObject);
#else
    wxDataFormat df;
    df.SetId(wxT("text/plain"));
    SetDataObject( new wxCustomDataObject( df ) );
#endif    //SetDataObject(new wxURLDataObject);
}

bool CDropUrl::OnDrop( wxCoord x, wxCoord y )
{//yes ,have event
    return true;
}

//call after ondrop return true;
wxDragResult CDropUrl::OnData( wxCoord x, wxCoord y, wxDragResult def )
{
	
    if ( !GetData() )
    {
		//fail here in windows	
        return wxDragNone;
    }
#ifdef WIN32
	OnDropURL(((wxURLDataObject *)m_dataObject)->GetURL());
	return def;
	
#else
    char* pdata = ( char* ) ( ( wxCustomDataObject* ) m_dataObject ) ->GetData();

    int len = ( ( wxCustomDataObject* ) m_dataObject ) ->GetSize();

    if ( len > 510 )
        return wxDragNone;

    char us[ 512 ];

    memcpy( us, pdata, len );

    us[ len ] = 0;

    wxString url = wxString( us, wxConvLocal );

    OnDropURL( url );

    return def;
#endif
}

wxDragResult CDropUrl::OnDragOver( wxCoord x, wxCoord y, wxDragResult def )
{
    return wxDragLink;
}

void CDropUrl::OnDropURL( const wxString& url )
{
    m_pParent->OnDropURL( url );
}

