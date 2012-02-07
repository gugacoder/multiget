

#include "proants.h"
#include "mgftpants.h"
#include "mghttpants.h"
#include "mgftpinfo.h"
#include "mghttpinfo.h"
#include "mgfilemanager.h"
#include "mgsingletask.h"
#include "mgurlparser.h"
#include "common.h"
#include <iostream>

CProAnts::CProAnts(
    CMgSingleTask	*parent,
    CMgFileManager	*fm,
    int antid,
    llong filesize,
    int retry,
    int retrywait

)
{
    m_pFM = fm;
#ifdef WIN32
    m_pid.x=0;
	m_pid.p=NULL;
#else
    m_pid = 0;  //线程的pthread_t
#endif
    m_pParent = parent;
    m_nAntId = antid;  //1 based ant id
    m_nRetry = retry;
    m_nTotalByte = 0;
    m_nRetryWait = retrywait;
    m_nFileSize = filesize;
    m_bUseProxy = false;
    m_bUseFtpProxy = false;
    m_bUseHttpProxy = false;
    pthread_mutex_init( &m_QuitMutex, NULL );
}

CProAnts::~CProAnts()
{
	
#ifdef WIN32	
    if ( m_pid.p != NULL )
#else
    if ( m_pid != 0 )
#endif
    { //the thread is up
        pthread_cancel( m_pid );
        pthread_join( m_pid, NULL );
    }
}

//to get a task piece from fm
bool CProAnts::GetTask(
    std::string& fullurl,
    bool& urlneedcheck,   //这个url是否需要检查正确性
    llong& from,
    std::string& refer
)
{
    return m_pFM->GetTask( m_nAntId, fullurl, urlneedcheck, from, refer );
}

bool CProAnts::Go()
{
    pthread_create( &m_pid, NULL, SwapThread, this );
#ifdef WIN32
	return m_pid.p!=NULL;
#else
    return m_pid!=0;
#endif
}

//swap to real workthread
void* CProAnts::SwapThread( void* pthis )
{
    ( ( CProAnts* ) pthis ) ->WorkThread();

    return NULL;
}

//workthread call this to quit
void CProAnts::ThreadQuit()
{
    int state;
    pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, &state );
    m_pFM->ThreadQuit( m_nAntId );
    m_pParent->ThreadQuit( m_nAntId );
    pthread_setcancelstate ( state, NULL );
    pthread_exit( 0 );
}

void CProAnts::WorkThread()
{
    bool urlneedcheck;
    CUrlParser par;
    pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );

    OutMsg( "task thread up...", MSG_INFO );

again:

    //if(!GetTask(m_sUrl,urlneedcheck,m_Protocol,m_sServer,m_nPort,m_nFrom)) {

    if ( !GetTask( m_sUrl, urlneedcheck, m_nFrom, m_sRefer ) )
    {
        OutMsg( "thread gets no task,quit!", MSG_ERROR );
        ThreadQuit();
    }

    OutMsg( std::string( "task url=" ) + m_sUrl, MSG_INFO );

    if ( urlneedcheck )
        OutMsg( "url need check", MSG_INFO );
    else
        OutMsg( "url need not check", MSG_INFO );

    //OutMsg("analysis url...",MSG_INFO);

    par.SetUrl( m_sUrl );

    m_Protocol = par.GetUrlType();

