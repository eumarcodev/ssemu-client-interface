#pragma once
#include "pch.h"


enum eLogType
{
    LOG_GENERAL = 0,
};

class LogInfo {
public:
    LogInfo() : active(false), day(0), month(0), year(0), file(INVALID_HANDLE_VALUE) {}

    // Métodos getters e setters para acesso seguro aos membros
    bool isActive() const { return active; }
    void setActive(bool isActive) { active = isActive; }

    const std::string& getDirectory() const { return directory; }
    void setDirectory(const std::string& dir) { directory = dir; }

    int getDay() const { return day; }
    void setDay(int d) { day = d; }

    int getMonth() const { return month; }
    void setMonth(int m) { month = m; }

    int getYear() const { return year; }
    void setYear(int y) { year = y; }

    const std::string& getFilename() const { return filename; }
    void setFilename(const std::string& fname) { filename = fname; }

    HANDLE getFile() const { return file; }
    void setFile(HANDLE f) { file = f; }

private:
    bool active;
    std::string directory;
    int day;
    int month;
    int year;
    std::string filename;
    HANDLE file;
};