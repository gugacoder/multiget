/***************************************************************************
*            mgurlparser.cpp
*
*  Tue Sep 26 16:17:23 2006
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


#include "mgurlparser.h"
#include "common.h"
#include <iostream>
extern std::string gDefFtpPass;

using namespace std;
///%CF%D6%B9%DB%D7%AF%D1%CF%C2%DBCD/1-2a.mp3

#define XDIGIT_TO_XCHAR(x) (((x) < 10) ? ((x) + '0') : ((x) - 10 + 'A'))
#define ISXDIGIT(x) ( ((x) >= '0' && (x) <= '9')||\
					  ((x) >= 'a' && (x) <= 'z')||\
					  ((x) >= 'A' && (x) <= 'Z') )


#define XCHAR_TO_XDIGIT(x) 	(((x) >= '0' && (x) <= '9') ? \
							((x) - '0') : (toupper(x) - 'A' + 10))

enum {
    urlchr_reserved = 1,   // rfc1738 reserved chars
    urlchr_unsafe = 2  // rfc1738 unsafe chars
};

/* Shorthands for the table: */
#define R  1 // reserved char
#define U  2 // unsafe char
#define RU 3 // R|U

const static unsigned char urlchr_table[ 256 ] =
    {
        U, U, U, U, U, U, U, U,     /* NUL SOH STX ETX  EOT ENQ ACK BEL */
        U, U, U, U, U, U, U, U,     /* BS  HT  LF  VT   FF  CR  SO  SI  */
        U, U, U, U, U, U, U, U,     /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
        U, U, U, U, U, U, U, U,     /* CAN EM  SUB ESC  FS  GS  RS  US  */
        U, 0, U, RU, R, U, R, 0,     /* SP  !   "   #    $   %   &   '   */
        0, 0, 0, R, R, 0, 0, R,     /* (   )   *   +    ,   -   .   /   */
        0, 0, 0, 0, 0, 0, 0, 0,     /* 0   1   2   3    4   5   6   7   */
        0, 0, RU, R, U, R, U, R,     /* 8   9   :   ;    <   =   >   ?   */
        RU, 0, 0, 0, 0, 0, 0, 0,     /* @   A   B   C    D   E   F   G   */
        0, 0, 0, 0, 0, 0, 0, 0,     /* H   I   J   K    L   M   N   O   */
        0, 0, 0, 0, 0, 0, 0, 0,     /* P   Q   R   S    T   U   V   W   */
        0, 0, 0, RU, U, RU, U, 0,     /* X   Y   Z   [    \   ]   ^   _   */
        U, 0, 0, 0, 0, 0, 0, 0,     /* `   a   b   c    d   e   f   g   */
        0, 0, 0, 0, 0, 0, 0, 0,     /* h   i   j   k    l   m   n   o   */
        0, 0, 0, 0, 0, 0, 0, 0,     /* p   q   r   s    t   u   v   w   */
        0, 0, 0, U, U, U, RU, U,     /* x   y   z   {    |   }   ~   DEL */

        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,

        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
        U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U,
    };

#define urlchr_test(c, mask) (urlchr_table[(unsigned char)(c)] & (mask))
#define URL_RESERVED_CHAR(c) urlchr_test(c, urlchr_reserved)
#define URL_UNSAFE_CHAR(c) urlchr_test(c, urlchr_unsafe)


CUrlParser::CUrlParser()
{
    m_bValidMirrorUrl = false;
}

