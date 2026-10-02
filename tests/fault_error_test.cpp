#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../shared/vhmonitor_ioctl.h"

int main()
{
    const int fd = open("/dev/vhmonitor", O_RDWR);
    if (fd == -1) {
        std::perror("open read/write");
        return 1;
    }

    const int readonly_fd = open("/dev/vhmonitor", O_RDONLY);
    if (readonly_fd == -1) {
        std::perror("open read-only");
        close(fd);
        return 1;
    }

    bool passed = true;

    const auto check = [&](bool condition, const char *name) {
        std::printf("%s: %s\n", name, condition ? "PASS" : "FAIL");
        if (!condition)
            passed = false;
    };

    __u32 fault = VHMONITOR_FAULT_OVERHEAT;
    if (ioctl(fd, VHMONITOR_SET_FAULT, &fault) == -1) {
        std::perror("set initial overheat fault");
        close(readonly_fd);
        close(fd);
        return 1;
    }

    __u32 invalid_fault = 999;
    errno = 0;
    int result = ioctl(fd, VHMONITOR_SET_FAULT, &invalid_fault);
    check(result == -1 && errno == EINVAL,
          "Invalid fault rejected with EINVAL");

    errno = 0;
    result = ioctl(fd, VHMONITOR_SET_FAULT,
                   static_cast<void *>(nullptr));
    check(result == -1 && errno == EFAULT,
          "Invalid fault pointer rejected with EFAULT");

    fault = VHMONITOR_FAULT_FAN;
    errno = 0;
    result = ioctl(readonly_fd, VHMONITOR_SET_FAULT, &fault);
    check(result == -1 && errno == EBADF,
          "Read-only fault request rejected with EBADF");

    errno = 0;
    result = ioctl(readonly_fd, VHMONITOR_RESET);
    check(result == -1 && errno == EBADF,
          "Read-only reset rejected with EBADF");

    vhmonitor_data data{};
    result = ioctl(fd, VHMONITOR_GET_DATA, &data);
    check(result == 0 &&
              data.temperature_mc == 95000 &&
              data.voltage_mv == 12000 &&
              data.fan_status == VHMONITOR_STATUS_OK &&
              data.device_status == VHMONITOR_STATUS_FAILED &&
              data.fault_state == VHMONITOR_FAULT_OVERHEAT,
          "Rejected requests preserve device state");

    result = ioctl(fd, VHMONITOR_RESET);
    check(result == 0, "Reset after testing");

    data = {};
    result = ioctl(fd, VHMONITOR_GET_DATA, &data);
    check(result == 0 &&
              data.temperature_mc == 35000 &&
              data.voltage_mv == 12000 &&
              data.fan_status == VHMONITOR_STATUS_OK &&
              data.device_status == VHMONITOR_STATUS_OK &&
              data.fault_state == VHMONITOR_FAULT_NONE,
          "Normal state restored");

    check(close(readonly_fd) == 0, "Close read-only descriptor");
    check(close(fd) == 0, "Close read/write descriptor");

    return passed ? 0 : 1;
}
