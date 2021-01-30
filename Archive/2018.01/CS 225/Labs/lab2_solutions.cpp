/******************************************************************************
* Name       : Cody Morgan
* Assignment : Lab 2
* Class      : CS 225
* Semester   : Fall 2018
******************************************************************************/

#include <iostream>

void prob1()
{
  char a = 113;
  char b = 14;
  std::cout << "a   = 01110001 " << (short)a << "\n";
  std::cout << "b   = 00001110 " << (short)b << "\n";
  std::cout << "a&b = 00000000 " << (a & b) << "\n\n";

  char c = 79;
  short d = 0x8B;
  std::cout << "c   = 01001111 " << (short)c << "\n";
  std::cout << "d   = 10001011 " << (short)d << "\n";
  std::cout << "c^d = 11000100 " << (c ^ d) << "\n\n";

  std::cout << "a<<2      = 11000100\n";
  std::cout << "b>>3      = 00000001\n";
  std::cout << "a<<2|b>>3 = 11000101 " << ((a << 2) % 256 | (b >> 3)) << "\n\n";
}

bool testBit(unsigned x, unsigned i)
{
  unsigned mask = 1 << i;
  x &= mask;
  x >>= i;
  return x;
}

unsigned setBit(unsigned x, unsigned i)
{
  unsigned mask = 1 << i;
  x |= mask;
  x >> i;
  return x;
}

int mul11(int n)
{
  n += (n << 3) + (n << 1);
  return n;
}

void changeEvenIndices(int* A, int n, int(*f)(int))
{
  for (int i = 0; i < n; i += 2)
  {
    A[i] = f(A[i]);
  }
}

void sum(int x, void* ptr)
{
  *(reinterpret_cast<int*>(ptr)) += x;
}

void sumOdd(int value, void* ptr)
{
  SumOddData* sod = reinterpret_cast<SumOddData*>(ptr);
  if (sod->index % 2 == 1)
  {
    sod->sum += value;
  }
  sod->index++;
}

//  Driver Functions \\

void prob2()
{
  // lab2_problem2.cpp
  // -- template and driver for lab #2, problem #2
  // cs225 9/18
  unsigned n = 0x4;
  bool q_bit0 = testBit(n, 0),
    q_bit2 = testBit(n, 2);
  std::cout << "expected: bit #0 = 0, bit #2 = 1" << endl;
  std::cout << "     got: bit #0 = " << q_bit0
    << ", bit #2 = " << q_bit2 << endl;
}

void prob3()
{
  unsigned m = setBit(0x4, 1),
    n = setBit(0xf, 3);
  std::cout << "expected: m = 6, n = 15" << endl;
  std::cout << "     got: m = " << m
    << ", n = " << n << endl;
}

void prob4()
{
  int m = mul11(5),
    n = mul11(123);
  std::cout << "expected: m = 55, n = 1353" << endl;
  std::cout << "     got: m = " << m
    << ", n = " << n << endl;
}

int mul3(int n)
{
  return 3 * n;
}

void prob5()
{


  int A[] = { 4, 9, 8, 0, 5, 3, 1 };
  changeEvenIndices(A, 7, mul3);
  std::cout << "expected: 12 9 24 0 15 3 3" << endl;
  std::cout << "     got:";
  for (int i = 0; i < 7; ++i)
    std::cout << ' ' << A[i];
  std::cout << endl;
}

void prob6()
{
  int y = 10;
  sum(3, &y);
  int y1 = y;
  sum(5, &y);

  std::cout << "expected: 13 18" << endl;
  std::cout << "     got: " << y1 << ' ' << y << endl;
}

void processArray(const int *A, int n, void(*f)(int, void*), void *ptr)
{
  for (int i = 0; i < n; ++i)
    f(A[i], ptr);
}

void prob7()
{
  int A[] = { 0, 1, 2, 3, 4, 5 };
  SumOddData sodA = { 0, 0 };
  processArray(A, 6, sumOdd, &sodA);

  int B[] = { 8, 2, -3, 5, 1 };
  SumOddData sodB = { 0, 0 };
  processArray(B, 5, sumOdd, &sodB);

  cout << "expected: 9 7" << endl;
  cout << "     got: " << sodA.sum
    << ' ' << sodB.sum << endl;
}

/*int main(void)
{
  int prob = 1;
  cout << prob++ << "\n";
  prob1();
  cout << prob++ << "\n";

  prob2();
  cout << prob++ << "\n";

  prob3();
  cout << prob++ << "\n";

  prob4();
  cout << prob++ << "\n";

  prob5();
  cout << prob++ << "\n";

  prob6();
  cout << prob++ << "\n";

  prob7();

  return 0;
}*/
