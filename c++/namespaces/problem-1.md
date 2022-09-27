## C++ / Namespaces / Problem 1

```cpp
string x = "global-scope";

namespace ns {
  string x = "ns-scope";
}

void func() {
  using ns::x;
  /**
   * How would you access variable `x` defined in the global scope? 
   *
   * using ns::x hides the global variable `x` and now the symbol `x`
   * will be binded to ns::x.
   * To access the global variable `x`, we need to use ::x 
   */
  
  cout << x << endl;    /* prints ns-scope */
  cout << ::x << endl;  /* prints global-scope */
}
```
