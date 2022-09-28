## Computing offset of any data member for a C++ struct

Consider the following structure

```cpp
struct Item {
  int x;
  char y;
  double z;
  bool w;
}
```

How to get offset of any of the above data members of `Item` structure at compile time without creating any instance?

We can define a macro `OFFSET(type, field)` that will compute offset of data member `field` in structure `type` as 

```cpp
#define OFFSET(type, field) &((static_cast<type *>(nullptr))->field)
```
