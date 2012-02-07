/***************************************************************************
*  mgftpbase.cpp
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

#ifdef WIN32
#include <winsock2.h>
#endif
#include "mgftpbase.h" 
/*
	the UTF-8 Chinese char will break vc6 compile
	
	this file is created on 2006/09/04 by liubin,China
	
	
*/
CMgFtpBase::CMgFtpBase()
{
    //	m_bBinaryMode=false;
}

CMgFtpBase::~CMgFtpBase()
{}

int CMgFtpBase::GetBufLine( char *buf, int movepos, char *line )
{
#define MAXLINE 256

    for ( int i = 0;i <= movepos - 2;i++ )
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

bool CMgFtpBase::Login( const char *user, const char *pass )
{
    int nret;
    char buffer[ 256 ];

    if ( !m_bUseFtpProxy )
    {
        sprintf( buffer, "USER %s\r\n", user );

        if ( !Send( buffer, strlen( buffer ) ) )
        {
            return false;
        }

        sprintf( buffer, "USER %s", user );
        OutMsg( buffer, MSG_OUT );
    }
    else
    {
        sprintf( buffer, "USER %s@%s:%d\r\n", user, m_Server.c_str(), m_Port );

        if ( !Send( buffer, strlen( buffer ) ) )
        {
            return false;
        }

        sprintf( buffer, "USER %s@%s:%d", user, m_Server.c_str(), m_Port );
        OutMsg( buffer, MSG_OUT );
    }

    nret = GetRetCode( );

    if ( nret >= 500 )
    {
        return false;
    }

    if ( nret == 230 )
    {
        return true; //no pass need
    }

    sprintf( buffer, "PASS %s\r\n", pass );

    if ( !Send( buffer, strlen( buffer ) ) )
    {
        return false;
    }

    sprintf( buffer, "PASS %s", pass );
    OutMsg( buffer, MSG_OUT );

    nret = GetRetCode( );

    if ( nret != 230 )
    {
        return false;
    }

    return true;
}

int CMgFtpBase::GetRetCode()
{

    int nret;
    char buffer[ 1024 ];
    int movepos = 0;
    char line[ 256 ];
    int nlinelen;

nextline:

    while ( ( nlinelen = GetBufLine( buffer, movepos, line ) ) == -1 )
    { // wait line finish

        if ( movepos > 1000 )
        {
            OutMsg( "server send too long msg,fail.", MSG_ERROR );
            return -3; //buffer runs out
        }


        nret = Read( buffer + movepos, 1024 - movepos );

        if ( nret <= 0 )
        {
            return -2; //read data error
        }

        movepos += nret;
    }

    //get a line,show out
    OutMsg( line, MSG_IN );

    if ( movepos > nlinelen )
    {  //multi line
        memmove( buffer, buffer + nlinelen, movepos - nlinelen );
    }

    movepos -= nlinelen;

    if ( strlen( line ) >= 3 )
    { //at least 3 number

        if ( strlen( line ) >= 4 )
        {
            if ( line[ 3 ] == '-' )
            { //info line
                goto nextline;
            }
            else
            { //maybe info,maybe finish line

                line[ 3 ] = 0;

                if ( line[ 0 ] >= '0' && line[ 0 ] <= '9' &&
                        line[ 1 ] >= '0' && line[ 1 ] <= '9' &&
                        line[ 2 ] >= '0' && line[ 2 ] <= '9'
                   )
                {
                    return atoi( line );
                }
                else
                { //invalid line
                    goto nextline;
                }
            }
        }
        else
        {
            line[ 3 ] = 0;
            return atoi( line );
        }
    }
    else
    { //invalid line
        goto nextline;
    }

}

bool CMgFtpBase::SetResume( llong pos )
{
    int nret;
    char buffer[ 30 ];

    sprintf( buffer, "REST %lld\r\n", pos );

    if ( !Send( buffer, strlen( buffer ) ) )
    {
        return false; //write data error
    }

    sprintf( buffer, "REST %lld", pos );
    OutMsg( buffer, MSG_OUT );

    nret = GetRetCode();

    if ( nret == 350 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CMgFtpBase::Logout()
{
    if ( !Send( "QUIT\r\n", 6 ) )
    {
        return false; //write data error
    }

    OutMsg( "QUIT", MSG_OUT );

    int nret = GetRetCode();

    if ( nret == 221 )
        return true;
    else
        return false;
}

bool CMgFtpBase::EnterTextMode()
{
    if ( !Send( "TYPE A\r\n", 8 ) )
    {
        return false; //write data error
    }

    OutMsg( "TYPE A", MSG_OUT );

    int nret = GetRetCode( );

    if ( nret == 200 )
    {
        //m_bBinaryMode=false;
        return true;
    }
    else
        return false;
}

bool CMgFtpBase::EnterBinaryMode( )
{
    //	if (m_bBinaryMode) return true;

    if ( !Send( "TYPE I\r\n", 8 ) )
    {
        return false; //write data error
    }

    OutMsg( "TYPE I", MSG_OUT );

    int nret = GetRetCode( );

    if ( nret == 200 )
    {
        //m_bBinaryMode=true;
        return true;
    }
    else
        return false;
}

bool CMgFtpBase::AbortTransfer()
{
    if ( !Send( "ABOR\r\n", 6 ) )
    {
        return false;
    }

    OutMsg( "ABOR", MSG_OUT );
    int nret = GetRetCode();

    if ( nret == 200 )
    {
        return true;
    }

    return false;
}

//dip have 25 size
bool CMgFtpBase::EnterPasv( char *dip, int *dport )
{
    int nret;
    int i = 0;
    char buffer[ 1024 ];

    if ( !Send( "PASV\r\n", 6 ) )
    {
        return false; //write data error
    }

    OutMsg( "PASV" , MSG_OUT );

    int movepos = 0;
    char line[ 256 ];

    while ( GetBufLine( buffer, movepos, line ) == -1 )
    { //read for line end;

        if ( movepos > 1000 )
        {
            OutMsg( "server send too long msg,fail.", MSG_ERROR );
            return false; //buffer runs out
        }

        if ( ( nret = Read( buffer + movepos, 1024 - movepos ) ) <= 0 )
        {
            return false; //read data error
        }

        movepos += nret;
    }

    OutMsg( line, MSG_IN );

    if ( strlen( line ) < 5 )
    {
        OutMsg( "line shorter than 5,fail.", MSG_ERROR );
        return false;
    }

    //read return code
    char code[ 4 ];

    memcpy( code, line, 3 );

    code[ 3 ] = 0;

    if ( atoi( code ) != 227 )
    {
        OutMsg( "can't enter PASV mode.", MSG_ERROR );
        return false;
    }

    //read pasv args (219,87,250,160,243,239)
    int leftb, rightb = 0;

    char s[ 32 ] = {0};

    i = 4;

    //cout<<"this line="<<line<<endl;

    while ( line[ i ] != '(' && line[ i ] != 0 )
        i++;

    if ( line[ i ] != '(' )
    {
        OutMsg( "invalid server data.", MSG_ERROR );
        return false;
    }
    else
    {
        leftb = i;
    }

    i = strlen( line );

    while ( line[ i ] != ')' && i > 1 )
        i--;

    if ( line[ i ] != ')' )
    {
        OutMsg( "invalid server data.", MSG_ERROR );
        return false;
    }
    else
    {
        rightb = i;
    }

    memcpy( s, &line[ leftb ], rightb - leftb + 1 );
    //cout<<"dinfo="<<s<<endl;

    int a, b, c, d, p1, p2;

    if ( 6 != sscanf( s, "(%d,%d,%d,%d,%d,%d)", &a, &b, &c, &d, &p1, &p2 ) )
    {
        return false;
    }

    sprintf( dip, "%d.%d.%d.%d", a, b, c, d );
    *dport = ( p1 << 8 ) + p2;

    return true;

}

//to get size
bool CMgFtpBase::ListFile( const char* filename, llong& size )
{

    char buffer[ 1024 ];
    char dataip[ 25 ];
    int dataport;

    CMgFtpSocket datasock;

    int movepos = 0;

    //LIST to get file length

    if ( !EnterTextMode() )
    {
        return false;
    }

    if ( !EnterPasv( dataip, &dataport ) )
    {
        return false;
    }

    datasock.SetProxy( *this );
    //datasock.SetFtpProxy(*this);

    if ( ! datasock.Connect( dataip, dataport ) )
    {
        OutMsg( "data connection fail", MSG_ERROR );
        return false;
    }

    if ( !ListFile( filename ) )
    {
        return false;
    }

    if ( GetRetCode( ) != 150 )
    {
        return false;
    }

    movepos = 0;

    while ( 1 )
    {
        int bytes = datasock.Read( buffer + movepos, 1024 - movepos );

        if ( bytes == 0 )
            break;

        if ( bytes < 0 )
            return false;

        if ( bytes > 0 )
        {
            movepos += bytes;
            //end by \r\n

            if ( buffer[ movepos - 1 ] == '\n' && buffer[ movepos - 2 ] == '\r' )
                break;
            else
                continue;
        }
    }

    buffer[ ( movepos - 2 ) >= 0 ? ( movepos - 2 ) : 0 ] = 0; //remove \r\n
    OutMsg( buffer, MSG_INFO );

    datasock.Close();

    if ( GetRetCode( ) != 226 )
    {
        return false;
    }

    if ( strlen( buffer ) < 12 )
    {
        return false;
    }

    return GetLenFromBuf( buffer, size );

}

//only send command
bool CMgFtpBase::ListFile( const char* filename )
{
    char buffer[ 512 ];
    sprintf( buffer, "LIST %s\r\n", filename );

    if ( !Send( buffer, strlen( buffer ) ) )
    {
        return false;
    }

    sprintf( buffer, "LIST %s", filename );
    OutMsg( buffer, MSG_OUT );

    return true;
}

bool CMgFtpBase::SizeFile( const char* filename, llong& size )
{
    char buf[ 1024 ];
    int movepos = 0;
    int nret;

    if ( !EnterBinaryMode() )
    {
        return false;
    }

    sprintf( buf, "SIZE %s\r\n", filename );

    if ( !Send( buf, strlen( buf ) ) )
    {
        return false;
    }

    sprintf( buf, "SIZE %s", filename );
    OutMsg( buf, MSG_OUT );

    movepos = 0;
    char line[ 256 ];

    while ( GetBufLine( buf, movepos, line ) == -1 )
    { //read line end

        if ( movepos > 1020 )
            return false; //buffer runs out

        if ( ( nret = Read( buf + movepos, 1024 - movepos ) ) < 0 )
            return false; //read data error

        movepos += nret;
    }

    OutMsg( line, MSG_INFO );
    //read retcode

	char codes[4];
	memcpy(codes,line,3);
	codes[3]=0;
	nret=atoi(codes);
    //read size
	char lens[60];
	strcpy(lens,&line[4]);
	
#ifdef WIN32	
	size=_atoi64(lens);
#else
	size=atoll(lens);
#endif

	return nret==213;
/*	
    //read size
    llong tsize = -1;
    nret = -1;

    if ( 2 != sscanf( line, "%d %lld", &nret, &tsize ) )
    {
        return false;
    }

    if ( nret == 213 )
    {
        size = tsize;
        return true;
    }
    else
    {
        return false;
    }
	*/
}

bool CMgFtpBase::GetLenFromBuf( const char* listdataline, llong& size )
{
    char s1[ 12 ];
    char s2[ 12 ];
    char s3[ 12 ];
    char s4[ 12 ];

    size = -1;
    int nc = sscanf( listdataline, "%s %s %s %s %lld", s1, s2, s3, s4, &size );

    if ( nc == 5 && size != -1 )
        return true;
    else
        return false;
}

bool CMgFtpBase::MakeCtrlConn( const char *server, int port )
{

    if ( !Connect( server, port ) )
    {
        return false;
    }
    else
    {
        if ( 220 == GetRetCode() )
        {
            m_Server = std::string( server );
            m_Port = port;
            return true;	//connect ok
        }
        else
        {
            OutMsg( "init code wrong,maybe server too load", MSG_ERROR );
            return false;
        }
    }
}

