# Driver and C++ Application Integration Test

Date: 2026-10-02
Platform: Ubuntu Linux VM
Implementation baseline: f890cee

## Setup

The IOCTL-enabled vhmonitor module loaded successfully.
Its allocated device number for this run was major 239, minor 0.
A matching character device node was created at /dev/vhmonitor.

The major number is dynamically allocated and must be checked after loading.

## Test 1: Application read command

Command:
sudo ./app/vhmonitor read

Verified output:
Temperature: 35 C
Voltage: 12000 mV
Fan: OK
Device: OK
Fault: NONE

Exit status: 0
Result: PASS

The application opened the device, read until EOF, and closed it.
It terminated automatically without repeated or truncated output.

## Test 2: Application status command

Command:
sudo ./app/vhmonitor status

Verified output:
Temperature: 35.000 C
Voltage: 12000 mV
Fan: OK
Device: OK
Fault code: 0

Exit status: 0
Result: PASS

VHMONITOR_GET_DATA transferred structured readings from kernel space
to the C++ application through the shared interface.

## Cleanup

The module unloaded successfully.
Neither lsmod nor /proc/devices listed vhmonitor afterward.
The kernel log confirmed: vhmonitor: module unloaded
The manually created /dev/vhmonitor node was removed.

## Scope

These results verify normal-state read and IOCTL integration.
Fault injection has not yet been implemented or tested.
