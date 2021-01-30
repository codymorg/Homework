//#pragma warning (disable:4996)
#include <sstream>      // std::stringstream
#include <iomanip>      // left,right,fill,setw - see handout
#include <stack>        // ring container
#include <string>       //string
#include <iostream>

using std::stack;
using std::stringstream;
using std::string;
using std::cout;

// Private functions //
class Tower
{
public:
  Tower(int size, string name):name(name), peg_()
  {
    //    __        __        __    //
    //   ____      ____      ____   //
    //  _______   _______   _______ //
    for (size_t i = size; i > 0; i--)
    {
      peg_.push(i);
    }
  }


  // pop the top AAAANNND return what the top was
  int pop()
  {
    if (!peg_.empty())
    {
      int popper = peg_.top();
      peg_.pop();
      count_--;
      return popper;
    }
    else
      return 0;
  }

  int top()
  {
    if (!peg_.empty())
      return peg_.top();
    else
      return 0;
  }

  void push(int ring)
  {
    peg_.push(ring);
    count_++;
  }

  bool operator < (Tower other)
  {
    // we need to allow for null to be treated like 0
    if (this->peg_.empty())
      return false;

    return (other.peg_.empty() || this->peg_.top() < other.peg_.top());
  }

  // public variables
  string name;

private:
  stack<int> peg_;
  int count_ = 0;
};

static string moveRing(Tower& source, Tower& destination)
{
  stringstream ss;
  // validate ring
  if (source < destination)
  {
    ss <<  "move disk " << source.top() << " from " << source.name << " to " << destination.name;
    destination.push(source.pop());

  }

  return ss.str();
}


template< typename CALLBACK >
static void hanoi_tower_1_rec( int n, CALLBACK cb, Tower& source, Tower& aux, Tower& dest )
{
  if (n == 1)
  {
    cb(moveRing(source, dest));
  }
  else
  {
    hanoi_tower_1_rec(n - 1, cb, source, dest, aux);
    cb(moveRing(source, dest));
    hanoi_tower_1_rec(n - 1, cb, aux, source, dest);
  }
}



// Public Functions //

template< typename CALLBACK >

void hanoi_tower_1( int n, CALLBACK cb ) 
{
  Tower a(n, "A"); // source
  Tower b(0, "B"); // aux
  Tower c(0, "C"); // destination

  // build initial ring set


  hanoi_tower_1_rec(n, cb, a, b, c);

}

////////////////////////////////////////////////////////////////////////////////
template< typename CALLBACK >
void hanoi_tower_2( int n, CALLBACK cb ) 
{
    hanoi_tower_1( n, cb ); // if not doing extra credit just leave it like this
}

////////////////////////////////////////////////////////////////////////////////
template< typename CALLBACK >
void hanoi_tower_3( int n, CALLBACK cb ) 
{
    hanoi_tower_1( n, cb ); // if not doing extra credit just leave it like this
}
