#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <asm/param.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "list_linear"

static int line_number;

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
};

struct task_struct *task;

/* This function is called when the module is loaded. */
int proc_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_ops);

    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    printk( KERN_INFO "  PID   STATE COMMAND \n");
    for_each_process(task){   // It's OK to just start from NULL...
        printk( KERN_INFO "%5d %8ld %s\n", task->pid, task->state , task->comm);
        line_number++;
    }

    printk( KERN_INFO "wc -l : %d\n", line_number);

    return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);

    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linear List Tasks Module");
MODULE_AUTHOR("SGG");