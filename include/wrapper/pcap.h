#ifndef PCAP_H
#define PCAP_H

#include <pcap/pcap.h>
#include <utils/errorutils.h>

#define Pcap_create(source, errbuf, ...) ({                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_PCAP_BUFFER;                                            \
    _Pcap_create0(source, errbuf, ##__VA_ARGS__, _ebo, 0, 0);                  \
})
#define _Pcap_create0(source, errbuf, _behavior, _action, ...) ({              \
    pcap_t *_pcap;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pcap_create";                                          \
        if ((_pcap = pcap_create(source, errbuf)) == NULL) {                   \
            _eb0.buf = errbuf;                                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pcap_create\n");                             \
        exit(-1);                                                              \
    }                                                                          \
    _pcap;                                                                     \
})

#define Pcap_activate(p, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_PCAP;                                                   \
    _Pcap_activate0(p, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Pcap_activate0(p, _behavior, _action, ...) ({                         \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pcap_activate";                                        \
        if (pcap_activate(p) != 0) {                                           \
            _eb0.pcap = p;                                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pcap_activate\n");                           \
        exit(-1);                                                              \
    }                                                                          \
})

#define Pcap_list_datalinks(p, dlt_buf, ...) ({                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_PCAP;                                                   \
    _Pcap_list_datalinks0(p, dlt_buf, ##__VA_ARGS__, _ebo, 0, 0);              \
})
#define _Pcap_list_datalinks0(p, dlt_buf, _behavior, _action, ...) ({          \
    int _num;                                                                  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pcap_list_datalinks";                                  \
        if ((_num = pcap_list_datalinks(p, dlt_buf)) == -1) {                  \
            _eb0.pcap = p;                                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pcap_list_datalinks\n");                     \
        exit(-1);                                                              \
    }                                                                          \
    _num;                                                                      \
})

#define Pcap_inject(p, buf, size, ...) ({                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_PCAP;                                                   \
    _Pcap_inject0(p, buf, size, ##__VA_ARGS__, _ebo, 0, 0);                    \
})
#define _Pcap_inject0(p, buf, size, _behavior, _action, ...) ({                \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pcap_inject";                                          \
        if (pcap_inject(p, buf, size) != size) {                               \
            _eb0.pcap = p;                                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pcap_inject\n");                             \
        exit(-1);                                                              \
    }                                                                          \
})

#define Pcap_loop(p, cnt, callback, user, ...) ({                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_PCAP;                                                   \
    _Pcap_loop0(p, cnt, callback, user, ##__VA_ARGS__, _ebo, 0, 0);            \
})
#define _Pcap_loop0(p, cnt, callback, user, _behavior, _action, ...) ({        \
    _ret;                                                                      \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pcap_loop";                                            \
        _ret = pcap_loop(p, cnt, callback, user);                              \
        if (_ret == -1) {                                                      \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        } else if (_ret == -2) {                                               \
            LOG(WARNING, "pcap_loop interrupted");                             \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pcap_loop\n");                               \
        exit(-1);                                                              \
    }                                                                          \
})

#endif // PCAP_H

