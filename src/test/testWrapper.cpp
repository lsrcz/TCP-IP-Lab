#include <protocol/device.h>
#include <stdio.h>
#include <sys/stat.h>
#include <wrapper/pcap.h>
#include <wrapper/unix_file.h>

int b() {
    Close(10, ErrorBehavior("Something will be printed", false, true),
          printf("1234., return -3\n");
          return -3);
    return 1;
}

int main() {
    char buf[1000];
    for (auto& p : listAvailableDevices()) {
        printf("%s\n", p.c_str());
    }

    pcap_t* p = Pcap_create("ens33", buf);
    if (p == NULL)
        printf("is null");
    Pcap_activate(p);

    Close(10, 10);
    Open("/tmp/aaaa", O_WRONLY);
    Open("/root/aaa", O_CREAT, 0);
    Open("/tmp/aaaa", O_WRONLY,
         ErrorBehavior("User msg can be printed", false, true),
         printf("User action can be executed\n"));
    Close(10);
    printf("should be -3: %d\n", b());
    Close(10,
          ErrorBehavior("This should be fatal and no more would be printed, "
                        "and the program will end with -1",
                        true, true),
          return -1);
    Close(10);
}
