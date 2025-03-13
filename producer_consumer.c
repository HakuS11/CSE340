#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Producer Consumer Kernel Module");


static int prod = 2;
static int cons = 2;
static int size = 5;

module_param(prod, int, 0);
module_param(cons, int, 0);
module_param(size, int, 0);

static struct semaphore empty;
static struct semaphore full;

static int producer_thread(void *arg) {

    int id = *(int *)arg;

    while (!kthread_should_stop()) {
        //Try to acquite the empty semaphore
        if(down_interruptible(&empty)){
          break;
    }
        //critical section
        printk(KERN_INFO "An item has been produced by %d\n", id);

        //signal that an item is available
        up(&full);

        // Simulate work by sleeping 
        msleep(100); 
    }
    return 0;
}

static struct task_struct **producer_threads;
static struct task_struct **consumer_threads;
static int *producer_ids;
static int *consumer_ids;

static int __init producer_consumer_init(void) {
    int i;
    // Initialize semaphores
    sema_init(&empty, size);
    sema_init(&full, 0);

    //Allocate memory for thread tracking
    producer_tasks = kmalloc(prod * sizeof(struct task_struct *), GFP_KERNEL);
    consumer_tasks = kmalloc(cons * sizeof(struct task_struct *), GFP_KERNEL);
    producer_ids = kmalloc(prod * sizeof(int), GFP_KERNEL);
    consumer_ids = kmalloc(cons * sizeof(int), GFP_KERNEL);

    //Create producer threads
    for (i = 0; i < prod; i++) {
        producer_ids[i] = i + 1;
        producer_tasks[i] = kthread_run(producer_thread, &producer_ids[i], "Producer-%d, i + 1);
        if (IS_ERR(producer_tasks[i])) {
          printk(KERR_ERR "Failed to create produer thread %d\n", i + 1);
          return -ENOMEM
        }
    }

}

static void __exit producer_consumer_exit(void) {
    int i;

    //stop producer threads
    for (i = 0; i < prod; i++) {
        if (producer_threads[i]) {
            kthread_stop(producer_tasks[i]);
        }
    }

    //free allocated memory
    kfree(producer_tasks);
    kfree(consumer_tasks);

    printk(KERN_INFO "Producer-Consumer module unloaded\n");
}

module_init(producer_consumer_init);
module_exit(producer_consumer_exit);
