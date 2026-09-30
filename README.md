# Virtual Hardware Monitoring Device Driver and C++ System Monitor for Linux

## Overview

This project implements a software-based virtual hardware monitoring
system on Linux. It runs without physical hardware.

A C++ user-space application communicates with a Linux character device
driver through /dev/vhmonitor using open(), read(), and ioctl().

## Objectives

- Implement a Linux character device representing a virtual monitoring device.
- Monitor simulated temperature, voltage, fan/device status, and fault state.
- Demonstrate communication between user space and kernel space.
- Support simple fault injection: overheating, abnormal voltage,
  fan/device failure, and resetting the virtual device to normal.
- Maintain version history, tests, and project documentation.

## Technologies

- Linux (Ubuntu development VM)
- C for the Linux kernel driver
- C++ for the user-space monitoring application
- Linux system calls and file descriptors
- GCC, G++, GNU Make, and Git

## Architecture

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
Virtual Hardware Device

## Directory Structure

- driver/ — Linux character device driver and build files.
- app/ — C++ user-space monitoring application.
- shared/ — Shared interface headers and IOCTL definitions.
- tests/ — Test scripts and verification checks.
- docs/ — Documentation, UML diagrams, and report materials.

## Current Status

Project repository setup is in progress.
Driver and application implementation have not started.
