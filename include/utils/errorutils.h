#ifndef ERRORUTILS_H
#define ERRORUTILS_H

#include <utils/logutils.h>
#include <errno.h>
#include <string.h>
#include <pcap.h>

enum ErrorSource {
    SOURCE_SYSTEM,
    SOURCE_PCAP,
    SOURCE_USER
};

enum ActionType {
    NOACTION,
    USER_ACTION,
    RETURN_MINUS_ONE,
    RETURN_ZERO,
    NORMAL_EXIT,
    EXIT_MINUS_ONE
};

struct ErrorBehavior {
    const char *msg;
    const char *sys_msg;
    bool fatalFlag;
    ErrorSource from;
    ActionType actionType;
    pcap_t *pcap;

    inline ErrorBehavior(const char *msg,
                         bool fatalFlag = false,
                         ActionType actionType = NOACTION,
                         ErrorSource from = SOURCE_USER,
                         const char *sys_msg = "",
                         pcap_t *pcap = NULL)
        : msg(msg), from(from), fatalFlag(fatalFlag),
        actionType(actionType), sys_msg(sys_msg), pcap(pcap) {}

};

template <typename T>
bool isErrorBehavior(T t) {
    return false;
}

template <>
bool isErrorBehavior(ErrorBehavior p) {
    return true;
}

template <typename T>
ErrorBehavior behaviorFrom(T t) {
    return ErrorBehavior("");
}

template <>
ErrorBehavior behaviorFrom(ErrorBehavior eb) {
    return eb;
}

#define ERROR_WITH_BEHAVIOR(behavior, action) {        \
    LogLevel _lv = LogLevel::ERROR;\
    if ((behavior).fatalFlag) {    \
        _lv = LogLevel::FATAL;\
    }                                               \
    const char *_msg = (behavior).msg;              \
    const char *_sys_msg = (behavior).sys_msg;  \
    switch ((behavior).from) {                  \
    case SOURCE_SYSTEM:\
        LOG(_lv, std::string(_msg) + " \033[36m[" + _sys_msg + ": " + strerror(errno) + "]\033[0m"); \
        break;\
    case SOURCE_PCAP:\
        LOG(_lv, std::string(_msg) + " \033[36m[" + _sys_msg + ": " + pcap_geterr((behavior).pcap) + "]\033[0m"); \
        break;\
    case SOURCE_USER:\
        LOG(_lv, std::string(_msg));     \
        break;\
    }\
    switch ((behavior).actionType) {\
    case USER_ACTION:\
        {\
            action;\
        }\
        break;\
    case RETURN_ZERO:\
        return 0;\
    case RETURN_MINUS_ONE:\
        return -1;\
    case NORMAL_EXIT:\
        exit(0);\
    case EXIT_MINUS_ONE:\
        exit(-1);\
    default:\
        break;\
    }\
}

#endif // ERRORUTILS_H
