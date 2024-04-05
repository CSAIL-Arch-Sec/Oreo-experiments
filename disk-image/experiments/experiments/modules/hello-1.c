/* 
 * hello-1.c - The simplest kernel module. 
 */ 
#include <linux/module.h> /* Needed by all modules */ 
#include <linux/printk.h> /* Needed for pr_info() */

void foo(void)
{
    pr_info("foo\n");
}
 
int init_module(void) 
{ 
    pr_info("@@@ Hello world 1.\n");
    pr_info("@@@ foo %lx\n", (unsigned long) foo);
 
    /* A non 0 return means init_module failed; module can't be loaded. */ 
    return 0; 
} 
 
void cleanup_module(void) 
{ 
    pr_info("@@@ Goodbye world 1.\n");
} 
 
MODULE_LICENSE("GPL");