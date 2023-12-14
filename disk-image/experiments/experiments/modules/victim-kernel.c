//
// Created by shixinsong on 10/5/23.
//
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/fs.h>
#include <linux/proc_fs.h>

#include "victim-kernel.h"

#define NUM_TRAIN 5

MODULE_LICENSE("GPL");

typedef struct blindside_command_t {
    unsigned long f_ptr;
    unsigned long idx;
} blindside_command;

static struct proc_dir_entry *blindside_victim = NULL;
static const struct file_operations  blindside_victim_ops = {
        .write = blindside_victim_write,
        .read = blindside_victim_read,
};

int (*func_ptrs[NUM_TRAIN])();

int foo_x;
int foo_y;

static volatile unsigned long __attribute__((aligned(32768))) secret_leak_limit = NUM_TRAIN;

int test_foo() {
    if (foo_x == foo_y) {
        pr_info("test_foo\n");
    }

    return (foo_x * 353 - foo_y % 23) * foo_x / foo_y;
}

int init_module(void) {
    unsigned long i;

    pr_info("Attack kernel module.\n");
    for (i = 0; i < NUM_TRAIN; i++)
        func[i] = test_foo;
    foo_x = 0;
    foo_y = 1;

    blindside_victim = proc_create("blindside-victim", 0, NULL, &blindside_victim_ops);

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

void blindside(unsigned long f_ptr, unsigned long idx) {
    func[NUM_TRAIN] = f_ptr;
    clflush(&secret_leak_limit);
    if (idx != secret_leak_limit)
        (func_ptrs[idx])();
}

ssize_t blindside_victim_read(struct file *file_in, char __user *userbuf, size_t num_bytes, loff_t *offset) {
    // Reading does nothing
    return 0;
}

ssize_t blindside_victim_write(struct file *file_in, const char __user *userbuf, size_t num_bytes, loff_t *offset) {
    blindside_command_t user_command;


}

void cleanup_module(void) {
    pr_info("Unload system call helper\n");
    proc_remove(blindside_victim);
}
