/***************************************************************************
*            batch.cpp
*
*  Fri Oct 20 13:40:14 2006
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

#include "batch.h"
#include "common.h"
#include <iostream>

CBatch::CBatch( std::string str )
{
    /*
    [a-z]=a,b,c ...z //not support
    [0-9]=0,1,..9
    [00-99]=00,01,02...99
    [1-999]=1,2,3...999	
    [000-999]=000,001,002...999
    [A-Z]=A,B,C...Z  //not support
    */

    //find if there have '-' and it's in []
    std::string strset = "-";
    unsigned int mid = str.find_last_of( strset );

    if ( mid == std::string::npos )
    {
        m_List.push_back( str );
        return ;
    }


    strset = "]";
    unsigned int right = str.find_last_of( strset );

    if ( right == std::string::npos )
    {
        m_List.push_back( str );
        return ;
    }



    strset = "[";
    unsigned int left = str.find_last_of( strset );

    if ( left == std::string::npos )
    {
        m_List.push_back( str );
        return ;
    }


    if ( left > mid || right < mid )
    {
        m_List.push_back( str );
        return ;
    }

    //looks ok
    std::string leftstr; //[xxx-yyy]=>xxx

    std::string rightstr; //[xxx-yyy]=>yyy

    std::string headstr; //http://xxxxx[a-b]yyy=>http://xxxx

    std::string tailstr; //http://xxxxx[a-b]yyy=>yyy

    leftstr = str.substr( left + 1, mid - left - 1 );

    rightstr = str.substr( mid + 1, right - mid - 1 );

    headstr = str.substr( 0, left );

    tailstr = str.substr( right + 1, str.length() - right + 1 - 1 ); //???

    if ( leftstr.length() > 3 || rightstr.length() > 3 )
    { //too long string
        m_List.push_back( str );
        return ;
    }

    Trim( leftstr );
    Trim( rightstr );

    if ( leftstr.empty() || rightstr.empty() )
    {
        m_List.push_back( str );
        return ;
    }

    //confirm all char is digit, else return
    if ( !IsDigital( leftstr ) || !IsDigital( rightstr ) )
    {
        m_List.push_back( str );
        return ;
    }

    bool ext = false;
    int digit = 0;

    if ( ( leftstr[ 0 ] == '0' || rightstr[ 0 ] == '0' ) && leftstr.length() == rightstr.length() )
    {
        digit = leftstr.length();
        ext = true;
    }

    //convert to digit
    int less = atoi( leftstr.c_str() );

    int more = atoi( rightstr.c_str() );

    if ( less > more )
    {
        int s = less;
        less = more;
        more = s;
    }


    if ( ext )
    {
        //digit
        char buf[ 10 ];

        for ( int i = less;i <= more;i++ )
        {
            switch ( digit )
            {

                case 2:
                sprintf( buf, "%02d", i );
                break;

                case 3:
                sprintf( buf, "%03d", i );
                break;

                default:
                sprintf( buf, "%d", i );
                break;
            }

            std::string temp = headstr + std::string( buf ) + tailstr;
            m_List.push_back( temp );
        }
    }
    else
    {

        char buf[ 10 ];

        for ( int i = less;i <= more;i++ )
        {
            sprintf( buf, "%d", i );
            std::string temp = headstr + std::string( buf ) + tailstr;
            m_List.push_back( temp );
        }
    }
}

bool CBatch::IsDigital( std::string str )
{
    return true;
}

bool CBatch::IsChar( std::string str )
{
    return false;
}

CBatch::~CBatch()
{}


int CBatch::GetNum()
{
    return m_List.size();
}

std::string CBatch::Get( unsigned int seq )
{
    if ( seq < m_List.size() && seq >= 0 )
        return m_List[ seq ];
    else
        return std::string( "" );
}

