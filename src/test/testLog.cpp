#include <utils/logutils.h>

int a() {
    LOG(LogLevel::INFO, "Message from a.");
    return 1;
}

int main() {
    a();
    LOG(LogLevel::INFO, "Message from main.");
}
