#include "http_sniff_session.h"

HTTPSniffSession::HTTPSniffSession()
:m_pPeerSession(NULL),
m_bHostField(false)
{
    ::memset(&m_settings, 0, sizeof(m_settings));
    // look url :)
    m_settings.on_url = OnUrl;
    m_settings.on_header_field = OnHeaderField;
    m_settings.on_header_value = OnHeaderValue;

    http_parser_init(&m_parser, HTTP_REQUEST);
    m_parser.data = this;
}

HTTPSniffSession::~HTTPSniffSession()
{
    assert(!m_pPeerSession);
}

void HTTPSniffSession::OnConnect(snet::SessionManager *pSessionManager)
{
    LOG("HTTPSniffSession OnConnect %d", getid());
}

void HTTPSniffSession::OnDisconnect(snet::SessionManager *pSessionManager)
{
    LOG("HTTPSniffSession OnDisconnect %d", getid());
    if (m_pPeerSession != NULL)
    {
        pSessionManager->Delete(m_pPeerSession);
        m_pPeerSession = NULL;
    }

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

void HTTPSniffSession::OnRead(snet::SessionManager *pSessionManager)
{
    char buffer[100];
    while (true)
    {
        int nLen = recv(buffer, 100);

        /* Start up / continue the parser.
         * Note we pass recved==0 to signal that EOF has been recieved.
         */
        int nparsed = http_parser_execute(&m_parser, &m_settings, buffer, nLen);
        if (nparsed != nLen) 
        {
            /* Handle error. Usually just close the connection. */
            LOG("http parser error"); 
        }

        if (nLen <= 0)
        {
            break;
        }

        m_buffer.put(buffer, nLen);
    }

    //LOG("from sniff ready : %d", m_buffer.size());
    if (!m_strHost.empty() && m_pPeerSession == NULL)
    {
        //LOG("Connect to %s", m_strHost.c_str());
        snet::Address addr = get_host_name(m_strHost.c_str());
        addr.set_port(80);

        snet::SessionFactoryImp<HTTPConnectSession> factory;
        m_pPeerSession = pSessionManager->Connect(factory, addr, false);
        HTTPConnectSession *pSession = dynamic_cast<HTTPConnectSession *>(m_pPeerSession);
        pSession->SetPeerSession(this);
    }

    if (m_pPeerSession != NULL)
    {
        HTTPConnectSession *pSession = dynamic_cast<HTTPConnectSession *>(m_pPeerSession);
        if (pSession->IsConnect())
        {
            int nSend = pSession->send(m_buffer.raw(), m_buffer.size());
            //LOG("from sniff send HTTP: %d", nSend);
            m_buffer.clear();
        }
    }
}

void HTTPSniffSession::OnError(snet::SessionManager *pSessionManager, int nErrorCode, const char *pStr)
{
    LOG("HTTPSniffSession errorcode %d OnError %d", nErrorCode, getid());
}

void HTTPSniffSession::CallBack_OnConnect(snet::ISession *pSession)
{
    if (m_buffer.size() > 0)
    {
        pSession->send(m_buffer.raw(), m_buffer.size());
        //LOG("from sniff send HTTP: %d", m_buffer.size());
        m_buffer.clear();
    }
}

int HTTPSniffSession::OnUrl(http_parser *parser, const char *at, size_t length)
{
    std::string str(at, length);
    //LOG("In OnUrl %s", str.c_str());
    return 0;
}

int HTTPSniffSession::OnHeaderField(http_parser *parser, const char *at, size_t length)
{
    std::string str(at, length);
    if (str == "Host")
    {
        HTTPSniffSession *pSession = (HTTPSniffSession *)(parser->data);
        pSession->m_bHostField = true;
        LOG("In OnHeaderField %s", str.c_str());
    }
    return 0;
}

int HTTPSniffSession::OnHeaderValue(http_parser *parser, const char *at, size_t length)
{
    std::string str(at, length);

    HTTPSniffSession *pSession = (HTTPSniffSession *)(parser->data);
    if (pSession->m_bHostField)
    {
        pSession->m_strHost = str;
        pSession->m_bHostField = false;
        LOG("In OnHeaderValue Host:%s", str.c_str());
    }
    return 0;
}
