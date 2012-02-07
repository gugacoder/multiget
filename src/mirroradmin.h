/***************************************************************************
*            mirroradmin.h
*
*  Sat Oct  7 22:46:00 2006
*  Copyright  2006  liubin
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

#ifndef _MIRROR_ADMIN_H
#define _MIRROR_ADMIN_H

#include <vector>
#include <string>

class CMirrorAdmin
{

public:
    CMirrorAdmin();
    virtual ~CMirrorAdmin();
    void OpenGroup( bool filemirror );
    void CloseGroup(); //可以不用调这个
    void AddElement( std::string url );
    int GetFileMirror( std::string url, int num, std::string ar[] ); //一定是取一个文件的镜像
    //

protected:

    struct _mu
    {
        int key;  //group
        bool filemirror;  //only filemirror,not dir mirror
        std::string url;
    };

    std::vector<_mu> m_MirrorList;

    int m_nGroupId;
    bool m_bFileMirror;


};

#endif

