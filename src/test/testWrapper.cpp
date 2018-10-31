#include <wrapper/unix.h>
#include <stdio.h>

int a() {
    Close(10, ErrorBehavior("This should return -1", true, RETURN_MINUS_ONE));
    return 0;
}

int b() {
    Close(10, ErrorBehavior("Something will be printed", false, USER_ACTION), printf("1234.\n");return -3);
}

int main() {
    void *t = Calloc(5, 5);
    printf("%d\n", a());
    Close(10);
    printf("should be -3: %d\n", b());
    Close(10, ErrorBehavior("This should be fatal and no more would be printed, and the program will end with -1", true, EXIT_MINUS_ONE));
    Close(10);
}
