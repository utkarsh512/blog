# Semaphores

[Under construction]

Semaphores are very simple techniques to manage concurrent processes by using simple interger variables. They use `wait()` and `signal()` in this process. A simple implementation is given below:

```c
// semaphore initialization - should be initialized with
// available number of instances of a given resource
// here, it is assumed only one instance is available.

int s = 1; 

// wait API
void wait(int *ps) {
  while (*ps <= 0); // busy-wait
  *ps--;
}

// signal API
void signal(int *ps) {
  *ps++;
}
```
Consider two processes `P1` and `P2` such that there share a critical section. Then, they can be synchronized as:

```c
int mutex = 1; // semaphore associated with critical section
               // shared by both P1 and P2
               
// process P1
while (1) {
  wait(&mutex); // busy-wait
  // critical section
  signal(&mutex);
  // remainder section
}

// process P2
while (1) {
  wait(&mutex); // busy-wait
  // critical section
  signal(&mutex);
  // remainder section
}
```
