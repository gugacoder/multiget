/***************************************************************************
*            singlecheck.cpp
*
*  Sat Oct 21 11:45:48 2006
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

#include "singlecheck.h"
#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#ifdef WIN32
#else
#include <sys/wait.h>
#endif
#include <unistd.h>

#include <sys/stat.h>
#include <iostream>

CSingleCheck::CSingleCheck()
{

    m_pid = 0;
    m_old = false;


    FILE *fp;
    std::string temp;
    GetTempDir( temp );
    temp += "/single.lock";
    fp = fopen( temp.c_str(), "r" );

    if ( fp == NULL )
    {
        fp = fopen( temp.c_str(), "w" );

        if ( fp == NULL )
            return ; //some error

        fprintf( fp, "%ld", long( getpid() ) );

        fclose( fp );

        //make pipe
        if ( !MakePipe() )
        {
            DBGOUT( "error to make fifo" );
        }

    }
    else
    { //old exists,check if prog exists
        //read pid and check pid;
        long pid = 0;
        fscanf( fp, "%ld", &pid );
        char buf[ 20 ];
        sprintf( buf, "/proc/%ld", pid );
        FILE *fp2;
        fp2 = fopen( buf, "r" );

        if ( fp2 == NULL )
        { //prog quit;
            fclose( fp );

            Clean();

            fp = fopen( temp.c_str(), "w" );


            if ( fp == NULL )
                return ; //some error

            fprintf( fp, "%ld", ( long ) getpid() );

            fclose( fp );

            if ( !MakePipe() )
            {
                DBGOUT( "error to make fifo" );
            }
        }
        else
        { //prog exists
            fclose( fp2 );
            fclose( fp );
            m_old = true;
            m_pid = pid;
        }
    }
}

CSingleCheck::~CSingleCheck()
{
    Clean();
}

bool CSingleCheck::IsOldExists()
{
    return m_old;
}

long CSingleCheck::GetOldPid()
{
    return m_pid;
}

void CSingleCheck::Clean()
{
    //delete lock file
    std::string temp;
    GetTempDir( temp );

    std::string lock = temp + std::string( "/single.lock" )

                           ;
    std::string pipe = temp + std::string( "/cmdline.pipe" );

    remove
        ( lock.c_str() );

    remove
        ( pipe.c_str() );
}

bool CSingleCheck::MakePipe()
{
#ifndef WIN32
    std::string temp;
    GetTempDir( temp );
    std::string pipe = temp + std::string( "/cmdline.pipe" );
    return !mkfifo( pipe.c_str(), 0600 );
#else
	return false;
#endif
}

