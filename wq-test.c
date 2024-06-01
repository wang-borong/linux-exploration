/*
 * Copyright © 2024 Jason Wang. All Rights Reserved.
 *
 * This module is an example to workqueue.
 */

#include <linux/module.h>
#include <linux/workqueue.h>

#define MODULE_NAME "wq-test"

struct work_cont {
	struct work_struct wk;
	int arg;
};

static void work_func(struct work_struct *w)
{
	struct work_cont *c_ptr = container_of(w, struct work_cont, wk);

	printk(KERN_INFO "[Deferred work]=> PID: %d; NAME: %s\n", current->pid, current->comm);
	printk(KERN_INFO "[Deferred work]=> I am going to sleep 2 seconds\n");
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(2 * HZ); //Wait 2 seconds

	printk(KERN_INFO "[Deferred work]=> DONE. BTW the data is: %d\n", c_ptr->arg);
}


static int wq_test(void)
{
	struct work_cont *wc;

	wc = kmalloc(sizeof(*wc), GFP_KERNEL);
	INIT_WORK(&wc->wk, work_func);
	wc->arg = 10;
	schedule_work(&wc->wk);

	return 0;
}

static int __init wq_test_init(void)
{
	pr_info(MODULE_NAME " init\n");

	wq_test();

	return 0;
}

static void __exit wq_test_exit(void)
{
	pr_info(MODULE_NAME " exit\n");
}

module_init(wq_test_init);
module_exit(wq_test_exit);

MODULE_AUTHOR("Wangborong");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("a workqueue example");
MODULE_VERSION("0.1");
