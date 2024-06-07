#pragma once

#include <cstdint>

enum class Command : uint8_t {
    HEARTBEAT = 0x01,
    ACCOUNT_INFO = 0x02,
    REQUEST_JEWELS = 0x03,
    JEWELS_RESPONSE = 0x04,
};

#pragma pack(push, 1)

struct PBMSG_HEAD {
    uint8_t type;
    uint8_t size;
    uint8_t head;
};

struct HeartbeatMessage {
    PBMSG_HEAD h;
    Command command;
};

struct AccountInfoPacket {
    PBMSG_HEAD h;
    Command command;
    char account[20];
    char character[20];
};

struct RequestJewelsPacket {
    PBMSG_HEAD h;
    Command command;
    char account[20];
};

struct JewelsResponse {
    PBMSG_HEAD h;
    Command command;
    int bless;
    int soul;
    int life;
    int chaos;
    int creation;
    int guardian;
    int harmony;
    int refin;
    int lowRefin;
    int gemstone;
    int pickBless;
    int pickSoul;
    int pickLife;
    int pickChaos;
    int pickCreation;
    int pickGuardian;
    int pickHarmony;
    int pickRefin;
    int pickLowRefin;
    int pickGemstone;
};

#pragma pack(pop)
