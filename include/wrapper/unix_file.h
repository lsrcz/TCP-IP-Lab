#ifndef UNIX_FILE_H
#define UNIX_FILE_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
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

#define Fstat(fd, buf, ...) ({                                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fstat";                                                    \
    _Fstat0(fd, buf, ##__VA_ARGS__, _ebo, 0, 0);                               \
})
#define _Fstat0(fd, buf, _behavior, _action, ...) ({                           \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fstat(fd, buf) == -1) {                                            \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fstat\n");                                   \
    }                                                                          \
})

#define Lstat(pathname, buf, ...) ({                                           \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Lstat";                                                    \
    _Lstat0(pathname, buf, ##__VA_ARGS__, _ebo, 0, 0);                         \
})
#define _Lstat0(pathname, buf, _behavior, _action, ...) ({                     \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (lstat(pathname, buf) == -1) {                                      \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Lstat\n");                                   \
    }                                                                          \
})

#define Fstatat(fd, pathname, buf, flag, ...) ({                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fstatat";                                                  \
    _Fstatat0(fd, pathname, buf, flag, ##__VA_ARGS__, _ebo, 0, 0);             \
})
#define _Fstatat0(fd, pathname, buf, flag, _behavior, _action, ...) ({         \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fstatat(fd, pathname, buf, flag) == -1) {                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fstatat\n");                                 \
    }                                                                          \
})

#define Access(pathname, mode, ...) ({                                         \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Access";                                                   \
    _Access0(pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                       \
})
#define _Access0(pathname, mode, _behavior, _action, ...) ({                   \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (access(pathname, mode) == -1) {                                    \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Access\n");                                  \
    }                                                                          \
})

#define Faccessat(fd, pathname, mode, flag, ...) ({                            \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Faccessat";                                                \
    _Faccessat0(fd, pathname, mode, flag, ##__VA_ARGS__, _ebo, 0, 0);          \
})
#define _Faccessat0(fd, pathname, mode, flag, _behavior, _action, ...) ({      \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (faccessat(fd, pathname, mode, flag) == -1) {                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Faccessat\n");                               \
    }                                                                          \
})

#define Chmod(pathname, mode, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Chmod";                                                    \
    _Chmod0(pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Chmod0(pathname, mode, _behavior, _action, ...) ({                    \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (chmod(pathname, mode) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Chmod\n");                                   \
    }                                                                          \
})

#define Fchmod(fd, mode, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fchmod";                                                   \
    _Fchmod0(fd, mode, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Fchmod0(fd, mode, _behavior, _action, ...) ({                         \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fchmod(fd, mode) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchmod\n");                                  \
    }                                                                          \
})

#define Fchmodat(fd, pathname, mode, flag, ...) ({                             \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fchmodat";                                                 \
    _Fchmodat0(fd, pathname, mode, flag, ##__VA_ARGS__, _ebo, 0, 0);           \
})
#define _Fchmodat0(fd, pathname, mode, flag, _behavior, _action, ...) ({       \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fchmodat(fd, pathname, mode, flag) == -1) {                        \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchmodat\n");                                \
    }                                                                          \
})

#define Chown(pathname, owner, group, ...) ({                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Chown";                                                    \
    _Chown0(pathname, owner, group, ##__VA_ARGS__, _ebo, 0, 0);                \
})
#define _Chown0(pathname, owner, group, _behavior, _action, ...) ({            \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (chown(pathname, owner, group) == -1) {                             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Chown\n");                                   \
    }                                                                          \
})

#define Fchown(fd, owner, group, ...) ({                                       \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fchown";                                                   \
    _Fchown0(fd, owner, group, ##__VA_ARGS__, _ebo, 0, 0);                     \
})
#define _Fchown0(fd, owner, group, _behavior, _action, ...) ({                 \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fchown(fd, owner, group) == -1) {                                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchown\n");                                  \
    }                                                                          \
})

#define Fchownat(fd, pathname, owner, group, flag, ...) ({                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fchownat";                                                 \
    _Fchownat0(fd, pathname, owner, group, flag, ##__VA_ARGS__, _ebo, 0, 0);   \
})
#define _Fchownat0(fd, pathname, owner, group, flag, _behavior, _action, ...) ({  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fchownat(fd, pathname, owner, group, flag) == -1) {                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchownat\n");                                \
    }                                                                          \
})

#define Lchown(pathname, owner, group, ...) ({                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Lchown";                                                   \
    _Lchown0(pathname, owner, group, ##__VA_ARGS__, _ebo, 0, 0);               \
})
#define _Lchown0(pathname, owner, group, _behavior, _action, ...) ({           \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (lchown(pathname, owner, group) == -1) {                            \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Lchown\n");                                  \
    }                                                                          \
})