//返回false是不支持的协议或错误的地址格式
bool CUrlParser::SetUrl( std::string url )
{

    Trim( url );

    m_bValidMirrorUrl = false;
    //协议检查
    m_protocol = UrlType( url );

    if ( m_protocol == UNKNOW_PROTOCOL )
    {
        return false;
    }

    if ( m_protocol == FTP_PROTOCOL )
    { //ftp

        //提取用户和密码

        if ( !GetUserAndPass( url, m_user, m_pass ) )
        {
            return false;
        }

        //服务器和端口
        if ( !GetServerAndPort( url, m_server, m_port ) )
        {
            return false;
        }

        if ( !GetRawUrl( url, m_raw ) )
        {
            return false;
        }

        //文件汉路径
        if ( !GetPathFile( url, m_file ) )
        {
            return false;
        }

        if ( m_file[ m_file.length() - 1 ] == '/' )
        { //ftp dir not a valid file url
            m_bValidMirrorUrl = true;
            return false;
        }

        //编码文件

        //Precode(m_file,m_escfile);
        m_escfile = m_file;

        //DBGOUT("m_file="<<m_file);
        UnEscape( m_escfile ); //m_escfile反而存放非转换串

        //DBGOUT("m_escfile="<<m_escfile);
    }
    else if ( m_protocol == HTTP_PROTOCOL )
    { //http

        //提取用户和密码

        if ( !GetUserAndPass( url, m_user, m_pass ) )
        {
            return false;
        }

        //服务器和端口
        if ( !GetServerAndPort( url, m_server, m_port ) )
        {
            return false;
        }

        if ( !GetRawUrl( url, m_raw ) )
        {
            return false;
        }

        //文件汉路径
        if ( !GetPathFile( url, m_file ) )
        {
            return false;
        }


        //编码文件
        Precode( m_file, m_escfile );

        if ( !GetRefer( m_raw, m_refer ) )
        {
            return false;
        }

    }
    else
    {
        return false;
    }

    return true;
}

string CUrlParser::GetUser()
{
    return m_user;
}

string CUrlParser::GetPass()
{
    return m_pass;
}

string CUrlParser::GetServer()
{
    return m_server;
}

string CUrlParser::GetFilePathName()
{
    return m_file;
}

std::string CUrlParser::GetFileName()
{
    //only file name without path

    string::size_type pos;

    pos = m_file.find_last_of( '/' ); //for linux only

    if ( pos == std::string::npos )
    {
		std::string uns=m_file;
		UnEscape(uns);
		return uns;
	}
    else
    {
        std::string uns= m_file.substr( pos + 1, m_file.length() - pos - 1 );
		UnEscape(uns);
		return uns;
    }

    /*
       char fn[ 512 ];
       strcpy( fn, m_file.c_str() );
       int i = strlen( fn );

       while ( fn[ i ] != '/' && i > 0 )
           i--;

       return string( fn + i + 1 );
    */

}

//only http/https need refer
std::string CUrlParser::GetRefer()
{
    return m_refer;
}

int CUrlParser::GetPort()
{
    return m_port;
}

//目前就支持两种协议，HTTP,FTP
_PROTYPE CUrlParser::UrlType( std::string url )
{
    if ( url.length() < 7 )
        return UNKNOW_PROTOCOL;  //太短了！

#ifdef WIN32

    if ( strnicmp( url.c_str(), "ftp://", 6 ) == 0 )
        return FTP_PROTOCOL;

    if ( strnicmp( url.c_str(), "http://", 7 ) == 0 )
        return HTTP_PROTOCOL;

#else

    if ( strncasecmp( url.c_str(), "ftp://", 6 ) == 0 )
        return FTP_PROTOCOL;

    if ( strncasecmp( url.c_str(), "http://", 7 ) == 0 )
        return HTTP_PROTOCOL;

#endif

    return UNKNOW_PROTOCOL;

}

_PROTYPE CUrlParser::GetUrlType()
{
    return m_protocol;
}


std::string CUrlParser::GetEscFilePathName()
{
    return m_escfile;
}


//新的提取用户和密码的函数
bool CUrlParser::GetUserAndPass( const std::string& fullurl, std::string& user, std::string& pass )
{
    //从后向前寻找＠，如果有则服务器从＠后开始，到/结束
    //如果没有，则从ftp://开始

    //是否太长而无法处理？

    if ( fullurl.length() > 510 )
        return false;

    //考到url
    char url[ 512 ];

    strcpy( url, fullurl.c_str() );

    //p是移动的指针
    char * p = url;

    p += strlen( url );

    //look for @
    //while ( *p != '@' && *p != 0 )
    //    p++;

findat:
    while ( *p != '@' && p != url )
        p--;

	//根据提交的BUG，有时后面会有这个@字符，添加检查过滤无效的@
	if ( *p == '@' )
	{
		//检查是否前方的'/'位置是否是第2个'/'
		char *q=p;
		while ( *q != '/' && q > url )
            q--;

		if( q == url ) return false; //其实不会出现这个情况
		char *m = url; //从前向后找第二个'/'，应该等于q
		while ( *m != '/' ) m++;
		if( q != m + 1 ) { p--; goto findat; }
	}

    if ( *p == '@' )
    { //get @
        char *e = p - 1;
        char *p = e;

        while ( *p != '/' && p > url )
            p--;

        if ( p == url )
        {
            return false; //没找到前面的斜杠，一个不合法的URL
        }
        else
        {
            char *s = p + 1;
            //assert( e > s );
            //int len=e-s+1;
            //search ':'

            while ( *p != ':' && p < e )
                p++;

            if ( p == e )
            {
                return false; //无冒号分割用户名和密码，一个不合法的URL
            }

            if ( 30 < p - s + 1 )  //用户名超过30，有点长了
            {
                return false;
            }

            char temp[ 31 ];
            memcpy( temp, s, p - s );
            temp[ p - s ] = 0;
            user = std::string( temp );

            if ( 30 < e - p + 1 )  //密码超过30，有点长了
            {
                return false;
            }

            memcpy( temp, p + 1, e - p );
            temp[ e - p ] = 0;
            pass = std::string( temp );
            return true;
        }
    }
    else
    {
        user = std::string( "anonymous" );
        pass = gDefFtpPass;
        return true;
    }

    return false;
}

