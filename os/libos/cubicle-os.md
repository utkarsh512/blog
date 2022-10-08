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


## CubicleOS

The solution has three major contributions:

* It mutually isolates exisiting, third-party components for data integrity and privacy

* Minimal code changes to express isolation policies for components

* Efficient implementation based on existing harware with trivial modifications

### Core Abstractions

CubicleOS uses three core abstractions:

* **Cubicles**

  * Transparent spatial memory isolation for libOS and application component
  * Attempt to read from / write to memory of another *cubicle* leads to memory protection fault

* **Windows**
  
  * User-managed temporal memory isolation
  * Allows *cubicles* to temporarily share data with each other without copying

* **Cross-cubicle Calls**

  * Control flow integrity when calling functions across cubicle boundaries

### Implementation

* CubicleOS is prototyped on top of Unikraft
* Developers only need to manage the *windows* (to grant memory access across cubicles)
* Generates secure cross-cubicle call trampolines for each component duing build.
* Memory isolation is enforces using Intel's MPK (Memory Protection Keys)
  * Each cubicle has an MPK tag
  * Used for managing access permissions for windows

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

## CubicleOS Design and API

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
| `cubicle_window_add(wid_type wid, void *ptr, size_t size)` | Associate memory range (ptr, ptr+size) to window wid |
| `cubicle_window_remove(wid_type wid, void *ptr)` | Remove a memory range previously associated to window wid |
| `cubicle_window_open(wid_type wid, cid_type cid)` | Allow cubicle cid to access contents of window wid |
| `cubicle_window_close(wid_type wid, cid_type cid)` | Disallow cubicle cid to access contents of window wid |
| `cubicle_window_close_all(wid_type wid)`|  Disallow all accesses to wid from other cubicles |
| `cubicle_window_destroy(wid_type wid)` | Destroy window wid |

* **Cubicle Loader**
  
  * Takes a set of pages owned by a cubicle, containing code and data of a component to load into the system, and switches their ownership into a newly created
    cubicle, 
  * It is similar to `dlopen` with following additional constraints:
    
    * Page identified as code are given execute-only permissions
    * Data pages are given read/read-write permissions (as specified in binary)
    * CubicleOS doesn't allow cubicles to change the execution permissions of any page
    * Loader scans the code pages to ensure that they do not contain any instructions that would affect the integrity of the isolation mechanisms, i.e., cubicles
      cannot directly execute system calls nor MPK-related operations
