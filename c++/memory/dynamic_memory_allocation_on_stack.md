__Question__: Is it possible to allocate dynamic memory on stack?

__Answer__: Yes, it is possible to do so in `C++` using "placement `new`" operator.

For example,

```cpp
void foo() {
  unsigned char buf[sizeof(int) * 2];
  
  int *p = new (buf) int(3);
  int *q = new (buf + sizeof(int)) int(5);
  
  // in this case, data is allocated on stack instead of heap
}
```
