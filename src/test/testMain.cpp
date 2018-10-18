#include <testDevice.h>
#include <cstdio>

int main() {
    int ret;
    ret = testDevice();
    if (ret == -1) {
        printf("testDevice failed");
        return -1; 
    }
}
