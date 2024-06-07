#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <format>
#include <ctime>
#include <tlhelp32.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Psapi.h>
#include <iomanip>
#include <unordered_map>
#include <functional>