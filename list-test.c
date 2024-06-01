/*
 * Copyright © 2024 Jason Wang. All Rights Reserved.
 */

#include <linux/module.h>
#include <linux/list.h>

#define MODULE_NAME "list-test"

LIST_HEAD(my_list);

struct my_struct {
	int data;
	struct list_head node;
};

static int list_test(void)
{
	struct my_struct ms[5];
	struct my_struct msx[5];
	struct list_head *i;
	struct my_struct *msp;

	ms[0].data = 1;
	ms[1].data = 2;
	ms[2].data = 3;
	ms[3].data = 4;
	ms[4].data = 5;
	list_add(&ms[0].node, &my_list);
	list_add(&ms[1].node, &my_list);
	list_add(&ms[2].node, &my_list);
	list_add(&ms[3].node, &my_list);
	list_add(&ms[4].node, &my_list);


	list_for_each(i, &my_list) {
		msp = list_entry(i, struct my_struct, node);
		pr_info("%d\n", msp->data);
	}

	list_del(&ms[0].node);
	list_del(&ms[1].node);
	list_del(&ms[2].node);
	list_del(&ms[3].node);
	list_del(&ms[4].node);

	pr_info("---------------------\n");

	msx[0].data = 1;
	msx[1].data = 2;
	msx[2].data = 3;
	msx[3].data = 4;
	msx[4].data = 5;
	list_add_tail(&msx[0].node, &my_list);
	list_add_tail(&msx[1].node, &my_list);
	list_add_tail(&msx[2].node, &my_list);
	list_add_tail(&msx[3].node, &my_list);
	list_add_tail(&msx[4].node, &my_list);

	list_for_each_entry(msp, &my_list, node) {
		pr_info("%d\n", msp->data);
	}

	return 0;
}

static int __init list_test_init(void)
{
	pr_info(MODULE_NAME " init\n");

	list_test();

	return 0;
}

static void __exit list_test_exit(void)
{
	pr_info(MODULE_NAME " exit\n");
}

module_init(list_test_init);
module_exit(list_test_exit);

MODULE_AUTHOR("Wangborong");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("bitmap test");
MODULE_VERSION("0.1");
