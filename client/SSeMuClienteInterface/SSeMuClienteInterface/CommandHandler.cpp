#include "pch.h"
#include "CommandHandler.h"
#include "Protocol.h"

void CommandHandler::handleCommand(const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(Command)) {
        std::cerr << "Received data is too small to contain a command." << std::endl;
        return;
    }

    Command command = static_cast<Command>(data[0]);
    auto it = handlers.find(command);
    if (it != handlers.end()) {
        it->second(data);
    }
    else {
        std::cerr << "No handler registered for command: " << static_cast<int>(command) << std::endl;
    }
}

void CommandHandler::registerHandler(Command command, std::function<void(const std::vector<uint8_t>&)> handler) {
    handlers[command] = handler;
}
