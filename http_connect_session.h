#ifndef _HTTP_CONNECT_SESSION_H__
#define _HTTP_CONNECT_SESSION_H__

#include "config.h"
#include "lib-linux/buffer.h"
#include "SimpleNet/snet.h"

class HTTPConnectSession:public snet::ISession
{
public:
    HTTPConnectSession();
    ~HTTPConnectSession();
    void SetPeerSession(snet::ISession *pSession);
    inline bool IsConnect()
    {
        return m_bConnect;
    }

protected:
    void OnConnect(snet::SessionManager *pSessionManager);
    void OnDisconnect(snet::SessionManager *pSessionManager);
    void OnRead(snet::SessionManager *pSessionManager);
    void OnError(snet::SessionManager *pSessionManager, int nErrorCode, const char *pStr);

private:
    snet::ISession *m_pPeerSession;
    bool m_bConnect;
    Buffer m_buffer;
};
#endif
