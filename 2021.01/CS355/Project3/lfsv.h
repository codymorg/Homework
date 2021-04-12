#include <algorithm>
#include <atomic>   // std::atomic
#include <deque>    // std::deque
#include <iostream> // std::cout
#include <mutex>    // std::mutex
#include <thread>   // std::thread
#include <vector>   // std::vector
using std::atomic_bool;
using std::atomic_int;
using std::sort;
using std::vector;
typedef vector<int>*        vecptr;
typedef std::atomic<vecptr> atomicVec;

class LFSV
{
public:
  LFSV()
  {
    vecptr vec = data.load();
    vec        = new vector<int>;
    vec->reserve(1000);
    data.store(vec);
    refCount.store(0);
  }

  ~LFSV()
  {
    delete data.load();
  }

  void Insert(const int& value)
  {
    vecptr vec  = nullptr;
    vecptr temp = new vector<int>;
    temp->reserve(1000);

    int ref = refCount.load();

    do
    {
      // load from atomic
      vec = data.load();

      // copy to temp
      temp->clear();
      *temp = *vec;
      temp->push_back(value);

      // add and sort
      std::sort(temp->begin(), temp->end());

      // attempt to put back into atomic
    } while (!refCount.compare_exchange_strong(ref, ref + 1));
    data.store(temp);
    delete vec;
    vec = nullptr;
  }

  int operator[](int pos)
  {
    return data.load()->at(pos);
  }

  atomicVec  data;
  atomic_int refCount;
};
