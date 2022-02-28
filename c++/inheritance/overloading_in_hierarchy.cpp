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
