#ifndef UNIX_FILE_H
#define UNIX_FILE_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <utils/errorutils.h>

#define Open(path, oflag, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Open";                                                     \
    _Open0(path, oflag, ##__VA_ARGS__, _ebo, 0, 0, 0);                         \
})
#define _Open0(path, oflag, _behavior, _action, ...) ({                        \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_fd = open(path, oflag)) == -1) {                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        _Open1(path, oflag, _behavior, _action, ##__VA_ARGS__);                \
    }                                                                          \
    _fd;                                                                       \
})
#define _Open1(path, oflag, _a0, _behavior, _action, ...) ({                   \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb1 = behaviorFrom(_behavior);                          \
        if ((_fd = open(path, oflag, _a0)) == -1) {                            \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Open\n");                                    \
    }                                                                          \
    _fd;                                                                       \
})

#define Openat(fd, path, oflag, ...) ({                                        \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Openat";                                                   \
    _Openat0(fd, path, oflag, ##__VA_ARGS__, _ebo, 0, 0, 0);                   \
})
#define _Openat0(fd, path, oflag, _behavior, _action, ...) ({                  \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_fd = openat(fd, path, oflag)) == -1) {                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        _Openat1(fd, path, oflag, _behavior, _action, ##__VA_ARGS__);          \
    }                                                                          \
    _fd;                                                                       \
})
#define _Openat1(fd, path, oflag, _a0, _behavior, _action, ...) ({             \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb1 = behaviorFrom(_behavior);                          \
        if ((_fd = openat(fd, path, oflag, _a0)) == -1) {                      \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Openat\n");                                  \
    }                                                                          \
    _fd;                                                                       \
})

#define Creat(path, mode, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Creat";                                                    \
    _Creat0(path, mode, ##__VA_ARGS__, _ebo, 0, 0);                            \
})
#define _Creat0(path, mode, _behavior, _action, ...) ({                        \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_fd = creat(path, mode)) == -1) {                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Creat\n");                                   \
    }                                                                          \
    _fd;                                                                       \
})

#define Close(fd, ...) ({                                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Close";                                                    \
    _Close0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                    \
})
#define _Close0(fd, _behavior, _action, ...) ({                                \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (close(fd) == -1) {                                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Close\n");                                   \
    }                                                                          \
})

#define Lseek(fd, offset, whence, ...) ({                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Lseek";                                                    \
    _Lseek0(fd, offset, whence, ##__VA_ARGS__, _ebo, 0, 0);                    \
})
#define _Lseek0(fd, offset, whence, _behavior, _action, ...) ({                \
    off_t _off;                                                                \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_off = lseek(fd, offset, whence)) == -1) {                        \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Lseek\n");                                   \
    }                                                                          \
    _off;                                                                      \
})

#define Read(fd, buf, nbytes, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Read";                                                     \
    _Read0(fd, buf, nbytes, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Read0(fd, buf, nbytes, _behavior, _action, ...) ({                    \
    ssize_t _size;                                                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_size = read(fd, buf, nbytes)) == -1) {                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Read\n");                                    \
    }                                                                          \
    _size;                                                                     \
})

#define Write(fd, buf, nbytes, ...) ({                                         \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Write";                                                    \
    _Write0(fd, buf, nbytes, ##__VA_ARGS__, _ebo, 0, 0);                       \
})
#define _Write0(fd, buf, nbytes, _behavior, _action, ...) ({                   \
    ssize_t _size;                                                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_size = write(fd, buf, nbytes)) == -1) {                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Write\n");                                   \
    }                                                                          \
    _size;                                                                     \
})

#define Pread(fd, buf, nbytes, offset, ...) ({                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Pread";                                                    \
    _Pread0(fd, buf, nbytes, offset, ##__VA_ARGS__, _ebo, 0, 0);               \
})
#define _Pread0(fd, buf, nbytes, offset, _behavior, _action, ...) ({           \
    ssize_t _size;                                                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_size = pread(fd, buf, nbytes, offset)) == -1) {                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pread\n");                                   \
    }                                                                          \
    _size;                                                                     \
})

#define Pwrite(fd, buf, nbytes, offset, ...) ({                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Pwrite";                                                   \
    _Pwrite0(fd, buf, nbytes, offset, ##__VA_ARGS__, _ebo, 0, 0);              \
})
#define _Pwrite0(fd, buf, nbytes, offset, _behavior, _action, ...) ({          \
    ssize_t _size;                                                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_size = pwrite(fd, buf, nbytes, offset)) == -1) {                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pwrite\n");                                  \
    }                                                                          \
    _size;                                                                     \
})

#define Dup(fd, ...) ({                                                        \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Dup";                                                      \
    _Dup0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                      \
})
#define _Dup0(fd, _behavior, _action, ...) ({                                  \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_fd = dup(fd)) == -1) {                                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Dup\n");                                     \
    }                                                                          \
    _fd;                                                                       \
})

