# CrossFS: A Cross-layered Direct-Access File System

## Motivation

![img](https://img-9gag-fun.9cache.com/photo/angog70_460s.jpg)

Modern ultra-fast storage devices provide high bandwidth
(8-16 GB/s) and two orders of lower access latency (<
20µsec) compared to HDD storage. The performance benefits can be attributed to innovation in faster storage hardware and access interface (e.g., PCIe support), increase in storage-level compute (4-8 CPUs, 4-8 GB DRAM, 64K I/O queues), fault protection equipped with capacitors, and evolving support for storage programmability. In recent years, file systems have evolved to exploit
high-performance modern storage devices. However, for applications to truly benefit from modern storage, file system
support for scalable concurrency and efficient data sharing is
critical.


## The Three Musketeers

![img](https://i.pinimg.com/736x/40/1c/33/401c33346c31f616d0ba2b161588d05d--the-three-musketeers-the-pen.jpg)

### Kernel-FS

Kernel-level file systems, though satisfy *fundamental file system guarantees* such as integrity, consistency, durability, and security, continue to suffer from three 
major bottlenecks:

* **Increased latency**: Applications must enter and exit the OS for performing I/O
* **High concurrent access overheads**: The SOTA designs enforce unnecessary serialization when accessing disjoint portion of data in a file
* **Fail to fully exploit storage hardware-level capabilities**: Kernel-FS designs fail to fully exploit storage hardware-level capabitlities such as compute, thousands of I/O queues, and firmware-level scheduling, ultimately impacting I/O latency, throughput, and concurrency in I/O-intensive applications

### User-FS

There is a renewed focus to bypass the OS and directly access storage hardware
from a user-level library. However, managing a file system
from an untrusted library introduces a myriad of challenges,
which include atomicity, crash consistency, and security challenges.
Unfortunately, these approaches require high-overhead
concurrency control and use coarse-grained locks that do not
scale.

### Firmware-FS

It embeds the file system into the device firmware for direct-access. It acts as a central entity to satisfy fundamental file system properties. These designs miss out on benifiting from host-level multi-core parallelism.

![70dfn9](https://user-images.githubusercontent.com/42999231/201324848-80dad0ac-d6c3-48c2-9ad2-f4d6ed802ec5.jpg)

In summary, current User-FS, Kernel-FS, and Firmware-FS designs lack a synergistic design across the user, the kernel,
and the firmware layers, which is critical for achieving direct storage access and scaling concurrent I/O performance
without compromising fundamental file system properties.

## Cross-layered File System

Unfortunately, state-of-the-art Kernel-FS, User-FS, and Firmware-FS designs suffer from three
prominent limitations:
* They lack a synergistic design that could benefit from the combined use of host and device resources, such as host and device-level CPUs, and thousands of hardware     I/O queues. 
* The use of an inode-centric design limits concurrent access scalability. 
* For file sharing across processes, applications must trap into an OS for control or data plane or both.

![image](https://user-images.githubusercontent.com/42999231/201326591-eb5d2a61-9b98-4f9a-8f7f-21f97264d7cb.png)


It is a crosslayered direct-access file system that provides scalability,
high concurrent access throughput, and lower access latency.
CrossFS achieves these goals by disaggregating the file system across the user-level, the device firmware, and the OS
layer, thereby exploiting the benefits of each layer. The
firmware component (FirmFS) is the heart of the file system enabling applications to directly access the storage without compromising fundamental file system properties. The
FirmFS taps into storage hardware’s I/O queues, computational capability, and I/O scheduling capability for improving
I/O performance. The user-level library component (LibFS)
provides POSIX compatibility and handles concurrency control and conflict resolution using the host-level CPUs (hostCPUs). The OS component sets up the initial interface between LibFS and FirmFS (e.g., I/O queues) and converts
software-level access control to hardware security control.
