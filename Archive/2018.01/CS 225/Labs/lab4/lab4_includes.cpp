// lab4_includes.cpp
// -- things you are to assume are already included in your
//    solutions to the problems in lab #4
// cs225 9/18

#include <iostream>
#include <stdexcept>
#include <initializer_list>
using namespace std;


struct A {
  A(int n) : _value(n)     {}
  virtual ~A(void)         {}
  virtual int value(void)  { return _value; }
  private:
    int _value;
};


struct B : A {
  B(int m, int n) : A(m), _value(n) {}
  ~B(void) override                 {}
  int value(void) override          { return A::value() + _value; }
  private:
    int _value;
};


struct C : A {
  C(int n) : A(n)          {}
  ~C(void) override        {}
  int value(void) override { return A::value() + 2; }
};


struct D : B, C {
  D(int m, int n) : B(m,n), C(n) {}
  ~D(void) override              {}
  int value(void) override       { return B::value()+C::value(); }
};



#include "lab4_solutions.cpp"
// Your code should successfully compile (but not link) with the commands
//    cl /EHsc /c lab4_includes.cpp
// and/or
//    g++ -std=c++11 -c lab4_includes.cpp


