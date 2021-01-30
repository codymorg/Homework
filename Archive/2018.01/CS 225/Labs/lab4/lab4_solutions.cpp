// lab4_problem2.cpp
// -- template/driver for lab #4, problem #2
// cs225 9/18

#include <iostream>
#include <initializer_list>
#include <stdexcept>

using namespace std;


//prob 2
float average(initializer_list<int> il) 
{
  int sum = 0;
  for(int i : il)
  {
    sum+= i;
  }
  float average = 0.0f;
  if (il.size() > 0)
    average = (float)sum / il.size();
  return average;
}

//prob 3
int count(initializer_list<initializer_list<int>> il)
{
  int count = 0;
  for (initializer_list<int> parent : il)
  {
    count += parent.size();
  }

  return count;
}

//prob 5
A *createD(int m, int n)
{
  D* newD = new D(m, n);

  B* bPath = dynamic_cast<B*>(newD);
  return dynamic_cast<A*>(bPath);

}

int getCValue(A *pa)
{
  C* cClass = dynamic_cast<C*>(pa);
  if (cClass)
    return cClass->value();
  else
    throw runtime_error("Il y a un problem avec votre class");
  return 0;
}


