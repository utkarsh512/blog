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
  
## CrossFS Layers

CrossFS enables unmodified POSIX-compatible applications to benefit from direct storage access.

![image](https://user-images.githubusercontent.com/42999231/201344797-7b40ff7e-483b-4237-867e-a2e8a8fc16a7.png)

As shown in
the figure above, CrossFS comprises of a user-level library (LibFS),
a firmware file system component (FirmFS), and an OS component.

## Scalability

File system disaggregation alone is insufficient
for achieving I/O scalability, which demands revisiting file
system concurrency control, reducing journaling cost, and
designing I/O scheduling that matches the concurrency control. It is observed that file descriptors (and not inode) are a
natural abstraction of access in most concurrent applications,
where threads and processes use independent file descriptors
to access/update different regions of shared or private files
(for example, RocksDB maintains 3.5K open file descriptors). Hence, for I/O scalability, in CrossFS, file
descriptor-based concurrency control is introduced, which allows threads
or processes to update or access non-conflicting blocks of a
file simultaneously.

![image](https://user-images.githubusercontent.com/42999231/201345558-f66c64c0-534c-452c-a807-d14340d7352c.png)

## Concurrency Control

> CrossFS concurrency design turns the file
> synchronization problem into a queue ordering problem.

In CrossFS,
file descriptors are mapped to dedicated hardware I/O queues
to exploit storage hardware parallelism and fine-grained concurrency control. All non-conflicting requests (i.e., requests
to different blocks) issued using a file descriptor are added
to a file descriptor-specific queue. In contrast, conflicting requests are ordered by using a single queue. This provides an
opportunity for device-CPUs and FirmFS to dispatch requests
concurrently with almost zero synchronization between host
and device-CPUs. For conflict resolution and ordering updates to blocks across file descriptors, CrossFS uses a per-inode interval tree, interval tree read-write semaphore
(interval tree `rw-lock`), and global timestamps for concurrency control. However, unlike current file systems that must
hold inode-level locks until request completion, CrossFS only
acquires interval tree `rw-lock` for request ordering to FDqueues.

## Challenges

Moving away from an inode-centric
to a file descriptor-centric design introduces CrossFS-specific
challenges: 

* Using fewer and wimpier device-CPUs for
  conflict resolution and concurrency control impacts performance. 

* Mapping a file descriptor to an I/O queue (a
  device-accessible DMA memory buffer) increases the number
  of queues that CrossFS must manage, potentially leading to
  data loss after a crash.

* Overburdening device-CPUs
  for serving I/O requests across hundreds of file descriptor
  queues could impact performance, specifically for blocking
  I/O operations (e.g., read, fsync).

![image](https://user-images.githubusercontent.com/42999231/201347458-11a47ef0-4b8b-45c5-9cf8-675d20cbf3e0.png)

# Host Delegation

To overcome the challenge of fewer (and
wimpier) device-CPUs, CrossFS utilizes the cross-layered
design and delegates the responsibility of request ordering to
host-CPUs. The host-CPUs order data updates to files they
have access to, whereas FirmFS is ultimately responsible for
updating and maintaining metadata integrity, consistency, and
security with POSIX-level guarantees.

# Crash-Consistency and Scheduler

![image](https://i.imgflip.com/170wdc.jpg)

To handle crash consistency and protect data loss across tens and possibly hundreds
of FD-queues, CrossFS uses byte-addressable, persistent
NVMs as DMA’able and append-only FD-queues from which
FirmFS can directly fetch requests or pool responses. CrossFS
also designs low-cost data journaling for crash-consistency of
firmware file system state (§4.4). Finally, for efficient scheduling of device-CPUs, CrossFS smashes traditional two-level
I/O schedulers spread across the host-OS and the firmware
into one FirmFS scheduler. CrossFS also equips the scheduler
with policies that enhance file descriptor-based concurrency.

## Conclusion

![image](https://user-images.githubusercontent.com/42999231/201326591-eb5d2a61-9b98-4f9a-8f7f-21f97264d7cb.png)
