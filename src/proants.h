
#ifndef _PROANTS_H
#define _PROANTS_H

#include <string>
#include "common.h"
#include <pthread.h>

class CMgSingleTask;

class CMgFileManager;

class CProAnts
{

public:
    CProAnts(
        CMgSingleTask	*parent,
        CMgFileManager	*fm,
        int aid,
        llong filesize,
        int retry = 99,
        int retrywait = 30
    );

    virtual ~CProAnts();
    bool Go();
    //socks proxy set
    void SetProxy( std::string server, int port, std::string user, std::string pass, _SOCKVER ver = V5 );
    //http proxy set
    void SetHttpProxy( std::string server, int port );
    //ftp proxy set
    void SetFtpProxy( std::string server, int port );

protected:
    //get a new task piece from task manager
    bool GetTask(
        std::string& fullurl,
        bool& urlneedcheck,
        llong& from,
        std::string& refer
    ); //get task from fm;


protected:
    static void* SwapThread( void* p );
    void WorkThread();
    void ThreadQuit();
    void OutMsg( std::string str, _MSGTYPE type );
    bool Retry();
    void ReportUrl( int prior, std::string url );
    void ReportRedirect( std::string origin, std::string redirect );

private:
    CMgSingleTask	*m_pParent;
    CMgFileManager *m_pFM;
    pthread_t m_pid;
    pthread_mutex_t m_QuitMutex;
    std::string m_sServer;
    int	m_nPort;
    std::string	m_sUrl;
    _PROTYPE	m_Protocol;
    std::string	m_sRefer;
    std::string	m_sFile;
    int	m_nRetry;
    int	m_nRetryWait;
    int	m_nAntId;
    llong	m_nFrom;
    llong	m_nFileSize;  //the total file size
    llong	m_nTotalByte; //this thread download bytes

    //SOCKS proxy
    bool m_bUseProxy;
    std::string m_Proxy;
    int m_ProxyPort;
    std::string m_ProxyUser;
    std::string m_ProxyPass;
    _SOCKVER m_ProxyVersion;
    //HTTP proxy
    bool m_bUseHttpProxy;
    std::string m_HttpProxy;
    int m_HttpProxyPort;
    //FTP proxy
    bool m_bUseFtpProxy;
    std::string m_FtpProxy;
    int m_FtpProxyPort;
};


#endif

