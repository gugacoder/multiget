/***************************************************************************
*  mgproxysocket.cpp
*
*  Wed Sep  6 22:19:52 2006
*  Copyright  2006  liubin,China
*  Email: multiget@gmail.com
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

/* SOCKS 4
SOCKS: A protocol for TCP proxy across firewalls

		Ying-Da Lee
	Principal Member Technical Staff
	  NEC Systems Laboratory, CSTC
		ylee@syl.dl.nec.com

SOCKS was originally developed by David Koblas and subsequently modified
and extended by me to its current running version -- version 4. It is a
protocol that relays TCP sessions at a firewall host to allow application
users transparent access across the firewall. Because the protocol is
independent of application protocols, it can be (and has been) used for
many different services, such as telnet, ftp, finger, whois, gopher, WWW,
etc. Access control can be applied at the beginning of each TCP session;
thereafter the server simply relays the data between the client and the
application server, incurring minimum processing overhead. Since SOCKS
never has to know anything about the application protocol, it should also
be easy for it to accommodate applications which use encryption to protect
their traffic from nosey snoopers.

Two operations are defined: CONNECT and BIND.

1) CONNECT

The client connects to the SOCKS server and sends a CONNECT request when
it wants to establish a connection to an application server. The client
includes in the request packet the IP address and the port number of the
destination host, and userid, in the following format.

	+----+----+----+----+----+----+----+----+----+----+....+----+
	| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
	+----+----+----+----+----+----+----+----+----+----+....+----+
bytes:	   1    1      2              4           variable       1

VN is the SOCKS protocol version number and should be 4. CD is the
SOCKS command code and should be 1 for CONNECT request. NULL is a byte
of all zero bits.

The SOCKS server checks to see whether such a request should be granted
based on any combination of source IP address, destination IP address,
destination port number, the userid, and information it may obtain by
consulting IDENT, cf. RFC 1413.  If the request is granted, the SOCKS
server makes a connection to the specified port of the destination host.
A reply packet is sent to the client when this connection is established,
or when the request is rejected or the operation fails. 

	+----+----+----+----+----+----+----+----+
	| VN | CD | DSTPORT |      DSTIP        |
	+----+----+----+----+----+----+----+----+
bytes:	   1    1      2              4

VN is the version of the reply code and should be 0. CD is the result
code with one of the following values:

90: request granted
91: request rejected or failed
92: request rejected becasue SOCKS server cannot connect to
    identd on the client
93: request rejected because the client program and identd
    report different user-ids

The remaining fields are ignored.

The SOCKS server closes its connection immediately after notifying
the client of a failed or rejected request. For a successful request,
the SOCKS server gets ready to relay traffic on both directions. This
enables the client to do I/O on its connection as if it were directly
connected to the application server.


2) BIND

The client connects to the SOCKS server and sends a BIND request when
it wants to prepare for an inbound connection from an application server.
This should only happen after a primary connection to the application
server has been established with a CONNECT.  Typically, this is part of
the sequence of actions:

-bind(): obtain a socket
-getsockname(): get the IP address and port number of the socket
-listen(): ready to accept call from the application server
-use the primary connection to inform the application server of
the IP address and the port number that it should connect to.
-accept(): accept a connection from the application server

The purpose of SOCKS BIND operation is to support such a sequence
but using a socket on the SOCKS server rather than on the client.

The client includes in the request packet the IP address of the
application server, the destination port used in the primary connection,
and the userid.

	+----+----+----+----+----+----+----+----+----+----+....+----+
	| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
	+----+----+----+----+----+----+----+----+----+----+....+----+
# of bytes:	   1    1      2              4           variable       1

VN is again 4 for the SOCKS protocol version number. CD must be 2 to
indicate BIND request.

The SOCKS server uses the client information to decide whether the
request is to be granted. The reply it sends back to the client has
the same format as the reply for CONNECT request, i.e.,

	+----+----+----+----+----+----+----+----+
	| VN | CD | DSTPORT |      DSTIP        |
	+----+----+----+----+----+----+----+----+
# of bytes:	   1    1      2              4

VN is the version of the reply code and should be 0. CD is the result
code with one of the following values:

90: request granted
91: request rejected or failed
92: request rejected becasue SOCKS server cannot connect to
    identd on the client
93: request rejected because the client program and identd
    report different user-ids.

However, for a granted request (CD is 90), the DSTPORT and DSTIP fields
are meaningful.  In that case, the SOCKS server obtains a socket to wait
for an incoming connection and sends the port number and the IP address
of that socket to the client in DSTPORT and DSTIP, respectively. If the
DSTIP in the reply is 0 (the value of constant INADDR_ANY), then the
client should replace it by the IP address of the SOCKS server to which
the cleint is connected. (This happens if the SOCKS server is not a
multi-homed host.)  In the typical scenario, these two numbers are
made available to the application client prgram via the result of the
subsequent getsockname() call.  The application protocol must provide a
way for these two pieces of information to be sent from the client to
the application server so that it can initiate the connection, which
connects it to the SOCKS server rather than directly to the application
client as it normally would.

The SOCKS server sends a second reply packet to the client when the
anticipated connection from the application server is established.
The SOCKS server checks the IP address of the originating host against
the value of DSTIP specified in the client's BIND request.  If a mismatch
is found, the CD field in the second reply is set to 91 and the SOCKS
server closes both connections.  If the two match, CD in the second
reply is set to 90 and the SOCKS server gets ready to relay the traffic
on its two connections. From then on the client does I/O on its connection
to the SOCKS server as if it were directly connected to the application
server.



For both CONNECT and BIND operations, the server sets a time limit
(2 minutes in current CSTC implementation) for the establishment of its
connection with the application server. If the connection is still not
establiched when the time limit expires, the server closes its connection
to the client and gives up.
*/
/* SOCKS 4A
	SOCKS 4A: A  Simple Extension to SOCKS 4 Protocol

		Ying-Da Lee
	yingda@best.com  or  yingda@esd.sgi.com

Please read SOCKS4.protocol first for an description of the version 4
protocol. This extension is intended to allow the use of SOCKS on hosts
which are not capable of resolving all domain names.

In version 4, the client sends the following packet to the SOCKS server
to request a CONNECT or a BIND operation:

	+----+----+----+----+----+----+----+----+----+----+....+----+
	| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
	+----+----+----+----+----+----+----+----+----+----+....+----+
# of bytes:	   1    1      2              4           variable       1

VN is the SOCKS protocol version number and should be 4. CD is the
SOCKS command code and should be 1 for CONNECT or 2 for BIND. NULL
is a byte of all zero bits.

For version 4A, if the client cannot resolve the destination host's
domain name to find its IP address, it should set the first three bytes
of DSTIP to NULL and the last byte to a non-zero value. (This corresponds
to IP address 0.0.0.x, with x nonzero. As decreed by IANA  -- The
Internet Assigned Numbers Authority -- such an address is inadmissible
as a destination IP address and thus should never occur if the client
can resolve the domain name.) Following the NULL byte terminating
USERID, the client must sends the destination domain name and termiantes
it with another NULL byte. This is used for both CONNECT and BIND requests.

A server using protocol 4A must check the DSTIP in the request packet.
If it represent address 0.0.0.x with nonzero x, the server must read
in the domain name that the client sends in the packet. The server
should resolve the domain name and make connection to the destination
host if it can. 

SOCKSified sockd may pass domain names that it cannot resolve to
the next-hop SOCKS server.
*/
#include "mgproxysocket.h"

