# Development Plan and Milestone Evidence

## Purpose

This document records the fixed development roadmap, implementation
sequence, version-control strategy, and saved milestone evidence.

It was written retrospectively from the agreed roadmap and verified Git
history. It does not claim that this document existed before development.

## Fixed Roadmap and Implementation Plan

| Step | Task | Implementation approach and verification |
| --- | --- | --- |
| 0 | Environment setup | Verify Linux VM, compiler tools, Git, and matching kernel headers |
| 1 | Repository and directory structure | Create driver, app, shared, tests, and docs directories; initialize Git |
| 2 | Basic kernel module | Implement initialization and exit; build, load, inspect logs, and unload |
| 3 | Character device registration | Allocate device number, register cdev, create matching node, verify cleanup |
| 4 | Driver file operations | Implement open/read/release; verify normal reads, partial reads, and EOF |
| 5 | IOCTL interface | Define shared structure and commands; implement user-copy handling and C++ tests |
| 6 | C++ application | Implement read/status commands, argument validation, and error reporting |
| 7 | Driver/application integration | Verify both application communication paths and record results |
| 8 | Fault injection | Add three faults, reset, mutex protection, and CLI controls; verify both interfaces |
| 9 | Testing and debugging | Exercise error paths, state preservation, regression tests, and cleanup |
| 10 | Documentation and UML | Document architecture, interfaces, execution, and verified behavior |
| 11 | GitHub cleanup and progress evidence | Publish repository, verify synchronization, links, diagrams, and history |
| 12 | Final report and presentation preparation | Prepare report, demonstration checklist, and official documentation requirements |

The implementation scope remains fixed. Completing documentation does not
authorize additional project features.

## Development Method

Each action followed a controlled sequence:
1. Explain the action and expected result.
2. Execute it manually in the Ubuntu VM.
3. Inspect actual terminal output.
4. Resolve any error before continuing.
5. Review and stage intended files.
6. Commit a verified milestone.

Source edits were compiled before runtime verification.
Kernel modules were unloaded normally and manually created device nodes
were removed after test sessions.

## Planned Development Timeline

Planned start: 26 September 2026
Planned end: 2 October 2026
Duration: 7 calendar days

The following is the planned roadmap supplied for this document. It is
not a record of verified daily completion. Git history does not establish
that the listed activities were completed on 26–30 September.

| Planned date | Planned activities |
| --- | --- |
| 26 September 2026 | Requirements analysis, project objective, scope, and Linux environment planning |
| 27 September 2026 | Project structure, architecture planning, Linux device-driver concepts, and interface design |
| 28 September 2026 | Basic kernel module and character-device design/implementation |
| 29 September 2026 | File operations, virtual monitoring data, and shared IOCTL interface |
| 30 September 2026 | C++ monitoring application and driver/application integration |
| 1 October 2026 | Fault injection, reset mechanism, testing/debugging, and Git milestone work |
| 2 October 2026 | Final testing, UML/architecture, documentation, final report, GitHub, and demonstration preparation |

The Recorded Git Timeline below is separate milestone evidence. Its
timestamps do not necessarily represent the start or end dates of all
development activity.

## Recorded Git Timeline

All timestamps below are commit author timestamps with UTC offset +05:30.
They record saved milestones, not exact activity start times, end times,
or development durations.

| Commit | Recorded timestamp | Milestone |
| --- | --- | --- |
| 09670f3 | 2026-10-01T01:30:10+05:30 | Initial README and ignore rules |
| 2582e9d | 2026-10-01T01:45:38+05:30 | Basic module load/unload logging |
| 818ce94 | 2026-10-02T12:24:04+05:30 | Character device registration and cleanup |
| 98c17ae | 2026-10-02T12:45:54+05:30 | Open, read, and release operations |
| 5a7a901 | 2026-10-02T13:09:09+05:30 | Shared IOCTL interface and runtime tests |
| f890cee | 2026-10-02T13:38:40+05:30 | C++ monitoring application |
| 049308a | 2026-10-02T13:51:09+05:30 | Integration test evidence |
| a1ff866 | 2026-10-02T19:05:51+05:30 | Fault injection and reset |
| ae04d02 | 2026-10-02T19:31:04+05:30 | Error-path tests and testing report |
| b829310 | 2026-10-02T19:50:50+05:30 | Architecture, UML, execution guide, and README |
| e0a5a7e | 2026-10-02T20:39:48+05:30 | Final report and demonstration checklist |

Environment setup and GitHub publication were verified separately.
Their exact activity times are not inferred from these commits.

## Git and Version-Control Strategy

- Use the main branch for this individual project.
- Commit small, verified milestones with descriptive messages.
- Track source, build definitions, shared interfaces, tests, and documentation.
- Exclude compiled executables, kernel build artifacts, and temporary files.
- Review pending files and staged whitespace before committing.
- Push verified commits to origin/main.
- Compare local HEAD and origin/main to verify synchronization.
- Preserve the existing milestone history as progress evidence.

Repository: https://github.com/Pratik-098/vhmonitor

## Development Environment

- Ubuntu Linux VM
- Kernel 7.0.0-34-generic with matching headers
- GCC/G++ 13.3.0
- GNU Make 4.3
- Git 2.43.0
- 5.8 GiB guest-visible RAM, consistent with a 6 GiB allocation
- 2 virtual CPUs
- 35 GiB virtual disk

## Verification and Progress Evidence

- Git history: implementation and documentation milestones.
- docs/integration-test.md: normal-state application/driver integration.
- docs/testing.md: functional, error-path, regression, and cleanup results.
- tests/ioctl_test.cpp: normal readings and IOCTL error checks.
- tests/fault_error_test.cpp: fault validation, access modes, and state preservation.
- docs/execution.md: reproducible build and execution instructions.
- docs/demo-checklist.md: GitHub-based demonstration sequence.

## Remaining Preparation at Document Creation

- Complete and verify the remaining required architecture diagrams.
- Document possible future improvements without changing current scope.
- Link the supplementary documents from the repository overview.
- Review, commit, push, and verify the documentation additions on GitHub.
- Contact the trainer after preparation is complete to arrange evaluation.

These items are preparation tasks, not claims of completed evaluation
or institutional submission.
