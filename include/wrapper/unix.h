#ifndef UNIX_H
#define UNIX_H

#include <stdlib.h>
#include <utils/errorutils.h>

#define Malloc(size, ...)                          \
    ({                                             \
        ErrorBehavior _ebo = ErrorBehavior("");    \
        _ebo.from          = SOURCE_SYSTEM;        \
        _Malloc0(size, ##__VA_ARGS__, _ebo, 0, 0); \
    })
#define _Malloc0(size, _behavior, _action, ...)                             \
    ({                                                                      \
        void*                       _ptr;                                   \
        decltype(({ _behavior; }))* _decleb;                                \
        if (std::is_same<ErrorBehavior,                                     \
                         std::remove_pointer<decltype(_decleb)>::type>()) { \
            ErrorBehavior _eb0 = behaviorFrom(({ _behavior; }));            \
            _eb0.sys_msg       = "malloc";                                  \
            if ((_ptr = malloc(size)) == NULL) {                            \
                ERROR_WITH_BEHAVIOR(_eb0, _action);                         \
            }                                                               \
        } else {                                                            \
            printf("Too many args for Malloc\n");                           \
            exit(-1);                                                       \
        }                                                                   \
        _ptr;                                                               \
    })

#endif  // UNIX_H
