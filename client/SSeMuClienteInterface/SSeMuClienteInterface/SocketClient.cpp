#include "pch.h"
#include "SocketClient.h"
#include "Protocol.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

SocketClient::SocketClient() : clientSocket(INVALID_SOCKET), running(false) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }

    // Register command handlers
    commandHandler.registerHandler(Command::HEARTBEAT, [this](const std::vector<uint8_t>& data) {
        std::cout << "Heartbeat received." << std::endl;
        });

    commandHandler.registerHandler(Command::ACCOUNT_INFO, [this](const std::vector<uint8_t>& data) {
        std::cout << "Account info received." << std::endl;
        });
}

SocketClient::~SocketClient() {
    Disconnect();
    WSACleanup();
}

bool SocketClient::Connect(const std::string& serverAddress, unsigned short port) {
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

    int result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connection to server failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    running = true;
    StartHeartbeat();
    messageThread = std::thread(&SocketClient::ProcessIncomingMessages, this);

    return true;
}

void SocketClient::Disconnect() {
    if (running) {
        running = false;
        StopHeartbeat();
        if (messageThread.joinable()) {
            messageThread.join();
        }
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
    }
}

bool SocketClient::Send(const char* message, int length) {
    int result = send(clientSocket, message, length, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

std::string SocketClient::Receive() {
    char buffer[512];
    int result = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (result > 0) {
        return std::string(buffer, result);
    }
    else if (result == 0) {
        std::cerr << "Connection closed" << std::endl;
    }
    else {
        std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
    }
    return "";
}

bool SocketClient::IsConnected() const {
    return running && clientSocket != INVALID_SOCKET;
}

void SocketClient::StartHeartbeat() {
    heartbeatThread = std::thread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if (running) {
                HeartbeatMessage heartbeat;
                heartbeat.h.type = 0xC1;
                heartbeat.h.size = sizeof(heartbeat);
                heartbeat.h.head = static_cast<uint8_t>(Command::HEARTBEAT);
                heartbeat.command = Command::HEARTBEAT;
                if (send(clientSocket, reinterpret_cast<const char*>(&heartbeat), sizeof(heartbeat), 0) == SOCKET_ERROR) {
                    std::cerr << "Heartbeat send failed: " << WSAGetLastError() << std::endl;
                }
            }
        }
        });
}

void SocketClient::StopHeartbeat() {
    if (heartbeatThread.joinable()) {
        heartbeatThread.join();
    }
}

void SocketClient::ProcessIncomingMessages() {
    while (running) {
        std::string data = Receive();
        if (!data.empty()) {
            std::vector<uint8_t> buffer(data.begin(), data.end());
            commandHandler.handleCommand(buffer);
        }
    }
}
