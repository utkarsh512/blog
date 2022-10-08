# CubicleOS: A Library OS with Software Componentisation for Practical Isolation

[Vasily A. Sartakov](mailto:v.sartakov@imperial.ac.uk),
Imperial College London,
United Kingdom

[Lluís Vilanova](mailto:vilanova@imperial.ac.uk),
Imperial College London,
United Kingdom

[Peter Pietzuch](mailto:prp@imperial.ac.uk),
Imperial College London,
United Kingdom

## Motivation

The monolithic approach for library OS design raises security, robustness and reliability concerns due to lack of compartmentalisation between the libOS components. Though the microkernel approach enforces protection boundaries between components by imposing standard interfaces (based on messgae passing), such designs have seen limited uptake as they lack full POSIX compatibility. The extra copies imposed by microkernel interfaces adds further to the overhead of libOS designs.

## Problem Statement

To design modular and compartmentalized libOS which contains third-party components and, at the same time, enforces pratical isolation between these components.

### Points to consider

* Retain the flexibility of in-kernel interfaces (as in monolithic approach)

* Enforce spatial and temporal memory isolation between components with acceptable perfomance overhead

* Minimum source code changes in application / libOS and retain full POSIX compatibility


## Major Contributions

The solution has three major contributions:

* It mutually isolates exisiting, third-party components for data integrity and privacy

* Minimal code changes to express isolation policies for components

* Efficient implementation based on existing harware with trivial modifications

## Core Abstractions

CubicleOS uses three core abstractions:

* **Cubicles**

  * Transparent spatial memory isolation for libOS and application component
  * Attempt to read from / write to memory of another *cubicle* leads to memory protection fault

* **Windows**
  
  * User-managed temporal memory isolation
  * Allows *cubicles* to temporarily share data with each other without copying

* **Cross-cubicle Calls**

  * Control flow integrity when calling functions across cubicle boundaries


## Interfaces between Components

