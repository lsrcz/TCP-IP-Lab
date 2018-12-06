#ifndef LOCKUTILS_H
#define LOCKUTILS_H

#include <shared_mutex>
#include <mutex>

struct upgrade_mutex_wp {
    std::mutex mu_o;
    std::mutex mu_i;
    std::shared_mutex smu;
};

class upgrade_lock_wp {
    upgrade_mutex_wp *m;
    std::shared_lock<std::shared_mutex> lock;
    std::unique_lock<std::shared_mutex> ulock;
    std::unique_lock<std::mutex> lock_i;
 public:
    inline explicit upgrade_lock_wp (upgrade_mutex_wp& m): m(std::addressof(m)) {
        std::lock_guard<std::mutex> lock_o(m.mu_o);
        std::lock_guard<std::mutex> lock_i(m.mu_i);
        lock = std::shared_lock<std::shared_mutex>(m.smu);
    }
    upgrade_lock_wp(upgrade_lock_wp&& rhs)
        : m(rhs.m), lock(std::move(rhs.lock)), ulock(std::move(rhs.ulock)), lock_i(std::move(rhs.lock_i)) {}
    upgrade_lock_wp& operator=(upgrade_lock_wp&& rhs) {
        if (this == &rhs)
            return *this;
        m = rhs.m;
        lock = std::move(rhs.lock);
        ulock = std::move(rhs.ulock);
        lock_i = std::move(rhs.lock_i);
        return *this;
    }
    inline void upgrade() {
        printf("upgrade called\n");
        lock.unlock();
        lock_i = std::unique_lock<std::mutex>(m->mu_i);
        printf("upgrade acquired\n");
        ulock = std::unique_lock<std::shared_mutex>(m->smu);
    }
    inline void degrade() {
        ulock.unlock();
        lock = std::shared_lock<std::shared_mutex>(m->smu);
        lock_i.unlock();
    }
};

#endif // LOCKUTILS_H
