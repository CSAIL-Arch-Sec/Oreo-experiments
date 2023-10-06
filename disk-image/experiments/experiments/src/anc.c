//
// Created by shixinsong on 10/5/23.
//
//
// Created by shixinsong on 10/2/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/syscall.h>
#include "dummy.h"
#include "m5ops.h"

#define START_KERNEL 0xffffffff80000000
#define ALIGN 0x2000000
#define NUM_OFFSETS 32


char eviction_buffer[64 * 1024]; // 64KB
void prime_l1() {
    uint64_t i = 0;
    for (i = 0; i < 1024; i++) {
        eviction_buffer[i * 64]++;
    }
}


void flush_itlb() {
    uint64_t i = 0;
    for (i = 0; i < sizeof(func) / 8; i++) {
        func[i](i, 1);
    }
}


int main() {
    flush_itlb();
    prime_l1();
    m5_work_begin(0, 0);
    printf("printf triggers system call1\n");
    printf("printf triggers system call2\n");
    printf("printf triggers system call3\n");
    m5_work_end(0, 0);
}
