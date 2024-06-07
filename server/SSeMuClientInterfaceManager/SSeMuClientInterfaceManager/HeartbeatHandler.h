#pragma once

#include "CommandHandler.h"

class HeartbeatHandler {
public:
    static void handle(SOCKET clientSocket, const std::vector<uint8_t>& data) {
        // Nada a fazer para heartbeat
    }
};
