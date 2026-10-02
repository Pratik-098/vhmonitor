// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

#define DEVICE_NAME "vhmonitor"

static dev_t vhmonitor_dev;
static struct cdev vhmonitor_cdev;

static const struct file_operations vhmonitor_fops = {
    .owner = THIS_MODULE,
};

static int __init vhmonitor_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&vhmonitor_dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("vhmonitor: device number allocation failed: %d\n", ret);
        return ret;
    }

    cdev_init(&vhmonitor_cdev, &vhmonitor_fops);
    vhmonitor_cdev.owner = THIS_MODULE;

    ret = cdev_add(&vhmonitor_cdev, vhmonitor_dev, 1);
    if (ret < 0) {
        pr_err("vhmonitor: character device registration failed: %d\n", ret);
        unregister_chrdev_region(vhmonitor_dev, 1);
        return ret;
    }

    pr_info("vhmonitor: registered major=%u minor=%u\n",
            MAJOR(vhmonitor_dev), MINOR(vhmonitor_dev));
    pr_info("vhmonitor: character device registered\n");
    pr_info("vhmonitor: module loaded\n");
    return 0;
}

static void __exit vhmonitor_exit(void)
{
    cdev_del(&vhmonitor_cdev);
    unregister_chrdev_region(vhmonitor_dev, 1);
    pr_info("vhmonitor: module unloaded\n");
}

module_init(vhmonitor_init);
module_exit(vhmonitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pratik Panda");
MODULE_DESCRIPTION("Virtual hardware monitoring device driver");
MODULE_VERSION("0.1");
