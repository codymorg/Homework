#include "semaphore.h"

Semaphore::Semaphore(int threadCount) : allowedThreads(threadCount)
{
}
void Semaphore::wait()
{
  if (threadsIn >= allowedThreads)
  {
    std::unique_lock<std::mutex> waitForSignal(lock);
    cond.wait(waitForSignal, [&] { return threadsIn < allowedThreads; });
  }
  else
    threadsIn++;
}

void Semaphore::signal()
{
  cond.notify_one();
  threadsIn--;
}
