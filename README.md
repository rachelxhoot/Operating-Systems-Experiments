# Operating System Experiments

This repository contains implementations and simulations for various operating system experiments conducted as part of the OS Course.

## Experiments

### 1. Dynamic Relocatable Partition Memory Management

**File:** `DynamicRepositionable.cpp`

The initialization of the physical memory space layout and basic operations such as process memory allocation and deallocation. The objective is to simulate the dynamic relocatable partition management for a specified size of user memory space and provide testing and validation.

### 2. FAT File System

**File:** `FAT.cpp`

Implement the FAT file system, supporting simulation of disk volumes (disk image files) and performing operations for accessing directories and files. This includes disk volume formatting, creating directories, changing the current directory, renaming directories, displaying directories, deleting directories, creating empty files, renaming files, redirecting file writes, displaying file contents, deleting files, etc.
### 3. Moving Disk Head Scheduling

- **File:** `DiskScheduling.cpp`

Implement First-Come-First-Serve (FCFS), Shortest Seek Time First (SSTF), SCAN elevator scheduling, Circular-SCAN (CSCAN) elevator scheduling, and Double-Queue SCAN (FSCAN) elevator scheduling algorithms for disk head movement. Use a randomly generated sequence of disk access events (track numbers) to conduct testing and performance comparisons of the mentioned algorithms.

### 4. Page Elimination

- **File:** `PageElimination.cpp`

Implement optimal replacement algorithm, First-In-First-Out (FIFO) replacement algorithm, Least Recently Used (LRU) replacement algorithm, Simple Clock replacement algorithm, and Improved Clock replacement algorithm. Conduct testing and performance comparisons of these algorithms with a randomly generated page access sequence.

### 5. Processor Scheduling

**File:** `ProcessorScheduling.cpp`

Implement system scheduling processes based on specific processor scheduling algorithms, such as First-Come-First-Serve scheduling, Shortest Job First scheduling, High Priority First scheduling, High Response Ratio Priority scheduling, Time Slice Round Robin scheduling, Multi-Level Feedback Queue scheduling, etc.

### 6. Synchronization Mechanism

#### a. Peterson Algorithm

- **File:** `SynchronizationMechanism/Peterson/Peterson.cpp`

Address the issue of synchronous bank account transfers using the Peterson algorithm.

#### b. Windows System Mutex Semaphores

- **File:** `SynchronizationMechanism/WindowsSystem MutexSemaphores/WindowsSystem MutexSemaphores.cpp`

Implement a synchronization solution using the synchronization mechanisms of the Windows (or Linux) operating system for bank account transfers. Compare and analyze the efficiency of the custom synchronization mechanism with the built-in synchronization mechanism of the operating system.
