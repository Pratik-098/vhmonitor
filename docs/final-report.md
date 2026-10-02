# Virtual Hardware Monitoring Device Driver and C++ System Monitor for Linux

Author: Pratik Panda
Project type: Individual capstone
Repository: https://github.com/Pratik-098/vhmonitor
Report date: 2026-10-02

## Abstract

This project implements a virtual hardware monitoring system on Linux.
A character device driver written in C maintains simulated temperature,
voltage, fan status, device status, and fault state. A C++ command-line
application accesses the driver through /dev/vhmonitor using Linux system
calls. The system supports three fault scenarios and restoration to normal
state. Functional, error-path, and regression tests verified the implemented
behavior on an Ubuntu virtual machine. No physical hardware is required.

## 1. Problem Statement

Physical monitoring hardware may be unavailable during driver development
and academic demonstrations. A software-based device provides a controlled
environment for demonstrating kernel programming, user-space communication,
device behavior, and fault handling.

The project focuses on a small, understandable implementation suitable for
an individual capstone and a reproducible Linux demonstration.

## 2. Objectives

- Implement a Linux character device driver.
- Model a virtual monitoring device in kernel space.
- Implement a C++ user-space monitoring application.
- Demonstrate open(), read(), ioctl(), close(), and file descriptors.
- Provide overheating, abnormal-voltage, and fan-failure injection.
- Verify normal operation, errors, reset, and resource cleanup.
- Maintain source, documentation, tests, and progress history on GitHub.

## 3. Scope

All project programs are written in C or C++ and execute on Linux.
The command-line interface provides read, status, fault, and reset commands.

The project does not require physical hardware, networking, a database,
a graphical interface, cloud services, or machine learning.
The simulated readings are demonstration values rather than measurements.

## 4. Development Environment

- Ubuntu Linux virtual machine
- Kernel: 7.0.0-34-generic with matching build headers
- GCC/G++: 13.3.0
- GNU Make: 4.3
- Git: 2.43.0
- VM resources: 5.8 GiB guest-visible RAM (consistent with a 6 GiB allocation), 2 virtual CPUs, 35 GiB virtual disk.

## 5. System Architecture

The communication path is:

C++ application -> Linux system calls -> /dev/vhmonitor ->
character device driver -> virtual device state

The application handles command validation and output.
The driver owns the state and implements device operations.
A shared header defines the binary data layout and IOCTL commands.

Architecture and sequence diagrams are provided in docs/architecture.md.

## 6. Driver Implementation

The module initializes normal state and allocates one character-device
number using alloc_chrdev_region(). cdev_init() and cdev_add() associate
that number with the driver's file_operations table.

A matching /dev/vhmonitor node is created manually using mknod.
The major number is dynamically assigned and must be checked after loading.

The driver implements:
- open(): logs device access.
- read(): returns formatted virtual readings.
- release(): logs final release of an open file.
- unlocked_ioctl(): retrieves state, injects faults, or resets the device.

simple_read_from_buffer() handles user-space text copying, partial reads,
file-position advancement, and EOF.

On unloading, cdev_del() removes the character device and
unregister_chrdev_region() releases its device number.
The manually created node is removed separately.

## 7. Shared Interface and State Management

The shared vhmonitor_data structure contains five 32-bit fields:
temperature_mc, voltage_mv, fan_status, device_status, and fault_state.

Temperature is represented in millidegrees Celsius and voltage in millivolts.
The structure occupies 20 bytes in the verified interface.

IOCTL commands:
- VHMONITOR_GET_DATA: copies structured state to user space.
- VHMONITOR_SET_FAULT: accepts a 32-bit fault selection.
- VHMONITOR_RESET: restores normal state without a data buffer.

copy_to_user() and copy_from_user() handle user/kernel memory transfers.
A mutex protects state updates and snapshots.
Fault and reset operations require a file descriptor opened for writing.

## 8. C++ Application

The application accepts:
- read
- status
- fault overheat
- fault voltage
- fault fan
- reset

Monitoring commands use O_RDONLY; mutation commands use O_RDWR.
The read command handles interrupted and partial reads until EOF.
The status command formats structured IOCTL results.
System-call errors are reported, and failures return a nonzero exit status.

## 9. Fault-Injection Model

| State | Temperature | Voltage | Fan | Device | Code |
| --- | --- | --- | --- | --- | --- |
| Normal | 35 C | 12000 mV | OK | OK | 0 |
| Overheat | 95 C | 12000 mV | OK | FAILED | 1 |
| Voltage | 35 C | 15000 mV | OK | FAILED | 2 |
| Fan failure | 35 C | 12000 mV | FAILED | FAILED | 3 |

Each fault replaces the previous fault.
Reset and module initialization restore normal state.

Invalid fault values return EINVAL. Invalid transfer pointers return EFAULT.
Unsupported IOCTL commands return ENOTTY. Fault/reset requests through
read-only descriptors return EBADF.

## 10. Testing and Results

Verified results:
- Kernel module build, load, registration, unload, and cleanup: PASS.
- C++ application and test compilation with warnings enabled: PASS.
- Normal read and status integration: PASS.
- All three faults through read and IOCTL status: PASS.
- Reset through read and IOCTL status: PASS.
- One-byte partial reads and EOF: PASS.
- Invalid IOCTL commands and output pointers: PASS.
- Invalid fault values and input pointers: PASS.
- Read-only fault/reset rejection: PASS.
- State preservation after rejected requests: PASS.

The original IOCTL test and fault error test both returned exit status 0.
Detailed evidence and test scope are recorded in docs/testing.md.

## 11. Architecture Concepts Demonstrated

- Separation of user-space and kernel-space responsibilities.
- A shared binary interface across a privilege boundary.
- File-descriptor-based access to a character device.
- Major/minor device addressing and operation dispatch.
- Controlled shared-state access using a mutex.
- Explicit initialization, error cleanup, and resource release.
- Software representation of hardware-like state and faults.

The implementation does not access physical registers or implement
interrupt handling, DMA, or a physical hardware bus protocol.

## 12. Limitations

- Readings are simulated and do not vary automatically with time.
- Device-node management is manual.
- Tests were performed sequentially on the documented VM.
- State changes between partial reads can mix text from different snapshots.
- No 32-bit compatibility IOCTL handler is implemented.
- Other kernel versions and physical hardware were not validated.

## 13. Version Control and Delivery

Development milestones were committed incrementally and pushed to the
public GitHub repository. The repository includes source, shared interface,
tests, architecture diagrams, execution instructions, and test evidence.

See docs/execution.md for reproduction commands and
docs/demo-checklist.md for the demonstration sequence.

## 14. Conclusion

The implemented system demonstrates Linux character-device programming and
C++ system programming without physical hardware. The application retrieves
virtual readings and controls simple fault scenarios through system calls.
The documented tests verified the stated normal, fault, error, and cleanup
behavior within the project's defined scope.

## References

- Linux kernel IOCTL interface guidance:
  https://docs.kernel.org/driver-api/ioctl.html
- Linux external module build documentation:
  https://docs.kernel.org/kbuild/modules.html
- Linux system-call manual pages:
  man 2 open
  man 2 read
  man 2 ioctl
  man 2 close
