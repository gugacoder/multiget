/***************************************************************************
*  speedctrl.cpp
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

#include "speedctrl.h"
#include "common.h"

#include <iostream>

extern _SPEED_MODE gSpeedMode;

CSpeedCtrl::CSpeedCtrl()
{
    pthread_mutex_init( &m_mutex, NULL );
    Init();
}

CSpeedCtrl::~CSpeedCtrl()
{
}

void CSpeedCtrl::Init()
{
#ifndef WIN32	
    _ss np;
    np.bytes = 0;
    gettimeofday( &np.tti, &m_tz );
    m_list.push_back( np );
#endif
    //m_packet=0;

}

int CSpeedCtrl::DataIn( int len )
{
    if ( !gSpeedMode )
        return 0;

    pthread_mutex_lock( &m_mutex );

    //	m_packet++;

    _ss np;

    np.bytes = m_list.back().bytes + len;

#ifndef WIN32	
    gettimeofday( &np.tti, &m_tz );
#endif
    m_list.push_back( np );

    if ( m_list.size() > 100 )
        m_list.pop_front();

    //	if(!(m_packet%2))
    {
        //compute on list

        struct timeval t1 = m_list.back().tti;

        struct timeval t2 = m_list.front().tti;
        int tt = ( t1.tv_sec - t2.tv_sec ) * 1000 * 1000 + ( t1.tv_usec - t2.tv_usec );
        int bytes = ( m_list.back().bytes - m_list.front().bytes );
        //float sp = (m_list.back().bytes - m_list.front().bytes)/tt; // bytes/s

        int proputime;

        switch ( gSpeedMode )
        {

            case _SPEED_5M:  //5M
            proputime = int( float( bytes ) / 5 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_2M:  //2M
            proputime = int( float( bytes ) / 2 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_1M:  //1M
            proputime = int( float( bytes ) / 1 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_700K:
            proputime = int( float( bytes ) / 0.7 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_500K:  //500K
            proputime = int( float( bytes ) / 0.5 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_400K:
            proputime = int( float( bytes ) / 0.4 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_300K:
            proputime = int( float( bytes ) / 0.3 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_200K:  //200K
            proputime = int( float( bytes ) / 0.2 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_100K:  //100K
            proputime = int( float( bytes ) / 0.1 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_50K:  //50K
            proputime = int( float( bytes ) / 0.05 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_20K:  //20K
            proputime = int( float( bytes ) / 0.02 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            case _SPEED_10K:  //10K
            proputime = int( float( bytes ) / 0.01 );

            if ( proputime > tt )
            {
                pthread_mutex_unlock( &m_mutex );
                return int( proputime -tt );
            }

            break;

            default:
            break;
        }

    } //if(!(m_packet%5))

    pthread_mutex_unlock( &m_mutex );

    return 0;
}

