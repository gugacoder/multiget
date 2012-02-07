/***************************************************************************
*            mirroradmin.cpp
*
*  Sat Oct  7 22:46:10 2006
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

#include "mirroradmin.h"
#include "common.h" 
//这个类不支持多线程
//这里算法还有错，需要大改，暂时就这样吧。
CMirrorAdmin::CMirrorAdmin()
{
    m_nGroupId = 0;
}

CMirrorAdmin::~CMirrorAdmin()
{}

void CMirrorAdmin::OpenGroup( bool filemirror )
{
    m_nGroupId++;
    m_bFileMirror = filemirror;
}

//这个算法还太简单了，以后改
void CMirrorAdmin::AddElement( std::string url )
{
    Trim( url );
    //搜索所有的地址，如果有相同的，则修改group
    std::vector<_mu>::const_iterator it;

    for ( it = m_MirrorList.begin();it != m_MirrorList.end();it++ )
    {
        if ( url == it->url )
        {
            m_nGroupId = it->key;
            return ;
        }
    }

    _mu temp;
    temp.key = m_nGroupId;
    temp.filemirror = m_bFileMirror;
    temp.url = url;
    m_MirrorList.push_back( temp );
}

void CMirrorAdmin::CloseGroup()
{
}

//url 和返回的ar都设定为文件
int CMirrorAdmin::GetFileMirror( std::string fileurl, int num, std::string ar[] )
{
    //先找文件镜像,两次检索
    int group = -1;
    std::vector<_mu>::const_iterator it;

    for ( it = m_MirrorList.begin();it != m_MirrorList.end();it++ )
    {
        if ( it->url == fileurl )
        {
            group = it->key;
            break;
        }
    }

    if ( group == -1 )
    {
        ar[ 0 ] = fileurl;
        return 1;
    }

    int count = 0;

    for ( it = m_MirrorList.begin();it != m_MirrorList.end();it++ )
    {
        if ( it->key == group && it->filemirror )
        {
            ar[ count ] = it->url;
            count++;

            if ( count >= num )
                break;
        }
    }

    return count;

}

