
/*
 * Assume all functions are non-fatal
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <utils/errorutils.h>

#define Calloc(n, size, ...)                            \
    ({                                                  \
        ErrorBehavior _ebo = ErrorBehavior("");         \
        _Calloc(n, size, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Calloc(n, size, _behavior, _action, ...)   \
    ({                                              \
        ErrorBehavior _eb = _behavior;              \
        _eb.from          = SOURCE_SYSTEM;          \
        _eb.sys_msg       = "Calloc";               \
        void* _ptr;                                 \
        if ((_ptr = calloc((n), (size))) == NULL) { \
            ERROR_WITH_BEHAVIOR(_eb, _action);      \
        }                                           \
        _ptr;                                       \
    })

#define Close(fd, ...)                            \
    ({                                            \
        ErrorBehavior _ebo = ErrorBehavior("");   \
        _Close(fd, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Close(fd, _behavior, _action, ...)    \
    ({                                         \
        ErrorBehavior _eb = _behavior;         \
        _eb.from          = SOURCE_SYSTEM;     \
        _eb.sys_msg       = "Close";           \
        if (close((fd)) == -1) {               \
            ERROR_WITH_BEHAVIOR(_eb, _action); \
        }                                      \
    })

#define Dup2(fd1, fd2, ...)                            \
    ({                                                 \
        ErrorBehavior _ebo = ErrorBehavior("");        \
        _Dup2(fd1, fd2, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Dup2(fd1, fd2, _behavior, _action, ...) \
    ({                                           \
        ErrorBehavior _eb = _behavior;           \
        _eb.from          = SOURCE_SYSTEM;       \
        _eb.sys_msg       = "Dup2";              \
        if (dup2((fd1), (fd2)) == -1) {          \
            ERROR_WITH_BEHAVIOR(_eb, _action);   \
        }                                        \
    })

#define Fcntl(fd, cmd, arg, ...)                            \
    ({                                                      \
        ErrorBehavior _ebo = ErrorBehavior("");             \
        _Fcntl(fd, cmd, arg, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Fcntl(fd, cmd, arg, _behavior, _action, ...) \
    ({                                                \
        ErrorBehavior _eb = _behavior;                \
        _eb.from          = SOURCE_SYSTEM;            \
        _eb.sys_msg       = "Fcntl";                  \
        int _n;                                       \
        if ((_n = fcntl((fd), (cmd), (arg))) == -1) { \
            ERROR_WITH_BEHAVIOR(_eb, _action);        \
        }                                             \
        _n;                                           \
    })

#define Gettimeofday(tv, foo, ...)                            \
    ({                                                        \
        ErrorBehavior _ebo = ErrorBehavior("");               \
        _Gettimeofday(tv, foo, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Gettimeofday(tv, foo, _behavior, _action, ...) \
    ({                                                  \
        ErrorBehavior _eb = _behavior;                  \
        _eb.from          = SOURCE_SYSTEM;              \
        _eb.sys_msg       = "Gettimeofday";             \
        if (gettimeofday((tv), (foo)) == -1) {          \
            ERROR_WITH_BEHAVIOR(_eb, _action);          \
        }                                               \
    })

#define Ioctl(fd, request, arg, ...)                            \
    ({                                                          \
        ErrorBehavior _ebo = ErrorBehavior("");                 \
        _Ioctl(fd, request, arg, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Ioctl(fd, request, arg, _behavior, _action, ...) \
    ({                                                    \
        ErrorBehavior _eb = _behavior;                    \
        _eb.from          = SOURCE_SYSTEM;                \
        _eb.sys_msg       = "Ioctl";                      \
        int _n;                                           \
        if ((_n = ioctl((fd), (request), (arg))) == -1) { \
            ERROR_WITH_BEHAVIOR(_eb, _action);            \
        }                                                 \
        _n;                                               \
    })

#define Fork(...)                               \
    ({                                          \
        ErrorBehavior _ebo = ErrorBehavior(""); \
        _Fork(##__VA_ARGS__, _ebo, exit(0));    \
    })
#define _Fork(_behavior, _action, ...)         \
    ({                                         \
        ErrorBehavior _eb = _behavior;         \
        _eb.sys_msg       = "Fork";            \
        _eb.from          = SOURCE_SYSTEM;     \
        pid_t _pid;                            \
        if ((_pid = fork()) == -1) {           \
            ERROR_WITH_BEHAVIOR(_eb, _action); \
        }                                      \
        _pid;                                  \
    })

#define Malloc(size, ...)                            \
    ({                                               \
        ErrorBehavior _ebo = ErrorBehavior("");      \
        _Malloc(size, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Malloc(size, _behavior, _action, ...) \
    ({                                         \
        ErrorBehavior _eb = _behavior;         \
        _eb.from          = SOURCE_SYSTEM;     \
        _eb.sys_msg       = "Malloc";          \
        void* _ptr;                            \
        if ((_ptr = malloc((size))) == NULL) { \
            ERROR_WITH_BEHAVIOR(_eb, _action); \
        }                                      \
        _ptr;                                  \
    })

#define Open(pathname, oflag, mode, ...)                            \
    ({                                                              \
        ErrorBehavior _ebo = ErrorBehavior("");                     \
        _Open(pathname, oflag, mode, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Open(pathname, oflag, mode, _behavior, _action, ...)  \
    ({                                                         \
        ErrorBehavior _eb = _behavior;                         \
        _eb.from          = SOURCE_SYSTEM;                     \
        _eb.sys_msg       = "Open";                            \
        int _fd;                                               \
        if ((_fd = open((pathname), (oflag), (mode))) == -1) { \
            ERROR_WITH_BEHAVIOR(_eb, _action);                 \
        }                                                      \
        _fd;                                                   \
    })

#define Pipe(fds, ...)                            \
    ({                                            \
        ErrorBehavior _ebo = ErrorBehavior("");   \
        _Pipe(fds, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Pipe(fds, _behavior, _action, ...)    \
    ({                                         \
        ErrorBehavior _eb = _behavior;         \
        _eb.from          = SOURCE_SYSTEM;     \
        _eb.sys_msg       = "Pipe";            \
        if (pipe((fds)) < 0) {                 \
            ERROR_WITH_BEHAVIOR(_eb, _action); \
        }                                      \
    })

#define Read(fd, ptr, nbytes, ...)                            \
    ({                                                        \
        ErrorBehavior _ebo = ErrorBehavior("");               \
        _Read(fd, ptr, nbytes, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Read(fd, ptr, nbytes, _behavior, _action, ...) \
    ({                                                  \
        ErrorBehavior _eb = _behavior;                  \
        _eb.from          = SOURCE_SYSTEM;              \
        _eb.sys_msg       = "Read";                     \
        ssize_t _n;                                     \
        if ((_n = read((fd), (ptr), (nbytes))) == -1) { \
            ERROR_WITH_BEHAVIOR(_eb, _action);          \
        }                                               \
        _n;                                             \
    })

#define Sigaddset(set, signo, ...)                            \
    ({                                                        \
        ErrorBehavior _ebo = ErrorBehavior("");               \
        _Sigaddset(set, signo, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigaddset(set, signo, _behavior, _action, ...) \
    ({                                                  \
        ErrorBehavior _eb = _behavior;                  \
        _eb.from          = SOURCE_SYSTEM;              \
        _eb.sys_msg       = "Sigaddset";                \
        if (sigaddset((set), (signo)) == -1) {          \
            ERROR_WITH_BEHAVIOR(_eb, _action);          \
        }                                               \
    })

#define Sigdelset(set, signo, ...)                            \
    ({                                                        \
        ErrorBehavior _ebo = ErrorBehavior("");               \
        _Sigdelset(set, signo, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigdelset(set, signo, _behavior, _action, ...) \
    ({                                                  \
        ErrorBehavior _eb = _behavior;                  \
        _eb.from          = SOURCE_SYSTEM;              \
        _eb.sys_msg       = "Sigdelset";                \
        if (sigdelset((set), (signo)) == -1) {          \
            ERROR_WITH_BEHAVIOR(_eb, _action);          \
        }                                               \
    })

#define Sigemptyset(set, ...)                            \
    ({                                                   \
        ErrorBehavior _ebo = ErrorBehavior("");          \
        _Sigemptyset(set, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigemptyset(set, _behavior, _action, ...) \
    ({                                             \
        ErrorBehavior _eb = _behavior;             \
        _eb.from          = SOURCE_SYSTEM;         \
        _eb.sys_msg       = "Sigemptyset";         \
        if (sigemptyset((set)) == -1) {            \
            ERROR_WITH_BEHAVIOR(_eb, _action);     \
        }                                          \
    })

#define Sigfillset(set, ...)                            \
    ({                                                  \
        ErrorBehavior _ebo = ErrorBehavior("");         \
        _Sigfillset(set, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigfillset(set, _behavior, _action, ...) \
    ({                                            \
        ErrorBehavior _eb = _behavior;            \
        _eb.from          = SOURCE_SYSTEM;        \
        _eb.sys_msg       = "Sigfillset";         \
        if (sigfillset((set)) == -1) {            \
            ERROR_WITH_BEHAVIOR(_eb, _action);    \
        }                                         \
    })

#define Sigismember(set, signo, ...)                            \
    ({                                                          \
        ErrorBehavior _ebo = ErrorBehavior("");                 \
        _Sigismember(set, signo, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigismember(set, signo, _behavior, _action, ...) \
    ({                                                    \
        ErrorBehavior _eb = _behavior;                    \
        _eb.from          = SOURCE_SYSTEM;                \
        _eb.sys_msg       = "Sigismember";                \
        int _n;                                           \
        if ((_n = sigismember((set), (signo))) == -1) {   \
            ERROR_WITH_BEHAVIOR(_eb, _action);            \
        }                                                 \
        _n;                                               \
    })

#define Sigpending(set, ...)                            \
    ({                                                  \
        ErrorBehavior _ebo = ErrorBehavior("");         \
        _Sigpending(set, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigpending(set, _behavior, _action, ...) \
    ({                                            \
        ErrorBehavior _eb = _behavior;            \
        _eb.from          = SOURCE_SYSTEM;        \
        _eb.sys_msg       = "Sigpending";         \
        if (sigpending((set)) == -1) {            \
            ERROR_WITH_BEHAVIOR(_eb, _action);    \
        }                                         \
    })

#define Sigprocmask(how, set, oset, ...)                            \
    ({                                                              \
        ErrorBehavior _ebo = ErrorBehavior("");                     \
        _Sigprocmask(how, set, oset, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Sigprocmask(how, set, oset, _behavior, _action, ...) \
    ({                                                        \
        ErrorBehavior _eb = _behavior;                        \
        _eb.from          = SOURCE_SYSTEM;                    \
        _eb.sys_msg       = "Sigprocmask";                    \
        if (sigprocmask((how), (set), (oset)) == -1) {        \
            ERROR_WITH_BEHAVIOR(_eb, _action);                \
        }                                                     \
    })

#define Wait(iptr, ...)                            \
    ({                                             \
        ErrorBehavior _ebo = ErrorBehavior("");    \
        _Wait(iptr, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Wait(iptr, _behavior, _action, ...)   \
    ({                                         \
        ErrorBehavior _eb = _behavior;         \
        _eb.from          = SOURCE_SYSTEM;     \
        _eb.sys_msg       = "Wait";            \
        pid_t _pid;                            \
        if ((_pid = wait((iptr))) == -1) {     \
            ERROR_WITH_BEHAVIOR(_eb, _action); \
        }                                      \
        _pid;                                  \
    })

#define Waitpid(pid, iptr, options, ...)                            \
    ({                                                              \
        ErrorBehavior _ebo = ErrorBehavior("");                     \
        _Waitpid(pid, iptr, options, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Waitpid(pid, iptr, options, _behavior, _action, ...)      \
    ({                                                             \
        ErrorBehavior _eb = _behavior;                             \
        _eb.from          = SOURCE_SYSTEM;                         \
        _eb.sys_msg       = "Waitpid";                             \
        pid_t _retpid;                                             \
        if ((_retpid = waitpid((pid), (iptr), (options))) == -1) { \
            ERROR_WITH_BEHAVIOR(_eb, _action);                     \
        }                                                          \
        _retpid;                                                   \
    })

#define Write(fd, ptr, nbytes, ...)                            \
    ({                                                         \
        ErrorBehavior _ebo = ErrorBehavior("");                \
        _Write(fd, ptr, nbytes, ##__VA_ARGS__, _ebo, exit(0)); \
    })
#define _Write(fd, ptr, nbytes, _behavior, _action, ...) \
    ({                                                   \
        ErrorBehavior _eb = _behavior;                   \
        _eb.from          = SOURCE_SYSTEM;               \
        _eb.sys_msg       = "Write";                     \
        if (write((fd), (ptr), (nbytes)) != nbytes) {    \
            ERROR_WITH_BEHAVIOR(_eb, _action);           \
        }                                                \
    })
