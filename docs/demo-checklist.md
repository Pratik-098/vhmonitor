# Final Demonstration Checklist

Repository: https://github.com/Pratik-098/vhmonitor

## 1. Introduce the Project on GitHub

Open the repository and explain:
- The project simulates a monitoring device without physical hardware.
- The driver is written in C; the application and tests use C++.
- The application communicates with /dev/vhmonitor on Linux.
- Temperature, voltage, fan/device status, and faults are simulated.

Show:
- README.md for the project overview.
- docs/architecture.md for the architecture and UML diagrams.
- The driver, app, shared, and tests directories.
- Git commit history as development progress evidence.
- docs/testing.md for recorded test results.

If Mermaid rendering fails in a browser session, retry the document in
a fresh private browser window; both diagrams were verified there.

## 2. Prepare the Linux VM

Follow docs/execution.md to build all components, load the module, inspect
its dynamically assigned major number, and create the matching device node.

Do not assume major 239 will be assigned on every load.
Stop and diagnose any setup error before continuing.

## 3. Demonstrate Normal Monitoring

From the repository root:

```bash
sudo ./app/vhmonitor read
sudo ./app/vhmonitor status
```

Explain:
- open() returns a file descriptor.
- read() returns text and eventually EOF.
- ioctl() returns the shared structured data.
- close() releases the application's descriptor.

Expected: 35 C, 12000 mV, fan/device OK, fault code 0.

## 4. Demonstrate Faults and Recovery

```bash
sudo ./app/vhmonitor fault overheat
sudo ./app/vhmonitor status
sudo ./app/vhmonitor read
```

Expected: 95 C, device FAILED, fault code 1.

```bash
sudo ./app/vhmonitor fault voltage
sudo ./app/vhmonitor status
```

Expected: temperature back to 35 C, voltage 15000 mV, fault code 2.

```bash
sudo ./app/vhmonitor fault fan
sudo ./app/vhmonitor status
```

Expected: voltage back to 12000 mV, fan/device FAILED, fault code 3.

```bash
sudo ./app/vhmonitor reset
sudo ./app/vhmonitor status
sudo ./app/vhmonitor read
```

Expected: all normal values restored.
Explain that only one fault is active at a time.

## 5. Demonstrate Tests

Run in normal state:

```bash
sudo ./tests/ioctl_test.out
echo "IOCTL test exit status: $?"
sudo ./tests/fault_error_test.out
echo "Fault test exit status: $?"
sudo dd if=/dev/vhmonitor bs=1 status=none
echo "Partial-read exit status: $?"
```

Expected: all checks PASS, all exit statuses 0.
Explain invalid-command, invalid-pointer, access-mode, and EOF checks.

## 6. Explain the Driver Design

Be ready to identify:
- alloc_chrdev_region(): reserves a device number.
- cdev_init()/cdev_add(): connects file operations to that number.
- file_operations: open, read, release, and unlocked_ioctl callbacks.
- copy_to_user()/copy_from_user(): transfers across the privilege boundary.
- Mutex: protects virtual state updates and snapshots.
- cdev_del()/unregister_chrdev_region(): releases registration on unload.

Explain limitations honestly:
- The device is simulated, not a physical sensor.
- Device-node creation and removal are manual.
- State changes during partial reads can mix snapshots.
- No 32-bit compatibility IOCTL handler is implemented.

## 7. Demonstrate Cleanup

```bash
sudo rmmod vhmonitor
```

After successful unloading:

```bash
lsmod | grep '^vhmonitor'
grep -w vhmonitor /proc/devices
sudo rm /dev/vhmonitor
ls -l /dev/vhmonitor
```

Expected: no vhmonitor registration; final ls reports no such file.

## Submission Items

- Public GitHub repository URL.
- Source code and commit history.
- README and complete execution instructions.
- Architecture/UML documentation.
- Test source and verified results.
- Final report in the format required by the institution.

This checklist prepares the demonstration; it does not claim a final
rehearsal or submission has already been completed.
