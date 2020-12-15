#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <asm/param.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "list_dfs"

static int line_num;

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
};

struct task_struct *task;

/* This function is called when the module is loaded. */
void dfs(struct task_struct *start_task){
    struct task_struct *task;
    struct list_head *list;
    list_for_each(list, &start_task->children){
        ++line_num;
        task = list_entry(list, struct task_struct, sibling);
        printk( KERN_INFO "%5d %8ld %s\n", task->pid, task->state , task->comm);
        dfs(task);
    }
}

int proc_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_ops);

    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    printk( KERN_INFO "  PID   STATE COMMAND \n");

    dfs(&init_task);

    printk( KERN_INFO "wc -l : %d\n", line_num);

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
MODULE_DESCRIPTION("DFS List Tasks Module");
MODULE_AUTHOR("SGG");