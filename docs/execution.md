# Build and Execution Guide

## Requirements

- Linux with matching headers for the running kernel
- GCC, G++, GNU Make, and Git
- Administrator access for loading modules and creating device nodes
- No physical hardware required

Verified environment: Ubuntu VM, kernel 7.0.0-34-generic,
GCC/G++ 13.3.0, GNU Make 4.3, and Git 2.43.0.

Run commands from the repository root. For the development VM:

```bash
cd /home/pratik-panda/vhmonitor
```

After cloning elsewhere, enter that repository directory instead.

## 1. Check Kernel Headers

```bash
uname -r
ls -ld /lib/modules/$(uname -r)/build
```

The build directory must correspond to the running kernel.

## 2. Build the Driver, Application, and Tests

Run without sudo:

```bash
make -C /lib/modules/$(uname -r)/build M="$PWD/driver" modules
g++ -std=c++17 -Wall -Wextra -Wpedantic app/vhmonitor.cpp -o app/vhmonitor
g++ -std=c++17 -Wall -Wextra -Wpedantic tests/ioctl_test.cpp -o tests/ioctl_test.out
g++ -std=c++17 -Wall -Wextra -Wpedantic tests/fault_error_test.cpp -o tests/fault_error_test.out
```

Stop if any build command fails.

## 3. Load and Inspect the Driver

```bash
sudo insmod driver/vhmonitor.ko
lsmod | grep '^vhmonitor'
grep -w vhmonitor /proc/devices
sudo dmesg | tail -n 10
```

Stop if insmod fails. Record the assigned major number.
The minor number is 0.

## 4. Create the Device Node

If the reported major number is 239:

```bash
sudo mknod /dev/vhmonitor c 239 0
ls -l /dev/vhmonitor
```

Replace 239 with the actual major number reported for this load.
Do not assume it stays the same after reloading.

The listing must identify a character device with matching numbers.
If the node already exists, inspect it and the loaded module before
proceeding; do not blindly reuse an old device number.

## 5. Monitor Normal State

```bash
sudo ./app/vhmonitor read
sudo ./app/vhmonitor status
```

Expected: 35 C, 12000 mV, fan OK, device OK, fault NONE/code 0.
The status command displays temperature as 35.000 C.

## 6. Demonstrate Fault Injection

```bash
sudo ./app/vhmonitor fault overheat
sudo ./app/vhmonitor status
sudo ./app/vhmonitor read

sudo ./app/vhmonitor fault voltage
sudo ./app/vhmonitor status
sudo ./app/vhmonitor read

sudo ./app/vhmonitor fault fan
sudo ./app/vhmonitor status
sudo ./app/vhmonitor read

sudo ./app/vhmonitor reset
sudo ./app/vhmonitor status
sudo ./app/vhmonitor read
```

Expected states:

| Command | Main change | Fault code |
| --- | --- | --- |
| fault overheat | Temperature 95 C; device FAILED | 1 |
| fault voltage | Voltage 15000 mV; device FAILED | 2 |
| fault fan | Fan FAILED; device FAILED | 3 |
| reset | All normal readings restored | 0 |

Each fault replaces the previous fault. State persists across application
runs until another fault, a reset, or module unloading.

## 7. Run Tests

Start in normal state:

```bash
sudo ./app/vhmonitor reset
sudo ./tests/ioctl_test.out
echo "IOCTL test exit status: $?"
sudo ./tests/fault_error_test.out
echo "Fault error test exit status: $?"
sudo dd if=/dev/vhmonitor bs=1 status=none
echo "Partial-read exit status: $?"
```

Each test must return 0. The C++ tests print PASS for each check.
The fault error test deliberately changes state and restores normal state
on successful completion. If it fails, inspect the output and reset before
continuing with tests that expect normal readings.

The one-byte read must print the complete normal text exactly once and
terminate automatically.

## 8. Unload and Clean Up

Close applications using the device first:

```bash
sudo rmmod vhmonitor
```

Only after successful unloading:

```bash
lsmod | grep '^vhmonitor'
grep -w vhmonitor /proc/devices
sudo dmesg | tail -n 5
sudo rm /dev/vhmonitor
ls -l /dev/vhmonitor
```

Both grep commands should produce no output.
The final ls should report that the node does not exist.
A manually created node is not removed automatically by rmmod.

## Troubleshooting

- Permission denied: use sudo for the documented privileged commands.
- Module already loaded: inspect lsmod; unload the existing module before
  inserting a rebuilt version.
- Module in use: close open device handles, then retry normal unloading.
  Do not force unloading.
- No such file or directory from the application: check that the module
  is loaded and a matching device node has been created.
- Invalid module format: check uname -r, rebuild against matching headers,
  and inspect dmesg.
- Key was rejected by service: module signing/Secure Boot policy may be
  blocking the unsigned module; resolve the VM policy before retrying.
- Unknown IOCTL or unexpected data: confirm the loaded module, application,
  and tests were built from the same shared header and source revision.

On the verified VM, compiler-name and pahole warnings and skipped BTF
generation did not prevent successful builds and runtime tests.
Do not assume unrelated build errors are harmless.

## Execution Scope

All project programs are C/C++ and run on Linux.
Commands shown here use the Linux shell and standard build/system tools.
Testing was sequential with native applications on the verified VM.
See architecture.md for concurrency and compatibility limitations.
