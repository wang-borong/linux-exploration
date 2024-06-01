/*
 * Copyright © 2024 Jason Wang. All Rights Reserved.
 *
 * This module is an example to delayed workqueue.
 */

#include <linux/module.h>
#include <linux/workqueue.h>


#define MODULE_NAME "wqd-test"

struct work_cont {
	struct delayed_work dwk;
	int arg;
};

static struct work_cont wc;

static void work_func(struct work_struct *w)
{
	struct delayed_work *dwk;
	struct work_cont *wcp;
	
	dwk = container_of(w, struct delayed_work, work);
	wcp = container_of(dwk, struct work_cont, dwk);

	printk(KERN_INFO "[Deferred work]=> PID: %d; NAME: %s\n", current->pid, current->comm);
	printk(KERN_INFO "[Deferred work]=> BTW the data is: %d\n", wcp->arg);
}

static int wqd_test(void)
{
	INIT_DELAYED_WORK(&wc.dwk, work_func);
	wc.arg = 100;

	pr_info("launching the delayed work for 2 seconds\n");
	schedule_delayed_work(&wc.dwk, (2 * HZ));

	return 0;
}

static int __init wqd_test_init(void)
{
	pr_info(MODULE_NAME " init\n");

	wqd_test();

	return 0;
}

static void __exit wqd_test_exit(void)
{

	pr_info(MODULE_NAME " exit\n");
}

module_init(wqd_test_init);
module_exit(wqd_test_exit);

MODULE_AUTHOR("Wangborong");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("The dealyed workqueue example");
MODULE_VERSION("0.1");
