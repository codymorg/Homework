// lab5_problem2.cpp
// -- driver/template for lab #5, problem #2
// cs225 10/18

#include <deque>
#include <iostream>
#include <stack>
#include <queue>
using namespace std;


/////////////////////////////////////////////////////////////////
// implement the following function
/////////////////////////////////////////////////////////////////
float average(const stack<int>& s) 
{
  stack<int> copy(s);
  float sum = 0.0f;
  while (!copy.empty())
  {
    sum += copy.top();
    copy.pop();
  }
  return sum / s.size();
}

/////////////////////////////////////////////////////////////////
// implement the following function
/////////////////////////////////////////////////////////////////
void order(queue<int>& q)
{
  priority_queue<int> copy;

  while (!q.empty())
  {
    copy.push(q.front());
    q.pop();
  }

  queue<int> qReturn;
  while (!copy.empty())
  {
    qReturn.push(copy.top());
    copy.pop();
  }

  q = qReturn;
}



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
