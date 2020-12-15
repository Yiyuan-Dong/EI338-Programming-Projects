#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>
#include <asm/param.h>

/* This function is called when the module is loaded. */

int simple_init(void)
{
	printk(KERN_INFO "Loading Kernel Module\n");
	printk(KERN_INFO "%lu\n", GOLDEN_RATIO_PRIME);
	return 0;
}

/* This function is called when the module is removed. */

void simple_exit(void)
{
	printk(KERN_INFO "Removing Kernel Module\n");
	printk(KERN_INFO "%lu\n", gcd(3300, 24));
}
/* Macros for registering module entry and exit points. */

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