![image](https://user-images.githubusercontent.com/42999231/194690094-7c68644b-c555-4b64-aefc-5428c470ad35.png)

Consider two components `FOO` and `BAR`. Each component has one function: `foo()` is located inside `FOO` and `bar()` is located inside `BAR`. Function `foo()` 
has two stack variables: an array of 10 bytes, `array[10]`, and an integer `a = 5`. Function `foo()` invokes function `bar()` and passes the pointer to the array and 
the integer.

In *direct function call* approach, `bar()` has access to the `foo`'s stack and, therefore, can directly change the array. Such calls are fast, but they cannot
be used for the interaction between isolated components (as any attempt to access `FOO`'s stack inside `BAR` would cause a memory protection fault).

In *message-passing* based interfaces, `foo` prepares a message and requests kernel to send it to callee. At the callee's side, message dispatcher retrives
the arguments and calls the actual function with passed arguments. Though this approach possesses isolation properties, the overhead in data marshalling, switching
between caller, kernel, and then callee degrades perfomance.

CubicleOS uses *window*-based interfaces that combines the efficiency and flexibility of direct function calls with isolation properties of message-based 
implementations. Calls have the same semantics as direct function calls: e.g., the caller can pass a pointer and a scalar value to the callee, and the callee, 
in turn, directly accesses the passed values. This becomes possible because individual memory pages are assigned to protection domains (indicated by
different colours). Before the invocation of function `bar()`, component `FOO` makes the memory pages with the array accessible to `BAR`.
After the call, the caller revokes the access permissions to the stack variable, and the components are again fully isolated.

## Design and API

To enforce the desired isolation policies, CubicleOS has four trusted components:

* **Component Builder**
  
  * Identifies each component in the system and their public functions and generates for each function a *cross-cubicle call trampoline*
  * The auto-generated trampoline has three tasks:
     
    * Switching memory access permissions across cubicles via the trusted memory monitor
    * Switching stack pointers
    * Copying in-stack arguments when caller and callee cubicles use different stack

* **Memory Monitor**

  * A trusted cubicle with an interface to manage permissions and window ownership
  * Offers stack/heap allocation primitives which assigns pages to the calling cubicle as well as the CubicleOS specific APIs
  

| API function | Description |
| --- | --- |
| `wid_type cubicle_window_init()` | Initialise an empty window |
| `cubicle_window_add(wid_type wid, void *ptr, size_t size)` | Associate memory range (`ptr`, `ptr+size`) to window `wid` |
| `cubicle_window_remove(wid_type wid, void *ptr)` | Remove a memory range previously associated to window `wid` |
| `cubicle_window_open(wid_type wid, cid_type cid)` | Allow cubicle `cid` to access contents of window `wid` |
| `cubicle_window_close(wid_type wid, cid_type cid)` | Disallow cubicle `cid` to access contents of window `wid` |
| `cubicle_window_close_all(wid_type wid)`|  Disallow all accesses to `wid` from other cubicles |
| `cubicle_window_destroy(wid_type wid)` | Destroy window `wid` |

* **Cubicle Loader**
  
  * Takes a set of pages owned by a cubicle, containing code and data of a component to load into the system, and switches their ownership into a newly created
    cubicle, 
  * It is similar to `dlopen` with following additional constraints:
    
    * Page identified as code are given execute-only permissions
    * Data pages are given read/read-write permissions (as specified in binary)
    * CubicleOS doesn't allow cubicles to change the execution permissions of any page
    * Loader scans the code pages to ensure that they do not contain any instructions that would affect the integrity of the isolation mechanisms, i.e., cubicles
      cannot directly execute system calls nor MPK-related operations

## Implementation

* CubicleOS is implemented on top of Unikraft framework
  
  * Leverages its modular architecture as the basis for automatically identifying and isolating cubicles

* Uses *Intel MPK* to enforce the memory access policies expressed by cublicles and windows

### Unikraft Architecture

* Framework for building unikernels
* Modular architecture in which each component implements a single OS function 
* Components are selected at compile-time and linked into monolithic image together with the application
* Components interact by referencing each other's function and data symbols (resolved at dynamic link time) either directly, or using callbacks

### CubicleOS Builder

* Extends Unikraft build logic to produce information necessary for CubicleOS
* Each component is compiled as separate dynamic library. Each component can either be isolated or shared
* Identifies functions used across cubicles and generates cross-cubicle call trampoline

### Monitor

* Responsible for bootstraping the system and enforcing the isolation of cubicles and the access permissions of windows
* Each cubicle has three arrays with window descriptors for global, stack, and heap data. The descriptors are stored in a simple
  array, created by the CubicleOS monitor, and contain the address and size of a single memory range and a bitmask of cubicles for which this window is open.
* CubicleOS authorises memory accesses across cubicles via a lazy trap-and-map approach.

  ![image](https://user-images.githubusercontent.com/42999231/194707745-fa82e99c-09be-4e76-b964-455ff748b0f6.png)



### Loader

* The loader enforces two properties on untrusted code that ensure the integrity of the memory isolation mechanisms:

  * No access to system calls, as they can change the MPK tags and access permissions on pages via the host OS
  * No access to the `wrpkru` instruction, as it can be used to change access permissions to the per-cubicle MPK tags

### Cross-cubicle Call Trampolines

* Enforces control flow integrity (CFI), i.e., calls and returns across cubicles can only happen through the intended entry points
* The code thunk of a trampoline is generated and signed by CubicleOS builder and thus trusted by the loader. It is in charge of performing the actual permission and context switch between cubicles.

## Important Features

### Trap-and-map Model

Previous work on compartmentalisation
has used MPK to share pages that are used for communication
across compartments. This implies that each compartment needs
one additional tag for every other compartment that it communicates
with, and must copy data to/from these shared buffers. Such
an approach requires changes to component interfaces to perform
these memory copies, but also adds overhead due to the extra copies.
It also requires a larger number of used MPK tags, which must be
virtualised when exhausted. Instead, CubicleOS’s trap-andmap
model maintains existing interfaces and avoids memory copies
by dynamically changing a page’s tag only when necessary, which
also uses fewer tags.

### Casual Tag Consistency

Closing a window does not immediately
revoke access to its contents, i.e., page tags are not reassigned to the
window’s owning cubicle. Instead, CubicleOS keeps pages with their
current tag, and lazily reassigns tags using trap-and-map only when
a page is accessed by a cubicle with access to that window (which
includes the owning cubicle). This is an optimisation to decrease
the overhead of cubicle switches, and maintains causal consistency
with respect to window operations. Since a callee cubicle could have
accessed a page from a window before closing, it is thus correct to
let it access it anytime before another cubicle accesses such a page.

### Nested Calls

A cubicle cannot open a new window on data shared
by another cubicle (via a window) because a cubicle must be the
owner of a window to modify its permissions through its window
table. In the case of a nested call, i.e., when data is shared with more
than one other cubicle, the window must be opened by its owner
for all cubicles ahead of time. Alternatively, nested calls could copy
shared data and open windows for intermediate buffers, or the
monitor could be modified to keep track of “sub-windows”, but we
have not encountered such a case for the evaluated applications.


## Evaluation

### Overhead for I/O-intensive workload (NGINX)

To benchmark NGINGX, the siege utility is used to generate
requests to random static files located inside the RAMFS of the
server. As a performance metric, the download latency
of files with different sizes is measured. As a baseline, the implementation is compared against the
baseline Unikraft system.

![image](https://user-images.githubusercontent.com/42999231/194707730-c74828b9-28d7-4d60-9633-9654cadd9b78.png)

The figure above shows the cubicles used as part of NGINX, and the number
of cross-cubicle calls during the execution. The main isolated
cubicles are the application (NGINX), the TCP/IP stack (LWIP), the
network device driver (NETDEV), the virtual file system (VFSCORE), the
file system backend (RAMFS), and the platform code (PLAT). In addition,
there is a system-wide memory allocator (ALLOC) and the time
module (TIME). Shared cubicles are not shown, but are comprised
of newlibc and the random device driver.

![image](https://user-images.githubusercontent.com/42999231/194707717-d1c280ae-b1f6-4bf4-b1d4-9998d291e39e.png)

The figure shows the request latencies for different file sizes. Latency
is almost constant for small files (5–6 ms for the baseline;
6–7 ms for CubicleOS), and grows when the file size reaches 64 kB.
After that, it grows almost linearly with file size. The change in
slope for files larger than 1 MB is due to the buffer size inside
LWIP. The overhead of cubicles also changes after 64 kB: it increases
from 15% to 2×. In other words, the partitioning of NGINX into
eight components that exchange a high volume of data halves the
throughput.

### Overhead for CPU-intensive workload (SQLite)

SQLite is ported to CubicleOS and performance under
the speedtest1 benchmark workload is observed. This benchmark includes
various queries, starting from simple INSERT queries and ending
with complex multi-way JOINs.

![image](https://user-images.githubusercontent.com/42999231/194707889-6c1bb971-7aab-4424-a1e4-7633de844f90.png)

The figure above shows the configuration of cubicles. 7 isolated
cubicles and 4 shared ones (not shown) are used. Three isolated cubicles
provide the main functionality and are thus invoked frequently: the
SQLite library combined with the speedtest1 benchmark, the virtual
file system VFSCORE, and the driver for the RAMFS file system. Compared
to NGINX, each cubicle uses only its own memory allocation
library, and ALLOC is used only for coarse-grained allocations.

In this experiment, the impact of the CubicleOS
mechanisms on performance is compared. 
CubicleOS’s three main mechanisms is explored: 

* Switching mechanism between cubicles;
* MPK-based memory protection
* Window-based cross-cubicle calls. 

We measure the impact of each mechanism on
performance. Therefore, we compare: 

* the baseline Unikraft;
* CubicleOS without MPK protection
* CubicleOS with MPK protection but without ACLs (i.e., the windows are “open” for any access)
* full-fledged CubicleOS.

![image](https://user-images.githubusercontent.com/42999231/194707993-867a9aab-96eb-44a9-8d64-67cc2fc15281.png)

The figure above shows the results for query execution times. The queries
can be separated into two groups: almost two thirds of queries (100–
120, 140–161, 180, 190, 230, 250, 300, 320, 400, 500, 520, 990) demonstrate
a low moderate impact of cubicles on performance. Here,
the introduction of the trampolines adds 2% overhead, MPK adds
50%, while the windows add 20% overhead. On average, CubicleOSbased
SQLite requires 1.8× longer to process a query. A common
feature of this group of queries is that they use the OS interface
infrequently. They benefit from caching and only involve the OS
interface to write batched pages evicted from the cache.
The second group of queries has significant overhead. While
the trampolines add a reasonable overhead (17%), the use of MPK
and windows increases request time by 4× and 1.2×, respectively.
For these queries, on average, CubicleOS-based SQLite requires 8×
more time to process queries. These queries benefit less from the
use of the database page cache, and they significantly more often
use the OS interface. The overhead of cubicle switches therefore
become more significant, and the average slowdown factor is 4×.



