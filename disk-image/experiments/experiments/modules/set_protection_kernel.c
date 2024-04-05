#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use the proc fs */
#include <linux/uaccess.h> /* for copy_from_user */
#include <linux/version.h>

#include <asm/setup.h>
#include <asm/page_types.h>
#include <asm/pgtable_64.h>
#include <asm/kaslr.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_NAME "set_protection"

/* This structure hold information about the /proc file */
static struct proc_dir_entry *our_proc_file;

typedef struct set_protection_command_t {
    unsigned long module_delta;
    unsigned long user_delta;
} set_protection_command;


void set_protection(unsigned long module_delta, unsigned long user_delta) {
    // Set delta for module and user ASLR
    set_gem5_module_high_delta(module_delta);
    set_gem5_user_high_offset(gem5_aslr_get_offset(user_delta));

    pr_info("@@@ module_delta %lx, user_delta %lx\n",
            user_delta, user_delta);
}


/* This function is called then the /proc file is read */
static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset) {
    return 0;
}


/* This function is called with the /proc file is written. */
static ssize_t procfile_write(struct file *file, const char __user *buff,
                              size_t len, loff_t *off) {
    set_protection_command user_cmd;

    if (len != sizeof(user_cmd)) {
        return len;
    }

    if (copy_from_user(&user_cmd, buff, len))
        return -EFAULT;

    set_protection(user_cmd.module_delta, user_cmd.user_delta);

    pr_info("Set ASLR protection module_delta: %lx, user_delta: %lx\n",
            user_cmd.module_delta, user_cmd.user_delta);

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
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    if (NULL == our_proc_file) {
        pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

static void __exit procfs2_exit(void) {
    pr_info("Exit set_protection kernel module\n");

    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(procfs2_init);
module_exit(procfs2_exit);

MODULE_LICENSE("GPL");

