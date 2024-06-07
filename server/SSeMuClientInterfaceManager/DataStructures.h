#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <format>
#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <array>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_MAIN_PACKET_SIZE 8192
#define MAX_SIDE_PACKET_SIZE 262144

#define MAX_ACCOUNT_LEN 20
#define MAX_CHARACTER_LEN 20

#define IO_RECV 0
#define IO_SEND 1

#define MAX_CLIENTS 1000

struct IO_MAIN_BUFFER {
    BYTE buff[MAX_MAIN_PACKET_SIZE];
    int size;
};

struct IO_SIDE_BUFFER {
    BYTE buff[MAX_SIDE_PACKET_SIZE];
    int size;
};

struct IO_CONTEXT {
    WSAOVERLAPPED overlapped;
    WSABUF wsabuf;
    int IoType;
    int IoSize;
};

struct IO_RECV_CONTEXT {
    WSAOVERLAPPED overlapped;
    WSABUF wsabuf;
    int IoType;
    int IoSize;
    IO_MAIN_BUFFER IoMainBuffer;
};

struct IO_SEND_CONTEXT {
    WSAOVERLAPPED overlapped;
    WSABUF wsabuf;
    int IoType;
    int IoSize;
    IO_MAIN_BUFFER IoMainBuffer;
    IO_SIDE_BUFFER IoSideBuffer;
};

struct Message {
    BYTE command;
    char account[MAX_ACCOUNT_LEN];
    char character[MAX_ACCOUNT_LEN];
};

struct Response {
    BYTE command;
    BYTE window;
};