#define Truncate(pathname, length, ...) ({                                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Truncate";                                                 \
    _Truncate0(pathname, length, ##__VA_ARGS__, _ebo, 0, 0);                   \
})
#define _Truncate0(pathname, length, _behavior, _action, ...) ({               \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (truncate(pathname, length) == -1) {                                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Truncate\n");                                \
    }                                                                          \
})

#define Ftruncate(fd, length, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Ftruncate";                                                \
    _Ftruncate0(fd, length, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Ftruncate0(fd, length, _behavior, _action, ...) ({                    \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (ftruncate(fd, length) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Ftruncate\n");                               \
    }                                                                          \
})

#define Link(existingpath, newpath, ...) ({                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Link";                                                     \
    _Link0(existingpath, newpath, ##__VA_ARGS__, _ebo, 0, 0);                  \
})
#define _Link0(existingpath, newpath, _behavior, _action, ...) ({              \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (link(existingpath, newpath) == -1) {                               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Link\n");                                    \
    }                                                                          \
})

#define Linkat(efd, existingpath, nfd, newpath, flag, ...) ({                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Linkat";                                                   \
    _Linkat0(efd, existingpath, nfd, newpath, flag, ##__VA_ARGS__, _ebo, 0, 0);  \
})
#define _Linkat0(efd, existingpath, nfd, newpath, flag, _behavior, _action, ...) ({  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (linkat(efd, existingpath, nfd, newpath, flag) == -1) {             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Linkat\n");                                  \
    }                                                                          \
})

#define Unlink(pathname, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Unlink";                                                   \
    _Unlink0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Unlink0(pathname, _behavior, _action, ...) ({                         \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (unlink(pathname) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Unlink\n");                                  \
    }                                                                          \
})

#define Unlinkat(fd, pathname, flag, ...) ({                                   \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Unlinkat";                                                 \
    _Unlinkat0(fd, pathname, flag, ##__VA_ARGS__, _ebo, 0, 0);                 \
})
#define _Unlinkat0(fd, pathname, flag, _behavior, _action, ...) ({             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (unlinkat(fd, pathname, flag) == -1) {                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Unlinkat\n");                                \
    }                                                                          \
})

#define Remove(pathname, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Remove";                                                   \
    _Remove0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Remove0(pathname, _behavior, _action, ...) ({                         \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (remove(pathname) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Remove\n");                                  \
    }                                                                          \
})

#define Rename(oldname, newname, ...) ({                                       \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Rename";                                                   \
    _Rename0(oldname, newname, ##__VA_ARGS__, _ebo, 0, 0);                     \
})
#define _Rename0(oldname, newname, _behavior, _action, ...) ({                 \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (rename(oldname, newname) == -1) {                                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Rename\n");                                  \
    }                                                                          \
})

#define Renameat(oldfd, oldname, newfd, newname, ...) ({                       \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Renameat";                                                 \
    _Renameat0(oldfd, oldname, newfd, newname, ##__VA_ARGS__, _ebo, 0, 0);     \
})
#define _Renameat0(oldfd, oldname, newfd, newname, _behavior, _action, ...) ({  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (renameat(oldfd, oldname, newfd, newname) == -1) {                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Renameat\n");                                \
    }                                                                          \
})

#define Symlink(actualpath, sympath, ...) ({                                   \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Symlink";                                                  \
    _Symlink0(actualpath, sympath, ##__VA_ARGS__, _ebo, 0, 0);                 \
})
#define _Symlink0(actualpath, sympath, _behavior, _action, ...) ({             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (symlink(actualpath, sympath) == -1) {                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Symlink\n");                                 \
    }                                                                          \
})

#define Symlinkat(actualpath, fd, sympath, ...) ({                             \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Symlinkat";                                                \
    _Symlinkat0(actualpath, fd, sympath, ##__VA_ARGS__, _ebo, 0, 0);           \
})
#define _Symlinkat0(actualpath, fd, sympath, _behavior, _action, ...) ({       \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (symlinkat(actualpath, fd, sympath) == -1) {                        \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Symlinkat\n");                               \
    }                                                                          \
})

#define Readlink(pathname, buf, bufsize, ...) ({                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Readlink";                                                 \
    _Readlink0(pathname, buf, bufsize, ##__VA_ARGS__, _ebo, 0, 0);             \
})
#define _Readlink0(pathname, buf, bufsize, _behavior, _action, ...) ({         \
    ssize_t _byte;                                                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_byte = readlink(pathname, buf, bufsize)) == -1) {                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Readlink\n");                                \
    }                                                                          \
    _byte;                                                                     \
})

#define Readlinkat(fd, pathname, buf, bufsize, ...) ({                         \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Readlinkat";                                               \
    _Readlinkat0(fd, pathname, buf, bufsize, ##__VA_ARGS__, _ebo, 0, 0);       \
})
#define _Readlinkat0(fd, pathname, buf, bufsize, _behavior, _action, ...) ({   \
    ssize_t _byte;                                                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_byte = readlinkat(fd, pathname, buf, bufsize)) == -1) {          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Readlinkat\n");                              \
    }                                                                          \
    _byte;                                                                     \
})

#define Futimens(fd, times, ...) ({                                            \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Futimens";                                                 \
    _Futimens0(fd, times, ##__VA_ARGS__, _ebo, 0, 0);                          \
})
#define _Futimens0(fd, times, _behavior, _action, ...) ({                      \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (futimens(fd, times) == -1) {                                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Futimens\n");                                \
    }                                                                          \
})

