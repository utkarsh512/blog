# CrossFS: A Cross-layered Direct-Access File System

## The Three Musketeers

![img](https://i.pinimg.com/736x/40/1c/33/401c33346c31f616d0ba2b161588d05d--the-three-musketeers-the-pen.jpg)

### Kernel-FS

Kernel-level file systems, though satisfy *fundamental file system guarantees* such as integrity, consistency, durability, and security, continue to suffer from three 
major bottlenecks:

* **Increased latency**: Applications must enter and exit the OS for performing I/O
* **High concurrent access overheads**: The SOTA designs enforce unnecessary serialization when accessing disjoint portion of data in a file
* **Fail to fully exploit storage hardware-level capabilities**: Kernel-FS designs fail to fully exploit storage hardware-level capabitlities such as compute, thousands of I/O queues, and firmware-level scheduling, ultimately impacting I/O latency, throughput, and concurrency in I/O-intensive applications

### User-FS

Though User-FS provide direct storage access by bypassing the OS, satisfying the fundamental file system guarantees from untrusted user-level is challenging. 
Most designs fail to exploit the hardware capabilities of modern storage.

### Firmware-FS

It embeds the file system into the device firmware for direct-access. It acts as a central entity to satisfy fundamental file system properties. These designs miss out on benifiting from host-level multi-core parallelism.

