// lab5_problem4a.cpp
// -- template/driver for lab #5, problem 4(a)
// cs225 10/18

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;


/////////////////////////////////////////////////////////////////
// class declarations and partial implementations
// -- do NOT put this in your solutions file
/////////////////////////////////////////////////////////////////
struct Proxy {
  const string &str;
  unsigned index;
  Proxy(const string &s, unsigned i) : str(s), index(i) { }
  bool operator==(const char *s);
};

struct istring {
  string str;
  istring(const char *s) : str(s) { }
  Proxy operator[](int i)  { return Proxy(str,i); }
};


/////////////////////////////////////////////////////////////////
// implement the following function
// --  add ONLY this function to your solutions file
/////////////////////////////////////////////////////////////////
bool Proxy::operator==(const char *s) 
{
  int pI = this->index;
  int i = 0;

  while (s[i] != '\0' && this->str[pI] != '\0')
  {
    if (s[i] != this->str[pI])
      return false;
    i++;
    pI++;
  }

  return true;
}


/////////////////////////////////////////////////////////////////
// test code
/////////////////////////////////////////////////////////////////
int main(void) {
  cout << boolalpha;

  istring s = "Yabba dabba doo!";
  bool result = (s[1] == "abba");
  cout << "expected: true" << endl;
  cout << "     got: " << result << endl;
  cout << endl;

  result = (s[5] == "abba");
  cout << "expected: false" << endl;
  cout << "     got: " << result << endl;

  return 0;
}

