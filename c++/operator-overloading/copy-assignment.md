# Copy assignment operator in C++

```cpp
// Generic class with overloaded copy-assignment operator

class Complex:
 private:
  double re, im;
 public:
  ...
  Complex& operator=(const Complex& other) {
    re = other.re;
    im = other.im;
    return *this;
  }
  ...
```

## Why do we return a reference while overloading copy assignment operator?

While overloading the copy assignment operator, *return-by-reference* is used instead of *return-by-value* (or not returning anything!). 
To understand why we do this, we can analyze the situation in two steps:

* Why do we need to return anything at all in copy assignment overload?
* Once, we answered the previous question, we have a follow-up. Why *return-by-reference* instead of *return-by-value*?

### Why do we need to return anything at all in copy assignment overload?

This is done to support chained assignment for user-defined classes (which is supported by built-in types).

```cpp
Complex p = {3, 4}, q, r;
r = q = p;
```

Above code will not work if the return type for overloaded copy-assignment operator is `void`.

### Why return-by-reference instead of return-by-value?

This is tricky. In C++, we can do something like this:

```cpp
int a = 10, b, c = 2;
(b = c) = a;
```

This is a valid(!) C++ code. After the second line has been executed, the values will be: `a`(10), `b`(10), `c`(2). 

This is only possible beacuse the expression `b = c` knows the address of `b`. To extend this feature to user-defined class, we use *return-by-reference*.
