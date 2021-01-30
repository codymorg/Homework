#include <iostream>

int getValue(const int *A, unsigned rows, unsigned cols, unsigned r, unsigned c)
{
  return A[r * cols + c];
}

void test5()
{
  int x = 8;
  std::cout << x <<std::endl;

  int& y = x;
  std::cout << y<<std::endl;

  x = 7;
  std::cout << x<<std::endl;

  int&& z = y - 2;
  std::cout << z << y<<std::endl;

  int&& w = z - 1;
  std::cout << w << z<<std::endl;

  y = z;
  std::cout << y << z<<std::endl;

  z = 6;
  std::cout << z <<std::endl;

}
class Array 
{
  public:
    Array(unsigned n) : size(n)        { data = new float[n]; }
    ~Array(void)                       { delete[] data; }
    Array(const Array &A) = delete;
    Array(Array &&A) noexcept;
    Array& operator=(const Array &A) = delete;
    Array& operator=(Array &&A) noexcept;
    float operator[](unsigned i) const { return data[i]; }
    float& operator[](unsigned i)      { return data[i]; }
    unsigned length(void) const        { return size; }
  private:
    float *data;
    unsigned size;
};

//
Array::Array(Array &&A) noexcept 
{

}


Array& Array::operator=(Array &&A) noexcept 
{

}
int main(int argc,char* argv[])
{
  test5();
  getchar();
  return 0;
}
/*
deconst
copy const
assn operator

r2 = Ralph() //inefficient copy const and assignment

//new in c++11
move copy const
  steals mem from rhs (temp)
move assign operator

Ralph& operator=(Ralph&& r)
{
  //steals the mem from the temp r
  //shallow copy
  r = nullptr // deconst will be called so make sure it's safe
}

//reg copy const
  r2 = Ralph(2)
//move copy const
  #include <utility>
  std::move(r3) // changes r3 into a temp and can thereby be treated as an rvalue ref

Ralph(Ralph&& r)
{
  r.value = nullptr;
}
*/