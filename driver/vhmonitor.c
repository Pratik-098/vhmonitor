// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/mutex.h>

#include "../shared/vhmonitor_ioctl.h"

#define DEVICE_NAME "vhmonitor"

static dev_t vhmonitor_dev;
static struct cdev vhmonitor_cdev;
static DEFINE_MUTEX(vhmonitor_lock);

static const struct vhmonitor_data normal_state = {
    .temperature_mc = 35000,
    .voltage_mv = 12000,
    .fan_status = VHMONITOR_STATUS_OK,
    .device_status = VHMONITOR_STATUS_OK,
    .fault_state = VHMONITOR_FAULT_NONE,
};

static struct vhmonitor_data vhmonitor_state;

static const char *vhmonitor_fault_name(__u32 fault)
{
    switch (fault) {
    case VHMONITOR_FAULT_OVERHEAT:
        return "OVERHEAT";
    case VHMONITOR_FAULT_VOLTAGE:
        return "VOLTAGE";
    case VHMONITOR_FAULT_FAN:
        return "FAN";
    default:
        return "NONE";
    }
}

static int vhmonitor_open(struct inode *inode, struct file *file)
{
    pr_info("vhmonitor: device opened\n");
    return 0;
}

static int vhmonitor_release(struct inode *inode, struct file *file)
{
    pr_info("vhmonitor: device released\n");
    return 0;
}

static ssize_t vhmonitor_read(struct file *file, char __user *buffer,
                             size_t count, loff_t *position)
{
    struct vhmonitor_data snapshot;
    char text[192];
    int length;

    mutex_lock(&vhmonitor_lock);
    snapshot = vhmonitor_state;
    mutex_unlock(&vhmonitor_lock);

    length = scnprintf(text, sizeof(text),
                       "Temperature: %d C\n"
                       "Voltage: %u mV\n"
                       "Fan: %s\n"
                       "Device: %s\n"
                       "Fault: %s\n",
                       snapshot.temperature_mc / 1000,
                       snapshot.voltage_mv,
                       snapshot.fan_status == VHMONITOR_STATUS_OK
                           ? "OK" : "FAILED",
                       snapshot.device_status == VHMONITOR_STATUS_OK
                           ? "OK" : "FAILED",
                       vhmonitor_fault_name(snapshot.fault_state));

    return simple_read_from_buffer(buffer, count, position, text, length);
}

static long vhmonitor_ioctl(struct file *file, unsigned int command,
                            unsigned long argument)
{
    struct vhmonitor_data snapshot;
    __u32 fault;

    switch (command) {
    case VHMONITOR_GET_DATA:
        mutex_lock(&vhmonitor_lock);
        snapshot = vhmonitor_state;
        mutex_unlock(&vhmonitor_lock);

        if (copy_to_user((void __user *)argument,
                         &snapshot, sizeof(snapshot)))
            return -EFAULT;
        return 0;

    case VHMONITOR_SET_FAULT:
        if (!(file->f_mode & FMODE_WRITE))
            return -EBADF;

        if (copy_from_user(&fault, (void __user *)argument, sizeof(fault)))
            return -EFAULT;

        if (fault != VHMONITOR_FAULT_OVERHEAT &&
            fault != VHMONITOR_FAULT_VOLTAGE &&
            fault != VHMONITOR_FAULT_FAN)
            return -EINVAL;

        mutex_lock(&vhmonitor_lock);
        vhmonitor_state = normal_state;
        vhmonitor_state.fault_state = fault;
        vhmonitor_state.device_status = VHMONITOR_STATUS_FAILED;

        switch (fault) {
        case VHMONITOR_FAULT_OVERHEAT:
            vhmonitor_state.temperature_mc = 95000;
            break;
        case VHMONITOR_FAULT_VOLTAGE:
            vhmonitor_state.voltage_mv = 15000;
            break;
        case VHMONITOR_FAULT_FAN:
            vhmonitor_state.fan_status = VHMONITOR_STATUS_FAILED;
            break;
        }

        mutex_unlock(&vhmonitor_lock);
        return 0;

    case VHMONITOR_RESET:
        if (!(file->f_mode & FMODE_WRITE))
            return -EBADF;

        mutex_lock(&vhmonitor_lock);
        vhmonitor_state = normal_state;
        mutex_unlock(&vhmonitor_lock);
        return 0;

    default:
        return -ENOTTY;
    }
}

static const struct file_operations vhmonitor_fops = {
    .owner = THIS_MODULE,
    .open = vhmonitor_open,
    .read = vhmonitor_read,
    .release = vhmonitor_release,
    .unlocked_ioctl = vhmonitor_ioctl,
};

static int __init vhmonitor_init(void)
{
    int ret;

    vhmonitor_state = normal_state;

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
