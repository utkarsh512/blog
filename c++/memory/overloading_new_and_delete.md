`C++` allows overloading the `new` and `delete` as follows:

* Overloading `operator new` 

```cpp
void *operator new(size_t n) {  // other arguments can be added here
  void *p = operator new(n);
  return p;
}
```

* Overloading `operator delete`

```cpp
void operator delete(void *p) {
  operator delete(p);
}
```

* Overloading `operator new[]`

```cpp
void *operator new[](size_t n, char setv) {
  void *p = operator new(n);
  memset(p, setv, sizeof(p));
  return p;
}

// above overloading must be used as
char *p = new ('#') char[10];
                ^
                additional parameters goes here
```

* Overloading `operator delete[]`

```cpp
void operator delete[](void *p) {
  operator delete(p);
}
```
