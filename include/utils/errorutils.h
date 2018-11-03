#ifndef ERRORUTILS_H
#define ERRORUTILS_H

#include <utils/logutils.h>
#include <errno.h>
#include <string.h>
#include <pcap.h>

enum ErrorSource {
    SOURCE_SYSTEM,
    SOURCE_PCAP,
    SOURCE_USER,
    SOURCE_PCAP_BUFFER
};

struct ErrorBehavior {
    const char *msg;
    bool fatalFlag;
    bool haveAction;
    ErrorSource from;
    const char *sys_msg;
    pcap_t *pcap;
    const char *buf;

    inline ErrorBehavior(const char *msg,
                         bool fatalFlag = false,
                         bool haveAction = false,
                         ErrorSource from = SOURCE_USER,
                         const char *sys_msg = "",
                         pcap_t *pcap = NULL,
                         const char *buf = NULL)
        : msg(msg), fatalFlag(fatalFlag), haveAction(haveAction), from(from),
        sys_msg(sys_msg), pcap(pcap), buf(buf) {}
    inline ErrorBehavior& setFlag(bool fatalFlag, bool haveAction, ErrorSource from) {
        this->fatalFlag = fatalFlag;
        this->haveAction = haveAction;
        this->from = from;
        return *this;
    }
    inline ErrorBehavior& setSysFatal() {
        this->fatalFlag = true;
        this->from = SOURCE_SYSTEM;
        return *this;
    }
    inline ErrorBehavior& setSysError() {
        this->fatalFlag = false;
        this->from = SOURCE_SYSTEM;
        return *this;
    }
    inline ErrorBehavior& setPcapFatal() {
        this->fatalFlag = true;
        this->from = SOURCE_PCAP;
        return *this;
    }
    inline ErrorBehavior& setPcapError() {
        this->fatalFlag = false;
        this->from = SOURCE_PCAP;
        return *this;
    }
    inline ErrorBehavior& setUserError() {
        this->fatalFlag = false;
        this->from = SOURCE_USER;
        return *this;
    }
    inline ErrorBehavior& setUserFatal() {
        this->fatalFlag = true;
        this->from = SOURCE_USER;
        return *this;
    }
    inline ErrorBehavior& setHaveAction() {
        this->haveAction = true;
        return *this;
    }
    inline ErrorBehavior& setNoAction() {
        this->haveAction = false;
        return *this;
    }
};

template <typename T>
inline
bool isErrorBehavior(T t) {
    return false;
}

template <>
inline
bool isErrorBehavior(ErrorBehavior p) {
    return true;
}

template <typename T>
inline
ErrorBehavior behaviorFrom(T t) {
    return ErrorBehavior("");
}

template <>
inline
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
    case SOURCE_PCAP_BUFFER:\
        LOG(_lv, std::string(_msg) + " \033[36m[" + _sys_msg + ": " + (behavior).buf + "]\033[0m");\
        break;\
    }\
    if ((behavior).haveAction) {\
        action;\
    }\
}

#endif // ERRORUTILS_H
