#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

typedef int (*fptr)();

int foo() {
    return 1;
}

int main() {
    int x = 0;
    uint64_t correct_aslr_delta = ((uint64_t) foo) >> 48;
    uint64_t incorrect_aslr_delta =
            correct_aslr_delta < 31 ? correct_aslr_delta + 1 : correct_aslr_delta - 1;
    fptr incorrect_pointer = (void *) ((((uint64_t) foo) & 0xffffffffffff) | (incorrect_aslr_delta << 48));
    printf("hello world foo %p incorrect foo %p\n", foo, incorrect_pointer);
    sleep(1);
    // Jump to the invalid address
    incorrect_pointer();
}