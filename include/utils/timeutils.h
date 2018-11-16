#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <functional>
#include <mutex>
#include <thread>

inline uint64_t getTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock,
                            std::chrono::milliseconds>
        tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch());
    uint64_t timestamp = tmp.count();
    return timestamp;
}

class Timer {
    std::thread               timer_thread;
    std::function<void(void)> callback;

public:
    inline Timer() {}
    inline Timer(Timer&& rhs) {
        timer_thread = std::move(rhs.timer_thread);
        callback     = std::move(rhs.callback);
    }
    inline Timer& operator=(Timer&& rhs) {
        if (this == &rhs)
            return *this;
        cancelTimer();
        timer_thread = std::move(rhs.timer_thread);
        callback     = std::move(rhs.callback);
        return *this;
    }
    template <typename Rep, typename Period>
    inline void
    setTimer(const std::chrono::duration<Rep, Period>& timer_duration,
             std::function<void(void)>                 callback) {
        cancelTimer();
        timer_thread = std::thread([timer_duration = timer_duration,
                                    callback       = std::move(callback)]() {
            std::mutex                   mu;
            std::condition_variable      cv;
            std::unique_lock<std::mutex> lock(mu);
            if (cv.wait_for(lock, timer_duration) == std::cv_status::timeout) {
                std::invoke(callback);
            }
        });
    }
    template <class Rep, class Period>
    inline void
    setPeriodTimer(const std::chrono::duration<Rep, Period>& timer_duration,
                   std::function<void(void)>                 callback) {
        cancelTimer();
        this->callback = callback;
        timer_thread   = std::thread([timer_duration = timer_duration,
                                    callback       = std::move(callback)]() {
            std::mutex                   mu;
            std::condition_variable      cv;
            std::unique_lock<std::mutex> lock(mu);
            while (true) {
                if (cv.wait_for(lock, timer_duration)
                    == std::cv_status::timeout) {
                    std::invoke(callback);
                }
            }
        });
    }
    inline void cancelTimer() {
        if (timer_thread.joinable()) {
            pthread_t pt = timer_thread.native_handle();
            pthread_cancel(pt);
            timer_thread.join();
        }
    }
    inline void waitTimer() {
        timer_thread.join();
    }
    inline ~Timer() {
        cancelTimer();
    }
};

#endif  // TIMEUTILS_H
