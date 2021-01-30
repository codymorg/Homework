// lab4_problem5.cpp
// -- template/driver for lab #4, problem #5
// cs225 9/18

#include <iostream>
#include <stdexcept>
using namespace std;


/////////////////////////////////////////////////////////////////
// class hierarchy from problem #4
/////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////
// Implement the following functions
//   (do not include the above classes in your solutions file)
/////////////////////////////////////////////////////////////////
A *createD(int m, int n) 
{
  D* newD = new D(m,n);

  B* bPath = dynamic_cast<B*>(newD);
  return dynamic_cast<A*>(bPath);

}


int getCValue(A *pa) 
{
  C* cClass = dynamic_cast<C*>(pa);
  if(cClass)
    return cClass->value();
  else
    throw runtime_error("Il y a un problem avec votre class");
  return 0;
}


/////////////////////////////////////////////////////////////////
int main(void) {

  A *pa = createD(2,3);
  int value = pa->value();
  cout << "expected: 10" << endl;
  cout << "     got: " << value << endl;
  delete pa;
  cout << endl;
  
  A *array[] = { new A(0), new B(1,2), new C(3), createD(4,5) };
  cout << "expected: * * 5 7" << endl;
  cout << "     got: ";
  for (A *pa : array) {
    try {
      cout << getCValue(pa) << ' ';
    }
    catch (exception &e) {
      cout << '*' << ' ';
    }
  }
  for (A *pa : array)
    delete pa;
  cout << endl;

  return 0;
}

