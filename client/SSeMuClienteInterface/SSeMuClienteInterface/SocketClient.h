#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "ISocketClient.h"
#include "CommandHandler.h"
#include <WinSock2.h>
#include <string>
#include <thread>
#include <atomic>

class SocketClient : public ISocketClient {
public:
    SocketClient();
    ~SocketClient();

    bool Connect(const std::string& serverAddress, unsigned short port) override;
    void Disconnect() override;
    bool Send(const char* message, int length) override;
    std::string Receive() override;
    bool IsConnected() const override;

private:
    void StartHeartbeat();
    void StopHeartbeat();
    void ProcessIncomingMessages();

    SOCKET clientSocket;
    std::thread heartbeatThread;
    std::thread messageThread;
    std::atomic<bool> running;
    CommandHandler commandHandler;
};

#endif // SOCKETCLIENT_H
