## Creating a new scope for accessing any C++ construct

Consider the following code snippet

```cpp
namespace ns_1 {
  int x = 4;
}

namespace ns_2 {
  using ns_1::x;
}
```

For this program, `ns_1::x` and `ns_2::x` refer to the same variable.
