#include "pch.h"
#include "SocketManager.h"
#include "ServerDisplayer.h"
#include "CommandHandler.h"

#define sServerDisplayer ServerDisplayer::GetInstance()

SocketManager::SocketManager(size_t numThreads)
    : m_port(0), m_listen(INVALID_SOCKET), running(false), activeClients(0), threadPool(numThreads) {
    for (auto& client : clientActive) {
        client = std::make_unique<std::atomic<bool>>(false);
    }
}

SocketManager::~SocketManager() {
    stop();
}

bool SocketManager::start(unsigned short port) {
    this->m_port = port;
    running = true;

    if (!CreateListenSocket()) {
        sServerDisplayer.LogAddText(eLogColor::LOG_RED, "Failed to create listen socket.");
        return false;
    }

    sServerDisplayer.LogAddText(eLogColor::LOG_GREEN, std::format("Server started at port {}", this->m_port));

    messageThread = std::thread(&SocketManager::processMessages, this);
    acceptThread = std::thread(&SocketManager::acceptConnections, this);

    return true;
}

void SocketManager::stop() {
    running = false;
    closesocket(m_listen);
    queueCondVar.notify_all();
    if (messageThread.joinable()) {
        messageThread.join();
    }
    if (acceptThread.joinable()) {
        acceptThread.join();
    }
}

bool SocketManager::CreateListenSocket() {
    this->m_listen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (this->m_listen == INVALID_SOCKET) {
        sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSASocket() failed with error: {}", WSAGetLastError()));
        return false;
    }

    SOCKADDR_IN socketAddr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddr.sin_port = htons(this->m_port);

    if (bind(this->m_listen, (SOCKADDR*)&socketAddr, sizeof(socketAddr)) == SOCKET_ERROR) {
        sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("Bind() failed with error: {}", WSAGetLastError()));
        return false;
    }

    if (listen(this->m_listen, SOMAXCONN) == SOCKET_ERROR) {
        sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("Listen() failed with error: {}", WSAGetLastError()));
        return false;
    }

    return true;
}

void SocketManager::acceptConnections() {
    while (running) {
        SOCKET clientSocket = accept(this->m_listen, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (running) {
                sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("Accept failed with error: {}", WSAGetLastError()));
            }
            continue;
        }

        int clientIndex = -1;
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!(*clientActive[i])) {
                clientIndex = i;
                *clientActive[i] = true;
                break;
            }
        }

        if (clientIndex == -1) {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, "Maximum clients reached. Connection rejected.");
            closesocket(clientSocket);
            continue;
        }

        activeClients++;
        threadPool.enqueue([this, clientSocket, clientIndex] { ClientThread(this, clientSocket, clientIndex); });
    }
}

void SocketManager::ClientThread(SocketManager* manager, SOCKET clientSocket, int clientIndex) {
    manager->handleClient(clientSocket, clientIndex);
    *manager->clientActive[clientIndex] = false;
    manager->activeClients--;
}

void SocketManager::handleClient(SOCKET clientSocket, int clientIndex) {
    IO_RECV_CONTEXT recvContext;
    ZeroMemory(&recvContext.overlapped, sizeof(WSAOVERLAPPED));
    recvContext.overlapped.hEvent = WSACreateEvent();
    recvContext.wsabuf.buf = reinterpret_cast<char*>(recvContext.IoMainBuffer.buff);
    recvContext.wsabuf.len = MAX_MAIN_PACKET_SIZE;
    recvContext.IoType = IO_RECV;

    CommandHandler commandHandler;

    while (running) {
        DWORD RecvSize = 0, Flags = 0;
        if (WSARecv(clientSocket, &recvContext.wsabuf, 1, &RecvSize, &Flags, &recvContext.overlapped, NULL) == SOCKET_ERROR) {
            if (WSAGetLastError() != WSA_IO_PENDING) {
                sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSARecv() failed with error: {}", WSAGetLastError()));
                break;
            }
        }

        DWORD waitResult = WSAWaitForMultipleEvents(1, &recvContext.overlapped.hEvent, TRUE, WSA_INFINITE, FALSE);
        if (waitResult == WSA_WAIT_FAILED) {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSAWaitForMultipleEvents() failed with error: {}", WSAGetLastError()));
            break;
        }

        DWORD bytesTransferred;
        if (!WSAGetOverlappedResult(clientSocket, &recvContext.overlapped, &bytesTransferred, FALSE, &Flags)) {
            int error = WSAGetLastError();
            if (error == WSAECONNRESET) {
                sServerDisplayer.LogAddText(eLogColor::LOG_RED, "Connection reset by client.");
            }
            else {
                sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSAGetOverlappedResult() failed with error: {}", error));
            }
            break;
        }

        if (bytesTransferred == 0) {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, "Connection closed by client.");
            break;
        }

        // Process the received data
        std::vector<uint8_t> data(recvContext.wsabuf.buf, recvContext.wsabuf.buf + bytesTransferred);
        commandHandler.handle(clientSocket, data);

        // Prepare and send a response
        Response response;
        response.command = (BYTE)Command::HEARTBEAT;
        response.window = 0x00;

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
                break;
            }
        }

        waitResult = WSAWaitForMultipleEvents(1, &sendContext.overlapped.hEvent, TRUE, WSA_INFINITE, FALSE);
        if (waitResult == WSA_WAIT_FAILED) {
            sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSAWaitForMultipleEvents() failed with error: {}", WSAGetLastError()));
            break;
        }

        if (!WSAGetOverlappedResult(clientSocket, &sendContext.overlapped, &bytesTransferred, FALSE, &Flags)) {
            int error = WSAGetLastError();
            if (error == WSAECONNRESET) {
                sServerDisplayer.LogAddText(eLogColor::LOG_RED, "Connection reset by client.");
            }
            else {
                sServerDisplayer.LogAddText(eLogColor::LOG_RED, std::format("WSAGetOverlappedResult() failed with error: {}", error));
            }
            break;
        }

        WSACloseEvent(sendContext.overlapped.hEvent); // Ensure the event is closed
    }

    closesocket(clientSocket);
    WSACloseEvent(recvContext.overlapped.hEvent); // Ensure the event is closed
}

void SocketManager::processMessages() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondVar.wait(lock, [this] { return !messageQueue.empty() || !running; });

        if (!running) break;

        Message message = messageQueue.front();
        messageQueue.pop();
        lock.unlock();

        sServerDisplayer.LogAddText(eLogColor::LOG_GREEN, std::format("Processing message: Command {}, Account {}, Character {}", static_cast<int>(message.command), message.account, message.character));
    }
}
