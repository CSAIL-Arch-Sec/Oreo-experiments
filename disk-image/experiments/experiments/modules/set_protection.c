#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/printk.h> /* Needed for pr_info() */

#include <asm/kaslr.h>

static long module_delta;
static long user_delta;

module_param(module_delta, long, 0);
MODULE_PARM_DESC(module_delta, "Delta for module KASLR");

module_param(user_delta, long, 0);
MODULE_PARM_DESC(user_delta, "Delta for user ASLR");

int init_module(void) {
    pr_info("@@@ module set_protection\n");
    pr_info("@@@ module_delta %lx\n", module_delta);
    pr_info("@@@ user_delta %lx\n", user_delta);

    set_gem5_module_high_delta((unsigned long) module_delta);
    set_gem5_user_high_offset((unsigned long) user_delta);

    pr_info("@@@ finish set_protection\n");

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

void cleanup_module(void) {}

MODULE_LICENSE("GPL");

