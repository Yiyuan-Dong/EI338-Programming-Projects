#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <asm/param.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define BUFFER_SIZE 128

#define PROC_NAME "list_traverse"

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
};

struct color{
    int red;
    int blue;
    int green;
    struct list_head list;
};

static LIST_HEAD(color_list);


/* This function is called when the module is loaded. */
int proc_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_ops);

    printk( KERN_INFO "/proc/%s created\n", PROC_NAME);

    struct color *violet;

    int i;
    for (i = 0; i < 4; ++i) {
        violet = kmalloc(sizeof(*violet), GFP_KERNEL);
        violet->red = i + 100;
        violet->blue = i * 5 + 110;
        violet->green = i * 10 + 200;
        INIT_LIST_HEAD(&violet->list);
        list_add_tail(&violet->list, &color_list);
    }

    struct color *ptr;
    printk(KERN_INFO "    RED   BLUE  GREEN\n");
    list_for_each_entry(ptr, &color_list, list){
        printk( KERN_INFO "%7d%7d%7d\n", ptr->red, ptr->blue, ptr->green);
    }

    return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);

    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);

    struct color *ptr, *next;
    list_for_each_entry_safe(ptr, next, &color_list, list){
        list_del(&ptr->list);
        kfree(ptr);
    }
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linear Traverse Tasks Module");
MODULE_AUTHOR("SGG");