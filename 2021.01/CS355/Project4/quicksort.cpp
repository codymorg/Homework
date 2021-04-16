#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using std::atomic_int;
using std::cout;
using std::mutex;
using std::thread;
using std::vector;
class Semaphore
{
public:
  Semaphore(int threadCount) : allowedThreads(threadCount), cond(), lock()
  {
  }
  void wait()
  {
    if (threadsIn >= allowedThreads)
    {
      std::unique_lock<std::mutex> waitForSignal(lock);
      cond.wait(waitForSignal, [&] { return threadsIn < allowedThreads; });
    }
    else
      threadsIn++;
  }

  void signal()
  {
    cond.notify_one();
    threadsIn--;
  }

private:
  const int               allowedThreads = 0;
  std::atomic_int         threadsIn;
  std::condition_variable cond;
  std::mutex              lock;
};

template <typename T> void quicksort(T* a, unsigned begin, unsigned size, int threadCount);

static vector<thread*> threadPool;
static Semaphore*      sem;
static mutex           threadLock;

template <typename T> static void threadFn(T* a, int lowerbound, int size);

template <typename T> static thread* launchThread(T* a, int begin, int size)
{
  sem->wait();
  threadLock.lock();
  threadPool.push_back(new thread(threadFn<T>, a, begin, size));
  thread* newthread = threadPool.back();
  threadLock.unlock();
  return newthread;
}

template <typename T> static void printArray(T* a, int size)
{
  return;
  threadLock.lock();
  for (auto i = 0; i < size; i++)
  {
    std::cout << a[i] << " ";
  }
  std::cout << '\n';
  threadLock.unlock();
}
static void printNum(const char* let, int index)
{
  return;
  threadLock.lock();
  for (auto i = 0; i < index; i++)
  {
    std::cout << "  ";
  }
  std::cout << let << '\n';
  threadLock.unlock();
}

template <typename T> static void threadFn(T* a, int lowerbound, int size)
{
  int upperbound = size - 1;
  T   pivot      = a[upperbound];
  int left       = lowerbound - 1;
  int right      = upperbound;

  if (lowerbound >= upperbound)
    return;

  printNum("start", 0);
  printArray(a, size);
  while (left < right)
  {
    do
    {
      left++;
      printNum("L", left);
    } while (left < size && a[left] < pivot);
    do
    {
      right--;
      printNum("R", right);
    } while (right >= 0 && a[right] > pivot);
    if (left < right)
    {
      T tmp    = a[left];
      a[left]  = a[right];
      a[right] = tmp;
      printArray(a, size);
    }
  }
  printNum("Pivot swap----", 0);
  printArray(a, size);
  printNum("L", left);
  printNum("R", upperbound);
  T tmp         = a[left];
  a[left]       = pivot;
  a[upperbound] = tmp;
  printArray(a, size);
  printNum("Split-----", 0);
  printArray(a + lowerbound, left - lowerbound);
  printArray(a + left + 1, upperbound - left);

  sem->signal();
  auto leftThread = launchThread(a, lowerbound, left);
  sem->signal();
  auto rightThread = launchThread(a, left + 1, size);

  leftThread->join();
  rightThread->join();
}

void static cleanupThreads()
{
  for (auto th : threadPool)
  {
    delete th;
  }
  threadPool.clear();
  delete sem;
}

template <typename T> void quicksort(T* a, unsigned begin, unsigned size, int threadCount)
{
  printArray(a, size);
  sem = new Semaphore(threadCount);
  threadPool.reserve(size);
  auto root = launchThread(a, begin, size);
  root->join();
  printArray(a, size);
  cleanupThreads();
}
