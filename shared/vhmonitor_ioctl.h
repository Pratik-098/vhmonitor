#ifndef VHMONITOR_IOCTL_H
#define VHMONITOR_IOCTL_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define VHMONITOR_STATUS_OK     0U
#define VHMONITOR_STATUS_FAILED 1U

#define VHMONITOR_FAULT_NONE     0U
#define VHMONITOR_FAULT_OVERHEAT 1U
#define VHMONITOR_FAULT_VOLTAGE  2U
#define VHMONITOR_FAULT_FAN      3U

struct vhmonitor_data {
    __s32 temperature_mc;
    __u32 voltage_mv;
    __u32 fan_status;
    __u32 device_status;
    __u32 fault_state;
};

#define VHMONITOR_IOC_MAGIC 'V'

#define VHMONITOR_GET_DATA \
    _IOR(VHMONITOR_IOC_MAGIC, 1, struct vhmonitor_data)

#define VHMONITOR_SET_FAULT \
    _IOW(VHMONITOR_IOC_MAGIC, 2, __u32)

#define VHMONITOR_RESET \
    _IO(VHMONITOR_IOC_MAGIC, 3)

#endif
