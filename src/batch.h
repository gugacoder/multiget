/***************************************************************************
*            batch.h
*
*  Fri Oct 20 13:40:48 2006
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

#ifndef _BATCH_H
#define _BATCH_H

#include <string>
#include <vector>

class CBatch
{

public:
    CBatch( std::string str );
    virtual ~CBatch();
    int GetNum();
    std::string Get( unsigned int seq );

protected:
    bool IsDigital( std::string str );
    bool IsChar( std::string str );
    std::vector<std::string> m_List;
};

#endif

