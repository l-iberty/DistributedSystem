#include <iostream>
#include "TCPClient.h"

int main()
{
    TCPClient client;
    uint16_t port;
    char buf[1];
    for (;;)
    {
        std::cout << "send request to? ";
        std::cin >> port;
        if (client.connectServer("127.0.0.1", port) && client.sendToServer(buf, sizeof(buf)))
        {
            std::cout << "done" << std::endl;
        }
        else
        {
            std::cerr << "unable to send request to server" << std::endl;
        }
        client.closeConnection();

    }
}