#include <wrapper/unix_file.h>
#include <sys/stat.h>
#include <stdio.h>

int a() {
    Close(10, ErrorBehavior("This should return -1", true, RETURN_MINUS_ONE));
    return 0;
}

int b() {
    Close(10, ErrorBehavior("Something will be printed", false, USER_ACTION), printf("1234., return -3\n");return -3);
}

int main() {
    Close(10,10);
    Open("/tmp/aaaa", O_WRONLY);
    Open("/root/aaa", O_CREAT, 0);
    Open("/tmp/aaaa", O_WRONLY, ErrorBehavior("User msg can be printed", false, USER_ACTION), printf("User action can be executed\n"));
    Close(10);
    printf("should be -1: %d\n", a());
    printf("should be -3: %d\n", b());
    Close(10, ErrorBehavior("This should be fatal and no more would be printed, and the program will end with -1", true, EXIT_MINUS_ONE));
    Close(10);
}
