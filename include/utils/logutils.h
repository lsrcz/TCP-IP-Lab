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

#ifndef NOLOG
#define LOG(lv,msg)                             \
    char buf[40];  \
    sprintf(buf, "%d", __LINE__);\
    logPrint((lv),std::string("\033[35m[") + __FILE__             \
             + ":" + buf + " " + __FUNCTION__ + "]\033[0m " + (msg))
#endif // ifndef NOLOG

#endif // LOGUTILS_H