bool CUrlParser::GetServerAndPort( const std::string& fullurl, std::string& server, int& port )
{
    //从后向前寻找＠，如果有则服务器从＠后开始，到/结束
    //如果没有，则从ftp://开始

    //是否太长而无法处理？

    if ( fullurl.length() > 510 )
        return false;

    //考到url
    char url[ 512 ];

    strcpy( url, fullurl.c_str() );

    //p是移动的指针
    char * p = url;

    p += strlen( url );

    //look for @
    //while ( *p != '@' && *p != 0 )
    //    p++;

findat:
    while ( *p != '@' && p != url )
        p--;

	//根据提交的BUG，有时后面会有这个@字符，添加检查过滤无效的@
	if ( *p == '@' )
	{
		//检查是否前方的'/'位置是否是第2个'/'
		char *q=p;
		while ( *q != '/' && q > url )
            q--;

		if( q == url ) return false; //其实不会出现这个情况
		char *m = url; //从前向后找第二个'/'，应该等于q
		while ( *m != '/' ) m++;
		if( q != m + 1 ) { p--; goto findat; }
	}

    if ( *p == '@' )
    {
        char * s = p + 1;

        while ( *p != '/' && *p != 0 )
            p++;

        if ( *p == '/' )
        {
            int len = p - s;	//包含端口在内的长度
            //检查是否包含了端口在内
            char *ck = p - 1;

            while ( *ck != ':' && ck > s )
                ck--;

            if ( ck == s )
            { //无端口

                if ( 256 < len + 1 )
                    return false;  //server string too long

                char temp[ 256 ];

                memcpy( temp, s, len );

                temp[ len ] = 0;

                server = std::string( temp );

                //按协议类型给出缺省的端口，以后可括从
                port = GetDefaultPort( m_protocol );

                return true;

            }
            else
            {
                //有端口
                int slen = ck - s;
                int plen = len - slen - 1;

                if ( 256 < slen )
                    return false; //server too long

                char temp[ 256 ];

                memcpy( temp, s, slen );

                temp[ slen ] = 0;

                server = std::string( temp );

                if ( plen > 5 )
                    return false; //max port is 65535

                char prt[ 6 ];

                memcpy( prt, ck + 1, plen );

                prt[ plen ] = 0;

                //检查是否都为数字？
                for ( int i = 0;i < plen;i++ )
                {
                    if ( prt[ i ] < '0' || prt[ i ] > '9' )
                        return false; //wrong port
                }

                port = atoi( prt );

                return true;
            }

        }
        else
        { //url wrong format
            return false;
        }
    }
    else //到了结尾没有@
    {
        p = url;

        while ( *p != '/' && *p != 0 )
            p++;

        p++;

        while ( *p != '/' && *p != 0 )
            p++;

        p++;

        char *s = p;

        while ( *p != '/' && *p != 0 )
            p++;

        if ( *p == '/' )
        {
            int len = p - s;	//包含端口在内的长度
            //检查是否包含了端口在内
            char *ck = p - 1;

            while ( *ck != ':' && ck > s )
                ck--;

            if ( ck == s )
            { //无端口

                if ( 256 < len + 1 )
                    return false; //server too long

                char temp[ 256 ];

                memcpy( temp, s, len );

                temp[ len ] = 0;

                server = std::string( temp );

                //按协议类型给出缺省的端口，以后可括从
                port = GetDefaultPort( m_protocol );

                return true;

            }
            else
            {
                //有端口
                int slen = ck - s;
                int plen = len - slen - 1;

                if ( 256 < slen )
                    return false;

                char temp[ 256 ];

                memcpy( temp, s, slen );

                temp[ slen ] = 0;

                server = std::string( temp );

                if ( plen > 5 )
                    return false; //max port is 65535

                char prt[ 6 ];

                memcpy( prt, ck + 1, plen );

                prt[ plen ] = 0;

                //检查是否都为数字？
                for ( int i = 0;i < plen;i++ )
                {
                    if ( prt[ i ] < '0' || prt[ i ] > '9' )
                        return false; //wrong port
                }

                port = atoi( prt );

                return true;
            }
        }
        else
        {
            return false;
        }
    }
}


