#pragma once
#include "pch.h"
#include "CommandHandler.h"
#include "ServerDisplayer.h"
#include "Protocol.h"

#define sServerDisplayer ServerDisplayer::GetInstance()

class RequestJewelsHandler {
public:
    static void handle(SOCKET clientSocket, const std::vector<uint8_t>& data) {
        if (data.size() < sizeof(RequestJewelsPacket)) {
            return;
        }

        RequestJewelsPacket* packet = reinterpret_cast<RequestJewelsPacket*>(const_cast<uint8_t*>(data.data()));
        std::string account(packet->account, sizeof(packet->account));

        // Aqui você deve buscar as informações de joias no banco de dados usando o packet->account
        ResponseJewelsPacket response;
        response.h.type = 0xC1;
        response.h.size = sizeof(response);
        response.h.head = static_cast<uint8_t>(Command::RESPONSE_JEWELS);
        response.command = Command::RESPONSE_JEWELS;
        response.BBless = 10; // Exemplo de valor, substituir pelos valores reais
        response.BSoul = 20;
        response.BLife = 30;
        response.BChaos = 40;
        response.BCreation = 50;
        response.BGuardian = 60;
        response.BHarmony = 70;
        response.BRefin = 80;
        response.BLowRefin = 90;
        response.BGemstone = 100;
        response.BPickBless = 110;
        response.BPickSoul = 120;
        response.BPickLife = 130;
        response.BPickChaos = 140;
        response.BPickCreation = 150;
        response.BPickGuardian = 160;
        response.BPickHarmony = 170;
        response.BPickRefin = 180;
        response.BPickLowRefin = 190;
        response.BPickGemstone = 200;

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
        DWORD Flags;
        if (!WSAGetOverlappedResult(clientSocket, &sendContext.overlapped, &bytesTransferred, FALSE, &Flags)) {
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
};
