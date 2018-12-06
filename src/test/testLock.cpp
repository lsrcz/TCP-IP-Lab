#include <cstdio>
#include <thread>
#include <unistd.h>
#include <utils/lockutils.h>

int main() {
    std::thread      r[10];
    std::thread      w[6];
    upgrade_mutex_wp wpum;
    for (int i = 0; i < 5; ++i) {
        r[i] = std::thread([&wpum, i]() {
            upgrade_lock_wp ulwp(wpum);
            printf("reader %d enter\n", i);
            sleep(2);
            printf("reader %d exit\n", i);
        });
    }
    for (int i = 0; i < 3; ++i) {
        w[i] = std::thread([&wpum, i]() {
            upgrade_lock_wp ulwp(wpum);
            printf("writer %d enter\n", i);
            ulwp.upgrade();
            printf("writer %d upgraded\n", i);
            sleep(2);
            ulwp.degrade();
            printf("writer %d degraded\n", i);
        });
    }
    for (int i = 3; i < 6; ++i) {
        w[i] = std::thread([&wpum, i]() {
            upgrade_lock_wp ulwp(wpum);
            printf("writer %d enter\n", i);
            ulwp.upgrade();
            printf("writer %d upgraded\n", i);
            sleep(2);
            printf("writer %d exit\n", i);
        });
    }
    sleep(1);
    for (int i = 5; i < 10; ++i) {
        r[i] = std::thread([&wpum, i]() {
            upgrade_lock_wp ulwp(wpum);
            printf("reader %d enter\n", i);
            sleep(2);
            printf("reader %d exit\n", i);
        });
    }
    for (auto& t : r) {
        t.join();
    }
    for (auto& t : w) {
        t.join();
    }
}
