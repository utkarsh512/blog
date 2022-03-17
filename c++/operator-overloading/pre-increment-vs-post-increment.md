# Pre-increment vs. post-increment in `C++`

Consider the following code:

```cpp
int t = 0;
(t++)++;   // <----------[1]
++(++t);   // <----------[2]
```

Do you think this code will compile successfully? Is there something fishy here? If you compile this code, you will receive an error for `[1]`, while `[2]` is a valid statement.
To understand this behavior, we need to look bit-more closely on the signatures for pre-increment and post-increment operator functions.

```cpp
class MyType {
  int val;

public:
  // constructors, destructors and other functions
  
  // overloading pre-increment operator
  MyType& operator++() {
    ++val;
    return *this;
  }
  
  // overloading post-increment operator
  MyType operator++(int) {
    MyType tmp = *this;
    ++val;
    return tmp;
  }
};
```

One major difference b/w the two operators is that while pre-incrementing, we return the same object on which it was invoked, however, a local object is returned while post-incrementing.
Therefore, the expression `++t` is the same object `t`, i.e., they both share the same location in the memory, and we can invoke another function using that expression.
Thus, `++(++t)` which translates to `(++t).operator++()` is valid. However, expression `t++` is a local object stored in temporary location, and thus the compiler will not allow us to invoke
another function using the expression `t++`, as the location becomes garbage. Hence, `(t++)++` which translates to `(t++).operator++(int)` fails.

You can verify that `++t` enjoys the same location as `t`, while `t++` has a different address as follows:
```cpp
int t = 0;
const int& pre = ++t;
const int& post = t++;

cout << "Address of t: " << &t << endl;
cout << "Address of ++t: " << &pre << endl;
cout << "Address of t++: " << &post << endl;
```