int CUrlParser::GetDefaultPort( _PROTYPE protocol )
{
    //按协议类型给出缺省的端口，以后可括从

    switch ( protocol )
    {

        case FTP_PROTOCOL:
        return 21;

        case HTTP_PROTOCOL:
        return 80;

        default:
        return 0;
    }

}

//去掉用户和密码的url
bool CUrlParser::GetRawUrl( std::string& fullurl, std::string& rawurl )
{
    //ftp://user:pa@ss@www.abc.com/fdfas.txt,去掉//到@之间的内容。

    char url[ 512 ];
    strcpy( url, fullurl.c_str() );

    char* p = url;

    while ( *p != '/' && *p != 0 )
        p++;

    if ( *p == 0 )
        return false;

    p++;

    while ( *p != '/' && *p != 0 )
        p++;

    if ( *p == 0 )
        return false;

    p++;

    //这里p指向'u'
    char* h = p;

    p = url + strlen( url );

findat:
    while ( *p != '@' && p != url )
        p--;

	//根据提交的BUG，有时后面会有这个@字符，添加检查过滤无效的@
	if ( *p == '@' )
	{
		//检查是否前方的'/'位置是否是第2个'/'
		char *q=p;
		while ( *q != '/' && q > url )
            q--;

		if( q == url ) return false; //其实不会出现这个情况
		char *m = url; //从前向后找第二个'/'，应该等于q
		while ( *m != '/' ) m++;
		if( q != m + 1 ) { p--; goto findat; }
	}

    if ( p == url )
    {
        rawurl = fullurl;

    }
    else
    {
        p++;
        char temp[ 512 ];
        memcpy( temp, url, h - url );
        strcpy( &temp[ h - url ], p );
        rawurl = std::string( temp );
    }

    return true;
}

//从url中获得路径和文件的数据
//不检查是否为目录，也不进行编码
bool CUrlParser::GetPathFile( std::string& fullurl, std::string& pathfile )
{
    //ftp://dfasdfas.com/dfad/dfs ＝》/dfad/dfs
    //找第三个'/'

    char url[ 512 ];
    strcpy( url, fullurl.c_str() );

    char *p = url;

    while ( *p != '/' && *p != 0 )
        p++;

    if ( *p == 0 )
        return false;

    p++;

    while ( *p != '/' && *p != 0 )
        p++;

    if ( *p == 0 )
        return false;

    p++;

    while ( *p != '/' && *p != 0 )
        p++;

    if ( *p == 0 )
        return false;

    if ( 510 < int( strlen( url ) - ( p - url ) ) )
    {
        return false; //too long
    }

    char temp[ 512 ];
    memcpy( temp, p, strlen( url ) - ( p - url ) );
    temp[ strlen( url ) - ( p - url ) ] = 0;
    pathfile = std::string( temp );
    return true;
}

//提取refer
bool CUrlParser::GetRefer( std::string rawurl, std::string& refer )
{

    if ( !m_refer.empty() )
        return true; //already have set refer

    //找最后一个'/',之前的就算refer

    char url[ 512 ];

    strcpy( url, rawurl.c_str() );

    char* p = url + strlen( url );

    while ( p != url && *p != '/' )
        p--;

    if ( p == url )
    {
        refer.clear();
    }
    else
    {
        if ( 512 < p - url )
            return false;

        char temp[ 512 ];

        memcpy( temp, url, p - url );

        temp[ p - url ] = 0;

        refer = std::string( temp );

    }

    return true;
}

