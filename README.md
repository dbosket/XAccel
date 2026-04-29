# XAccel: Self-Describing Accelerator Interface for Linux

XAccel is a Linux kernel module and userspace test framework for experimenting with a self-describing accelerator interface. The core idea is that an accelerator exposes a descriptor region that describes the available functions and their register windows. The Linux driver parses that descriptor and creates per-function character device nodes such as `/dev/xaccel0_func0` and `/dev/xaccel0_func1`.

This repository currently demonstrates the XAccel concept using a kernel module, descriptor parsing logic, dynamically created character devices, a UAPI header, and a userspace smoke-test application that exercises the driver through `ioctl()` calls.

---

## 1. Current Status

### Implemented

- Out-of-tree Linux kernel module: `xaccel.ko`
- Capability descriptor definitions
- Descriptor parsing and function discovery
- Dynamic character device creation
- Per-function device nodes:
  - `/dev/xaccel0_func0`
  - `/dev/xaccel0_func1`
  - additional nodes depending on descriptor contents
- Userspace UAPI through `ioctl()`
- Register read/write test path
- Software-backed mock/debug support for testing without real FPGA hardware
- Automated smoke test script
- Positive and negative/error-path tests

### Not Yet Implemented / Future Work

- `libxaccel.so` userspace library
- DMA engine support
- `mmap()` buffer interface
- Interrupt-driven async completion
- Async submit/poll API
- Production FPGA integration flow
- Full performance benchmarking

---

## 2. Tested Development Environment

The following environment was used for the current XAccel implementation and demo.

| Item | Version / Configuration |
|---|---|
| Operating System | Ubuntu 22.04 LTS |
| Kernel | `6.8.0-101-generic` |
| Architecture | `x86_64` |
| Compiler | `gcc-12` |
| GCC Version | `12.3.0` |
| Build System | Linux Kbuild + project `Makefile` |
| Shell | Bash |
| Optional Debug Tool | Valgrind |

You can verify your local environment with:

```bash
lsb_release -a
uname -a
uname -r
gcc --version
make --version
bash --version
```

For kernel module builds, it is important that the Linux kernel headers installed on the system match the running kernel shown by:

```bash
uname -r
```

---

## 3. Repository Layout

```text
.
├── demos/                  # Demo-related files and walkthrough material
├── docs/                   # Design notes and documentation
├── hls/                    # HLS accelerator examples and testbenches
│   ├── sha256.cpp
│   ├── sha256_tb.cpp
│   ├── testbench.c
│   ├── vector_add.c
│   ├── vector_add.cpp
│   ├── vector_add.h
│   └── vector_add_tb.cpp
├── include/                # Shared descriptor and macro definitions
│   ├── xaccel_desc.h
│   └── xaccel_macros.h
├── kernel/                 # Linux kernel module source
│   ├── xaccel_core.c       # Descriptor parsing, device creation, MMIO logic
│   ├── xaccel_core.h
│   ├── xaccel_debug_sw.c   # Software-backed mock/debug support
│   ├── xaccel_debug_sw.h
│   ├── xaccel_driver.c     # Module init/exit and driver entry points
│   ├── xaccel_driver.h
│   └── xaccel_uapi.h       # Userspace-visible ioctl API
├── reports/                # Report and presentation artifacts
├── scripts/                # Helper scripts
├── tests/                  # Smoke tests and userspace test application
│   ├── smoke.bash
│   ├── smoketest
│   └── smoketest.c
├── userspace/              # Placeholder for future userspace library/tools
├── Makefile
├── README.md
└── LICENSE
```

---

## 4. Dependencies

