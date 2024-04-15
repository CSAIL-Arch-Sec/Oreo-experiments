#include <stdio.h>

int foo() {
    return 1;
}

int main() {
    int x = 0;
    printf("hello world foo %p &x %p\n", foo, &x);
}