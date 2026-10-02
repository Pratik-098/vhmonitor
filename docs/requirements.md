# Project Requirements Document (PRD)

## 1. Project Identification

Project: Virtual Hardware Monitoring Device Driver and C++ System Monitor
Author: Pratik Panda
Type: Individual capstone
Target platform: Linux
Implementation languages: C and C++

## 2. Problem and Objective

Provide a software-only environment for demonstrating Linux device-driver
development and user-space/kernel-space communication without physical
monitoring hardware.

The system must expose simulated monitoring data through a character
device and support controlled fault injection from a C++ application.

## 3. Users

- Developer: builds, loads, tests, and maintains the system.
- Demonstrator: displays readings and injects faults.
- Trainer/evaluator: reviews GitHub evidence and observes a 5–10 minute
  project presentation/demonstration.

## 4. Fixed Scope

C++ application -> open/read/ioctl -> /dev/vhmonitor ->
Linux character device driver -> virtual device state.

Included:
- Temperature, voltage, fan status, device status, and fault state.
- Text readings and structured IOCTL readings.
- Overheat, voltage, and fan fault injection.
- Reset to normal state.
- Tests, architecture documentation, report, and Git history.

Excluded:
- Physical hardware dependencies.
- GUI, database, web application, networking, and cloud services.
- AI/ML and unrelated kernel subsystems.

## 5. Functional Requirements

| ID | Requirement | Acceptance evidence |
| --- | --- | --- |
| FR-01 | Build a loadable Linux character driver in C | Module build/load verification |
| FR-02 | Allocate a device number and register file operations | Registration logs and /proc/devices |
| FR-03 | Provide access through /dev/vhmonitor | Matching character-device node |
| FR-04 | Support open and release callbacks | Kernel log verification |
| FR-05 | Return text readings through read() | Application read output |
| FR-06 | Support partial reads and EOF | One-byte dd test terminates correctly |
| FR-07 | Return structured readings through IOCTL | ioctl_test.cpp |
| FR-08 | Provide a C++ command-line monitoring application | read and status integration tests |
| FR-09 | Inject overheat, voltage, and fan faults | Fault results through read and status |
| FR-10 | Replace the previous fault when selecting another | Verified fault-transition sequence |
| FR-11 | Reset every monitored field to normal | Reset results through both interfaces |
| FR-12 | Reject invalid commands, pointers, and fault values | C++ error-path tests |
| FR-13 | Require write access for fault/reset operations | Read-only descriptor rejection tests |
| FR-14 | Preserve state after rejected fault requests | fault_error_test.cpp |
| FR-15 | Release kernel registration on unloading | Unload and registration checks |

## 6. Non-Functional Requirements

| ID | Requirement | Verification or constraint |
| --- | --- | --- |
| NFR-01 | Project programs use only C/C++ on Linux | Source review and Linux builds |
| NFR-02 | Operate without physical hardware | All demonstrated state is simulated |
| NFR-03 | Keep the implementation understandable | Small driver, shared header, and CLI |
| NFR-04 | Protect shared state updates and snapshots | Mutex in driver source |
| NFR-05 | Safely transfer data across the privilege boundary | Kernel user-copy helpers |
| NFR-06 | Use an explicit shared binary layout | Five fixed-width fields; verified structure size 20 bytes |
| NFR-07 | Report application failures clearly | Error messages and nonzero exit status |
| NFR-08 | Provide reproducible execution instructions | docs/execution.md |
| NFR-09 | Preserve development progress evidence | Incremental Git commit history |
| NFR-10 | Publish source and documentation on GitHub | Public repository verification |

No real-time latency, throughput, or availability guarantee is claimed.
Tests cover the documented sequential usage on the verified VM.

## 7. Interfaces and Data

The shared header is shared/vhmonitor_ioctl.h.

vhmonitor_data contains:
- Signed temperature_mc in millidegrees Celsius.
- Unsigned voltage_mv in millivolts.
- Unsigned fan_status.
- Unsigned device_status.
- Unsigned fault_state.

Each field is 32 bits; the verified structure size is 20 bytes.

Commands:
- VHMONITOR_GET_DATA: retrieve state.
- VHMONITOR_SET_FAULT: select a supported fault.
- VHMONITOR_RESET: restore normal state.

The CLI provides read, status, fault overheat, fault voltage, fault fan,
and reset.

## 8. Assumptions and Constraints

- Matching headers are available for the running Linux kernel.
- Administrator access is available for module and device-node operations.
- Device-node creation and removal are manual.
- The assigned major number is checked after loading.
- Only one simulated fault is active at a time.
- No 32-bit compatibility IOCTL handler is implemented.
- State changes between partial reads can mix snapshots.
- GitHub-based presentation is compulsory; PPT is optional.

## 9. Required Documentation and Delivery

- PRD with functional and non-functional requirements.
- Development timeline, milestones, and implementation plan.
- Architecture, class/component, sequence, and applicable state diagrams.
- Data structures and interfaces.
- Development environment and version-control strategy.
- Progress evidence and testing/integration results.
- Final report covering achievements, limitations, and future improvements.
- Public GitHub repository with README and execution instructions.
- Contact the trainer after completion to arrange the evaluation.

The supplied instructions do not specify a PDF/DOCX report format or
a recorded-video requirement. The report is currently maintained in
Markdown; any later trainer-specific format requirement must be confirmed.

## 10. Verification References

- docs/architecture.md
- docs/execution.md
- docs/integration-test.md
- docs/testing.md
- docs/final-report.md
- docs/demo-checklist.md

This PRD records the agreed requirements retrospectively.
Its creation does not imply it existed before implementation.
Remaining documentation gaps are tracked separately until verified.