#define Dup2(fd, fd2, ...) ({                                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Dup2";                                                     \
    _Dup20(fd, fd2, ##__VA_ARGS__, _ebo, 0, 0);                                \
})
#define _Dup20(fd, fd2, _behavior, _action, ...) ({                            \
    int _fd;                                                                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_fd = dup2(fd, fd2)) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Dup2\n");                                    \
    }                                                                          \
    _fd;                                                                       \
})

#define Fsync(fd, ...) ({                                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fsync";                                                    \
    _Fsync0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                    \
})
#define _Fsync0(fd, _behavior, _action, ...) ({                                \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fsync(fd) == -1) {                                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fsync\n");                                   \
    }                                                                          \
})

#define Fdatasync(fd, ...) ({                                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fdatasync";                                                \
    _Fdatasync0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                \
})
#define _Fdatasync0(fd, _behavior, _action, ...) ({                            \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fdatasync(fd) == -1) {                                             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fdatasync\n");                               \
    }                                                                          \
})

#define Fcntl(fd, cmd, ...) ({                                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fcntl";                                                    \
    _Fcntl0(fd, cmd, ##__VA_ARGS__, _ebo, 0, 0, 0);                            \
})
#define _Fcntl0(fd, cmd, _behavior, _action, ...) ({                           \
    int _ret;                                                                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_ret = fcntl(fd, cmd)) == -1) {                                   \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        _Fcntl1(fd, cmd, _behavior, _action, ##__VA_ARGS__);                   \
    }                                                                          \
    _ret;                                                                      \
})
#define _Fcntl1(fd, cmd, _a0, _behavior, _action, ...) ({                      \
    int _ret;                                                                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb1 = behaviorFrom(_behavior);                          \
        if ((_ret = fcntl(fd, cmd, _a0)) == -1) {                              \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fcntl\n");                                   \
    }                                                                          \
    _ret;                                                                      \
})

#define Ioctl(fd, request, ...) ({                                             \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Ioctl";                                                    \
    _Ioctl0(fd, request, ##__VA_ARGS__, _ebo, 0, 0, 0, 0, 0);                  \
})
#define _Ioctl0(fd, request, _behavior, _action, ...) ({                       \
    int _ret;                                                                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_ret = ioctl(fd, request)) == -1) {                               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        _Ioctl1(fd, request, _behavior, _action, ##__VA_ARGS__);               \
    }                                                                          \
    _ret;                                                                      \
})
#define _Ioctl1(fd, request, _a0, _behavior, _action, ...) ({                  \
    int _ret;                                                                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb1 = behaviorFrom(_behavior);                          \
        if ((_ret = ioctl(fd, request, _a0)) == -1) {                          \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        _Ioctl2(fd, request, _a0, _behavior, _action, ##__VA_ARGS__);          \
    }                                                                          \
    _ret;                                                                      \
})
#define _Ioctl2(fd, request, _a0, _a1, _behavior, _action, ...) ({             \
    int _ret;                                                                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb2 = behaviorFrom(_behavior);                          \
        if ((_ret = ioctl(fd, request, _a0, _a1)) == -1) {                     \
            ERROR_WITH_BEHAVIOR(_eb2, _action);                                \
        }                                                                      \
    } else {                                                                   \
        _Ioctl3(fd, request, _a0, _a1, _behavior, _action, ##__VA_ARGS__);     \
    }                                                                          \
    _ret;                                                                      \
})
#define _Ioctl3(fd, request, _a0, _a1, _a2, _behavior, _action, ...) ({        \
    int _ret;                                                                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb3 = behaviorFrom(_behavior);                          \
        if ((_ret = ioctl(fd, request, _a0, _a1, _a2)) == -1) {                \
            ERROR_WITH_BEHAVIOR(_eb3, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Ioctl\n");                                   \
    }                                                                          \
    _ret;                                                                      \
})

#define Stat(pathname, buf, ...) ({                                            \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Stat";                                                     \
    _Stat0(pathname, buf, ##__VA_ARGS__, _ebo, 0, 0);                          \
})
#define _Stat0(pathname, buf, _behavior, _action, ...) ({                      \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (stat(pathname, buf) == -1) {                                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Stat\n");                                    \
    }                                                                          \
})

#endif // UNIX_FILE_H

