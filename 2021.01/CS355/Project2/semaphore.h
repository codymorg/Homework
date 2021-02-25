#include <atomic>
#include <condition_variable>
#include <thread>

class Semaphore
{
public:
  Semaphore(int threadCount);
  void wait();
  void signal();

private:
  const int               allowedThreads = 0;
  std::atomic_int         threadsIn;
  std::condition_variable cond;
  std::mutex              lock;
};