Install the build tools and kernel headers:

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r) make gcc-12
```

Optional, for userspace memory/debug checking:

```bash
sudo apt install -y valgrind
```

Confirm that your kernel build directory exists:

```bash
ls /lib/modules/$(uname -r)/build
```

If this directory does not exist, install the matching headers:

```bash
sudo apt install -y linux-headers-$(uname -r)
```

---

## 5. Build Instructions

From the repository root:

```bash
make clean
make
make smoke
```

Expected outputs:

- Kernel module: `xaccel.ko`
- Userspace smoke-test executable: `tests/smoketest`

The kernel module is built out-of-tree using the running kernel's build system. The userspace test application is compiled separately and links against standard userspace headers while including the XAccel UAPI header.

---

## 6. Load the Kernel Module

From the repository root:

```bash
sudo insmod ./xaccel.ko
```

Check that the module loaded:

```bash
lsmod | grep '^xaccel\b'
```

Inspect recent kernel logs:

```bash
sudo dmesg | tail -n 20
```

If the module is already loaded and you want to reload it:

```bash
sudo rmmod xaccel
sudo insmod ./xaccel.ko
```

---

## 7. Verify Device Nodes

After loading the module, XAccel should create one device node per discovered accelerator function.

Check for device nodes:

```bash
ls -l /dev/xaccel*_func*
```

Example expected output:

```text
/dev/xaccel0_func0
/dev/xaccel0_func1
```

For convenience during local testing, change permissions:

```bash
sudo chmod 666 /dev/xaccel*_func*
```

This is acceptable for a local demo environment. For production, device permissions should be handled through `udev` rules or a more restrictive access-control policy.

---

## 8. Userspace Test Application

The userspace test application is:

```text
tests/smoketest.c
```

It opens a device node and issues `ioctl()` commands through the UAPI defined in:

```text
kernel/xaccel_uapi.h
```

Supported commands:

```bash
./tests/smoketest <device> info
./tests/smoketest <device> read  <offset>
./tests/smoketest <device> write <offset> <value>
```

Examples:

```bash
./tests/smoketest /dev/xaccel0_func0 info
./tests/smoketest /dev/xaccel0_func0 write 0x0 0xdeadbeef
./tests/smoketest /dev/xaccel0_func0 read 0x0
```

---

## 9. IOCTL Interface

The current demo exercises three core `ioctl()` operations.

### `XACCEL_IOC_GET_INFO`

Queries descriptor-derived information for the function device.

The smoke test prints fields such as:

```text
FUNC_ID
FUNC_TYPE
FUNC_VERSION
IRQ_INDEX
MMIO_SIZE
CAPABILITIES
REG_LAYOUT_VERSION
```

### `XACCEL_IOC_READ_REG`

Reads a 32-bit register value from a function MMIO window.

Example:

```bash
./tests/smoketest /dev/xaccel0_func0 read 0x0
```

### `XACCEL_IOC_WRITE_REG`

Writes a 32-bit register value to a function MMIO window.

Example:

```bash
./tests/smoketest /dev/xaccel0_func0 write 0x0 0xdeadbeef
```

---

## 10. Automated Smoke Test

The main automated test script is:

```text
tests/smoke.bash
```

The script performs the following steps:

1. Builds the kernel module and userspace smoke-test application.
2. Inserts or reloads `xaccel.ko`.
3. Prints recent kernel logs with `dmesg`.
4. Verifies that `/dev/xaccel*_func*` device files exist.
5. Runs the userspace smoke test against each discovered function device.
6. Exercises valid register reads and writes.
7. Exercises invalid/error-path cases and confirms they fail as expected.

Run it with:

```bash
./tests/smoke.bash
```

If the script is not executable:

```bash
chmod +x ./tests/smoke.bash
./tests/smoke.bash
```

---

## 11. Smoke Test Coverage

For each `/dev/xaccel*_func*` device, the script currently runs:

### Positive Tests

```text
GET_INFO
WRITE_REG offset=0x0 value=0xdeadbeef
READ_REG  offset=0x0
WRITE_REG offset=0x4 value=0xcafebabe
READ_REG  offset=0x4
READ_REG  boundary offset=0x1fc
```

### Negative / Error-Path Tests

```text
READ_REG  out-of-range offset=0x200
WRITE_REG out-of-range offset=0x200
READ_REG  unaligned offset=0x1
WRITE_REG unaligned offset=0x1
Invalid command
```

The negative tests are expected to fail. The script treats those failures as passes because they confirm the driver rejects invalid requests.

---

## 12. Optional Valgrind Mode

The smoke script supports optional Valgrind checking for the userspace test executable.

Run:

```bash
USE_VALGRIND=1 ./tests/smoke.bash
```

This enables:

```bash
valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=99
```

Valgrind checks only the userspace `smoketest` program. It does not check memory inside the Linux kernel module.

---

## 13. Demo Walkthrough

A concise demo flow is:

```bash
cd /home/taco/Software/XAccel
make clean && make && make smoke
sudo insmod ./xaccel.ko
ls -l /dev/xaccel*_func*
sudo chmod 666 /dev/xaccel*_func*
./tests/smoketest /dev/xaccel0_func0 info
./tests/smoketest /dev/xaccel0_func0 write 0x0 0xdeadbeef
./tests/smoketest /dev/xaccel0_func0 read 0x0
./tests/smoke.bash
```

What the demo shows:

- The kernel module builds and loads successfully.
- XAccel creates character device nodes for discovered functions.
- Userspace can open function devices.
- Userspace can query function metadata with `GET_INFO`.
- Userspace can perform register-level read/write operations through `ioctl()`.
- The driver rejects invalid offsets, unaligned accesses, and invalid commands.

---

## 14. Clean Up

Unload the kernel module:

```bash
sudo rmmod xaccel
```

Clean build artifacts:

```bash
make clean
```

Verify the module is removed:

```bash
lsmod | grep '^xaccel\b'
```

If the command returns no output, the module is no longer loaded.

---

## 15. Troubleshooting

### `xaccel.ko` does not exist

Build the project first:

```bash
make clean
make
```

### Kernel headers are missing

If you see an error involving `/lib/modules/$(uname -r)/build`, install matching headers:

```bash
sudo apt install -y linux-headers-$(uname -r)
```

### Device files do not exist

Check that the module loaded:

```bash
lsmod | grep '^xaccel\b'
```

Check kernel logs:

```bash
sudo dmesg | tail -n 50
```

Reload the module:

```bash
sudo rmmod xaccel
sudo insmod ./xaccel.ko
```

### Permission denied when opening `/dev/xaccel*_func*`

For local testing:

```bash
sudo chmod 666 /dev/xaccel*_func*
```

Alternatively, run the test with `sudo`:

```bash
sudo ./tests/smoketest /dev/xaccel0_func0 info
```

### `rmmod: ERROR: Module xaccel is in use`

Check whether a process still has the device open:

```bash
sudo lsof /dev/xaccel*_func*
```

Stop the process, then retry:

```bash
sudo rmmod xaccel
```

### Invalid offset tests fail as expected

The negative tests intentionally use invalid offsets such as `0x200` and unaligned offsets such as `0x1`. These should fail. The smoke script reports those failures as passes when the driver rejects them correctly.

---

## 16. Design Notes

XAccel is motivated by the lack of a generic, runtime-discoverable accelerator interface in Linux. Instead of hard-coding accelerator function details into the driver, the accelerator exposes a descriptor. The driver reads this descriptor, determines which functions exist, and creates character devices for those functions.

Current data path:

```text
userspace smoketest
      |
      | open() + ioctl()
      v
xaccel kernel module
      |
      | descriptor parsing + register access
      v
software-backed mock region or FPGA-style MMIO region
```

This project currently focuses on proving the control path and descriptor-driven device model. High-throughput data movement through DMA is future work.

---

## 17. Safety and Scope

This project is a graduate operating systems prototype. It is not production hardened.

Current limitations:

- No production security model
- No `udev` permission rules
- No DMA data path
- No interrupt-based completion path
- No userspace library abstraction
- No production FPGA deployment package

The current implementation is intended to demonstrate descriptor-based discovery, dynamic device creation, and safe register-level access through a controlled kernel interface.

---

## 18. Project Context

Course: CIS7000-006 Advanced Operating Systems Concepts  
Project: XAccel  
Author: Demetrius A. Bosket  
Team: DevZero


