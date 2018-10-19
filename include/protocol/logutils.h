#ifndef LOGUTILS_H
#define LOGUTILS_H
#include <string>

enum LogLevel {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

extern LogLevel logLevel;

void logPrint(LogLevel lv, std::string str);
#endif
