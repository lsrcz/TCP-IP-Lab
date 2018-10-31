#ifndef ERRORUTILS_H
#define ERRORUTILS_H

#include <utils/logutils.h>
#include <errno.h>
#include <string.h>
#include <pcap.h>

struct ErrorBehavior {
    const char *msg;
    const char *sys_msg;

    enum class ErrorSource {
        SYSTEM,
        PCAP,
        USER
    } from;

    enum class FatalFlag {
        FATAL,
        ERROR
    } fatalFlag;

    pcap_t *pcap;

    inline ErrorBehavior(const char *msg, ErrorSource from,  FatalFlag fatalFlag)
        : msg(msg), from(from), fatalFlag(fatalFlag) {}

    inline void setMsg(const char *msg) {
        this->msg = msg;
    }

    inline void setSysMsg(const char *sys_msg) {
        this->sys_msg = sys_msg;
    }

    inline void setSource(ErrorSource from) {
        this->from = from;
    }

    inline void setFatal(FatalFlag fatalFlag) {
        this->fatalFlag = fatalFlag;
    }

    inline void setPcap(pcap_t *pcap) {
        this->pcap = pcap;
    }
};

inline ErrorBehavior defaultBehavior(const char *msg) {
    return ErrorBehavior(msg,ErrorBehavior::ErrorSource::USER,ErrorBehavior::FatalFlag::ERROR);
}
inline ErrorBehavior fatalBehavior(const char *msg) {
    return ErrorBehavior(msg,ErrorBehavior::ErrorSource::USER,ErrorBehavior::FatalFlag::FATAL);
}

#define ERROR_WITH_BEHAVIOR(behavior) { \
    LogLevel lv = LogLevel::ERROR;\
    if (behavior.fatalFlag == ErrorBehavior::FatalFlag::FATAL) {\
        lv = LogLevel::FATAL;\
    }                                               \
    const char *msg = behavior.msg;\
    const char *sys_msg = behavior.sys_msg;\
    switch (behavior.from) {\
    case ErrorBehavior::ErrorSource::SYSTEM:\
        LOG(lv, std::string(msg) + " \033[36m[" + sys_msg + ": " + strerror(errno) + "]\033[0m"); \
        break;\
    case ErrorBehavior::ErrorSource::PCAP:\
        LOG(lv, std::string(msg) + " \033[36m[" + sys_msg + ": " + pcap_geterr(behavior.pcap) + "]\033[0m");       \
        break;\
    case ErrorBehavior::ErrorSource::USER:\
        LOG(lv, std::string(msg));     \
        break;\
    }\
    if (lv == LogLevel::FATAL) {\
        exit(-1);\
    }\
}

#endif // ERRORUTILS_H
