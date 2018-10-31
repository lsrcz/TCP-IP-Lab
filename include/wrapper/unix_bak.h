
/*
 * Assume all functions are non-fatal
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/errorutils.h>
#include <string>

#define Calloc(n, size, ...)                                                   \
    _Calloc(n, size, ##__VA_ARGS__, defaultBehavior("calloc"))
#define _Calloc(n, size, _behavior, ...)                                       \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    void *ptr;                                                                 \
    if ((ptr = calloc((n),(size))) == NULL) {                                  \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
ptr;                                                                           \
})

#define Close(fd, ...)                                                         \
    _Close(fd, ##__VA_ARGS__, defaultBehavior("close"))
#define _Close(fd, _behavior, ...)                                             \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (close((fd)) == -1) {                                                   \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
    }                                                                          \
})

#define Dup2(fd1, fd2, ...)                                                    \
    _Dup2(fd1, fd2, ##__VA_ARGS__, defaultBehavior("dup2"))
#define _Dup2(fd1, fd2, _behavior, ...)                                        \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (dup2((fd1), (fd2)) == -1) {                                            \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
    }                                                                          \
})

#define Fcntl(fd, cmd, arg, ...)                                               \
    _Fcntl(fd, cmd, arg, ##__VA_ARGS__, defaultBehavior("fcntl"))
#define _Fcntl(fd, cmd, arg, _behavior, ...)                                   \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    int n;                                                                     \
    if ((n = fcntl((fd), (cmd), (arg))) == -1) {                               \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
n;                                                                             \
})

#define Gettimeofday(tv, foo, ...)                                             \
    _Gettimeofday(tv, foo, ##__VA_ARGS__, defaultBehavior("gettimeofday"))
#define _Gettimeofday(tv, foo, _behavior, ...)                                 \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (gettimeofday((tv), (foo)) == -1) {                                     \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
})

#define Ioctl(fd, request, arg, ...)                                           \
    _Ioctl(fd, request, arg, ##__VA_ARGS__, defaultBehavior("ioctl"))
#define _Ioctl(fd, request, arg, _behavior, ...)                               \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    int n;                                                                     \
    if ((n = ioctl((fd), (request), (arg))) == -1) {                           \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
        }                                                                      \
    })

#define Fork(, ...)                                                            \
    _Fork(, ##__VA_ARGS__, defaultBehavior("fork"))
#define _Fork(, _behavior, ...)                                                \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    pid_t pid;                                                                 \
    if ((pid = fork()) == -1) {                                                \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
})

#define Malloc(size, ...)                                                      \
    _Malloc(size, ##__VA_ARGS__, defaultBehavior("malloc"))
#define _Malloc(size, _behavior, ...)                                          \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    void *ptr;                                                                 \
    if ((ptr = malloc((size))) == NULL) {                                      \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
    ptr;                                                                       \
})

#define Open(pathname, oflag, mode, ...)                                       \
    _Open(pathname, oflag, mode, ##__VA_ARGS__, defaultBehavior("open"))
#define _Open(pathname, oflag, mode, _behavior, ...)                           \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    int fd;                                                                    \
    if ((fd = open((pathname), (oflag), (mode))) == -1) {                      \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
        }                                                                      \
    })

#define Pipe(fds, ...)                                                         \
    _Pipe(fds, ##__VA_ARGS__, defaultBehavior("pipe"))
#define _Pipe(fds, _behavior, ...)                                             \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (pipe((fds)) < 0) {                                                     \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
        }                                                                      \
    })

#define Read(fd, ptr, nbytes, ...)                                             \
    _Read(fd, ptr, nbytes, ##__VA_ARGS__, defaultBehavior("read"))
#define _Read(fd, ptr, nbytes, _behavior, ...)                                 \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    ssize_t n;                                                                 \
    if ((n = read((fd), (ptr), (nbytes))) == -1) {                             \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
        }                                                                      \
        n;                                                                     \
    })

#define Sigaddset(set, signo, ...)                                             \
    _Sigaddset(set, signo, ##__VA_ARGS__, defaultBehavior("sigaddset"))
#define _Sigaddset(set, signo, _behavior, ...)                                 \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (sigaddset((set), (signo)) == -1) {                                     \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
})

#define Sigdelset(set, signo, ...)                                             \
    _Sigdelset(set, signo, ##__VA_ARGS__, defaultBehavior("sigdelset"))
#define _Sigdelset(set, signo, _behavior, ...)                                 \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (sigdelset((set), (signo)) == -1) {                                     \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
})

#define Sigemptyset(set, ...)                                                  \
    _Sigemptyset(set, ##__VA_ARGS__, defaultBehavior("sigemptyset"))
#define _Sigemptyset(set, _behavior, ...)                                      \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (sigemptyset((set)) == -1) {                                            \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
})

#define Sigfillset(set, ...)                                                   \
    _Sigfillset(set, ##__VA_ARGS__, defaultBehavior("sigfillset"))
#define _Sigfillset(set, _behavior, ...)                                       \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (sigfillset((set)) == -1) {                                             \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
})

#define Sigismember(set, signo, ...)                                           \
    _Sigismember(set, signo, ##__VA_ARGS__, defaultBehavior("sigismember"))
#define _Sigismember(set, signo, _behavior, ...)                               \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    int n;                                                                     \
    if ( (n = sigismember((set), (signo))) == -1) {                            \
    ERROR_WITH_BEHAVIOR(_behavior);                                            \
}                                                                              \
n;                                                                             \
})

#define Sigpending(set, ...)                                                   \
    _Sigpending(set, ##__VA_ARGS__, defaultBehavior("sigpending"))
#define _Sigpending(set, _behavior, ...)                                       \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (sigpending((set)) == -1) {                                             \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
})

#define Sigprocmask(how, set, oset, ...)                                       \
    _Sigprocmask(how, set, oset, ##__VA_ARGS__, defaultBehavior("sigprocmask"))
#define _Sigprocmask(how, set, oset, _behavior, ...)                           \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (sigprocmask((how), (set), (oset)) == -1) {                             \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
})

#define Wait(iptr, ...)                                                        \
    _Wait(iptr, ##__VA_ARGS__, defaultBehavior("wait"))
#define _Wait(iptr, _behavior, ...)                                            \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    pid_t pid;                                                                 \
    if ( (pid = wait((iptr))) == -1) {                                         \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
    pid;                                                                       \
})

#define Waitpid(pid, iptr, options, ...)                                       \
    _Waitpid(pid, iptr, options, ##__VA_ARGS__, defaultBehavior("waitpid"))
#define _Waitpid(pid, iptr, options, _behavior, ...)                           \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    pid_t retpid;                                                              \
    if ( (retpid = waitpid((pid), (iptr), (options))) == -1) {                 \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
})

#define Write(fd, ptr, nbytes, ...)                                            \
    _Write(fd, ptr, nbytes, ##__VA_ARGS__, defaultBehavior("write"))
#define _Write(fd, ptr, nbytes, _behavior, ...)                                \
({                                                                             \
    _behavior.from = ErrorBehavior::ErrorSource::SYSTEM                        \
    if (write((fd), (ptr), (nbytes)) != nbytes) {                              \
        ERROR_WITH_BEHAVIOR(_behavior);                                        \
    }                                                                          \
})
