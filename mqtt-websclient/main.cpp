#include <thread>
#include <iostream>
#include "MQTTWebSClient/MQTTWebSClient.hpp"

const static std::string s_url("ws://192.168.1.165:9000/test");

int main(int argc, char *argv[]) 
{
    std::cout << "mqtt websclient demo" << std::endl;

    MQTTWebSClient objClient;
    objClient.setURL(s_url)
        .setOnMsgHandler([](const std::string& strMsg) {
            std::cout << "recv: ----> " << strMsg << std::endl;
            })
        .start();

    std::thread thd([&]() {
        while (true)
        {
            objClient.pollEvent();
        }
       });

    std::thread thd1([&]() {

        int i = 0;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (objClient.isConnected())
            {
                objClient.sendMsg("hello" + std::to_string(++i));
            }
        }
        });

    thd.join();
    thd1.join();

    return 0;
}
