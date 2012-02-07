/***************************************************************************
*            mgurlparser.h
*
*  Tue Sep 26 16:17:15 2006
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

#ifndef _URL_PARSER_H
#define _URL_PARSER_H
#include <string>

#include "common.h"



class CUrlParser
{

public:
    CUrlParser();
    std::string GetUser();
    std::string GetPass();
    std::string GetServer();
    int GetPort();
    std::string GetFilePathName();
    std::string GetEscFilePathName();
    std::string GetFileName();
    std::string GetRawUrl();
    bool IsValidMirror();
    _PROTYPE GetUrlType();
    bool SetUrl( std::string fullurl );
    bool RebuildUrl( std::string& fullurl, std::string user, std::string pass, std::string& rebuild );
    std::string GetRefer(); //get default refer
    void SetRefer( std::string& ref ); //set a user refer

private:

    _PROTYPE UrlType( std::string fullurl );
    bool GetUserAndPass( const std::string& fullurl, std::string& user, std::string& pass );
    bool GetServerAndPort( const std::string& fullurl, std::string& server, int& port );
    int GetDefaultPort( _PROTYPE protocol );
    bool GetRawUrl( std::string& fullurl, std::string& rawurl );
    bool GetPathFile( std::string& fullurl, std::string& pathfile );
    bool GetRefer( std::string rawurl, std::string& refer );
    void Precode( const std::string& pathfile, std::string& escfile );
    void UnEscape( char *s );
    void UnEscape( std::string& str );
    std::string m_user;
    std::string m_pass;
    std::string m_file;
    std::string m_server;
    std::string m_escfile;
    std::string m_refer; //only http
    std::string m_raw;   //不含用户名和密码的URL
    int m_port;
    bool	m_bValidMirrorUrl;
    _PROTYPE m_protocol;

};

#endif

