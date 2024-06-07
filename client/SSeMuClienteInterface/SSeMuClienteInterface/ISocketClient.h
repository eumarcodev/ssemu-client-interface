#ifndef ISOCKETCLIENT_H
#define ISOCKETCLIENT_H

#include <string>

class ISocketClient {
public:
    virtual ~ISocketClient() = default;
    virtual bool Connect(const std::string& serverAddress, unsigned short port) = 0;
    virtual void Disconnect() = 0;
    virtual bool Send(const char* message, int length) = 0;
    virtual std::string Receive() = 0;
    virtual bool IsConnected() const = 0; // Adicione esta função virtual pura aqui
};

#endif // ISOCKETCLIENT_H
