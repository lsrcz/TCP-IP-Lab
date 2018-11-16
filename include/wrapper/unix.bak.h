/*
 * Assume all functions are non-fatal
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <utils/errorutils.h>

#define Calloc(n, size)                             \
    ({                                              \
        void* _ptr;                                 \
        if ((_ptr = calloc((n), (size))) == NULL) { \
            ERROR_SYSTEM("calloc error");           \
        }                                           \
        _ptr;                                       \
    })

#define Close(fd)                        \
    ({                                   \
        if (close((fd)) == -1) {         \
            ERROR_SYSTEM("close error"); \
        }                                \
    })

#define Dup2(fd1, fd2)                  \
    ({                                  \
        if (dup2((fd1), (fd2)) == -1) { \
            ERROR_SYSTEM("dup2 error"); \
        }                               \
    })

#define Fcntl(fd, cmd, arg)                           \
    ({                                                \
        int _n;                                       \
        if ((_n = fcntl((fd), (cmd), (arg))) == -1) { \
            ERROR_SYSTEM("fcntl error");              \
        }                                             \
        _n;                                           \
    })

#define Gettimeofday(tv, foo)                   \
    ({                                          \
        if (gettimeofday((tv), (foo)) == -1) {  \
            ERROR_SYSTEM("gettimeofday error"); \
        }                                       \
    })

#define Ioctl(fd, request, arg)                           \
    ({                                                    \
        int _n;                                           \
        if ((_n = ioctl((fd), (request), (arg))) == -1) { \
            ERROR_SYSTEM("ioctl error");                  \
        }                                                 \
        _n;                                               \
    })

#define Fork()                          \
    ({                                  \
        pid_t _pid;                     \
        if ((_pid = fork()) == -1) {    \
            ERROR_SYSTEM("fork error"); \
        }                               \
        _pid;                           \
    })

#define Malloc(size)                           \
    ({                                         \
        void* _ptr;                            \
        if ((_ptr = malloc((size))) == NULL) { \
            ERROR_SYSTEM("malloc error");      \
        }                                      \
        _ptr;                                  \
    })

#define Open(pathname, oflag, mode)                                  \
    ({                                                               \
        int _fd;                                                     \
        if ((_fd = open((pathname), (oflag), (mode))) == -1) {       \
            ERROR_SYSTEM(std::string("open error for ") + pathname); \
        }                                                            \
        _fd;                                                         \
    })

#define Pipe(fds)                       \
    ({                                  \
        if (pipe((fds)) < 0) {          \
            ERROR_SYSTEM("pipe error"); \
        }                               \
    })

#define Read(fd, ptr, nbytes)                           \
    ({                                                  \
        ssize_t _n;                                     \
        if ((_n = read((fd), (ptr), (nbytes))) == -1) { \
            ERROR_SYSTEM("read error");                 \
        }                                               \
        _n;                                             \
    })

#define Sigaddset(set, signo)                  \
    ({                                         \
        if (sigaddset((set), (signo)) == -1) { \
            ERROR_SYSTEM("sigaddset error");   \
        }                                      \
    })

#define Sigdelset(set, signo)                  \
    ({                                         \
        if (sigdelset((set), (signo)) == -1) { \
            ERROR_SYSTEM("sigdelset error");   \
        }                                      \
    })

#define Sigemptyset(set)                       \
    ({                                         \
        if (sigemptyset((set)) == -1) {        \
            ERROR_SYSTEM("sigemptyset error"); \
        }                                      \
    })

#define Sigfillset(set)                       \
    ({                                        \
        if (sigfillset((set)) == -1) {        \
            ERROR_SYSTEM("sigfillset error"); \
        }                                     \
    })

#define Sigismember(set, signo)                         \
    ({                                                  \
        int _n;                                         \
        if ((_n = sigismember((set), (signo))) == -1) { \
            ERROR_SYSTEM("sigismember error");          \
        }                                               \
        _n;                                             \
    })

#define Sigpending(set)                       \
    ({                                        \
        if (sigpending((set)) == -1) {        \
            ERROR_SYSTEM("sigpending error"); \
        }                                     \
    })

#define Sigprocmask(how, set, oset)                    \
    ({                                                 \
        if (sigprocmask((how), (set), (oset)) == -1) { \
            ERROR_SYSTEM("sigprocmask error");         \
        }                                              \
    })

#define Wait(iptr)                         \
    ({                                     \
        pid_t _pid;                        \
        if ((_pid = wait((iptr))) == -1) { \
            ERROR_SYSTEM("wait error");    \
        }                                  \
        _pid;                              \
    })

#define Waitpid(pid, iptr, options)                                \
    ({                                                             \
        pid_t _retpid;                                             \
        if ((_retpid = waitpid((pid), (iptr), (options))) == -1) { \
            ERROR_SYSTEM("waitpid error");                         \
        }                                                          \
        _retpid;                                                   \
    })

#define Write(fd, ptr, nbytes)                        \
    ({                                                \
        if (write((fd), (ptr), (nbytes)) != nbytes) { \
            ERROR_SYSTEM("write error");              \
        }                                             \
    })
