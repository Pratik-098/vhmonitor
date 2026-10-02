#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../shared/vhmonitor_ioctl.h"

int main(int argc, char *argv[])
{
    if (argc != 2 ||
        (std::strcmp(argv[1], "read") != 0 &&
         std::strcmp(argv[1], "status") != 0)) {
        std::cerr << "Usage: " << argv[0] << " <read|status>\n";
        return 1;
    }

    const int fd = open("/dev/vhmonitor", O_RDONLY);
    if (fd == -1) {
        std::perror("open /dev/vhmonitor");
        return 1;
    }

    bool success = true;

    if (std::strcmp(argv[1], "read") == 0) {
        char buffer[128];

        while (true) {
            const ssize_t bytes = read(fd, buffer, sizeof(buffer));

            if (bytes == -1) {
                if (errno == EINTR)
                    continue;
                std::perror("read");
                success = false;
                break;
            }

            if (bytes == 0)
                break;

            std::cout.write(buffer, bytes);
        }
    } else {
        vhmonitor_data data{};

        if (ioctl(fd, VHMONITOR_GET_DATA, &data) == -1) {
            std::perror("VHMONITOR_GET_DATA");
            success = false;
        } else {
            std::cout << std::fixed << std::setprecision(3)
                      << "Temperature: "
                      << data.temperature_mc / 1000.0 << " C\n"
                      << "Voltage: " << data.voltage_mv << " mV\n"
                      << "Fan: "
                      << (data.fan_status == VHMONITOR_STATUS_OK
                              ? "OK" : "FAILED") << '\n'
                      << "Device: "
                      << (data.device_status == VHMONITOR_STATUS_OK
                              ? "OK" : "FAILED") << '\n'
                      << "Fault code: " << data.fault_state << '\n';
        }
    }

    if (close(fd) == -1) {
        std::perror("close");
        success = false;
    }

    std::cout.flush();
    if (!std::cout) {
        std::cerr << "Failed to write monitoring output\n";
        success = false;
    }

    return success ? 0 : 1;
}
