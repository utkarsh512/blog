#include <iostream>
using namespace std;

class A { public:
  virtual void f(int) { cout << "void A::f(int)" << endl;} // virtual, A::f
  virtual void g(double) { cout << "void A::g(double)" << endl;} // virtual, A::g
  int h(A *) {cout << "int A::h(A *)" << endl; return 0;} // non-virtual, A::h
};

class B: public A { public:
  void f(int) { cout << "void B::f(int)" << endl;} // override, virtual, B::f
  // void g(double) {} // inherited, virtual, A::g
  // int h(A *) {} // inherited, non-virtual, A::h(A *) <---------------- where does this function go?
  int h(B *) { cout << "int B::h(B *)" << endl; return 0;} // overload, virtual, B::h(B *)
};

class C: public B { public: 
  // void f(int) {} // inherited, virtual, B::f
  void g(double) { cout << "void C::g(double)" << endl;} // override, virtual, C::g
  // int h(A *) {} // inherited, non-virtual, A::h(A *)
  int h(B *) { cout << "int C::h(B *)" << endl; return 0;} // override, virtual, C::h(B *)
};

int main() {
  A a;
  B b;
  C c;
  A *pA;
  B *pB;

  pB = &b;
  pB->f(2);
  pB->g(3.2);
  pB->h(&a); // <----------------- raises error, but why?
  pB->h(&b);
  // Expected output
  // void B::f(int)
  // void A::g(double)
  // int A::h(A *)
  // int B::h(B *)

  pB = &c;
  pB->f(2);
  pB->g(3.2);
  pB->h(&a);
  pB->h(&b); // <----------------- raises error, but why?
  // Expected output
  // void B::f(int)
  // void C::g(double)
  // int A::h(A *)
  // int C::h(B *)

  pA = &a;
  pA->f(2);
  pA->g(3.2);
  pA->h(&a);
  pA->h(&b);
  // Expected output
  // void A::f(int)
  // void A::g(double)
  // int A::h(A *)
  // int A::h(A *)

  cout << endl;

  pA = &b;
  pA->f(2);
  pA->g(3.2);
  pA->h(&a);
  pA->h(&b);
  // Expected output
  // void B::f(int)
  // void A::g(double)
  // int A::h(A *)
  // int A::h(A *)

  cout << endl;

  pA = &c;
  pA->f(2);
  pA->g(3.2);
  pA->h(&a);
  pA->h(&b);
  // Expected output
  // void B::f(int)
  // void C::g(double)
  // int A::h(A *)
  // int A::h(A *)
  return 0;
}
