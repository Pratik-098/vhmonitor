// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

static int __init vhmonitor_init(void)
{
    pr_info("vhmonitor: module loaded\n");
    return 0;
}

static void __exit vhmonitor_exit(void)
{
    pr_info("vhmonitor: module unloaded\n");
}

module_init(vhmonitor_init);
module_exit(vhmonitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pratik Panda");
MODULE_DESCRIPTION("Virtual hardware monitoring device driver");
MODULE_VERSION("0.1");
