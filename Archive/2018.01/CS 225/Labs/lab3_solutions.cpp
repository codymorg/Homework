/******************************************************************************
* Name       : Cody Morgan
* Assignment : Lab 3
* Class      : CS 225
* Semester   : Fall 2018
******************************************************************************/

#include <iostream>
#include <utility>
using namespace std;


int getValue(const int* A, unsigned rows, unsigned cols, unsigned r, unsigned c)
{
  return A[r * cols + c];
}

//move constructor
Array::Array(Array &&A) noexcept 
{
  this->size = A.size;
  this->data = A.data;
  A.data = nullptr;
  A.size = 0;
}

//move assignment
Array& Array::operator=(Array &&A) noexcept 
{
  this->size = A.size;
  this->data = A.data;
  A.data = nullptr;
  A.size = 0;

  return *this;
}

ostream& operator<<(ostream &s, const Array &A) 
{
  for (unsigned i = 0; i < A.length(); ++i)
    s << A[i] << ' ';
  return s;
}


