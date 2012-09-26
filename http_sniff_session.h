#ifndef __HTTP_SNIFF_SESSION_H__
#define __HTTP_SNIFF_SESSION_H__

#include "config.h"
#include "http-parser/http_parser.h"
#include "lib-linux/buffer.h"
#include "SimpleNet/snet.h"

#include "http_connect_session.h"

class HTTPSniffSession:public snet::ISession
{
public:
    HTTPSniffSession();
    ~HTTPSniffSession();

protected:
    void OnConnect(snet::SessionManager *pSessionManager);
    void OnDisconnect(snet::SessionManager *pSessionManager);
    void OnRead(snet::SessionManager *pSessionManager);
    void OnError(snet::SessionManager *pSessionManager, int nErrorCode, const char *pStr);

    // m_pPeerSession is connect
    void CallBack_OnConnect(snet::ISession *pSession);

    friend class HTTPConnectSession;

    // HTTP callback
    static int OnUrl(http_parser *parser, const char *at, size_t length);
    static int OnHeaderField(http_parser *parser, const char *at, size_t length);
    static int OnHeaderValue(http_parser *parser, const char *at, size_t length);

private:
    Buffer m_buffer;
    snet::ISession *m_pPeerSession;

    // http parser
    http_parser_settings m_settings;
    http_parser m_parser;

    bool m_bHostField;
    std::string m_strHost;
};

#endif
