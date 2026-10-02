# Virtual Hardware Monitoring Device Driver and C++ System Monitor for Linux

An individual capstone project implementing a software-based monitoring
device through a Linux character driver and a C++ command-line application.
No physical hardware is required.

## Objectives

- Demonstrate Linux character-device driver development.
- Connect a C++ application to kernel-space functionality.
- Use file descriptors, open(), read(), ioctl(), and close().
- Simulate temperature, voltage, fan/device status, and fault state.
- Inject overheating, abnormal voltage, and fan failure.
- Verify behavior through testing and maintain progress in Git.

## Architecture

```text
C++ User-Space Application
        |
        | open() / read() / ioctl()
        v
/dev/vhmonitor
        |
        v
Linux Character Device Driver
        |
        v
Virtual Hardware Device State
  - Temperature
  - Voltage
  - Fan / Device Status
  - Fault State
```

The driver owns the virtual state. The application displays readings and
requests faults or reset through a shared IOCTL interface. A mutex protects
state updates and snapshots.

## Technologies

- Linux
- C for the kernel driver
- C++17 for the application and tests
- Linux character devices and system programming interfaces
- GCC, G++, GNU Make, and Git

There are no GUI, database, network, cloud, or physical hardware dependencies.

## Directory Structure

```text
driver/
  Makefile                 Kernel module build definition
  vhmonitor.c              Character driver and virtual device state
app/
  vhmonitor.cpp            C++ command-line monitoring application
shared/
  vhmonitor_ioctl.h        Shared data layout and IOCTL commands
tests/
  ioctl_test.cpp           Normal readings and IOCTL error tests
  fault_error_test.cpp     Fault validation and access-mode tests
docs/
  architecture.md          Design, UML diagrams, and limitations
  execution.md             Build, run, test, and cleanup instructions
  integration-test.md      Earlier normal-state integration evidence
  testing.md               Functional and regression test results
```

Compiled executables and kernel build artifacts are excluded from Git.

## Build and Run

Follow the [execution guide](docs/execution.md) to build the project,
load the driver, and create a device node using the assigned major number.

Once the driver and node are ready:

```bash
sudo ./app/vhmonitor read
sudo ./app/vhmonitor status
sudo ./app/vhmonitor fault overheat
sudo ./app/vhmonitor fault voltage
sudo ./app/vhmonitor fault fan
sudo ./app/vhmonitor reset
```

Run read or status after each fault command to inspect its effect.

## Virtual Device Behavior

| State | Temperature | Voltage | Fan | Device | Fault code |
| --- | --- | --- | --- | --- | --- |
| Normal | 35 C | 12000 mV | OK | OK | 0 |
| Overheat | 95 C | 12000 mV | OK | FAILED | 1 |
| Voltage fault | 35 C | 15000 mV | OK | FAILED | 2 |
| Fan failure | 35 C | 12000 mV | FAILED | FAILED | 3 |

Each fault replaces the previous fault. Reset restores normal values.
These are simulated demonstration readings.

## Verified Results

- Driver compilation, loading, registration, unloading, and cleanup.
- Application read and IOCTL status integration.
- All three faults and reset through both monitoring interfaces.
- One-byte partial reads and EOF.
- Invalid IOCTL requests and invalid user-space pointers.
- Invalid fault values and rejection of changes through read-only handles.
- State preservation after rejected fault requests.

See the [test report](docs/testing.md) for results and scope.

## Verified Environment

- Ubuntu Linux VM
- Kernel 7.0.0-34-generic with matching headers
- GCC/G++ 13.3.0
- GNU Make 4.3
- Git 2.43.0

Rebuild the kernel module against the running kernel's headers.

## Documentation

- [Architecture and UML](docs/architecture.md)
- [Build, execution, demonstration, and cleanup](docs/execution.md)
- [Integration test record](docs/integration-test.md)
- [Testing results](docs/testing.md)
- [Demonstration checklist](docs/demo-checklist.md)
- [Final project report](docs/final-report.md)

## Current Limitations

- The device node is created and removed manually.
- The dynamically allocated major number must be checked after loading.
- State changes and partial reads were tested sequentially. A state change
  between partial reads can mix text from different states.
- No 32-bit compatibility IOCTL handler is provided.
- Readings are simulated; no physical sensors are accessed.
