/***************************************************************************
*  mgstring.cpp
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

#include "mgstring.h"
#include <wx/strconv.h>
#include "common.h"


CMgString::CMgString()
{
#ifdef WIN32
    m_Lang = ZH_CN_18030;
    //you can add your language translation here by refering to english
#include "linux_str.txt" 
#else
    m_Lang = ZH_CN_UTF_8;
#include "linux_str.txt"  //don't edit this file in windows
#endif //ifdef WIN32
}



void CMgString::SetLang( _MGLANG lang )
{
    m_Lang = lang;
}

_MGLANG CMgString::GetLang()
{
    return m_Lang;
}

//big5:cp950
std::string CMgString::GetStr( _MGSTRID id )
{
    if (ZH_CN_UTF_8 == m_Lang || 
	EN_US_UTF_8 == m_Lang || 
	ZH_TW_UTF_8 == m_Lang || 
	DE_DE_UTF_8 == m_Lang ||
	PT_BR_UTF_8 == m_Lang)
    {
        return m_SA[ m_Lang ][ id ];
    }
    else if ( ZH_CN_18030 == m_Lang )
    {
        //Converts from UTF-8 encoding to Unicode
        wchar_t bufuc[ 512 ];
        wxMBConvUTF8 conv;
        int len = conv.MB2WC( bufuc, m_SA[ ZH_CN_UTF_8 ][ id ].c_str(), 512 );
        //wxCSConv tomb(wxT("zh_CN.GB18030"));
        wxCSConv tomb( wxT( "cp936" ) );
        char bufmb[ 512 ];
        len = tomb.WC2MB( bufmb, bufuc, 512 );
        std::string str( bufmb );
        return str;
    }
    else if ( ZH_TW_BIG5 == m_Lang )
    {
        //Converts from UTF-8 encoding to Unicode
        wchar_t bufuc[ 512 ];
        wxMBConvUTF8 conv;
        int len = conv.MB2WC( bufuc, m_SA[ ZH_TW_UTF_8 ][ id ].c_str(), 512 );

        wxCSConv tomb( wxT( "cp950" ) );
        char bufmb[ 512 ];
        len = tomb.WC2MB( bufmb, bufuc, 512 );
        std::string str( bufmb );
        return str;
    }

    //other lang goes here

    return std::string( "no lang" );

}

wxString CMgString::GetWxStr( _MGSTRID id )
{
    if (ZH_CN_UTF_8 == m_Lang || 
	EN_US_UTF_8 == m_Lang || 
	ZH_TW_UTF_8 == m_Lang || 
	DE_DE_UTF_8 == m_Lang ||
	PT_BR_UTF_8 == m_Lang)
    {
        wxString wxs( m_SA[ m_Lang ][ id ].c_str(), wxConvLocal ); 
        return wxs;
    }
    else if ( ZH_CN_18030 == m_Lang )
    {
        //Converts from UTF-8 encoding to Unicode
        wchar_t bufuc[ 512 ];
        wxMBConvUTF8 conv;
        int len = conv.MB2WC( bufuc, m_SA[ ZH_CN_UTF_8 ][ id ].c_str(), 512 );
        //wxCSConv tomb(wxT("zh_CN.GB18030"));
        wxCSConv tomb( wxT( "cp936" ) );
        char bufmb[ 512 ];
        len = tomb.WC2MB( bufmb, bufuc, 512 );
        wxString wxs = wxString ( bufmb, wxConvLocal ); //unicode patch
        return wxs;
    }
    else if ( ZH_TW_BIG5 == m_Lang )
    {
        //Converts from UTF-8 encoding to Unicode
        wchar_t bufuc[ 512 ];
        wxMBConvUTF8 conv;
        int len = conv.MB2WC( bufuc, m_SA[ ZH_TW_UTF_8 ][ id ].c_str(), 512 );
        //wxCSConv tomb(wxT("zh_CN.GB18030"));
        wxCSConv tomb( wxT( "cp950" ) );
        char bufmb[ 512 ];
        len = tomb.WC2MB( bufmb, bufuc, 512 );
        std::string str( bufmb );
        //return str;
	return wxString(str.c_str(),wxConvLocal);	
    }


    //other lang goes here
    return _( "no lang" );
}
