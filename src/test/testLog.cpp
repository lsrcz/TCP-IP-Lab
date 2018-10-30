#include <utils/logutils.h>

int a() {
    LOG(INFO, "Message from a.");
}

int main() {
    a();
    LOG(INFO, "Message from main.");
}