void CUrlParser::Precode( const std::string& pathfile, std::string& escfile )
{
    char * str_new;
    int str_new_len;
    char *ptr, *pptr;

    // get the length of the new string
    ptr = ( char* ) pathfile.c_str();
    str_new_len = 0;

    while ( *ptr != '\0' )
    {
        if ( *ptr == '%' )
        {
            if ( isxdigit( ptr[ 1 ] ) && isxdigit( ptr[ 2 ] ) )
            {
                str_new_len += 3;
                ptr += 3;
            }
            else
            {
                str_new_len += 3;
                ptr += 1;
            }
        }
        else if ( URL_UNSAFE_CHAR( *ptr ) && !URL_RESERVED_CHAR( *ptr ) )
        {
            str_new_len += 3;
            ptr += 1;
        }
        else
        {
            str_new_len += 1;
            ptr += 1;
        }
    } // end of while

    /* encode the url */
    str_new = new char[ str_new_len + 1 ];

    ptr = ( char* ) pathfile.c_str();

    pptr = str_new;

    while ( *ptr != '\0' )
    {
        if ( *ptr == '%' )
        {
            if ( isxdigit( ptr[ 1 ] ) && isxdigit( ptr[ 2 ] ) )
            {
                strncpy( pptr, ptr, 3 );
                ptr += 3;
                pptr += 3;
            }
            else
            {
                strncpy( pptr, "%25", 3 );
                ptr += 1;
                pptr += 3;
            }
        }
        else if ( URL_UNSAFE_CHAR( *ptr ) && !URL_RESERVED_CHAR( *ptr ) )
        {
            pptr[ 0 ] = '%';
            pptr[ 1 ] = XDIGIT_TO_XCHAR( ( ( unsigned char ) * ptr ) >> 4 );
            pptr[ 2 ] = XDIGIT_TO_XCHAR( ( ( unsigned char ) * ptr ) & 0x0f );
            ptr += 1;
            pptr += 3;
        }
        else
        {
            *pptr = *ptr;
            ptr += 1;
            pptr += 1;
        }
    } // end of while

    *pptr = '\0';

    escfile = std::string( str_new );

    delete[] str_new;

}

std::string CUrlParser::GetRawUrl()
{
    return m_raw;
}

//把user,pass添加到url中
bool CUrlParser::RebuildUrl(
    std::string& fullurl, std::string user, std::string pass, std::string& rebuild )
{
    std::string raw;

    if ( user.empty() || pass.empty() )
        return false;

    if ( !GetRawUrl( fullurl, raw ) )
        return false;

    string::size_type pos = raw.find( "//", 0 );

    if ( pos == string::npos )
        return false;

    raw.insert( pos + 2, user + ":" + pass + "@" );

    rebuild = raw;

    return true;
}

void CUrlParser::SetRefer( std::string& refer )
{
    m_refer = refer;
}

bool CUrlParser::IsValidMirror()
{
    return m_bValidMirrorUrl;
}

void CUrlParser::UnEscape( std::string& str )
{
    char buf[ 512 ];
    strcpy( buf, str.c_str() );
    UnEscape( buf );
    str = std::string( buf );
}

void CUrlParser::UnEscape ( char *s )
{
    char * t = s;
    char *h = s;

    for ( ; *h; h++, t++ )
    {
        if ( *h != '%' )
        {
            *t = *h;
        }
        else
        {
            unsigned char c;

            if ( !h[ 1 ] || !h[ 2 ] || !( ISXDIGIT ( h[ 1 ] ) && ISXDIGIT ( h[ 2 ] ) ) )
            {
                *t = *h;
                continue;
            }

            //c = X2DIGITS_TO_NUM (h[1], h[2]);
            //			printf("h[1]=%c,h[2]=%c\n",h[1],h[2]);

            unsigned char k, m;

            k = XCHAR_TO_XDIGIT( h[ 1 ] );

            m = XCHAR_TO_XDIGIT( h[ 2 ] );

            c = k * 16 + m;

            //	  		if (c == 0)
            //	    	{
            //DBGOUT("c==0");
            //				*t=*h;
            //				continue;
            //			}

            *t = c;

            h += 2;
        }
    }

    *t = '\0';
}
