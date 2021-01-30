// MatrixTest.cpp
// -- simple test of matrix class, with C++11 improvements
// cs225

#include <iostream>
#include "Matrix.h"
using namespace std;


ostream& operator<<(ostream& s, const Matrix &m) {
  for (unsigned i=0; i < m.getRows(); ++i) {
    s << (i==0 ? '{' : ',');
    for (unsigned j=0; j < m.getCols(); ++j)
      s << (j==0 ? '{' : ',') << m[i][j];
    s << '}';
  }
  s << '}';
  return s;
}


int main(void) 
{
  Matrix A(2, 3);
  A[0][0] = 1;  A[0][1] = 2;  A[0][2] = 3;
  A[1][0] = 4;  A[1][1] = 5;  A[1][2] = 6;
  cout << A << endl;

  Matrix C = A;
  cout << C << endl;

  Matrix B = {{7,8},
              {9,10},
              {11,12}};
  cout << B << endl;

  C = B;
  cout << C << endl;

  Matrix D = A*B;
  cout << D << endl;

  A = D - 2*D;
  cout << A << endl;

  A = {{6,5},{4,3},{2,1}};
  cout << A << endl;

  try {
    Matrix F = {{1,2,3},{4,5}};
    cout << F << endl;
  }
  catch (exception &e) {
    cout << e.what() << endl;
  }

  return 0;
}

