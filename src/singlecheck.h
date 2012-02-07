/***************************************************************************
*            singlecheck.h
*
*  Sat Oct 21 11:45:58 2006
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

#ifndef _SINGLE_CHECK_H
#define _SINGLE_CHECK_H

class CSingleCheck
{

public:
    CSingleCheck();
    ~CSingleCheck();
    bool IsOldExists();
    long GetOldPid();
    void Clean();
    bool MakePipe();

protected:
    long m_pid;
    bool m_old;
};

#endif

