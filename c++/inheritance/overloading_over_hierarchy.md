__Question__: Is overloading over a hierarchy allowed in `C++`?

__Answer__: No, overloading over a hierarchy is not allowed by default. Consider a hierarchy `A <- B` and suppose a function `foo` is defined in `class A`. Now, if a function `foo` is defined in `class B`, even with a different signature, it will shadow the `foo` defintion inherited from `class A`.

For example,

```cpp
// C++ doesn't allow function overloading in inheritance

class Base { public: 
  void f(int) { cout << i << endl; }
};

class Derived { public: 
  void f(string &s) { cout << s << endl; }
  // this function overrides Base::f(int), even though it has different signatures
  // reason -> function overloading doesn't happen across different scopes
  //
  // to actually overload, write
  using Base::f; // this will unshadow the function in Base
}
```