#define Utimensat(fd, path, times, flag, ...) ({                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Utimensat";                                                \
    _Utimensat0(fd, path, times, flag, ##__VA_ARGS__, _ebo, 0, 0);             \
})
#define _Utimensat0(fd, path, times, flag, _behavior, _action, ...) ({         \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (utimensat(fd, path, times, flag) == -1) {                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Utimensat\n");                               \
    }                                                                          \
})

#define Utimes(pathname, times, ...) ({                                        \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Utimes";                                                   \
    _Utimes0(pathname, times, ##__VA_ARGS__, _ebo, 0, 0);                      \
})
#define _Utimes0(pathname, times, _behavior, _action, ...) ({                  \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (utimes(pathname, times) == -1) {                                   \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Utimes\n");                                  \
    }                                                                          \
})

#define Mkdir(pathname, mode, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Mkdir";                                                    \
    _Mkdir0(pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Mkdir0(pathname, mode, _behavior, _action, ...) ({                    \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (mkdir(pathname, mode) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Mkdir\n");                                   \
    }                                                                          \
})

#define Mkdirat(fd, pathname, mode, ...) ({                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Mkdirat";                                                  \
    _Mkdirat0(fd, pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                  \
})
#define _Mkdirat0(fd, pathname, mode, _behavior, _action, ...) ({              \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (mkdirat(fd, pathname, mode) == -1) {                               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Mkdirat\n");                                 \
    }                                                                          \
})

#define Rmdir(pathname, ...) ({                                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Rmdir";                                                    \
    _Rmdir0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                              \
})
#define _Rmdir0(pathname, _behavior, _action, ...) ({                          \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (rmdir(pathname) == -1) {                                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Rmdir\n");                                   \
    }                                                                          \
})

#define Opendir(pathname, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Opendir";                                                  \
    _Opendir0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                            \
})
#define _Opendir0(pathname, _behavior, _action, ...) ({                        \
    DIR *_dir;                                                                 \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_dir = opendir(pathname)) == NULL) {                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Opendir\n");                                 \
    }                                                                          \
    _dir;                                                                      \
})

#define Fdopendir(fd, ...) ({                                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fdopendir";                                                \
    _Fdopendir0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                \
})
#define _Fdopendir0(fd, _behavior, _action, ...) ({                            \
    DIR *_dir;                                                                 \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_dir = fdopendir(fd)) == NULL) {                                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fdopendir\n");                               \
    }                                                                          \
    _dir;                                                                      \
})

#define Readdir(dp, ...) ({                                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Readdir";                                                  \
    _Readdir0(dp, ##__VA_ARGS__, _ebo, 0, 0);                                  \
})
#define _Readdir0(dp, _behavior, _action, ...) ({                              \
    struct dirent *_dirent;                                                    \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if ((_dirent = readdir(dp)) == NULL) {                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Readdir\n");                                 \
    }                                                                          \
    _dirent;                                                                   \
})

#define Closedir(dp, ...) ({                                                   \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Closedir";                                                 \
    _Closedir0(dp, ##__VA_ARGS__, _ebo, 0, 0);                                 \
})
#define _Closedir0(dp, _behavior, _action, ...) ({                             \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (closedir(dp) == -1) {                                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Closedir\n");                                \
    }                                                                          \
})

#define Chdir(pathname, ...) ({                                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Chdir";                                                    \
    _Chdir0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                              \
})
#define _Chdir0(pathname, _behavior, _action, ...) ({                          \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (chdir(pathname) == -1) {                                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Chdir\n");                                   \
    }                                                                          \
})

#define Fchdir(fd, ...) ({                                                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Fchdir";                                                   \
    _Fchdir0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                   \
})
#define _Fchdir0(fd, _behavior, _action, ...) ({                               \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (fchdir(fd) == -1) {                                                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchdir\n");                                  \
    }                                                                          \
})

#define Getcwd(buf, size, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _ebo.sys_msg = "Getcwd";                                                   \
    _Getcwd0(buf, size, ##__VA_ARGS__, _ebo, 0, 0);                            \
})
#define _Getcwd0(buf, size, _behavior, _action, ...) ({                        \
    if(isErrorBehavior(_behavior)) {                                           \
        ErrorBehavior _eb0 = behaviorFrom(_behavior);                          \
        if (getcwd(buf, size) == NULL) {                                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Getcwd\n");                                  \
    }                                                                          \
})

#endif // UNIX_FILE_H

