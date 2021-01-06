#include <iostream>
#include "BigNumber.h"
using std::cout;

int main()
{
  Huge huge("123456");
  cout << huge.isDivisible(2);

}

