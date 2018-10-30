#include <utils/logutils.h>
#include <errno.h>
#include <string.h>

#define ERROR_PCAP(msg,p) {                                            \
    LOG(ERROR, std::string(msg) + " \033[36m[" + pcap_geterr(p) + "]\033[0m");\
}

#define ERROR_FATAL_PCAP(msg,p)   {                                    \
    LOG(FATAL, std::string(msg) + " \033[36m[" + pcap_geterr(p) + "]\033[0m"); \
    exit(-1);\
}

#define ERROR_SYSTEM(msg) {                                             \
    LOG(ERROR, std::string(msg) + " \033[36m[" + strerror(errno) + "]\033[0m"); \
}

#define ERROR_FATAL_SYSTEM(msg) {                                      \
    LOG(FATAL, std::string(msg) + " \033[36m[" + strerror(errno) + "]\033[0m");\
    exit(-1);\
}

#define ERROR_USER(msg) {                      \
    LOG(ERROR, std::string(msg));\
}

#define ERROR_FATAL_USER(msg) {  \
    LOG(FATAL, std::string(msg)); \
    exit(-1);\
}