/*
Error code [0x0000**00~0x0000**00]:
0x01: connect to socks proxy error;
0x02: auto version socks proxy not support;
0x03: send init packet to socks proxy fail;
0x04: socks proxy return packet have wrong length;
0x05: socks proxy return wrong version;
0x06: socks proxy return wrong format response packet;
0x07: socks proxy method we not support;
0x08: socks proxy GSSAPI mothod we not support;
0x09: send socks proxy user/pass login packet fail;
0x0A: read socks proxy login respose fail;
0x0B: socks proxy login response malformat;
0x0C: socks v4a not support;
0x0D: socks v4 not support;
0x0E: no connection to socks proxy;
0x0F: not use
0x10: socks proxy login response too short;
0x11: socks proxy login response malformat;
0x12: socks proxy login response "fail";
0x13: socks proxy login response "not allow";
0x14: socks proxy login response "net unreach";
0x15: socks proxy login response "host unreach";
0x16: socks proxy login response "connect refuse";
0x17: socks proxy login response "TTL timeout";
0x18: socks proxy login response "not support cmd";
0x19: socks proxy login response "other fail";
0x1A: send socks proxy login packet wrong;
0x1B: recv socks proxy login response packet wrong;
*/

CMgProxySocket::CMgProxySocket()
{
    m_bAppConnected = false;
    //proxy varis
    m_ProxyPort = 0;
    m_ProxyVersion = V5;
    m_bUseProxy = false;

}

