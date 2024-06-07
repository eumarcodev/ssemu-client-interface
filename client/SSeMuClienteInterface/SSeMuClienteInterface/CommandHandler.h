#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include "Protocol.h"

class CommandHandler {
public:
    void handleCommand(const std::vector<uint8_t>& data);
    void registerHandler(Command command, std::function<void(const std::vector<uint8_t>&)> handler);

private:
    std::unordered_map<Command, std::function<void(const std::vector<uint8_t>&)>> handlers;
};

#endif // COMMANDHANDLER_H
