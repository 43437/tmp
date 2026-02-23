#ifndef MQTT_WEBSCLIENT_HPP__
#define MQTT_WEBSCLIENT_HPP__

#include <string>
#include <functional>

class MQTTWebSClientPriv;
class MQTTWebSClient
{   
public:
    typedef std::function<void(const std::string& strMsg)> FnMsgHandler;
    friend class MQTTWebSClientPriv;
    MQTTWebSClient();
    ~MQTTWebSClient();

    void pollEvent(int ms = 20);
    void start();
    void stop();
    MQTTWebSClient& setOnMsgHandler(FnMsgHandler fnMsgHandler);
    void sendMsg(const std::string& strMsg);
    MQTTWebSClient& setURL(const std::string& strURL);
    bool isConnected();

private:
    MQTTWebSClientPriv* m_pPriv;
};

#endif // MQTT_WEBSCLIENT_HPP__