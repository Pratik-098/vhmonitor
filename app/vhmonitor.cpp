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
    const auto usage = [&]() {
        std::cerr << "Usage:\n"
                  << "  " << argv[0] << " read\n"
                  << "  " << argv[0] << " status\n"
                  << "  " << argv[0] << " fault <overheat|voltage|fan>\n"
                  << "  " << argv[0] << " reset\n";
    };

    if (argc < 2) {
        usage();
        return 1;
    }

    const bool read_command = std::strcmp(argv[1], "read") == 0;
    const bool status_command = std::strcmp(argv[1], "status") == 0;
    const bool fault_command = std::strcmp(argv[1], "fault") == 0;
    const bool reset_command = std::strcmp(argv[1], "reset") == 0;
    __u32 fault = VHMONITOR_FAULT_NONE;

    if (fault_command && argc == 3) {
        if (std::strcmp(argv[2], "overheat") == 0)
            fault = VHMONITOR_FAULT_OVERHEAT;
        else if (std::strcmp(argv[2], "voltage") == 0)
            fault = VHMONITOR_FAULT_VOLTAGE;
        else if (std::strcmp(argv[2], "fan") == 0)
            fault = VHMONITOR_FAULT_FAN;
        else {
            usage();
            return 1;
        }
    } else if (argc != 2 ||
               !(read_command || status_command || reset_command)) {
        usage();
        return 1;
    }

    const int flags = (fault_command || reset_command) ? O_RDWR : O_RDONLY;
    const int fd = open("/dev/vhmonitor", flags);
    if (fd == -1) {
        std::perror("open /dev/vhmonitor");
        return 1;
    }

    bool success = true;

    if (read_command) {
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
    } else if (status_command) {
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
    } else if (fault_command) {
        if (ioctl(fd, VHMONITOR_SET_FAULT, &fault) == -1) {
            std::perror("VHMONITOR_SET_FAULT");
            success = false;
        } else {
            std::cout << "Fault injected: " << argv[2] << '\n';
        }
    } else if (reset_command) {
        if (ioctl(fd, VHMONITOR_RESET) == -1) {
            std::perror("VHMONITOR_RESET");
            success = false;
        } else {
            std::cout << "Device reset to normal\n";
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
