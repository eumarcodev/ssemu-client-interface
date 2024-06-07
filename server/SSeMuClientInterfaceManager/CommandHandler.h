#pragma once

#include "Protocol.h"
#include <WinSock2.h>
#include <map>
#include <vector>
#include <functional>

class CommandHandler {
public:
    CommandHandler();
    void handle(SOCKET clientSocket, const std::vector<uint8_t>& data);
    void registerHandler(Command command, std::function<void(SOCKET, const std::vector<uint8_t>&)> handler);

private:
    std::map<Command, std::function<void(SOCKET, const std::vector<uint8_t>&)>> handlers;
};

void handleHeartbeat(SOCKET clientSocket, const std::vector<uint8_t>& data);
void handleAccountInfo(SOCKET clientSocket, const std::vector<uint8_t>& data);
void handleRequestJewels(SOCKET clientSocket, const std::vector<uint8_t>& data);