redir:

    if ( m_Protocol == FTP_PROTOCOL )
    {

        if ( urlneedcheck )
        { //this url have not been checked

            OutMsg( "checking url...", MSG_INFO );
            CMgFtpInfo info(
                m_pParent,
                m_sUrl,
                1,
                m_nRetryWait,
                m_nAntId
            );

            if ( m_bUseProxy )
                info.SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );

            if ( m_bUseFtpProxy )
                info.SetFtpProxy( m_FtpProxy, m_FtpProxyPort );

            if ( !info.GetInfo() )
            {
                OutMsg( "fail to get file info.", MSG_WARNNING );
                ReportUrl( -10000, m_sUrl ); //还是给个机会吧
                m_nRetry--;

                if ( m_nRetry < 0 )
                    ThreadQuit();

                m_pParent->m_nError++;

                goto again; //get new task or url
            }

            if ( info.FileSize() != m_nFileSize	)
            {
                OutMsg( "file length check fail.", MSG_ERROR );
                ReportUrl( -100000, m_sUrl );  //an invalid url!
                m_nRetry--;

                if ( m_nRetry < 0 )
                    ThreadQuit();

                m_pParent->m_nError++;

                goto again; //get new task or url
            }

            ReportUrl( 500, m_sUrl );
        } //if(urlneedcheck)


        CMgFtpAnts ants(
            m_pParent,
            m_pFM,
            m_sUrl,
            m_nAntId,
            3,
            m_nRetryWait,
            m_nFrom,
            m_nFileSize  //check if this url valid
        );

        if ( m_bUseProxy )
            ants.SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );

        if ( m_bUseFtpProxy )
            ants.SetFtpProxy( m_FtpProxy, m_FtpProxyPort );

        int nret = ants.Go();

        if ( nret < 0 && nret != -3 )
        { //some error
            ReportUrl( -1000, m_sUrl );
            m_nRetry -= 3;

            if ( m_nRetry < 0 )
                ThreadQuit();

            m_pParent->m_nError += 3; //not atom,but not important as just a counter for show

            goto again;
        }
        else if ( nret == -3 )
        {
            OutMsg( "Session changes a url", MSG_INFO );
            goto again;
        }
        else if ( nret == 0 )
        { //redirect
            assert( 1 );
            goto again; //ftp should not have redirect return,never be here;
        }
        else
        { //ok finish
            goto again;
        }
    }
    else if ( m_Protocol == HTTP_PROTOCOL )
    {

        //OutMsg("url ok",MSG_INFO);

        if ( urlneedcheck )
        {
            OutMsg( "checking url...", MSG_INFO );
            CMgHttpInfo info(
                m_pParent,
                m_sUrl,
                1,
                m_nRetryWait,
                m_sRefer,
                m_nAntId
            );

            if ( m_bUseProxy )
                info.SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );

            if ( m_bUseHttpProxy )
                info.SetHttpProxy( m_HttpProxy, m_HttpProxyPort );

            if ( !info.GetInfo() )
            {
                OutMsg( "fail to get file info.", MSG_WARNNING );
                ReportUrl( -10000, m_sUrl ); //还是给个机会吧
                m_nRetry--;

                if ( m_nRetry < 0 )
                    ThreadQuit();

                m_pParent->m_nError++;

                goto again; //get new task or url
            }

            if ( info.IsRedirect() )
            {
                ReportRedirect( m_sUrl, info.GetRedirect() );
                m_sUrl = info.GetRedirect();
                goto redir;
            }

            if ( info.GetFileSize() != m_nFileSize )
            {
                OutMsg( "file length check fail.", MSG_ERROR );
                ReportUrl( -100000, m_sUrl ); //这里不能给机会了
                m_nRetry--;

                if ( m_nRetry < 0 )
                    ThreadQuit();

                m_pParent->m_nError++;

                goto again; //get new task or url
            }

            ReportUrl( 500, m_sUrl );

        } //if(urlneedcheck)


        //use httpinfo first to get file info or redirection
        //and notice fm to avoid anymore check

        CMgHttpAnts ants(
            m_pParent,
            m_pFM,
            m_sUrl,
            m_nAntId,
            3,
            m_nRetryWait,
            m_nFrom,
            m_nFileSize,
            m_sRefer
        );

        if ( m_bUseProxy )
            ants.SetProxy( m_Proxy, m_ProxyPort, m_ProxyUser, m_ProxyPass, m_ProxyVersion );

        if ( m_bUseHttpProxy )
            ants.SetHttpProxy( m_HttpProxy, m_HttpProxyPort );

        int nret = ants.Go();

        if ( nret < 0 && nret != -3 )
        { //some error
            m_nRetry -= 3;

            if ( m_nRetry < 0 )
                ThreadQuit();

            m_pParent->m_nError += 3; //not atom,but not important as just a counter for show

            ReportUrl( -1000, m_sUrl );

            goto again;
        }
        else if ( nret == -3 )
        {
            OutMsg( "Session changes a url", MSG_INFO );
            goto again;
        }
        else if ( nret == 0 )
        { //redirect,should not here
            m_nRetry -= 3;

            if ( m_nRetry < 0 )
                ThreadQuit();

            m_pParent->m_nError += 3;

            OutMsg( "should not be here", MSG_WARNNING );

            goto again;
        }
        else
        { //ok finish
            goto again;
        }
    }
    else
    { //other protocol we not support nowtime
        OutMsg( "not a support url,quit!", MSG_ERROR );
        ReportUrl( -100000, m_sUrl );
        ThreadQuit();
    }
}


//output message
void CProAnts::OutMsg( std::string str, _MSGTYPE type )
{
    m_pParent->OutMsg( m_nAntId, str, type );
}

bool CProAnts::Retry()
{
    if ( --m_nRetry > 0 )
    {
        char buf[ 40 ];
        sprintf( buf, "Wait %d seconds retry ...", m_nRetryWait );
        OutMsg( buf, MSG_INFO );

        m_pParent->m_nError++;

#ifdef WIN32

        Sleep( m_nRetryWait * 1000 );
#else

        sleep( m_nRetryWait );
#endif

        return true;
    }

    return false;
}


void CProAnts::ReportUrl( int prior, std::string url )
{
    m_pFM->ReportUrl( prior, url );
}

void CProAnts::ReportRedirect( std::string origin, std::string redirect )
{
    m_pFM->ReportRedirect( origin, redirect );
}


void CProAnts::SetProxy(
    std::string server,
    int port,
    std::string user,
    std::string pass,
    _SOCKVER ver )
{

    m_bUseProxy = true;
    m_Proxy = server;
    m_ProxyPort = port;
    m_ProxyUser = user;
    m_ProxyPass = pass;
    m_ProxyVersion = ver;

}

void CProAnts::SetHttpProxy( std::string server, int port )
{
    m_bUseHttpProxy = true;
    m_HttpProxy = server;
    m_HttpProxyPort = port;
}

void CProAnts::SetFtpProxy( std::string server, int port )
{
    m_bUseFtpProxy = true;
    m_FtpProxy = server;
    m_FtpProxyPort = port;
}

