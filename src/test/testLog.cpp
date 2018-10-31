#include <utils/logutils.h>

int a() {
    LOG(LogLevel::INFO, "Message from a.");
}

int main() {
    a();
    LOG(LogLevel::INFO, "Message from main.");
}
