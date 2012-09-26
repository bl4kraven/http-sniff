#include "http_connect_session.h"
#include "http_sniff_session.h"

HTTPConnectSession::HTTPConnectSession()
:m_pPeerSession(NULL),
m_bConnect(false)
{
}

HTTPConnectSession::~HTTPConnectSession()
{
}

void HTTPConnectSession::SetPeerSession(snet::ISession *pSession)
{
    m_pPeerSession = pSession;
}

void HTTPConnectSession::OnConnect(snet::SessionManager *pSessionManager)
{
    LOG("In HTTPConnectSession OnConnect %d", getid());
    LOG("connect to %s", get_peer_addr().get_ip().c_str());
    m_bConnect = true;

    assert(m_pPeerSession);
    // notify to sniff session
    HTTPSniffSession *pSession = dynamic_cast<HTTPSniffSession *>(m_pPeerSession);
    pSession->CallBack_OnConnect(this);
}

void HTTPConnectSession::OnDisconnect(snet::SessionManager *pSessionManager)
{
    LOG("In HTTPConnectSession OnDisconnect %d", getid());
    if (m_pPeerSession)
    {
        pSessionManager->Delete(m_pPeerSession);
        m_pPeerSession = NULL;   

#ifdef _DEBUG
        printf("SessionList: ");
        snet::SessionManager::SessionList list = pSessionManager->GetSessionList();
        for (snet::SessionManager::SessionList::iterator it=list.begin(); it!=list.end(); it++)
        {
            printf("%d ", (*it)->getid());
        }
        printf("\n");
#endif
    }
}

void HTTPConnectSession::OnRead(snet::SessionManager *pSessionManager)
{
    char buffer[100];
    while (true)
    {
        int nLen = recv(buffer, 100);
        if (nLen <= 0)
        {
            break;
        }

        m_buffer.put(buffer, nLen);
    }
    assert(m_pPeerSession);
    int nSend = m_pPeerSession->send(m_buffer.raw(), m_buffer.size());
    //LOG("from connect send: %d", nSend);
    m_buffer.clear();
}

void HTTPConnectSession::OnError(snet::SessionManager *pSessionManager, int nErrorCode, const char *pStr)
{
    LOG("In HTTPConnectSession errorcode %d OnError %d", nErrorCode, getid());
    if (m_pPeerSession)
    {
        pSessionManager->Delete(m_pPeerSession);
        m_pPeerSession = NULL;   

#ifdef _DEBUG
        printf("SessionList: ");
        snet::SessionManager::SessionList list = pSessionManager->GetSessionList();
        for (snet::SessionManager::SessionList::iterator it=list.begin(); it!=list.end(); it++)
        {
            printf("%d ", (*it)->getid());
        }
        printf("\n");
#endif
    }
}
