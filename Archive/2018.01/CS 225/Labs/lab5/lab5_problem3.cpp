// lab5_problem3.cpp
// -- driver/template for lab #5, problem #3
// cs225 10/18

#include <iostream>
#include <deque>
#include <queue>
using namespace std;


/////////////////////////////////////////////////////////////////
// implement the following function
/////////////////////////////////////////////////////////////////
void order(queue<int>& q) 
{
  priority_queue<int> copy;

  while(!q.empty())
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
// test code (do not include in your solution file)
/////////////////////////////////////////////////////////////////
ostream& operator<<(ostream &s, const queue<int> &q) {
  queue<int> q_copy(q);
  while (!q_copy.empty()) {
    s << q_copy.front() << ' ';
    q_copy.pop();
  }
  return s;
}


int main(void) {
  deque<int> d = { 3, 1, 5, 7, 2 };

  queue<int> q(d);
  order(q);
  cout << "expected: 7 5 3 2 1" << endl;
  cout << "     got: " << q << endl;

  return 0;
}

