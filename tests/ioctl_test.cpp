#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../shared/vhmonitor_ioctl.h"

int main()
{
    static_assert(sizeof(vhmonitor_data) == 20,
                  "Unexpected IOCTL data layout");

    const int fd = open("/dev/vhmonitor", O_RDONLY);
    if (fd == -1) {
        std::perror("open");
        return 1;
    }

    bool passed = true;
    vhmonitor_data data{};

    if (ioctl(fd, VHMONITOR_GET_DATA, &data) == -1) {
        std::perror("VHMONITOR_GET_DATA");
        passed = false;
    } else {
        const bool normal =
            data.temperature_mc == 35000 &&
            data.voltage_mv == 12000 &&
            data.fan_status == VHMONITOR_STATUS_OK &&
            data.device_status == VHMONITOR_STATUS_OK &&
            data.fault_state == VHMONITOR_FAULT_NONE;

        std::printf("Normal readings: %s\n", normal ? "PASS" : "FAIL");
        passed = passed && normal;
    }

    errno = 0;
    const int unsupported = ioctl(fd, _IO(VHMONITOR_IOC_MAGIC, 255));
    const bool rejected = unsupported == -1 && errno == ENOTTY;
    std::printf("Unsupported request: %s\n", rejected ? "PASS" : "FAIL");
    passed = passed && rejected;

    errno = 0;
    const int invalid = ioctl(fd, VHMONITOR_GET_DATA,
                              static_cast<void *>(nullptr));
    const bool bad_pointer = invalid == -1 && errno == EFAULT;
    std::printf("Invalid destination: %s\n", bad_pointer ? "PASS" : "FAIL");
    passed = passed && bad_pointer;

    if (close(fd) == -1) {
        std::perror("close");
        passed = false;
    }

    return passed ? 0 : 1;
}
