#ifndef QUICKSORT
#define QUICKSORT
#pragma once
#include <assert.h>
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using std::atomic_int;
using std::mutex;
using std::thread;
using std::vector;

template <typename T> void quicksort_rec(T* a, unsigned begin, unsigned end);
template <typename T> void quicksort(T* a, unsigned, unsigned, int);

#include "sort_small_arrays.h"
#include <iostream>

template <typename T> unsigned partition(T* a, unsigned begin, unsigned end)
{
  unsigned i = begin, last = end - 1;
  T        pivot = a[last];

  for (unsigned j = begin; j < last; ++j)
  {
    if (a[j] < pivot)
    {
      std::swap(a[j], a[i]);
      ++i;
    }
  }
  std::swap(a[i], a[last]);
  return i;
}

template <typename T> unsigned partition_new(T* a, unsigned begin, unsigned end)
{
  if (end - begin > 8)
    return partition_old(a, begin, end);

  unsigned i = begin, last = end - 1, step = (end - begin) / 4;

  T* pivots[5] = {a + begin, a + begin + step, a + begin + 2 * step, a + begin + 3 * step, a + last};
  quicksort_base_5_pointers(pivots);

  std::swap(a[last], a[begin + 2 * step]);
  T pivot = a[last];

  for (unsigned j = begin; j < last; ++j)
  {
    if (a[j] < pivot /*|| a[j]==pivot*/)
    {
      std::swap(a[j], a[i]);
      ++i;
    }
  }
  std::swap(a[i], a[last]);
  return i;
}

/* recursive */
template <typename T> void quicksort_rec(T* a, unsigned begin, unsigned end)
{
  if (end - begin < 6)
  {
    switch (end - begin)
    {
    case 5:
      quicksort_base_5(a + begin);
      break;
    case 4:
      quicksort_base_4(a + begin);
      break;
    case 3:
      quicksort_base_3(a + begin);
      break;
    case 2:
      quicksort_base_2(a + begin);
      break;
    }
    return;
  }

  unsigned q = partition(a, begin, end);

  quicksort_rec(a, begin, q);
  quicksort_rec(a, q, end);
}

/* iterative */
#define STACK
#define xVECTOR
#define xPRIORITY_QUEUE

#include <utility> // std::pair

template <typename T> using triple = typename std::pair<T*, std::pair<unsigned, unsigned>>;

template <typename T> struct compare_triples
{
  bool operator()(triple<T> const& op1, triple<T> const& op2) const
  {
    return op1.second.first > op2.second.first;
  }
};

#ifdef STACK
#include <stack>
template <typename T> using Container = std::stack<triple<T>>;
#define PUSH push
#define TOP top
#define POP pop
#define EMPTY empty

#endif

#ifdef VECTOR
#include <vector>
template <typename T> using Container = std::vector<triple<T>>;
#define PUSH push_back
#define TOP back
#define POP pop_back
#define EMPTY empty

#endif

#ifdef PRIORITY_QUEUE
#include <queue>
template <typename T> using Container = std::priority_queue<triple<T>, std::vector<triple<T>>, compare_triples<T>>;
#define PUSH push
#define TOP top
#define POP pop
#define EMPTY empty

#endif

template <typename T> void quicksort_iterative_aux(Container<T>& ranges);

template <typename T> void swapPivot(T* a, int begin, int pivotIndex)
{
  int i = pivotIndex - 1;
  while (a[i] >= a[pivotIndex] && i >= begin)
  {
    std::swap(a[i], a[pivotIndex]);
    i--;
    pivotIndex--;
  }
}

static vector<thread*> pool;
static mutex           lock;
static int             unsorted;

static void joinThreads()
{
  for (auto& th : pool)
  {
    th->join();
    delete th;
    th = nullptr;
#ifdef _DEBUG
    std::cout << "thread " << std::this_thread::get_id() << " joining\n";
#endif
  }
  pool.clear();
}

template <typename T> void CreateThreadPool(Container<T>& ranges, int threads);

template <typename T> void quicksort_iterative(T* a, unsigned begin, unsigned end, int threads)
{
  Container<T> threadJobs;
  threadJobs.PUSH(std::make_pair(a, std::make_pair(begin, end)));
  CreateThreadPool(threadJobs, threads);
  joinThreads();

  // quicksort_iterative_aux(threadJobs);
  swapPivot(a, begin, end);
}

template <typename T> void quicksort_iterative_aux(Container<T>& ranges)
{
#ifdef _DEBUG
  std::cout << "thread " << std::this_thread::get_id() << " starting " << unsorted << "\n";
#endif

  lock.lock();
  if (ranges.empty())
  {
#ifdef _DEBUG
    std::cout << "thread " << std::this_thread::get_id() << " failed to lock\n";
#endif
    lock.unlock();
    return;
  }
  triple<T> r = ranges.TOP();
#ifdef _DEBUG
  std::cout << "thread " << std::this_thread::get_id() << " takes   [" << r.second.first << "," << r.second.second
            << "]\n";
#endif
  ranges.POP();
  T*       a = r.first;
  unsigned b = r.second.first;
  unsigned e = r.second.second;
  if (e - b < 6)
    unsorted -= e - b;
  lock.unlock();

  // base case
  if (e - b < 6)
  {
#ifdef _DEBUG
    std::cout << "thread " << std::this_thread::get_id() << " solving [" << r.second.first << "," << r.second.second
              << "] status: " << unsorted << "\n";
#endif
    switch (e - b)
    {
    case 5:
      quicksort_base_5(a + b);
      break;
    case 4:
      quicksort_base_4(a + b);
      break;
    case 3:
      quicksort_base_3(a + b);
      break;
    case 2:
      quicksort_base_2(a + b);
      break;
    }
    return;
  }
  unsigned q = partition(a, b, e);

  lock.lock();
  unsorted--;
  ranges.PUSH(std::make_pair(a, std::make_pair(b, q)));
  ranges.PUSH(std::make_pair(a, std::make_pair(q + 1, e)));
  lock.unlock();

#ifdef _DEBUG
  std::cout << "thread " << std::this_thread::get_id() << " done status: " << unsorted << "\n";
#endif
}
template <typename T> void ThreadWaitingArea(Container<T>& ranges)
{
  do
  {
    quicksort_iterative_aux(ranges);
    std::this_thread::yield();
#ifdef _DEBUG
    std::cout << "thread " << std::this_thread::get_id() << " WAITING :::::\n";
#endif
    // std::cout << unsorted << '\n';
  } while (unsorted);
}

template <typename T> void CreateThreadPool(Container<T>& ranges, int threads)
{
  for (int i = 0; i < threads; i++)
  {
    pool.push_back(new thread(ThreadWaitingArea<T>, std::ref(ranges)));
  }
}

template <typename T> void quicksort(T* a, unsigned begin, unsigned size, int threads)
{
  unsorted = size - 1;
  quicksort_iterative(a, begin, size - 1, threads);
}
#endif
