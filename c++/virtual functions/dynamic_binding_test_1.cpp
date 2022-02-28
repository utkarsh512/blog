#include <iostream>
using namespace std;

class A { public:
  virtual void f(int) { cout << "void A::f(int)" << endl;} 
  virtual void g(double) { cout << "void A::g(double)" << endl;}
  int h(A *) {cout << "int A::h(A *)" << endl; return 0;} 
};

class B: public A { public:
  void f(int) { cout << "void B::f(int)" << endl;}
  virtual int h(B *) { cout << "int B::h(B *)" << endl; return 0;} 
};

class C: public B { public: 
  void g(double) { cout << "void C::g(double)" << endl;} 
  int h(B *) { cout << "int C::h(B *)" << endl; return 0;} 
};

int main() {
  A a;
  B b;
  C c;
  A *pA;
  B *pB;
  
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
  
  pB = &b;
  pB->f(2);
  pB->g(3.2);
  pB->h(&a);
  pB->h(&b);
  // Expected output
  // void B::f(int)
  // void A::g(double)
  // raises error -- cannot convert A * to B *
  // int B::h(B *)
  
  pB = &c;
  pB->f(2);
  pB->g(3.2);
  pB->h(&a);
  pB->h(&b); 
  // Expected output
  // void B::f(int)
  // void C::g(double)
  // raises error -- cannot convert A * to B *
  // int C::h(B *)
  return 0;
}
