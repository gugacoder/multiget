// MD5.h: interface for the CMD5 class.
//
//////////////////////////////////////////////////////////////////////
/*
	MD5计算类
	Version: 0.01
 
	Rewriter: multiget@gmail.com
	Create Date:	2001/07/26
	Last Revision:	2001/07/26
*/


#if !defined(AFX_MD5_H__B7341BB9_EB0A_4B6D_B813_B0F90CB98ABB__INCLUDED_)
#define AFX_MD5_H__B7341BB9_EB0A_4B6D_B813_B0F90CB98ABB__INCLUDED_


typedef struct
{
    unsigned int state[ 4 ];                                   /* state (ABCD) */
    unsigned int count[ 2 ];									 /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[ 64 ];                         /* input buffer */
}

MD5_CTX;

#include <string>

class CMD5
{

public:
    unsigned char* Encrypt( unsigned char*, unsigned int );
    std::string GetDigestStr();
    void InitData();
    void AddData( unsigned char* data, unsigned int dlen );
    void EndData();
    CMD5();
    virtual ~CMD5();

protected:
    unsigned char m_Digest[ 16 ];
    MD5_CTX m_context;

private:
    void MD5Init ( MD5_CTX* );
    void MD5Update ( MD5_CTX*, unsigned char*, unsigned int );
    void MD5Final ( unsigned char*, MD5_CTX* );

    void MD5Transform( unsigned int [ 4 ], unsigned char [ 64 ] );
    void Encode ( unsigned char*, unsigned int *, unsigned int );
    void Decode ( unsigned int *, unsigned char *, unsigned int );
    void MD5_memcpy ( unsigned char *, unsigned char *, unsigned int );
    void MD5_memset ( unsigned char *, int, unsigned int );
};

#endif // !defined(AFX_MD5_H__B7341BB9_EB0A_4B6D_B813_B0F90CB98ABB__INCLUDED_)

