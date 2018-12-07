#ifndef UNIX_FILE_H
#define UNIX_FILE_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utils/errorutils.h>

extern "C" {
    int __real_close(int fd);
    ssize_t __real_write(int fd, const void *buf, size_t count);
    ssize_t __real_read(int fd, void *buf, size_t count);
}

#define Open(path, oflag, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Open0(path, oflag, ##__VA_ARGS__, _ebo, 0, 0, 0);                         \
})
#define _Open0(path, oflag, _behavior, _action, ...) ({                        \
    int _fd;                                                                   \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "open";                                                 \
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
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb1 = behaviorFrom(({_behavior;}));                     \
        _eb1.sys_msg = "open";                                                 \
        if ((_fd = open(path, oflag, _a0)) == -1) {                            \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Open\n");                                    \
        exit(-1);                                                              \
    }                                                                          \
    _fd;                                                                       \
})

#define Openat(fd, path, oflag, ...) ({                                        \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Openat0(fd, path, oflag, ##__VA_ARGS__, _ebo, 0, 0, 0);                   \
})
#define _Openat0(fd, path, oflag, _behavior, _action, ...) ({                  \
    int _fd;                                                                   \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "openat";                                               \
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
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb1 = behaviorFrom(({_behavior;}));                     \
        _eb1.sys_msg = "openat";                                               \
        if ((_fd = openat(fd, path, oflag, _a0)) == -1) {                      \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Openat\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
    _fd;                                                                       \
})

#define Creat(path, mode, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Creat0(path, mode, ##__VA_ARGS__, _ebo, 0, 0);                            \
})
#define _Creat0(path, mode, _behavior, _action, ...) ({                        \
    int _fd;                                                                   \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "creat";                                                \
        if ((_fd = creat(path, mode)) == -1) {                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Creat\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _fd;                                                                       \
})

#define Close(fd, ...) ({                                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Close0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                    \
})
#define _Close0(fd, _behavior, _action, ...) ({                                \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "close";                                                \
        if (__real_close(fd) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Close\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Lseek(fd, offset, whence, ...) ({                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Lseek0(fd, offset, whence, ##__VA_ARGS__, _ebo, 0, 0);                    \
})
#define _Lseek0(fd, offset, whence, _behavior, _action, ...) ({                \
    off_t _off;                                                                \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "lseek";                                                \
        if ((_off = lseek(fd, offset, whence)) == -1) {                        \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Lseek\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _off;                                                                      \
})

#define Read(fd, buf, nbytes, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Read0(fd, buf, nbytes, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Read0(fd, buf, nbytes, _behavior, _action, ...) ({                    \
    ssize_t _size;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "read";                                                 \
        if ((_size = __real_read(fd, buf, nbytes)) == -1) {                    \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Read\n");                                    \
        exit(-1);                                                              \
    }                                                                          \
    _size;                                                                     \
})

#define Write(fd, buf, nbytes, ...) ({                                         \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Write0(fd, buf, nbytes, ##__VA_ARGS__, _ebo, 0, 0);                       \
})
#define _Write0(fd, buf, nbytes, _behavior, _action, ...) ({                   \
    ssize_t _size;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "write";                                                \
        if ((_size = __real_write(fd, buf, nbytes)) != nbytes) {               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Write\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _size;                                                                     \
})

#define Pread(fd, buf, nbytes, offset, ...) ({                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Pread0(fd, buf, nbytes, offset, ##__VA_ARGS__, _ebo, 0, 0);               \
})
#define _Pread0(fd, buf, nbytes, offset, _behavior, _action, ...) ({           \
    ssize_t _size;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pread";                                                \
        if ((_size = pread(fd, buf, nbytes, offset)) == -1) {                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pread\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _size;                                                                     \
})