CMgProxySocket::~CMgProxySocket()
{}

bool CMgProxySocket::Connect( const char* server, int port )
{
    if ( !m_bUseProxy )
    {
        return CMgSocket::Connect( server, port );
    }
    else
    { //proxy use

        if ( !CMgSocket::Connect( m_ProxyServer.c_str(), m_ProxyPort ) )
        {
            m_nLastError += ( 0x01 << 8 );  //connect socks proxy error
            return false;
        }

        //unsigned char buf[256];

        if ( m_ProxyVersion == AUTOVERSION )
        {
            if ( DoV5Login() )
            {
                return DoV5Connect( server, port );
            }
            else if ( DoV4AConnect( server, port ) )
            {
                return true;
            }
            else
            {
                return DoV4Connect( server, port );
            }
        }
        else
        {

            if ( m_ProxyVersion == V5 )  //SOCK V5
            {

                if ( !DoV5Login() )
                    return false;

                return DoV5Connect( server, port );

            }
            else if ( m_ProxyVersion == V4A )
            { //4a
                return DoV4AConnect( server, port );
            }
            else if ( m_ProxyVersion == V4 )
            { //4
                return DoV4Connect( server, port );
            }
            else
                return false;
        }
    } //else

    return false;
}

//zhi set bu run
bool CMgProxySocket::SetProxy(
    const char* server, int port, const char* user, const char* pass, _SOCKVER ver )
{

    m_ProxyServer = std::string( server );
    m_ProxyPort = port;
    m_ProxyUser = std::string( user );
    m_ProxyPass = std::string( pass );
    m_ProxyVersion = ver;
    m_bUseProxy = true;
    return true;
}

bool CMgProxySocket::SetProxy(
    std::string server, int port, std::string user, std::string pass, _SOCKVER ver )
{

    m_ProxyServer = ( server );
    m_ProxyPort = port;
    m_ProxyUser = ( user );
    m_ProxyPass = ( pass );
    m_ProxyVersion = ver;
    m_bUseProxy = true;
    return true;
}

void CMgProxySocket::SetProxy( CMgProxySocket& oldsock )
{ //copy the proxy setting from another,ftp use it

    if ( oldsock.m_bUseProxy )
    {
        m_ProxyServer = oldsock.m_ProxyServer;
        m_ProxyPort = oldsock.m_ProxyPort;
        m_ProxyUser = oldsock.m_ProxyUser;
        m_ProxyPass = oldsock.m_ProxyPass;
        m_ProxyVersion = oldsock.m_ProxyVersion;
        m_bUseProxy = true;
    }
    else
    {
        m_bUseProxy = false;
    }
}

/*
//send connect command to proxy
bool CMgProxySocket::DoProxyConnect(const char* server ,int port)
{//V5 shixian
	if(!m_bConnected) 
	{
		m_nLastError= (0x0E<<8); 
		return false;
	}
	
	
	switch(m_ProxyVersion)
	{
		case V4:
			return DoV4Connect(server,port);
		case V4A:
			return DoV4AConnect(server,port);
		case V5:
			return DoV5Connect(server,port);	
		case AUTOVERSION:		
		default:
			if( DoV5Connect(server,port)) {
				m_ProxyVersion=V5;
				return true;
			}else if(DoV4AConnect(server,port)) {
				m_ProxyVersion=V4A;
				return true;
			}else {
				if(DoV4Connect(server,port))
				{
					m_ProxyVersion=V4;
					return true;
				}
				else
				{
					return false;
				}
			}
		
	}
 
}
*/

