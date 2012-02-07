/***************************************************************************
*  mghttpbase.cpp
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
/*
	   filename	:	httpbase.cpp
       create	:	2006/09/03
       author	:	liubin
       EMAIL	:	multiget@gmail.com
       version  :	0.2
	   license	:	GPL		
	   
	   ******modify history*********
	   2006/08/19
	   		add cookie code
	   2006/09/03
	   		make CMgSocket as the base class ,so it will get sock proxy func.
*/

/*
error code:
 
-100 : app connection not established
 
*/
#ifdef WIN32
#include <winsock2.h>
#endif

#include "mghttpbase.h"
#include <iostream>

using namespace std;

CMgHttpBase::CMgHttpBase()
{}

CMgHttpBase::~CMgHttpBase()
{}

// if use http proxy ,have diff send way
//sfile begin with '/'
bool CMgHttpBase::SendHttpHeadRequest(
    const char* shost, const char* sfile, const char* srefer )
{

    char outbuf[ 2048 ] = {0};
    char buf[ 512 ] = {0};

    if ( !m_bUseHttpProxy )
    {
        sprintf( buf, "HEAD %s HTTP/1.1\r\n", sfile );
        strcpy( outbuf, buf );
        sprintf( buf, "HEAD %s HTTP/1.1", sfile );
        OutMsg( buf, MSG_OUT );
    }
    else
    {
        sprintf( buf, "HEAD HTTP://%s%s HTTP/1.1\r\n", shost, sfile );
        strcpy( outbuf, buf );
        sprintf( buf, "HEAD HTTP://%s%s HTTP/1.1", shost, sfile );
        OutMsg( buf, MSG_OUT );
    }

    strcat( outbuf, "User-Agent: Mozilla/4.0(compatible;MSIE 5.00;Windows 98)\r\n" );
    OutMsg( "User-Agent: Mozilla/4.0(compatible;MSIE 5.00;Windows 98)", MSG_OUT );

    strcat( outbuf, "Accept: */*\r\n" );
    OutMsg( "Accept: */*", MSG_OUT );

    sprintf( buf, "Host: %s\r\n", shost );
    strcat( outbuf, buf );

    sprintf( buf, "Host: %s", shost );
    OutMsg( buf, MSG_OUT );

    sprintf( buf, "Referer: %s\r\n", srefer );
    strcat( outbuf, buf );

    sprintf( buf, "Referer: %s", srefer );
    OutMsg( buf, MSG_OUT );

    //cookie addin

    for ( int i = 0; i < int( m_AutoCookie.size() ); i++ )
    {
        sprintf( buf, "Cookie: %s\r\n", m_AutoCookie[ i ].c_str() );
        strcat( outbuf, buf );
        sprintf( buf, "Cookie: %s", m_AutoCookie[ i ].c_str() );
        OutMsg( buf, MSG_OUT );
    }

    strcat( outbuf, "Pragma: no-cache\r\n" );
    OutMsg( "Pragma: no-cache", MSG_OUT );

    strcat( outbuf, "Cache-Control:no-cache\r\n" );
    OutMsg( "Cache-Control:no-cache", MSG_OUT );

    strcat( outbuf, "Connection: close\r\n\r\n" );
    OutMsg( "Connection: close", MSG_OUT );

    return Send( outbuf, strlen( outbuf ) );

}