#define Pwrite(fd, buf, nbytes, offset, ...) ({                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Pwrite0(fd, buf, nbytes, offset, ##__VA_ARGS__, _ebo, 0, 0);              \
})
#define _Pwrite0(fd, buf, nbytes, offset, _behavior, _action, ...) ({          \
    ssize_t _size;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "pwrite";                                               \
        if ((_size = pwrite(fd, buf, nbytes, offset)) == nbytes) {             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Pwrite\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
    _size;                                                                     \
})

#define Dup(fd, ...) ({                                                        \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Dup0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                      \
})
#define _Dup0(fd, _behavior, _action, ...) ({                                  \
    int _fd;                                                                   \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "dup";                                                  \
        if ((_fd = dup(fd)) == -1) {                                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Dup\n");                                     \
        exit(-1);                                                              \
    }                                                                          \
    _fd;                                                                       \
})

#define Dup2(fd, fd2, ...) ({                                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Dup20(fd, fd2, ##__VA_ARGS__, _ebo, 0, 0);                                \
})
#define _Dup20(fd, fd2, _behavior, _action, ...) ({                            \
    int _fd;                                                                   \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "dup2";                                                 \
        if ((_fd = dup2(fd, fd2)) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Dup2\n");                                    \
        exit(-1);                                                              \
    }                                                                          \
    _fd;                                                                       \
})

#define Fsync(fd, ...) ({                                                      \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fsync0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                    \
})
#define _Fsync0(fd, _behavior, _action, ...) ({                                \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fsync";                                                \
        if (fsync(fd) == -1) {                                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fsync\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fdatasync(fd, ...) ({                                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fdatasync0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                \
})
#define _Fdatasync0(fd, _behavior, _action, ...) ({                            \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fdatasync";                                            \
        if (fdatasync(fd) == -1) {                                             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fdatasync\n");                               \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fcntl(fd, cmd, ...) ({                                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fcntl0(fd, cmd, ##__VA_ARGS__, _ebo, 0, 0, 0);                            \
})
#define _Fcntl0(fd, cmd, _behavior, _action, ...) ({                           \
    int _ret;                                                                  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fcntl";                                                \
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
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb1 = behaviorFrom(({_behavior;}));                     \
        _eb1.sys_msg = "fcntl";                                                \
        if ((_ret = fcntl(fd, cmd, _a0)) == -1) {                              \
            ERROR_WITH_BEHAVIOR(_eb1, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fcntl\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _ret;                                                                      \
})

#define Ioctl(fd, request, ...) ({                                             \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Ioctl0(fd, request, ##__VA_ARGS__, _ebo, 0, 0, 0, 0, 0);                  \
})
#define _Ioctl0(fd, request, _behavior, _action, ...) ({                       \
    int _ret;                                                                  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "ioctl";                                                \
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
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb1 = behaviorFrom(({_behavior;}));                     \
        _eb1.sys_msg = "ioctl";                                                \
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
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb2 = behaviorFrom(({_behavior;}));                     \
        _eb2.sys_msg = "ioctl";                                                \
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
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb3 = behaviorFrom(({_behavior;}));                     \
        _eb3.sys_msg = "ioctl";                                                \
        if ((_ret = ioctl(fd, request, _a0, _a1, _a2)) == -1) {                \
            ERROR_WITH_BEHAVIOR(_eb3, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Ioctl\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _ret;                                                                      \
})

