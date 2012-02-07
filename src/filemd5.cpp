/***************************************************************************
*  filemd5.cpp
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

#include "filemd5.h"
#include <stdlib.h>
#include <stdio.h>
#include "MD5.h"
#include <iostream>

CFileMd5::CFileMd5( _TaskAttr& task )
        : wxThread( wxTHREAD_DETACHED )
{
    m_task = &task;
}

void* CFileMd5::Entry()
{
    FILE * fp;
    fp = fopen( ( m_task->sSavePath + std::string( "/" ) + m_task->sName ).c_str(), "rb" );

    if ( fp == NULL )
    {
        m_task->sMD5 = std::string( "open file error" );
        return NULL;
    }

    m_task->sMD5 = std::string( "caculating..." );

    llong lSize;
    //get size
#ifdef WIN32
    fseek( fp, 0, SEEK_END );
#else
    fseeko( fp, 0, SEEK_END );
#endif
    lSize = ftell( fp );
#ifdef WIN32	
    fseek( fp, 0, SEEK_SET );
#else
    fseeko( fp, 0, SEEK_SET );
#endif
#define BSZ 1024

    char buf[ BSZ ];

    CMD5 cal;

    llong blocks = lSize / BSZ;

    for ( llong i = 0;i < blocks;i++ )
    {
        fread( buf, BSZ, 1, fp );
        cal.AddData( ( unsigned char* ) buf, BSZ );
    }

    if ( lSize % BSZ )
    {
        fread( buf, lSize % BSZ, 1, fp );
        cal.AddData( ( unsigned char* ) buf, lSize % BSZ );
    }

    fclose( fp );

#undef BSZ

    cal.EndData();

    m_task->sMD5 = cal.GetDigestStr();

    return NULL;
}

