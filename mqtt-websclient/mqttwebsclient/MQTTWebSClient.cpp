#include "MQTTWebSClient.hpp"
#include "mongoose.h"
#include <iostream>

static void onEvent(struct mg_connection* c, int ev, void* ev_data);
class MQTTWebSClientPriv
{
public:
    mg_mgr m_stuMgr;
    mg_connection* m_pConn;
    MQTTWebSClient::FnMsgHandler m_fnMsgHandler;
    bool m_bIDLE;
    std::string m_strURL;

    MQTTWebSClientPriv() : m_pConn(nullptr)
        , m_fnMsgHandler(nullptr)
        , m_bIDLE(true)
    {
        init();
    }

    ~MQTTWebSClientPriv()
    {
        stop();
    }

    void init()
    {
        memset(&m_stuMgr, 0, sizeof(m_stuMgr));
    }

    void stop()
    {
        m_bIDLE = true;
        mg_mgr_free(&m_stuMgr);
    }

    void start()
    {
        m_bIDLE = false;
        //stop();

        init();
        mg_mgr_init(&m_stuMgr);
        m_pConn = mg_ws_connect(&m_stuMgr, m_strURL.c_str(), onEvent, this, nullptr);
    }

    void onError()
    {
        std::cout << "onError" << std::endl;
        m_bIDLE = true;
    }

    void onClose()
    {
        std::cout << "onClose" << std::endl;
        m_bIDLE = true;
    }

    void onOpen()
    {
        std::cout << "onOpen" << std::endl;
        //sendMsg("hello");
        m_bIDLE = false;
    }

    void onMsg(const std::string& strMsg)
    {
        if (nullptr != m_fnMsgHandler)
        {
            m_fnMsgHandler(strMsg);
        }
    }

    void sendMsg(const std::string& strMsg)
    {
        if (isConnected())
        {
            mg_ws_send(m_pConn, strMsg.c_str(), strMsg.size(), WEBSOCKET_OP_TEXT);
        }
    }

    bool isConnected()
    {
        return !m_bIDLE;
    }

    void pollEvent(int ms)
    {
        //if (isConnected())
        //{
            mg_mgr_poll(&m_stuMgr, ms);
        //}
    }
};

static void onEvent(struct mg_connection* c, int ev, void* ev_data)
{
    auto pWebSClientPriv = (MQTTWebSClientPriv*)c->fn_data;
    if (nullptr == pWebSClientPriv)
    {
        return;
    }

    switch (ev)
    {
    case MG_EV_OPEN:
        
        break;
    case MG_EV_CONNECT:
        break;
    case MG_EV_ERROR:
        pWebSClientPriv->onError();
        break;
    case MG_EV_WS_OPEN:
        pWebSClientPriv->onOpen();
        break;
    case MG_EV_WS_MSG:
    {
        struct mg_ws_message* wm = (struct mg_ws_message*)ev_data;
        pWebSClientPriv->onMsg(std::string(wm->data.buf, (int)wm->data.len));
    }
        break;
    case MG_EV_CLOSE:
        pWebSClientPriv->onClose();
        break;
    default:
        break;
    }
}

MQTTWebSClient::MQTTWebSClient() : m_pPriv(new MQTTWebSClientPriv)
{
}

MQTTWebSClient::~MQTTWebSClient()
{
    if (nullptr != m_pPriv)
    {
        delete m_pPriv;
    }
}

void MQTTWebSClient::pollEvent(int ms /*= 20*/)
{
    m_pPriv->pollEvent(ms);
}
void MQTTWebSClient::start()
{
    m_pPriv->start();
}

void MQTTWebSClient::stop()
{
    m_pPriv->stop();
}

MQTTWebSClient& MQTTWebSClient::setOnMsgHandler(FnMsgHandler fnMsgHandler)
{
    m_pPriv->m_fnMsgHandler = fnMsgHandler;
    return *this;
}

void MQTTWebSClient::sendMsg(const std::string& strMsg)
{
    m_pPriv->sendMsg(strMsg);
}

MQTTWebSClient& MQTTWebSClient::setURL(const std::string& strURL)
{
    m_pPriv->m_strURL = strURL;
    return *this;
}

bool MQTTWebSClient::isConnected()
{
    return m_pPriv->isConnected();
}