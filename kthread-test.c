/*
 * Copyright © 2024 Jason Wang. All Rights Reserved.
 */

#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define MODULE_NAME "kthread-test"

struct test_data {
	int a;
	struct mutex mlock;
};

static int thread_func(void *data)
{
	struct test_data *td = (struct test_data *)data;

	pr_info("%d(%s) your data is %d\n", current->pid, current->comm, td->a);

	mutex_lock(&td->mlock);
	td->a++;
	mutex_unlock(&td->mlock);

	return 0;
}

static int thread_func1(void *data)
{
	struct test_data *td = (struct test_data *)data;

	pr_info("%d(%s) your data is %d\n", current->pid, current->comm, td->a);

	mutex_lock(&td->mlock);
	td->a++;
	mutex_unlock(&td->mlock);

	return 0;
}

static int kthread_test(void)
{
	struct task_struct *kt1, *kt2;
	struct test_data td;
	td.a = 100;

	mutex_init(&td.mlock);

	kt1 = kthread_create(thread_func, (void *)&td, "test-thread%d", 0);
	if (!IS_ERR(kt1))
		wake_up_process(kt1);

	kt2 = kthread_run(thread_func1, (void *)&td, "test-thread%d", 1);
	if (!kt2) {
		pr_err("kthread_run create thread failed\n");
	}

	msleep(100);
	mutex_destroy(&td.mlock);

	return 0;
}

static int __init kthread_test_init(void)
{
	pr_info(MODULE_NAME " init\n");

	kthread_test();

	return 0;
}

static void __exit kthread_test_exit(void)
{
	pr_info(MODULE_NAME " exit\n");
}

module_init(kthread_test_init);
module_exit(kthread_test_exit);

MODULE_AUTHOR("Wangborong");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("kthread test");
MODULE_VERSION("0.1");
