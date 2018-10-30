#include <protocol/logutils.h>
#include <ctime>
#include <cstdio>
#include <string>

std::string color[] = {
    "31",
    "31",
    "33",
    "32",
    "37"
};

std::string header[] = {
    "FATAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};

std::string getTime() {
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    std::string t = asctime(timenow);
    t.pop_back();
    return t;
}

void logPrint(LogLevel lv, std::string str) {
    printf("\033[%sm[%-8s %s]\033[0m  %s\n", color[lv].c_str(), header[lv].c_str(), getTime().c_str(), str.c_str());
}
