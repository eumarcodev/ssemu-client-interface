#include "pch.h"
#include "CommandHandler.h"
#include "SocketManager.h"
#include "ServerDisplayer.h"

#define sServerDisplayer ServerDisplayer::GetInstance()

CommandHandler::CommandHandler() {
    registerHandler(Command::HEARTBEAT, handleHeartbeat);
    registerHandler(Command::ACCOUNT_INFO, handleAccountInfo);
    registerHandler(Command::REQUEST_JEWELS, handleRequestJewels);
}

void CommandHandler::handle(SOCKET clientSocket, const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(PBMSG_HEAD)) {
        return; // Invalid packet
    }

    const PBMSG_HEAD* header = reinterpret_cast<const PBMSG_HEAD*>(data.data());
    Command command = static_cast<Command>(header->head);

    auto it = handlers.find(command);
    if (it != handlers.end()) {
        it->second(clientSocket, data);
    }
}

void CommandHandler::registerHandler(Command command, std::function<void(SOCKET, const std::vector<uint8_t>&)> handler) {
    handlers[command] = handler;
}

void handleHeartbeat(SOCKET clientSocket, const std::vector<uint8_t>& data) {
    sServerDisplayer.LogAddText(eLogColor::LOG_GREEN, "Heartbeat received.");
}

void handleAccountInfo(SOCKET clientSocket, const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(AccountInfoPacket)) {
        sServerDisplayer.LogAddText(eLogColor::LOG_RED, "handleAccountInfo() received data too small for AccountInfoPacket");
        return;
    }

    const AccountInfoPacket* packet = reinterpret_cast<const AccountInfoPacket*>(data.data());

    std::string account(packet->account, packet->account + sizeof(packet->account));
    std::string character(packet->character, packet->character + sizeof(packet->character));

    // Remove potential trailing null characters
    account.erase(std::find(account.begin(), account.end(), '\0'), account.end());
    character.erase(std::find(character.begin(), character.end(), '\0'), character.end());

    sServerDisplayer.LogAddText(eLogColor::LOG_GREEN, std::format("handleAccountInfo() Account info received [Account: {}] [Character: {}]", account, character));

}

void handleRequestJewels(SOCKET clientSocket, const std::vector<uint8_t>& data) {
    sServerDisplayer.LogAddText(eLogColor::LOG_GREEN, "Request jewels received.");

    JewelsResponse response;
    response.h.type = 0xC1;
    response.h.size = sizeof(JewelsResponse);
    response.h.head = static_cast<uint8_t>(Command::JEWELS_RESPONSE);
    response.command = Command::JEWELS_RESPONSE;
    response.bless = 10; // Example values, replace with actual data
    response.soul = 20;
    response.life = 30;
    response.chaos = 40;
    response.creation = 50;
    response.guardian = 60;
    response.harmony = 70;
    response.refin = 80;
    response.lowRefin = 90;
    response.gemstone = 100;
    response.pickBless = 110;
    response.pickSoul = 120;
    response.pickLife = 130;
    response.pickChaos = 140;
    response.pickCreation = 150;
    response.pickGuardian = 160;
    response.pickHarmony = 170;
    response.pickRefin = 180;
    response.pickLowRefin = 190;
    response.pickGemstone = 200;

    IO_SEND_CONTEXT sendContext;
    ZeroMemory(&sendContext.overlapped, sizeof(WSAOVERLAPPED));
    sendContext.overlapped.hEvent = WSACreateEvent();
    sendContext.wsabuf.buf = reinterpret_cast<char*>(&response);
    sendContext.wsabuf.len = sizeof(response);
    sendContext.IoType = IO_SEND;

    DWORD SendSize = 0;
    if (WSASend(clientSocket, &sendContext.wsabuf, 1, &SendSize, 0, &sendContext.overlapped, NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSASend() failed with error: {}", WSAGetLastError()));
            return;
        }
    }

    DWORD waitResult = WSAWaitForMultipleEvents(1, &sendContext.overlapped.hEvent, TRUE, WSA_INFINITE, FALSE);
    if (waitResult == WSA_WAIT_FAILED) {
        sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSAWaitForMultipleEvents() failed with error: {}", WSAGetLastError()));
        return;
    }

    DWORD bytesTransferred;
    if (!WSAGetOverlappedResult(clientSocket, &sendContext.overlapped, &bytesTransferred, FALSE, NULL)) {
        int error = WSAGetLastError();
        if (error == WSAECONNRESET) {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, "Connection reset by client.");
        }
        else {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSAGetOverlappedResult() failed with error: {}", error));
        }
    }

    WSACloseEvent(sendContext.overlapped.hEvent); // Ensure the event is closed
}
