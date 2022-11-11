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

## Cross-layered File System

![70dfn9](https://user-images.githubusercontent.com/42999231/201324848-80dad0ac-d6c3-48c2-9ad2-f4d6ed802ec5.jpg)

Unfortunately, state-of-the-art Kernel-FS, User-FS, and Firmware-FS designs suffer from three
prominent limitations:
* They lack a synergistic design that could benefit from the combined use of host and device resources, such as host and device-level CPUs, and thousands of hardware     I/O queues. 
* The use of an inode-centric design limits concurrent access scalability. 
* For file sharing across processes, applications must trap into an OS for control or data plane or both.

![image](https://uploads.dailydot.com/2021/07/lokimeme.png?auto=compress&fm=png)

* CrossFS is a cross-layered design that disaggregates the file
  system to exploit the capabilities of userspace, firmware,
  and OS layers. 
* CrossFS also achieves high end-to-end concurrency in the user-level library, I/O request queues, and
  firmware-level file system, with or without file sharing across
  threads and processes. 
* CrossFS reduces system call
  cost, provides lightweight crash consistency, and efficient
  device-CPU scheduling.
 
## The Key Observation

![image](https://i.pinimg.com/originals/2a/f8/10/2af810d279a6937f1984c3fe9bb6e47b.jpg)

Applications that
perform conflicting updates to the same blocks of files, automatically resort to protecting file descriptors with applicationlevel locking. Hence, for concurrency and file sharing, CrossFS uses file descriptors as a fundamental unit of
synchronization, enabling threads and processes with separate file descriptors to update disjoint blocks of files concurrently. CrossFS assigns each file descriptor a dedicated I/O
queue (FD-queue) and adds non-conflicting (disjoint) block
requests across descriptors to their independent FD-queue,
whereas serializing conflicting block updates or access to a
single FD-queue. Consequently, CrossFS converts the file
synchronization problem to an I/O request ordering problem,
enabling device-CPUs to dispatch requests across FD-queues
concurrently.

## The Guiding Principles

![image](https://www.memecreator.org/static/images/memes/5068518.jpg)

* **Principle 1**: For achieving high performant direct-I/O, disaggregate file system across user-level, firmware, and OS layers
  to exploit host and device-level computing capabilities.
* **Principle 2**: For fine-grained concurrency, align each file
  descriptor to an independent hardware I/O queue (FD-queue),
  and design concurrency control focused around the file descriptor abstraction.
* **Principle 3**: To efficiently use device-CPUs, merge software
  and hardware I/O schedulers into a single firmware scheduler, and design scheduling policies to benefit from the filedescriptor design.
* **Principle 4**: For crash consistency in a cross-layered design,
  protect the in-transit user data and the file system state by
  leveraging persistence provided by byte-addressable NVMs.
  
## Conclusion

![image](https://user-images.githubusercontent.com/42999231/201326591-eb5d2a61-9b98-4f9a-8f7f-21f97264d7cb.png)
