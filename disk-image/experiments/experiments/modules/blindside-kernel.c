// https://github.com/sysprog21/lkmpg
/*
 * procfs2.c -  create a "file" in /proc
 */

#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use the proc fs */
#include <linux/uaccess.h> /* for copy_from_user */
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

//#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "blindside-victim"
#define NUM_TRAIN 5

/* This structure hold information about the /proc file */
static struct proc_dir_entry *our_proc_file;

/* The buffer used to store character for this module */
//static char procfs_buffer[PROCFS_MAX_SIZE];

/* The size of the buffer */
//static unsigned long procfs_buffer_size = 0;

typedef struct blindside_command_t {
    unsigned long f_ptr;
    unsigned long idx;
} blindside_command;

int (*func_ptrs[NUM_TRAIN + 1])(void);
int foo_x;
int foo_y;
static volatile unsigned long __attribute__((aligned(32768))) secret_leak_limit = NUM_TRAIN;

/* This function is called then the /proc file is read */
static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset) {
//    char s[13] = "HelloWorld!\n";
//    int len = sizeof(s);
//    ssize_t ret = len;
//
//    if (*offset >= len || copy_to_user(buffer, s, len)) {
//        pr_info("copy_to_user failed\n");
//        ret = 0;
//    } else {
//        pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
//        *offset += len;
//    }
//
//    return ret;
    return 0;
}

int test_foo(void) {
    if (foo_x == foo_y) {
        pr_info("test_foo\n");
    }

    return (foo_x * 353 - foo_y % 23) * foo_x / foo_y;
}

inline void jump_to_address(void* address) {
    asm volatile (
            "call *%0"  // Indirect call to the address pointed to by %0
            :
            : "r" (address)
            : "memory" // clobber memory to prevent compiler optimizations
            );
}

void blindside(unsigned long f_ptr, unsigned long idx) {
//    int (*f)(void);
    void *f;

    f = (idx ==  NUM_TRAIN) ? (void *) f_ptr : (void *) test_foo;
//    func_ptrs[NUM_TRAIN] = (void *) f_ptr;
//    pr_info("Before clflush Blindside f_ptr: %lx, idx: %lx\n", f_ptr, idx);
    clflush(&secret_leak_limit);
//    pr_info("After clflush Blindside f_ptr: %lx, idx: %lx\n", f_ptr, idx);
    if (idx != secret_leak_limit)
        jump_to_address(f);
    return;
}

/* This function is called with the /proc file is written. */
static ssize_t procfile_write(struct file *file, const char __user *buff,
                              size_t len, loff_t *off) {
    blindside_command user_cmd;

    if (len != sizeof(user_cmd)) {
        return len;
    }

    if (copy_from_user(&user_cmd, buff, len))
        return -EFAULT;

    blindside(user_cmd.f_ptr, user_cmd.idx);
    pr_info("Blindside f_ptr: %lx, idx: %lx\n", user_cmd.f_ptr, user_cmd.idx);

    return len;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
    .proc_write = procfile_write,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
    .write = procfile_write,
};
#endif

static int __init procfs2_init(void) {
    unsigned long i;

    pr_info("Init Blindside kernel module\n");
    for (i = 0; i < NUM_TRAIN; i++)
        func_ptrs[i] = test_foo;
    foo_x = 0;
    foo_y = 1;

    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    if (NULL == our_proc_file) {
        pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

static void __exit procfs2_exit(void) {
    pr_info("Exit Blindside kernel module\n");

    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(procfs2_init);
module_exit(procfs2_exit);

MODULE_LICENSE("GPL");
