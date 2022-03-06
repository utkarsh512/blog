# Order of Initialization of Data Members of a `C++` Class

It is often recommended to initialize data members of a class in `C++` in the initialization list, instead of constructor body. However, the order of initialization doesn't depend
on how you are initializing data members in the initialization list. This initialization order depends on the order in which the data members are listed in the class definition.

```cpp
class String {
  char *s;         // | 
  size_t len;      // | order of initialization is important here
  
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
