#ifndef LOGUTILS_H
#define LOGUTILS_H
#include <cstdio>
#include <ctime>
#include <string>

enum LogLevel { FATAL, ERROR, WARNING, INFO, DEBUG };

extern LogLevel logLevel;

static std::string color[] = { "31", "31", "33", "32", "37" };

static std::string header[] = { "FATAL", "ERROR", "WARNING", "INFO", "DEBUG" };

static std::string getTime() {
    time_t     now;
    struct tm* timenow;
    time(&now);
    timenow       = localtime(&now);
    std::string t = asctime(timenow);
    t.pop_back();
    return t;
}

inline void logPrint(LogLevel lv, std::string str) {
    fprintf(stderr, "\033[%sm[%-8s %s]\033[0m  %s\n", color[lv].c_str(),
            header[lv].c_str(), getTime().c_str(), str.c_str());
}

#ifndef NOLOG
#define LOG(lv, msg)                                                          \
    {                                                                         \
        char _buf[40];                                                        \
        sprintf(_buf, "%d", __LINE__);                                        \
        logPrint((lv), std::string("\033[35m[") + __FILE__ + ":" + _buf + " " \
                           + __FUNCTION__ + "]\033[0m " + (msg));             \
    }
#elif  // ifndef NOLOG
#define LOG
#endif  // ifndef NOLOG

#endif  // LOGUTILS_H
