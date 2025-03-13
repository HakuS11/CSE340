#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>

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
        // Try to acquire the empty semaphore
        if (down_interruptible(&empty)) {
            break;
        }
        // Critical section
        printk(KERN_INFO "An item has been produced by Producer-%d\n", id);
        // Signal that an item is available
        up(&full);
        // Simulate work by sleeping 
        msleep(100); 
    }
    return 0;
}

static int consumer_thread(void *arg) {
    int id = *(int *)arg;

    while (!kthread_should_stop()) {
        // Try to acquire the full semaphore
        if (down_interruptible(&full)) {
            break;
        }
        // Critical section
        printk(KERN_INFO "An item has been consumed by Consumer-%d\n", id);
        // Signal that an item has been consumed
        up(&empty);
        // Simulate work by sleeping 
        msleep(100); 
    }
    return 0;
}

static struct task_struct **producer_tasks;
static struct task_struct **consumer_tasks;
static int *producer_ids;
static int *consumer_ids;

static int __init producer_consumer_init(void) {
    int i;

    // Initialize semaphores
    sema_init(&empty, size);
    sema_init(&full, 0);

    // Allocate memory for thread tracking
    producer_tasks = kmalloc(prod * sizeof(struct task_struct *), GFP_KERNEL);
    consumer_tasks = kmalloc(cons * sizeof(struct task_struct *), GFP_KERNEL);
    producer_ids = kmalloc(prod * sizeof(int), GFP_KERNEL);
    consumer_ids = kmalloc(cons * sizeof(int), GFP_KERNEL);

    // Create producer threads
    for (i = 0; i < prod; i++) {
        producer_ids[i] = i + 1;
        producer_tasks[i] = kthread_run(producer_thread, &producer_ids[i], "Producer-%d", i + 1);
        if (IS_ERR(producer_tasks[i])) {
            printk(KERN_ERR "Failed to create producer thread %d\n", i + 1);
            return PTR_ERR(producer_tasks[i]);
        }
    }

    // Create consumer threads
    for (i = 0; i < cons; i++) {
        consumer_ids[i] = i + 1;
        consumer_tasks[i] = kthread_run(consumer_thread, &consumer_ids[i], "Consumer-%d", i + 1);
        if (IS_ERR(consumer_tasks[i])) {
            printk(KERN_ERR "Failed to create consumer thread %d\n", i + 1);
            return PTR_ERR(consumer_tasks[i]);
        }
    }

    return 0;
}

static void __exit producer_consumer_exit(void) {
    int i;

    // Stop producer threads
    for (i = 0; i < prod; i++) {
        if (producer_tasks[i]) {
            kthread_stop(producer_tasks[i]);
        }
    }

    // Stop consumer threads
    for (i = 0; i < cons; i++) {
        if (consumer_tasks[i]) {
            kthread_stop(consumer_tasks[i]);
        }
    }

    // Free allocated memory
    kfree(producer_tasks);
    kfree(consumer_tasks);
    kfree(producer_ids);
    kfree(consumer_ids);

    printk(KERN_INFO "Producer-Consumer module unloaded\n");
}

module_init(producer_consumer_init);
module_exit(producer_consumer_exit);
