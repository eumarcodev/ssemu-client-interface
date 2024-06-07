#pragma once
#include "DataStructures.h"
#include "ThreadPool.h"
#include "CommandHandler.h"

class SocketManager {
public:
    SocketManager(size_t numThreads);
    ~SocketManager();
    bool start(unsigned short port);
    void stop();
    void processMessages();

private:
    bool CreateListenSocket();
    static void ClientThread(SocketManager* manager, SOCKET clientSocket, int clientIndex);
    void handleClient(SOCKET clientSocket, int clientIndex);
    void acceptConnections();

    unsigned short m_port;
    SOCKET m_listen;
    std::atomic<bool> running;
    std::atomic<int> activeClients;
    std::array<std::unique_ptr<std::atomic<bool>>, MAX_CLIENTS> clientActive;
    std::queue<Message> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    ThreadPool threadPool;
    std::thread messageThread;
    std::thread acceptThread;

    CommandHandler commandHandler;
};
