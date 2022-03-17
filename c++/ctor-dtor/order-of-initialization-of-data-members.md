# Order of Initialization of Data Members of a `C++` Class

It is often recommended to initialize data members of a class in `C++` in the initialization list, instead of constructor body. However, the order of initialization doesn't depend
on how you are initializing data members in the initialization list. This initialization order depends on the order in which the data members are listed in the class definition.

```cpp
class String {
  char *s;         // | 
  size_t len;      // | order of initialization is important here
 
public:
  String(const char* w) : len(strlen(s)), s(strdup(w)) {}
  // even though initialization of `len` is written before that of `s`
  // `s` will be first initialized, and after then `len` will be initialized
  // this happend because declaration of `s` appears before declaration of `len`
  
  ~String() { free(s); }
  
  friend ostream& operator<<(ostream& os, const String& str) { 
    return os << str.s;
  }
};
```

## Special case of `static` data members

As the `static` data members cannot be initialized in the initialization list (as they don't belong to any particular instance of a class), they are initialized in the global scope. The order of initialization of `static` data member depend on the order in which they are initialized in the global scope and not the order in which they are declared in the class. For example, 

```cpp
class Data {
  int data;

public:
  Data(int data_) : data(data_) { cout << "Data: " << data << endl; }
};

class Complex {
  static Data PLUS_INF;       // for static members, this order is irrelevant
  static Data MINUS_INF;      // to the actual order of initialization
  // other declarations ...
};

Data Complex::PLUS_INF = (int) 1e8;           // order in which they are defined in the global scope
Data Complex::MINUS_INF = (int) (-1e8);       // determines the order in which they would be constructed
```

In this example, `Complex::PLUS_INF` will be constructed before `Complex::MINUS_INF` even if we swap their declaration in the `class Complex` as `Complex::PLUS_INF` is defined first in the global scope.