//sfile begin with '/'
bool CMgHttpBase::SendHttpGetRequest(
    const char* shost, const char* sfile, llong offset, const char* srefer )
{

    char outbuf[ 2048 ] = {0};
    char buf[ 512 ] = {0};

    if ( !m_bUseHttpProxy )
    {
        sprintf( buf, "GET %s HTTP/1.1\r\n", sfile );
        strcpy( outbuf, buf );
        sprintf( buf, "GET %s HTTP/1.1", sfile );
        OutMsg( buf, MSG_OUT );
    }
    else
    {
        sprintf( buf, "GET HTTP://%s%s HTTP/1.1\r\n", shost, sfile );
        strcpy( outbuf, buf );
        sprintf( buf, "GET HTTP://%s%s HTTP/1.1", shost, sfile );
        OutMsg( buf, MSG_OUT );
    }

    strcat( outbuf, "User-Agent: Mozilla/4.0(compatible;MSIE 5.00;Windows 98)\r\n" );
    OutMsg( "User-Agent: Mozilla/4.0(compatible;MSIE 5.00;Windows 98)", MSG_OUT );

    strcat( outbuf, "Accept: */*\r\n" );
    OutMsg( "Accept: */*", MSG_OUT );

    sprintf( buf, "Host: %s\r\n", shost );
    strcat( outbuf, buf );
    sprintf( buf, "Host: %s", shost );
    OutMsg( buf, MSG_OUT );

    sprintf( buf, "Referer: %s\r\n", srefer );
    strcat( outbuf, buf );

    sprintf( buf, "Referer: %s", srefer );
    OutMsg( buf, MSG_OUT );

    //cookie addin

    for ( int i = 0; i < int( m_AutoCookie.size() ); i++ )
    {
        sprintf( buf, "Cookie: %s\r\n", m_AutoCookie[ i ].c_str() );
        strcat( outbuf, buf );
        sprintf( buf, "Cookie: %s", m_AutoCookie[ i ].c_str() );
        OutMsg( buf, MSG_OUT );
    }

    strcat( outbuf, "Connection: close\r\n" );
    OutMsg( "Connection: close", MSG_OUT );

    sprintf( buf, "Range: bytes=%lld-\r\n\r\n", offset );
    strcat( outbuf, buf );
    sprintf( buf, "Range: bytes=%lld-", offset );
    OutMsg( buf, MSG_OUT );

    return Send( outbuf, strlen( outbuf ) );

}

//sfile begin with '/'
bool CMgHttpBase::SendHttpGetRequest(
    const char* shost, const char* sfile, const char* srefer )
{

    char outbuf[ 2048 ] = {0};
    char buf[ 512 ] = {0};

    if ( !m_bUseHttpProxy )
    {
        sprintf( buf, "GET %s HTTP/1.1\r\n", sfile );
        strcpy( outbuf, buf );
        sprintf( buf, "GET %s HTTP/1.1", sfile );
        OutMsg( buf, MSG_OUT );
    }
    else
    {
        sprintf( buf, "GET HTTP://%s%s HTTP/1.1\r\n", shost, sfile );
        strcpy( outbuf, buf );
        sprintf( buf, "GET HTTP://%s%s HTTP/1.1", shost, sfile );
        OutMsg( buf, MSG_OUT );
    }

    strcat( outbuf, "User-Agent: Mozilla/4.0(compatible;MSIE 5.00;Windows 98)\r\n" );
    OutMsg( "User-Agent: Mozilla/4.0(compatible;MSIE 5.00;Windows 98)", MSG_OUT );

    strcat( outbuf, "Accept: */*\r\n" );
    OutMsg( "Accept: */*", MSG_OUT );

    sprintf( buf, "Host: %s\r\n", shost );
    strcat( outbuf, buf );
    sprintf( buf, "Host: %s", shost );
    OutMsg( buf, MSG_OUT );

    sprintf( buf, "Referer: %s\r\n", srefer );
    strcat( outbuf, buf );

    sprintf( buf, "Referer: %s", srefer );
    OutMsg( buf, MSG_OUT );

    //cookie addin

    for ( int i = 0; i < int( m_AutoCookie.size() ); i++ )
    {
        sprintf( buf, "Cookie: %s\r\n", m_AutoCookie[ i ].c_str() );
        strcat( outbuf, buf );
        sprintf( buf, "Cookie: %s", m_AutoCookie[ i ].c_str() );
        OutMsg( buf, MSG_OUT );
    }

    strcat( outbuf, "Connection: close\r\n\r\n" );
    OutMsg( "Connection: close", MSG_OUT );

    return Send( outbuf, strlen( outbuf ) );

}


/*
从缓存中提取一个命令行放在line中，返回的是行的长度，line有128字节空间
如果缓冲中没有行结束就返回-1
*/
int CMgHttpBase::GetBufLine( char *buf, int maxpos, char *line )
{

#define MAXLINE 256

    for ( int i = 0;i <= maxpos - 2;i++ )
    {
        if ( buf[ i ] == '\r' && buf[ i + 1 ] == '\n' )
        {
            memcpy( line, buf, i > MAXLINE - 3 ? MAXLINE - 2 : i );
            line[ i > MAXLINE - 2 ? MAXLINE - 1 : i ] = 0;
            return i + 2;
        }
    }

    return -1;

#undef MAXLINE

}



void CMgHttpBase::AddCookie( string cookie )
{
    m_AutoCookie.push_back( cookie );
}

