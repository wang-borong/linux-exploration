/*
 * Copyright © 2024 Jason Wang. All Rights Reserved.
 */


#include <linux/module.h>
#include <linux/types.h>
#include <asm/bitops.h>

#define MODULE_NAME "bitmap-test"

DECLARE_BITMAP(mybm, 32);

static void bitmap_test(void)
{
	int n;
	pr_info("mybm array len = %ld\n", ARRAY_SIZE(mybm));
	set_bit(7, mybm);
	pr_info("mybm value = %lx\n", mybm[0]);
	n = find_first_bit(mybm, 32);
	pr_info("first bit = %d\n", n);
}

static int __init bitmap_test_init(void)
{
	pr_info(MODULE_NAME "init");

	bitmap_test();

	return 0;
}

static void __exit bitmap_test_exit(void)
{
	pr_info(MODULE_NAME "exit");
}

module_init(bitmap_test_init);
module_exit(bitmap_test_exit);

MODULE_AUTHOR("Wangborong");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("bitmap test");
MODULE_VERSION("0.1");
