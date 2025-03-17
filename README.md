# Inter-Process Communication (IPC) Simulation

## Overview
This project is a **multi-process simulation** that utilizes **semaphores and shared memory** for inter-process communication. The program simulates customers and workers interacting in a service environment, ensuring synchronization using **POSIX semaphores**.

## Features
- **Process synchronization** using POSIX semaphores
- **Shared memory** for global state management
- **Multi-process handling** with `fork()`
- **Randomized behavior** to simulate real-world concurrency
- **Logging system** for tracking process actions

## Requirements
### Hardware:
- Any Linux-based system (Ubuntu, Debian, Fedora, etc.) or **WSL on Windows**

### Software:
- **GCC Compiler**
- **Makefile** for build automation
- Required Libraries:
  - `stdio.h`, `stdlib.h`, `stdbool.h` (Standard C Libraries)
  - `semaphore.h` (POSIX semaphores)
  - `sys/mman.h` (Shared memory management)
  - `sys/types.h`, `sys/wait.h`, `unistd.h` (Process control)
  - `fcntl.h`, `errno.h` (File control and error handling)
  - `time.h` (Randomized delays for simulation)

## Installation and Setup
1. Clone the repository:
   ```bash
   git clone https://github.com/kristian-dobes/Process-Synchronization.git
   ```
2. Navigate to the project directory:
   ```bash
   cd Process-Synchronization
   ```
3. Compile the program using `make`:
   ```bash
   make
   ```
4. Run the executable:
   ```bash
   ./proj2 <NZ> <NU> <TZ> <TU> <F>
   ```
   - `NZ` – Number of customers
   - `NU` – Number of workers
   - `TZ` – Maximum time for customers before entering service
   - `TU` – Maximum worker break time
   - `F` – Office closing time

## File Structure
```
Process-Synchronization/
│── proj2.c      # Main program source code
│── Makefile     # Makefile for build automation
│── README.md    # Project documentation
```

## How It Works
1. **Initialization**:
   - Semaphores and shared memory are initialized.
   - The system starts with a set number of customers and workers.
2. **Customer Process**:
   - Customers arrive at the service office randomly.
   - If the office is open, they wait in queues for service.
   - If the office is closed, they leave.
3. **Worker Process**:
   - Workers serve customers based on queue priority.
   - If no customers are available, they take a break.
   - If the office is closing and no customers remain, they exit.
4. **Closing Process**:
   - The office closes after `F` milliseconds.
   - Any remaining customers are served or leave.
   - Workers finish their tasks and terminate.

## Cleanup and Error Handling
- All semaphores and shared memory are properly **released** on exit.
- **Error checking** is implemented for system calls (e.g., `fork()`, `mmap()`, `sem_open()`).

## License
This project is licensed under the **MIT License**.

## Author
Kristian Dobes - **xdobes22**

