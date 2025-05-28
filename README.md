# RAID-Level-Simulator

This project simulates RAID 0 and RAID 1 configurations in C. It demonstrates core RAID concepts such as:

- **RAID 0 (Striping)**: Distributes data across multiple disks to increase performance.
- **RAID 1 (Mirroring)**: Duplicates the same data on multiple disks for redundancy.

### Features

- Block-level data storage simulation
- Read and write operations with offset and size
- Visualization of disk states
- RAID 0 striping and RAID 1 mirroring implementation

### File Structure

- `raid_simulation.c`: Core implementation and driver code.

### How to Compile and Run

gcc raid_simulation.c -o raid_simulation
./raid_simulation

### Sample Output
The program demonstrates writing and reading operations on RAID 0 and RAID 1 systems, along with printing the disk states after each operation.

### License
This project is released for academic use and personal learning purposes.