#define Stat(pathname, buf, ...) ({                                            \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Stat0(pathname, buf, ##__VA_ARGS__, _ebo, 0, 0);                          \
})
#define _Stat0(pathname, buf, _behavior, _action, ...) ({                      \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "stat";                                                 \
        if (stat(pathname, buf) == -1) {                                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Stat\n");                                    \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fstat(fd, buf, ...) ({                                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fstat0(fd, buf, ##__VA_ARGS__, _ebo, 0, 0);                               \
})
#define _Fstat0(fd, buf, _behavior, _action, ...) ({                           \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fstat";                                                \
        if (fstat(fd, buf) == -1) {                                            \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fstat\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Lstat(pathname, buf, ...) ({                                           \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Lstat0(pathname, buf, ##__VA_ARGS__, _ebo, 0, 0);                         \
})
#define _Lstat0(pathname, buf, _behavior, _action, ...) ({                     \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "lstat";                                                \
        if (lstat(pathname, buf) == -1) {                                      \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Lstat\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fstatat(fd, pathname, buf, flag, ...) ({                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fstatat0(fd, pathname, buf, flag, ##__VA_ARGS__, _ebo, 0, 0);             \
})
#define _Fstatat0(fd, pathname, buf, flag, _behavior, _action, ...) ({         \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fstatat";                                              \
        if (fstatat(fd, pathname, buf, flag) == -1) {                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fstatat\n");                                 \
        exit(-1);                                                              \
    }                                                                          \
})

#define Access(pathname, mode, ...) ({                                         \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Access0(pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                       \
})
#define _Access0(pathname, mode, _behavior, _action, ...) ({                   \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "access";                                               \
        if (access(pathname, mode) == -1) {                                    \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Access\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Faccessat(fd, pathname, mode, flag, ...) ({                            \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Faccessat0(fd, pathname, mode, flag, ##__VA_ARGS__, _ebo, 0, 0);          \
})
#define _Faccessat0(fd, pathname, mode, flag, _behavior, _action, ...) ({      \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "faccessat";                                            \
        if (faccessat(fd, pathname, mode, flag) == -1) {                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Faccessat\n");                               \
        exit(-1);                                                              \
    }                                                                          \
})

#define Chmod(pathname, mode, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Chmod0(pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Chmod0(pathname, mode, _behavior, _action, ...) ({                    \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "chmod";                                                \
        if (chmod(pathname, mode) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Chmod\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fchmod(fd, mode, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fchmod0(fd, mode, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Fchmod0(fd, mode, _behavior, _action, ...) ({                         \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fchmod";                                               \
        if (fchmod(fd, mode) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchmod\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fchmodat(fd, pathname, mode, flag, ...) ({                             \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fchmodat0(fd, pathname, mode, flag, ##__VA_ARGS__, _ebo, 0, 0);           \
})
#define _Fchmodat0(fd, pathname, mode, flag, _behavior, _action, ...) ({       \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fchmodat";                                             \
        if (fchmodat(fd, pathname, mode, flag) == -1) {                        \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchmodat\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Chown(pathname, owner, group, ...) ({                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Chown0(pathname, owner, group, ##__VA_ARGS__, _ebo, 0, 0);                \
})
#define _Chown0(pathname, owner, group, _behavior, _action, ...) ({            \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "chown";                                                \
        if (chown(pathname, owner, group) == -1) {                             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Chown\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fchown(fd, owner, group, ...) ({                                       \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fchown0(fd, owner, group, ##__VA_ARGS__, _ebo, 0, 0);                     \
})
#define _Fchown0(fd, owner, group, _behavior, _action, ...) ({                 \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fchown";                                               \
        if (fchown(fd, owner, group) == -1) {                                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchown\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fchownat(fd, pathname, owner, group, flag, ...) ({                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fchownat0(fd, pathname, owner, group, flag, ##__VA_ARGS__, _ebo, 0, 0);   \
})
#define _Fchownat0(fd, pathname, owner, group, flag, _behavior, _action, ...) ({  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fchownat";                                             \
        if (fchownat(fd, pathname, owner, group, flag) == -1) {                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchownat\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Lchown(pathname, owner, group, ...) ({                                 \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Lchown0(pathname, owner, group, ##__VA_ARGS__, _ebo, 0, 0);               \
})
#define _Lchown0(pathname, owner, group, _behavior, _action, ...) ({           \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "lchown";                                               \
        if (lchown(pathname, owner, group) == -1) {                            \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Lchown\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Truncate(pathname, length, ...) ({                                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Truncate0(pathname, length, ##__VA_ARGS__, _ebo, 0, 0);                   \
})
#define _Truncate0(pathname, length, _behavior, _action, ...) ({               \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "truncate";                                             \
        if (truncate(pathname, length) == -1) {                                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Truncate\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Ftruncate(fd, length, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Ftruncate0(fd, length, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Ftruncate0(fd, length, _behavior, _action, ...) ({                    \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "ftruncate";                                            \
        if (ftruncate(fd, length) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Ftruncate\n");                               \
        exit(-1);                                                              \
    }                                                                          \
})

#define Link(existingpath, newpath, ...) ({                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Link0(existingpath, newpath, ##__VA_ARGS__, _ebo, 0, 0);                  \
})
#define _Link0(existingpath, newpath, _behavior, _action, ...) ({              \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "link";                                                 \
        if (link(existingpath, newpath) == -1) {                               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Link\n");                                    \
        exit(-1);                                                              \
    }                                                                          \
})

#define Linkat(efd, existingpath, nfd, newpath, flag, ...) ({                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Linkat0(efd, existingpath, nfd, newpath, flag, ##__VA_ARGS__, _ebo, 0, 0);  \
})
#define _Linkat0(efd, existingpath, nfd, newpath, flag, _behavior, _action, ...) ({  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "linkat";                                               \
        if (linkat(efd, existingpath, nfd, newpath, flag) == -1) {             \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Linkat\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Unlink(pathname, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Unlink0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Unlink0(pathname, _behavior, _action, ...) ({                         \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "unlink";                                               \
        if (unlink(pathname) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Unlink\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Unlinkat(fd, pathname, flag, ...) ({                                   \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Unlinkat0(fd, pathname, flag, ##__VA_ARGS__, _ebo, 0, 0);                 \
})
#define _Unlinkat0(fd, pathname, flag, _behavior, _action, ...) ({             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "unlinkat";                                             \
        if (unlinkat(fd, pathname, flag) == -1) {                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Unlinkat\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Remove(pathname, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Remove0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Remove0(pathname, _behavior, _action, ...) ({                         \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "remove";                                               \
        if (remove(pathname) == -1) {                                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Remove\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Rename(oldname, newname, ...) ({                                       \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Rename0(oldname, newname, ##__VA_ARGS__, _ebo, 0, 0);                     \
})
#define _Rename0(oldname, newname, _behavior, _action, ...) ({                 \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "rename";                                               \
        if (rename(oldname, newname) == -1) {                                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Rename\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Renameat(oldfd, oldname, newfd, newname, ...) ({                       \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Renameat0(oldfd, oldname, newfd, newname, ##__VA_ARGS__, _ebo, 0, 0);     \
})
#define _Renameat0(oldfd, oldname, newfd, newname, _behavior, _action, ...) ({  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "renameat";                                             \
        if (renameat(oldfd, oldname, newfd, newname) == -1) {                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Renameat\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Symlink(actualpath, sympath, ...) ({                                   \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Symlink0(actualpath, sympath, ##__VA_ARGS__, _ebo, 0, 0);                 \
})
#define _Symlink0(actualpath, sympath, _behavior, _action, ...) ({             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "symlink";                                              \
        if (symlink(actualpath, sympath) == -1) {                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Symlink\n");                                 \
        exit(-1);                                                              \
    }                                                                          \
})

#define Symlinkat(actualpath, fd, sympath, ...) ({                             \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Symlinkat0(actualpath, fd, sympath, ##__VA_ARGS__, _ebo, 0, 0);           \
})
#define _Symlinkat0(actualpath, fd, sympath, _behavior, _action, ...) ({       \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "symlinkat";                                            \
        if (symlinkat(actualpath, fd, sympath) == -1) {                        \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Symlinkat\n");                               \
        exit(-1);                                                              \
    }                                                                          \
})

#define Readlink(pathname, buf, bufsize, ...) ({                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Readlink0(pathname, buf, bufsize, ##__VA_ARGS__, _ebo, 0, 0);             \
})
#define _Readlink0(pathname, buf, bufsize, _behavior, _action, ...) ({         \
    ssize_t _byte;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "readlink";                                             \
        if ((_byte = readlink(pathname, buf, bufsize)) == -1) {                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Readlink\n");                                \
        exit(-1);                                                              \
    }                                                                          \
    _byte;                                                                     \
})

#define Readlinkat(fd, pathname, buf, bufsize, ...) ({                         \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Readlinkat0(fd, pathname, buf, bufsize, ##__VA_ARGS__, _ebo, 0, 0);       \
})
#define _Readlinkat0(fd, pathname, buf, bufsize, _behavior, _action, ...) ({   \
    ssize_t _byte;                                                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "readlinkat";                                           \
        if ((_byte = readlinkat(fd, pathname, buf, bufsize)) == -1) {          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Readlinkat\n");                              \
        exit(-1);                                                              \
    }                                                                          \
    _byte;                                                                     \
})

#define Futimens(fd, times, ...) ({                                            \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Futimens0(fd, times, ##__VA_ARGS__, _ebo, 0, 0);                          \
})
#define _Futimens0(fd, times, _behavior, _action, ...) ({                      \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "futimens";                                             \
        if (futimens(fd, times) == -1) {                                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Futimens\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Utimensat(fd, path, times, flag, ...) ({                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Utimensat0(fd, path, times, flag, ##__VA_ARGS__, _ebo, 0, 0);             \
})
#define _Utimensat0(fd, path, times, flag, _behavior, _action, ...) ({         \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "utimensat";                                            \
        if (utimensat(fd, path, times, flag) == -1) {                          \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Utimensat\n");                               \
        exit(-1);                                                              \
    }                                                                          \
})

#define Utimes(pathname, times, ...) ({                                        \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Utimes0(pathname, times, ##__VA_ARGS__, _ebo, 0, 0);                      \
})
#define _Utimes0(pathname, times, _behavior, _action, ...) ({                  \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "utimes";                                               \
        if (utimes(pathname, times) == -1) {                                   \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Utimes\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Mkdir(pathname, mode, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Mkdir0(pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Mkdir0(pathname, mode, _behavior, _action, ...) ({                    \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "mkdir";                                                \
        if (mkdir(pathname, mode) == -1) {                                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Mkdir\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Mkdirat(fd, pathname, mode, ...) ({                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Mkdirat0(fd, pathname, mode, ##__VA_ARGS__, _ebo, 0, 0);                  \
})
#define _Mkdirat0(fd, pathname, mode, _behavior, _action, ...) ({              \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "mkdirat";                                              \
        if (mkdirat(fd, pathname, mode) == -1) {                               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Mkdirat\n");                                 \
        exit(-1);                                                              \
    }                                                                          \
})

#define Rmdir(pathname, ...) ({                                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Rmdir0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                              \
})
#define _Rmdir0(pathname, _behavior, _action, ...) ({                          \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "rmdir";                                                \
        if (rmdir(pathname) == -1) {                                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Rmdir\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Opendir(pathname, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Opendir0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                            \
})
#define _Opendir0(pathname, _behavior, _action, ...) ({                        \
    DIR *_dir;                                                                 \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "opendir";                                              \
        if ((_dir = opendir(pathname)) == NULL) {                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Opendir\n");                                 \
        exit(-1);                                                              \
    }                                                                          \
    _dir;                                                                      \
})

#define Fdopendir(fd, ...) ({                                                  \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fdopendir0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                \
})
#define _Fdopendir0(fd, _behavior, _action, ...) ({                            \
    DIR *_dir;                                                                 \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fdopendir";                                            \
        if ((_dir = fdopendir(fd)) == NULL) {                                  \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fdopendir\n");                               \
        exit(-1);                                                              \
    }                                                                          \
    _dir;                                                                      \
})

#define Readdir(dp, ...) ({                                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Readdir0(dp, ##__VA_ARGS__, _ebo, 0, 0);                                  \
})
#define _Readdir0(dp, _behavior, _action, ...) ({                              \
    struct dirent *_dirent;                                                    \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "readdir";                                              \
        if ((_dirent = readdir(dp)) == NULL) {                                 \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Readdir\n");                                 \
        exit(-1);                                                              \
    }                                                                          \
    _dirent;                                                                   \
})

#define Closedir(dp, ...) ({                                                   \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Closedir0(dp, ##__VA_ARGS__, _ebo, 0, 0);                                 \
})
#define _Closedir0(dp, _behavior, _action, ...) ({                             \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "closedir";                                             \
        if (closedir(dp) == -1) {                                              \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Closedir\n");                                \
        exit(-1);                                                              \
    }                                                                          \
})

#define Chdir(pathname, ...) ({                                                \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Chdir0(pathname, ##__VA_ARGS__, _ebo, 0, 0);                              \
})
#define _Chdir0(pathname, _behavior, _action, ...) ({                          \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "chdir";                                                \
        if (chdir(pathname) == -1) {                                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Chdir\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fchdir(fd, ...) ({                                                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fchdir0(fd, ##__VA_ARGS__, _ebo, 0, 0);                                   \
})
#define _Fchdir0(fd, _behavior, _action, ...) ({                               \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fchdir";                                               \
        if (fchdir(fd) == -1) {                                                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fchdir\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Getcwd(buf, size, ...) ({                                              \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Getcwd0(buf, size, ##__VA_ARGS__, _ebo, 0, 0);                            \
})
#define _Getcwd0(buf, size, _behavior, _action, ...) ({                        \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "getcwd";                                               \
        if (getcwd(buf, size) == NULL) {                                       \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Getcwd\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#define Fopen(pathname, type, ...) ({                                          \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fopen0(pathname, type, ##__VA_ARGS__, _ebo, 0, 0);                        \
})
#define _Fopen0(pathname, type, _behavior, _action, ...) ({                    \
    _fp;                                                                       \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fopen";                                                \
        if ((_fp = fopen(pathname, type)) == NULL) {                           \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fopen\n");                                   \
        exit(-1);                                                              \
    }                                                                          \
    _fp;                                                                       \
})

#define Freopen(pathname, type, fp, ...) ({                                    \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Freopen0(pathname, type, fp, ##__VA_ARGS__, _ebo, 0, 0);                  \
})
#define _Freopen0(pathname, type, fp, _behavior, _action, ...) ({              \
    _fp;                                                                       \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "freopen";                                              \
        if ((_fp = freopen(pathname, type, fp)) == NULL) {                     \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Freopen\n");                                 \
        exit(-1);                                                              \
    }                                                                          \
    _fp;                                                                       \
})

#define Fdopen(fd, type, ...) ({                                               \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fdopen0(fd, type, ##__VA_ARGS__, _ebo, 0, 0);                             \
})
#define _Fdopen0(fd, type, _behavior, _action, ...) ({                         \
    _fp;                                                                       \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fdopen";                                               \
        if ((_fp = fdopen(fd, type)) == NULL) {                                \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fdopen\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
    _fp;                                                                       \
})

#define Fclose(fp, ...) ({                                                     \
    ErrorBehavior _ebo = ErrorBehavior("");                                    \
    _ebo.from = SOURCE_SYSTEM;                                                 \
    _Fclose0(fp, ##__VA_ARGS__, _ebo, 0, 0);                                   \
})
#define _Fclose0(fp, _behavior, _action, ...) ({                               \
    decltype(({_behavior;})) *_decleb;                                         \
    if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {  \
        ErrorBehavior _eb0 = behaviorFrom(({_behavior;}));                     \
        _eb0.sys_msg = "fclose";                                               \
        if (fclose(fp) == EOF) {                                               \
            ERROR_WITH_BEHAVIOR(_eb0, _action);                                \
        }                                                                      \
    } else {                                                                   \
        printf("Too many args for Fclose\n");                                  \
        exit(-1);                                                              \
    }                                                                          \
})

#endif // UNIX_FILE_H