bool CMgProxySocket::DoV5Connect( const char* server , int port )
{

    unsigned char buf[ 512 ];
    buf[ 0 ] = 0x05;
    buf[ 1 ] = 0x01; //connect
    buf[ 2 ] = 0x00; //reserve
    buf[ 3 ] = 0x03;
    buf[ 4 ] = strlen( server );
    memcpy( buf + 5, server, strlen( server ) );
    int pos = 5 + buf[ 4 ];
    short sport = htons( ( short ) port );
    memcpy( buf + pos, &sport, 2 );
    pos += 2;

    if ( !Send( buf, pos ) )
    {
        m_nLastError += ( 0x1A << 8 );
        return false;
    }

    int nret = Read( buf, 512 );

    if ( nret <= 0 )
    {
        m_nLastError += ( 0x1B << 8 );
        return false;
    }

    if ( nret < 10 )
    {
        m_nLastError = ( 0x0F << 8 );
        return false;
    }

    if ( buf[ 0 ] != 0x05 || buf[ 2 ] != 0x00 )
    {
        m_nLastError = ( 0x10 << 8 );
        return false;
    }

    /*
    # X'00' success
    # X'01' fail
    # X'02' not allow
    # X'03' net unreach
    # X'04' host unreach
    # X'05' connect refuse
    # X'06' TTL timeout
    # X'07' not support command
    # X'08' not support address
    # X'09' – X'FF' undef
    */
    if ( buf[ 1 ] == 0 )
    {
        return true;
    }
    else if ( buf[ 1 ] == 0x01 )
    {
        m_nLastError = ( 0x11 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x02 )
    {
        m_nLastError = ( 0x12 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x03 )
    {
        m_nLastError = ( 0x13 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x04 )
    {
        m_nLastError = ( 0x14 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x05 )
    {
        m_nLastError = ( 0x15 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x06 )
    {
        m_nLastError = ( 0x16 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x07 )
    {
        m_nLastError = ( 0x17 << 8 );
        return false;
    }
    else if ( buf[ 1 ] == 0x08 )
    {
        m_nLastError = ( 0x18 << 8 );
        return false;
    }
    else
    {
        m_nLastError = ( 0x19 << 8 );
        return false;
    }

    return false;
}

bool CMgProxySocket::DoV4Connect( const char* server , int port )
{ //resolv ip locally
    /*
    		+----+----+----+----+----+----+----+----+----+----+....+----+
    		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
    		+----+----+----+----+----+----+----+----+----+----+....+----+
    bytes:	   1    1      2              4           variable       1
    	
    	
    		+----+----+----+----+----+----+----+----+
    		| VN | CD | DSTPORT |      DSTIP        |
    		+----+----+----+----+----+----+----+----+
    bytes:	   1    1      2              4
     
    VN is the version of the reply code and should be 0. CD is the result
    code with one of the following values:
     
    	90: request granted
    	91: request rejected or failed
    	92: request rejected becasue SOCKS server cannot connect to
    	    identd on the client
    	93: request rejected because the client program and identd
    	    report different user-ids.	
    */


    unsigned char buf[ 256 ];

    struct hostent *hp;
    hp = gethostbyname( server );

    if ( !hp )
    {
        m_nLastError = 0x02; //??
        return false;
    }

    if ( hp->h_addrtype != AF_INET )
    { //neither AF_INET nor AF_INET6
        m_nLastError = 0x03; //??
        return false;
    }

    short iport = htons( ( short ) port );

    buf[ 0 ] = 0x04;
    buf[ 1 ] = 0x01;
    memcpy( buf + 2, &iport, 2 );
    memcpy( buf + 4, ( void* ) ( hp->h_addr ), 4 );
    memcpy( buf + 8, m_ProxyUser.c_str(), m_ProxyUser.length() );
    buf[ 8 + m_ProxyUser.length() ] = 0;

    if ( !Send( buf, 8 + m_ProxyUser.length() + 1 ) )
    {
        return false;
    }

    int nret = Read( buf, 256 );

    if ( nret != 8 )
    {
        return false;
    }

    if ( buf[ 0 ] != 0 )
    {
        return false;

    }

    if ( buf[ 1 ] == 90 )
    {
        return true;
    }
    else if ( buf[ 1 ] == 91 )
    {
        return false;
    }
    else if ( buf[ 1 ] == 92 )
    {
        return false;
    }
    else if ( buf[ 1 ] == 93 )
    {
        return false;
    }

    return false;
}

bool CMgProxySocket::DoV4AConnect( const char* server , int port )
{


    unsigned char buf[ 256 ];

    buf[ 0 ] = 0x04;
    buf[ 1 ] = 0x01;
    short iport = htons( ( short ) port );
    memcpy( buf + 2, &iport, 2 );
    buf[ 4 ] = buf[ 5 ] = buf[ 6 ] = 0;
    buf[ 7 ] = 0x01;
    memcpy( buf + 8, m_ProxyUser.c_str(), m_ProxyUser.length() );
    buf[ 8 + m_ProxyUser.length() ] = 0;
    memcpy( buf + 8 + m_ProxyUser.length() + 1, server, strlen( server ) );
    buf[ 8 + m_ProxyUser.length() + 1 + strlen( server ) ] = 0;

    if ( !Send( buf, 8 + m_ProxyUser.length() + 1 + strlen( server ) + 1 ) )
    {
        return false;
    }

    int nret = Read( buf, 256 );

    if ( nret != 8 )
    {
        return false;
    }

    if ( buf[ 0 ] != 0 )
    {
        return false;
    }

    if ( buf[ 1 ] == 90 )
    {
        return true;
    }
    else if ( buf[ 1 ] == 91 )
    {
        return false;
    }
    else if ( buf[ 1 ] == 92 )
    {
        return false;
    }
    else if ( buf[ 1 ] == 93 )
    {
        return false;
    }

    return false;
}

bool CMgProxySocket::DoV5Login()
{
    unsigned char init[ 4 ] = {0x05, 0x02, 0x00, 0x02};

    if ( !Send( init, 4 ) )
    {
        m_nLastError = ( 0x03 << 8 );
        return false;
    }

    unsigned char buf[ 256 ];

    int nret = Read( buf, 256 );

    if ( nret != 2 )
    {
        m_nLastError = ( 0x04 << 8 );
        return false;
    }

    if ( buf[ 0 ] != 0x05 )
    {
        m_nLastError = ( 0x05 << 8 );
        return false;
    }

    if ( buf[ 1 ] == 0xFF )
    {
        m_nLastError = ( 0x06 << 8 );
        return false;
    }

    if ( buf[ 1 ] != 0 && buf[ 1 ] != 1 && buf[ 1 ] != 2 )
    {
        m_nLastError = ( 0x07 << 8 );
        return false;
    }

    if ( buf[ 1 ] == 0 )
    { //ok
        return true;
    }
    else if ( buf[ 1 ] == 1 )
    { //GSSAPI
        m_nLastError = ( 0x08 << 8 );
        return false;
    }
    else
    { //u/p
        int tl = 0;
        buf[ 0 ] = 0x01;
        tl++;
        buf[ tl ] = m_ProxyUser.length();
        tl++;
        memcpy( buf + tl, m_ProxyUser.c_str(), m_ProxyUser.length() );
        tl += m_ProxyUser.length();
        buf[ tl ] = m_ProxyPass.length();
        tl++;
        memcpy( buf + tl, m_ProxyPass.c_str(), m_ProxyPass.length() );
        tl += m_ProxyPass.length();

        if ( !Send( buf, tl ) )
        {
            m_nLastError += ( 0x09 << 8 );
            return false;
        }

        if ( Read( buf, 256 ) != 2 )
        {
            m_nLastError = ( 0x0A << 8 );
            return false;
        }

        if ( buf[ 0 ] != 0x01 )
        {
            m_nLastError = ( 0x0B << 8 );
            return false;
        }

        if ( buf[ 1 ] != 0x00 )
        {
            m_nLastError = ( 0x0C << 8 );
            return false;
        }

        return true;
    }

    return false;
}

