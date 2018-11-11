#include <utils/timeutils.h>
#include <chrono>
#include <unistd.h>
int main() {
    Timer t;
    using namespace std::chrono_literals;
    t.setTimer(1s, [](){printf("a\n");});
    Timer t2 = std::move(t);
    sleep(10);
}
