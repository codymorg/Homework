// lab4_problem3.cpp
// -- template/driver for lab #4, problem #3
// cs225 9/18

#include <iostream>
#include <initializer_list>
using namespace std;


int count(initializer_list<initializer_list<int>> il) 
{
  int count = 0;
  for(initializer_list<int> parent : il)
  {
    count += parent.size();
  }

  return count;
}


int main(void) {

  int value = count({{1,2,3},{4,5},{6,7,8,9}});
  cout << "expected: " << 9 << endl;
  cout << "     got: " << value << endl;
  cout << endl;

  value = count({{9,2,4,1,0,4},{},{5},{}});
  cout << "expected: 7" << endl;
  cout << "     got: " << value << endl;

  getchar();

  return 0;
}

