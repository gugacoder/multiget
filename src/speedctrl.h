
/***************************************************************************
 *  speedctrl.h
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

#ifndef _SPEED_CTRL_H
#define _SPEED_CTRL_H
#include "common.h"
#include <pthread.h>
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

#include <list>

class CSpeedCtrl
{

    struct _ss
    {
        llong bytes;  //data bytes

        struct timeval tti;  //time
    };

public:
    CSpeedCtrl();
    ~CSpeedCtrl();
    int DataIn( int len );
    void Init();

protected:

    pthread_mutex_t m_mutex;
    //	struct timeval last, thistime;
#ifndef WIN32
    struct timezone m_tz;
#endif
    //	int m_packet;

    std::list<_ss> m_list;
};

#endif

