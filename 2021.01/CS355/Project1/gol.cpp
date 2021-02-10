#include "gol.h"
#include <iterator>
#include <pthread.h>

#ifdef _WIN32
#pragma comment(lib, "pthreadVC2.lib")
#pragma warning(disable : 4996)
#else
#include <unistd.h>
#endif

using std::vector;
typedef std::tuple<int, int> Coord;

static vector<Coord>     nextGen;
static vector<pthread_t> threads;
pthread_mutex_t          nextGenLock;

class Barrier
{
public:
  Barrier(int totalAllowed) : waituntil(totalAllowed)
  {
    int error = pthread_mutex_init(&gate, NULL);
    error += pthread_mutex_init(&exit, NULL);
    pthread_cond_init(&releaseGate, NULL);
    pthread_cond_init(&releaseExit, NULL);
    current = waituntil;
    if (error)
      throw error;
  };

  ~Barrier()
  {
    pthread_mutex_destroy(&gate);
    pthread_mutex_destroy(&exit);
  }

  void skipThread()
  {
    pthread_mutex_lock(&gate); // lock the gate
    current++;
    printf("skipping to %i\n", current);
    pthread_mutex_unlock(&gate); //
  }

  int getCurrentWaiting()
  {
    return current;
  }

  void holdMaster()
  {
    if (allowOut)
    {
      allowOut = false;
      return;
    }
    pthread_mutex_lock(&exit);
    pthread_cond_wait(&releaseExit, &exit);
    pthread_mutex_unlock(&exit);
    allowOut = false;
  }

  void hold()
  {
    pthread_mutex_lock(&gate);                    // lock the gate
                                                  // 
    bool thisCycle = cycle;                       // we're in this iteration
                                                  // 
    if (--current == 0)                           // count the threads as they enter
    {                                             // this is the last thread
      cycle   = !cycle;                           // so we're on a new iteration
      current = waituntil;                        // reset count
      pthread_cond_broadcast(&releaseGate);       // release the waiting threads
    }                                             // 
    else                                          // 
    {                                             // 
      while (thisCycle == cycle)                  // cycle until broken 
      {                                           //
        if (current == 1)                         // the master thread needs released
        {                                         //
          allowOut = true;                        // allow the master thread out if it isn't there yet
          pthread_cond_signal(&releaseExit);      // release master thread if it is waiting 
        }                                         //
        if(pthread_cond_wait(&releaseGate, &gate))// wait until released
          break;                                  // 
      }                                           // 
    }                                             // 
    pthread_mutex_unlock(&gate);                  // unlock the gate allowing next iteration in
    allowOut = false;
  }

private:
  pthread_mutex_t gate;
  pthread_cond_t  releaseGate;
  pthread_mutex_t exit;
  pthread_cond_t  releaseExit;
  int             waituntil = 0;
  int             current   = 0;
  bool            cycle     = false;
  bool            allowOut  = false;
};

Barrier* waitToExit  = nullptr;

struct ThreadData
{
  ThreadData(vector<Coord>& pop, int x, int y, int iter, int threadTotal, int number)
    : population(pop), coord(x, y), iterations(iter), threadCount(threadTotal), threadID(number){};

  vector<Coord>& population;
  const Coord    coord;
  int            iterations  = 0;
  int            threadID    = 0;
  int            threadCount = 0;
  bool           isMaster    = false;
};

static bool IsNeighbor(Coord me, Coord you)
{
  if (me == you)
    return false;

  int  mx = std::get<0>(me);
  int  my = std::get<1>(me);
  int  yx = std::get<0>(you);
  int  yy = std::get<1>(you);
  bool n  = std::abs(mx - yx) <= 1 && std::abs(my - yy) <= 1;
  // printf("%i %i <===> %i %i    [%s]\n", mx, my, yx, yy, n?"True":"False");
  return n;
}

static bool IsAlive(const vector<Coord>& population, const Coord& coord)
{
  for (const auto& c : population)
  {
    if (c == coord)
      return true;
  }

  return false;
}

static void Copy(vector<Coord>& pop)
{
  pop.clear();
  std::copy(nextGen.begin(), nextGen.end(), std::back_inserter(pop));
  nextGen.clear();
}

static bool GOL_Calc(vector<Coord>& population, Coord coord)
{
  bool alive = IsAlive(population, coord);

  // count cell neighbors
  int nCount = 0;
  for (auto neigh : population)
  {
    if (IsNeighbor(coord, neigh))
    {
      nCount++;
    }
  }

  // if cell will live
  if ((alive && (nCount == 2 || nCount == 3)) || (!alive && nCount == 3))
  {
    pthread_mutex_lock(&nextGenLock);
    nextGen.push_back(coord);
    pthread_mutex_unlock(&nextGenLock);
    return true;
  }

  return false;
}

void* Multithread(void* data)
{
  // determine if cell is alive
  ThreadData* threadData = (ThreadData*)data;

  for (int i = 0; i < threadData->iterations; i++)
  {
#ifdef _DEBUG
    printf("thread %i starting iteration %i\n", threadData->threadID, i);
#endif _DEBUG
    GOL_Calc(threadData->population, threadData->coord);

    // only one thread is needed to copy data to population
    if (threadData->isMaster)
    {
      waitToExit->holdMaster();
#ifdef _DEBUG
      printf("copying gen #%i data. threads waiting %i\n", i, waitToExit->getCurrentWaiting());
      for (auto e : nextGen)
      {
        printf("%i %i\n", std::get<0>(e), std::get<1>(e));
      }
#endif // DEBUG
      Copy(threadData->population);
    }
    waitToExit->hold();
  }

  delete (threadData);

  return nullptr;
}

static void CreateThreads(vector<Coord>& population, int x, int y, int iter)
{
  threads.resize(x * y);
  for (int i = 0; i < x * y; i++)
  {
    auto data = new ThreadData(population, i % x, i / y, iter, x * y, i);
    if (i == 0)
      data->isMaster = true;
#ifdef _DEBUG
    printf("thread %i created at (%i %i)\n", i, std::get<0>(data->coord), std::get<1>(data->coord));
#endif // DEBUG
    if (pthread_create(&threads[0], nullptr, Multithread, data))
    {
      printf("thread failed to create\n");
      throw -1;
    }
  }
}

static void CreateFlags(int threadCount)
{
  pthread_mutex_init(&nextGenLock, NULL);
  waitToExit  = new Barrier(threadCount);
}

static void Cleanup()
{
  for (auto& thread : threads)
  {
    pthread_join(thread, 0);
  }
}

vector<Coord> run(vector<Coord> pop, int iter, int x, int y)
{
  CreateFlags(x * y);
  CreateThreads(pop, x, y, iter);

  Cleanup();

  return pop;
}
