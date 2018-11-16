#ifndef PTHREAD_H
#define PTHREAD_H

#include <pthread.h>
#include <utils/errorutils.h>

#define Pthread_cancel(thread, ...)                          \
    ({                                                       \
        ErrorBehavior _ebo = ErrorBehavior("");              \
        _ebo.from          = SOURCE_SYSTEM;                  \
        _Pthread_cancel0(thread, ##__VA_ARGS__, _ebo, 0, 0); \
    })
#define _Pthread_cancel0(thread, _behavior, _action, ...)                   \
    ({                                                                      \
        decltype(({ _behavior; }))* _decleb;                                \
        if (std::is_same<ErrorBehavior,                                     \
                         std::remove_pointer<decltype(_decleb)>::type>()) { \
            ErrorBehavior _eb0 = behaviorFrom(({ _behavior; }));            \
            _eb0.sys_msg       = "pthread_cancel";                          \
            if (pthread_cancel(thread) != 0) {                              \
                ERROR_WITH_BEHAVIOR(_eb0, _action);                         \
            }                                                               \
        } else {                                                            \
            printf("Too many args for Pthread_cancel\n");                   \
            exit(-1);                                                       \
        }                                                                   \
    })

#endif  // PTHREAD_H
