// lab5_problem4b.cpp
// -- template/driver for lab #5, problem 4(b)
// cs225 10/18

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
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
  operator char() const;
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

Proxy::operator char() const 
{
  if (index > sizeof(this->str))
    throw(std::out_of_range("ca c'est n'est pas un message d'erreur"));

  return str[index];
}


/////////////////////////////////////////////////////////////////
// test code
/////////////////////////////////////////////////////////////////
int main(void) {
  cout << boolalpha;

  istring s = "Yabba dabba doo!";

  char c = s[6];
  cout << "expected: d" << endl;
  cout << "     got: " << c << endl;
  cout << endl;

  cout << "expected: out_of_range exception" << endl;
  try {
    char c = s[128];
    cout << "     got: " << c << endl;
  }
  catch (out_of_range &e) {  
    cout << "     got: out_of_range exception" << endl;
  }
  catch (...) {
    cout << "     got: other exception" << endl;
  }

  return 0;
}

