#include <wrapper/unix.h>
#include <stdio.h>

int main() {
    void *t = Calloc(5, 5);
    printf("%x\n", (long long)t);
    Close(10);
    Close(10, fatalBehavior("This should be the last line"));
    Close(10);
}
