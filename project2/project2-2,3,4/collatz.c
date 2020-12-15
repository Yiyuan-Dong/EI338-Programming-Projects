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
#include <linux/moduleparam.h>

#define BUFFER_SIZE 128

#define PROC_NAME "collatz"

static int start = 25;

module_param(start, int, 0);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
};

struct node{
    int number;
    struct list_head list;
};

static LIST_HEAD(node_list);


/* This function is called when the module is loaded. */
int proc_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_ops);

    printk( KERN_INFO "/proc/%s created\n", PROC_NAME);

    struct node *node_ptr;

    int curr_num = start;
    int count = 0;

    while (1){
        node_ptr = kmalloc(sizeof(*node_ptr), GFP_KERNEL);
        node_ptr->number = curr_num;
        INIT_LIST_HEAD(&node_ptr->list);
        list_add_tail(&node_ptr->list, &node_list);
        count++;

        if (curr_num == 1)
            break;

        if (curr_num & 1){
            curr_num = curr_num * 3 + 1;
        }
        else{
            while (! (curr_num & 1) ){
                curr_num /= 2;
            }
        }
    }


    list_for_each_entry(node_ptr, &node_list, list){
        printk( KERN_INFO "%d\n", node_ptr->number);
    }

    printk( KERN_INFO "total number: %d\n", count);

    return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);

    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);

    struct node *ptr, *next;
    int count = 0;
    list_for_each_entry_safe(ptr, next, &node_list, list){
        list_del(&ptr->list);
        kfree(ptr);
        count++;
    }

    printk( KERN_INFO "Free number : %d\n", count);
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Collatz Tasks' Module");
MODULE_AUTHOR("SGG");