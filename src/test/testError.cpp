#include <pcap.h>
#include <unistd.h>
#include <stdio.h>
#include <utils/errorutils.h>
#include <fcntl.h>

int main() {
    char buf[10000];
    open("dasfdsafa",0);
    ERROR_SYSTEM("Open an non-existing file");
    pcap_t *p = pcap_create("aaaa", buf);
    pcap_activate(p);
    ERROR_PCAP("Activate an non-existing device",p);
    ERROR_FATAL_USER("Failed");
    printf("This should not be printed");
}
