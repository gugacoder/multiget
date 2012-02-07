/***************************************************************************
*            listenpipe.cpp
*
*  Sat Oct 21 16:28:18 2006
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


#include <stdlib.h>
#include <stdio.h>
#include "listenpipe.h"
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
#include "mainframe.h"
CListenPipe::CListenPipe( MainFrame* mp )
        : wxThread( wxTHREAD_DETACHED )
{
    m_pParent = mp;
}

void* CListenPipe::Entry()
{

    char buf[ 1024 ];
    int nret;

    std::string tpipe;
    GetTempDir( tpipe );
    tpipe += "/cmdline.pipe";

again:

    FILE *fp = fopen( tpipe.c_str(), "r" );

    nret = fscanf ( fp, "%s", buf );

    if ( nret > 0 )
    {

        wxCommandEvent event( mgEVT_CMD_NOTICE, 1 );
        event.SetString( wxString( buf, wxConvLocal ) );
        m_pParent->AddPendingEvent( event );
        fclose ( fp );
        goto again;
    }
    else
    {
        //printf("pipe error");
        fclose( fp );
#ifdef WIN32
		Sleep(3000);
#else
        sleep( 3 );
#endif
        goto again;
    }

    return NULL;
}

