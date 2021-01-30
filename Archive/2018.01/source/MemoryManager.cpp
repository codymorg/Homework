/******************************************************************************/
/*!
\file   MemoryManager.cpp
\author Seth Kohler
\par    email: seth.kohler\@digipen.edu

\date   09/17/2018
\brief
    Memory Manager implementation

*/
/******************************************************************************/

#include "MemoryManager.h"
#include <cassert>
#include <new>

MemoryManager::MemoryManager()
{
  m_hMemoryManagerInstance = this; //todo: Singleton while not asleep
}

MemoryManager::~MemoryManager()
{
}

void* MemoryManager::StackAlloc(size_t size)
{
  return HeapAlloc(size);
}

void* MemoryManager::HeapAlloc(size_t size)
{
  try
  {
    char* allocation = new char[size];

    return reinterpret_cast<void *>(allocation);

  } catch (std::bad_alloc& e)
  {
    throw e;
  }
}

void* MemoryManager::FrameAlloc(size_t size)
{
  return HeapAlloc(size);
}

void* MemoryManager::BufferedFrameAlloc(size_t size)
{
  return HeapAlloc(size);
}

void MemoryManager::Free(void * object)
{
  delete[] reinterpret_cast<char *>(object);
}

MemoryManager* MemoryManager::GetInstance() const
{
  return m_hMemoryManagerInstance;
}
