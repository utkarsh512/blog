# `auto` vs. `decltype` 

## `auto` keyword

* `auto` keyword removes the burden of specifying the full type of a variable.
* `auto` keyword cannot be used without a variable.
* Data-type is inferred from the RHS expression.
* RHS expression is evaluated first, then data-type is inferred.
* `auto` keyowrd ignores `const`/`volatile` and reference signature.

```cpp
auto       x  = 10;  // int
auto       p  = &x;  // int * 
const auto q  = &x;  // int * const 
const auto *r = &x;  // const int *

int&       y  = x;   
auto       z  = y;   // int
auto&      w  = y;   // int&
```

## `decltype` keyword

* `decltype` keyword infers the type of given expression without evaluating it.
* `decltype` keyowrd can be used without a variable.
* `decltype` keyoerd remembers the `const`/`volatile` and reference signature.

```cpp
int        a = 10;
int&       b = a;
const int  c = 10;
const int& d = c;

decltype(a) x;  // int
decltype(b) y;  // int&
decltype(c) z;  // const int
decltype(d) w;  // const int&
```

## Recommendation

In C++14 and beyond, we can use `decltype(auto)` which should be preferred over `auto` and `decltype`.
