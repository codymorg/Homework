/******************************************************************************/
/*!
\file   ObjectAllocator.c
\author Cody Morgan
\par    email: cody.morgan\@digipen.edu
\par    DigiPen login: cody.morgan
\par    Course: CS280
\par    Section A
\par    Assignment #1
\date   01 JAN 2019
\brief
  memory manager using  a linked list


*/
/******************************************************************************/

#include "ObjectAllocator.h" // objectAllocator, stats, config
#include <cstring>            // memset

using std::memset;

/******************************************************************************
* brief : Creates a custom memory manager for objects of uniform size
* 
* input : objectSize - the uniform size of the objects to be allocated
*         config     - how the memory manager will be configured
******************************************************************************/
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
{
  config_ = config;

  //size of just the memory handed to client (includes the size of the next pointer)
  stats_.ObjectSize_ = ObjectSize;

  //size of an entire block associated with each memory address
  blockSize =
    config_.HBlockInfo_.size_ +
    (config_.PadBytes_ * 2) +
    stats_.ObjectSize_ +
    config_.InterAlignSize_;

  // I stuffed all the code to calc alignment in the stamper it was lookign messy here
  if(config_.Alignment_)
    stats_.PageSize_ = stampAlignment(); // alignment if any  

  //size of the whole page
  stats_.PageSize_ +=
    sizeof(pageList_) +                                     // page pointer
    (config_.HBlockInfo_.size_ * config_.ObjectsPerPage_) + // header if any
    (stats_.ObjectSize_ * config_.ObjectsPerPage_) +        // all objects
    (config_.PadBytes_ * config_.ObjectsPerPage_ * 2);      // all padding


  allocate_new_page();
}

// Destroys the ObjectManager (never throws)
ObjectAllocator::~ObjectAllocator()
{
  //free all the pages we've made
  while (pageList_)
  {
    //we need to check for any headers that may have been allocated and not freed
    if (config_.HBlockInfo_.type_ == config_.hbExternal)
    {

      while (freeList_)
      {
        updateExternalHeader(nullptr, false, freeList_);
        freeList_ = freeList_->Next;
      }
    }

    // delete the rest of the pages
    char* deleteMe = reinterpret_cast<char*>(pageList_);
    pageList_ = pageList_->Next;
    delete[] deleteMe;
  }
}

/******************************************************************************
* brief  : allocate memory for an object with an (optional) label
*
* input  : label - used for debugging
*
* output : pointer to the allocated memory
******************************************************************************/
void* ObjectAllocator::Allocate(const char* label)
{
  //if this is on for some reason
  if (config_.UseCPPMemManager_)
  {
    stats_.Allocations_++;
    stats_.ObjectsInUse_++;
    if (stats_.ObjectsInUse_ > stats_.MostObjects_)
      stats_.MostObjects_++;

    return new void*[stats_.ObjectSize_];
  }

  //this page is done - cut a new one
  if (freeList_ == nullptr)
  {
    if (config_.MaxPages_ == stats_.PagesInUse_)
      throw OAException(OAException::E_NO_PAGES, "Il n'y a pas une page");
    else
      allocate_new_page();
  }

  //reconnect free list
  void* returnMemory = freeList_;
  freeList_ = freeList_->Next;
  memset(returnMemory, ALLOCATED_PATTERN, stats_.ObjectSize_);

  //update stats
  stats_.ObjectsInUse_++;
  stats_.FreeObjects_--;
  stats_.Allocations_++;
  if (stats_.ObjectsInUse_ > stats_.MostObjects_)
    stats_.MostObjects_++;

  //update header stamp
  stampHeader(reinterpret_cast<GenericObject*>(returnMemory), stats_.Allocations_, true, label);

  return returnMemory;
}

/******************************************************************************
* brief  : stamps memory with padding bytes 0xDD defined by the client
******************************************************************************/
void ObjectAllocator::stampPadding()
{
  //get the location for both pads
  char* objStart = 
    reinterpret_cast<char*>(pageList_) + 
    sizeof(pageList_) + 
    config_.LeftAlignSize_ + 
    config_.HBlockInfo_.size_;

  char* objEnd = objStart + config_.PadBytes_ + stats_.ObjectSize_;

  // stamp em
  for (unsigned i = 0; i < config_.ObjectsPerPage_; i++)
  {
    memset(objStart, PAD_PATTERN, config_.PadBytes_);
    memset(objEnd, PAD_PATTERN, config_.PadBytes_);

    objStart += blockSize;
    objEnd += blockSize;
  }
}

/******************************************************************************
* brief : updates header with data used for debugging
*
* input : header     - update this header
*         writeCount - number of times this memory has been written  
*         inUse      - is this memory block in use
*         label      - used for debugging
******************************************************************************/
void ObjectAllocator::stampHeader(GenericObject* header, int writeCount, bool inUse, const char* label)
{
  //no header short circuit
  if (config_.HBlockInfo_.type_ == config_.hbNone)
    return;

  //external short circuit
  if (config_.HBlockInfo_.type_ == config_.hbExternal)
  {
    updateExternalHeader(label, inUse, header);
    return;
  }

  //standard stamping
  if (header)
  {
    //extended stamping
    if (config_.HBlockInfo_.type_ == config_.hbExtended)
      stampExtendedHeader(header, inUse);
    else
    {
      char* objStart = reinterpret_cast<char*>(header) - config_.PadBytes_ - config_.HBlockInfo_.size_;
      *reinterpret_cast<int*>(objStart) = writeCount;
      objStart += config_.HBlockInfo_.size_ - 1;
      *objStart = inUse;
    }
  }

  //stamp all headers with 0
  else
  {
    char* objStart = reinterpret_cast<char*>(pageList_) + sizeof(pageList_) + config_.LeftAlignSize_;

    for (unsigned i = 0; i < config_.ObjectsPerPage_; i++)
    {
      memset(objStart, '\0', config_.HBlockInfo_.size_);

      objStart += blockSize;
    }
  }
}

/******************************************************************************
* brief  : update an extended header with additional information
*
* input  : genericNode - update this node
*          inUse - this block is in use
******************************************************************************/
void ObjectAllocator::stampExtendedHeader(GenericObject* genericNode, bool inUse)
{
  // stamp the header with extended information
  char* header = reinterpret_cast<char*>(genericNode);
  header -= (config_.HBlockInfo_.size_ + config_.PadBytes_);

  // access count 
  header += config_.HBlockInfo_.additional_;
  if (inUse)
  {
    unsigned short accessCount = static_cast<unsigned short>(*header);
    *reinterpret_cast<unsigned short*>(header) = ++accessCount;
  }

  // alloc count
  header += sizeof(short);
  if (inUse)
  {
    *reinterpret_cast<unsigned int*>(header) = stats_.Allocations_;
  }
  else
  {
    *header = static_cast<short>(0);
  }

  // in use flag
  header += sizeof(stats_.Allocations_);
  *header = inUse;
}

unsigned ObjectAllocator::stampAlignment()
{
  //total alignment bytes needed for this page
  unsigned alignment = 0;

  //count out what you're going to stamp
  if (stats_.PageSize_ == 0)
  {
    //setup the first place for alignment
    alignment = (1 + config_.Alignment_) - sizeof(pageList_);
    config_.LeftAlignSize_ = alignment;

    // calc the rest of the object alignment sizes
    unsigned firstObject = alignment + sizeof(pageList_) + blockSize; 
    unsigned objectAlignment = 1 + (config_.Alignment_ - (firstObject % config_.Alignment_));
    config_.InterAlignSize_ = objectAlignment;
    alignment += objectAlignment * (config_.ObjectsPerPage_ - 1);
  }
  else if (config_.Alignment_)
  {
    //stamp the first alignment
    unsigned char* current = reinterpret_cast<unsigned char*>(pageList_) + sizeof(pageList_);
    memset(current, ALIGN_PATTERN, config_.LeftAlignSize_);

    //stamp the rest
    current += config_.LeftAlignSize_ + blockSize - config_.InterAlignSize_;
    for (size_t i = 0; i < config_.ObjectsPerPage_ - 1; i++)
    {
      memset(current, ALIGN_PATTERN, config_.InterAlignSize_);
      current += blockSize;
    }
  }

  return alignment;
}

/******************************************************************************
* brief  : updates an external header
*
* input  : label  - for debugging 
*          inUse  - this block is in use
*          update - update this header
******************************************************************************/
void ObjectAllocator::updateExternalHeader(const char* label, bool inUse, GenericObject* update)
{
  //allocate new headers for the page 
  if (!update)
  {
    GenericObject* current = pageList_;
    char* memlocation = reinterpret_cast<char*>(current) + sizeof(current);

    for (size_t i = 0; i < config_.ObjectsPerPage_; i++)
    {
      MemBlockInfo** headerP = reinterpret_cast<MemBlockInfo**>(memlocation);
      MemBlockInfo* newBlock = new MemBlockInfo();
      newBlock->label = const_cast<char*>(label);
      *headerP = newBlock;
      memlocation += blockSize;
    }
  }

  //just update a header
  else
  {
    char* memlocation = reinterpret_cast<char*>(update) - config_.PadBytes_ - config_.HBlockInfo_.size_;

    MemBlockInfo** header = reinterpret_cast<MemBlockInfo**>(memlocation);

    //bail if there's no header (probably freed)
    if (*header)
    {
      //update header block as needed
      (*header)->in_use = inUse;
      (*header)->label = const_cast<char*>(label);
      if (inUse)
      {
        (*header)->alloc_num = stats_.Allocations_;
      }
      else
      {
        (*header)->alloc_num = 0;
        delete *header;
        *header = nullptr;
      }
    }
  }
}

/******************************************************************************
* brief  : conect freeList_ to all free nodes
******************************************************************************/
void ObjectAllocator::connectFreePages()
{
  char* memLocation =
    reinterpret_cast<char*>(pageList_) +
    sizeof(pageList_) +
    config_.LeftAlignSize_ +
    (blockSize * (config_.ObjectsPerPage_ - 1)) + // all objects except for the last one 
    config_.PadBytes_ +                           // all partials before next pointer
    config_.HBlockInfo_.size_;

  freeList_ = reinterpret_cast<GenericObject*>(memLocation);
  GenericObject* current = freeList_;

  //page pad genobj pad AA pad...
  for (unsigned i = 0; i < config_.ObjectsPerPage_ - 1; i++)
  {
    memLocation = reinterpret_cast<char*>(current) - blockSize;
    current->Next = reinterpret_cast<GenericObject*>(memLocation);
    current = current->Next;
  }
  current->Next = nullptr;

}

/******************************************************************************
* brief  : allocate a new page when needed
******************************************************************************/
void ObjectAllocator::allocate_new_page()
{
  //make a new page
  char* pageStart = new char[stats_.PageSize_];
  memset(pageStart, UNALLOCATED_PATTERN, stats_.PageSize_);

  //connect pagelist
  GenericObject* newPage = reinterpret_cast<GenericObject*>(pageStart);
  if (pageList_ == nullptr)
  {
    pageList_ = newPage;
    pageList_->Next = nullptr;
  }
  else
  {
    newPage->Next = pageList_;
    pageList_ = newPage;
  }

  //connect all free blocks
  connectFreePages();

  //stats
  stats_.PagesInUse_++;
  stats_.FreeObjects_ += config_.ObjectsPerPage_;

  //stamping
  stampPadding();
  stampHeader();
  stampAlignment();
}

/******************************************************************************
* brief  : return the configuration for this memory allocator
*
* output : config_ contains much of the config data for the allocator
******************************************************************************/
OAConfig ObjectAllocator::GetConfig(void) const
{
  return config_;
}

/******************************************************************************
* brief  : did I do the extra credit? no
*
* output : false - no extra credit for me
           true  - extra credit yeah!
******************************************************************************/
bool ObjectAllocator::ImplementedExtraCredit(void)
{
  return true;
}

/******************************************************************************
* brief  : check latest node for corruption 
******************************************************************************/
void ObjectAllocator::checkCorruption() const
{
  if (!config_.DebugOn_)
    return;

  //scan this freed node for corruption
  unsigned char* memLocation = reinterpret_cast<unsigned char*>(freeList_) - config_.PadBytes_;
  int padCount = 2;
  while (padCount--)
  {
    // scan left then right pads
    for (size_t i = 0; i < config_.PadBytes_; i++)
    {
      if (memLocation[i] != PAD_PATTERN)
        throw OAException(OAException::E_CORRUPTED_BLOCK, "corruption is among us");
    }
    memLocation += stats_.ObjectSize_ + config_.PadBytes_;
  }
}

/******************************************************************************
* brief  : add a recently freed object to the freeList list
*
* input  : this is the object to add
******************************************************************************/
void ObjectAllocator::put_on_freelist(void* object)
{
  //find the page we're supposed to be on (this can't be done in constant time)
  unsigned char* pageAddress = reinterpret_cast<unsigned char*>(pageList_);
  unsigned char* objAddress = reinterpret_cast<unsigned char*>(object);
  GenericObject* currentPage = pageList_;

  //if the object is beyond the page or is before the page we need to look at the next page
  long right = objAddress - pageAddress;                     // should be positive
  long left = (pageAddress + stats_.PageSize_) - objAddress; //should be positive
  while (currentPage && (left <= 0 || right <= 0))
  {
    currentPage = currentPage->Next;
    pageAddress = reinterpret_cast<unsigned char*>(currentPage);
    right = objAddress - pageAddress;                     // should be positiv
    left = (pageAddress + stats_.PageSize_) - objAddress; //should be positive
  }

  //validate the page
  long invalid = objAddress - pageAddress;
  invalid -= config_.PadBytes_;
  invalid -= config_.HBlockInfo_.size_;
  invalid %= blockSize;
  invalid -= ( sizeof(pageList_) + config_.LeftAlignSize_);
  if (currentPage == nullptr || invalid)
  {
    throw OAException(OAException::E_BAD_BOUNDARY, "The memory address freed is invalid ");
  }

  //validate the node
  GenericObject* current = freeList_;
  while (config_.DebugOn_ && current)
  {
    unsigned char* nodeAddress = reinterpret_cast<unsigned char*>(current);

    if (nodeAddress == objAddress)
    {
      throw OAException(OAException::E_MULTIPLE_FREE, "you freed the same address twice");
    }
    current = current->Next;
  }


  //add the object to the freelist
  memset(objAddress, FREED_PATTERN, stats_.ObjectSize_);
  GenericObject* newNode = reinterpret_cast<GenericObject*>(object);
  if (freeList_ == nullptr)
  {
    freeList_ = newNode;
    freeList_->Next = nullptr;
  }
  else
  {
    newNode->Next = freeList_;
    freeList_ = newNode;
  }

  checkCorruption();
}

/******************************************************************************
* brief  : free this object (give the memory back to the memory manager)
*
* input  : this is the object to free
******************************************************************************/
void ObjectAllocator::Free(void* object)
{
  if (config_.UseCPPMemManager_)
  {
    delete[] reinterpret_cast<char*>(object);
    stats_.Deallocations_++;
    stats_.ObjectsInUse_--;
    return;
  }

  //check if address is valid
  put_on_freelist(object);

  stats_.Deallocations_++;
  stats_.FreeObjects_++;
  stats_.ObjectsInUse_--;

  //stamp header
  stampHeader(reinterpret_cast<GenericObject*>(object), '\0', false);
}

/******************************************************************************
* brief  : dump the memory in use
*
* input  : fn - dump memory using this funciton
*
* output : the number of leaks found
******************************************************************************/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
  unsigned leaks = 0;

  // scan every page
  GenericObject* page = pageList_;
  while (page)
  {
    // current node pointer
    char* current =
      reinterpret_cast<char*>(page) +
      sizeof(pageList_) +
      config_.LeftAlignSize_ +
      config_.HBlockInfo_.size_ +
      (2 * config_.PadBytes_);

    // scan the page
    while (current < reinterpret_cast<char*>(page) + stats_.PageSize_)
    {
      // scan the object
      for (size_t i = 0; i < stats_.ObjectSize_; i++)
      {
        //found an allocated block - dump it
        unsigned char test = *(current + i);
        if (test == ALLOCATED_PATTERN)
        {
          leaks++;
          fn(current, stats_.ObjectSize_);
          break;
        }
      }

      // next block
      current += blockSize;
    }

    page = page->Next;
  }

  // return number of dumps
  return leaks;
}

/******************************************************************************
* brief  : counts the nuimber of corruptions found
*
* input  : fn - page validation function
*
* output : the number of corruptions found
******************************************************************************/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
  unsigned corruption = 0;
  GenericObject* page = pageList_;

  // scan every page
  while (page)
  {
    unsigned char* current = reinterpret_cast<unsigned char*>(page) + sizeof(pageList_) + config_.LeftAlignSize_ + config_.HBlockInfo_.size_;

    // scan every pad in this page
    while (current < reinterpret_cast<unsigned char*>(page) + stats_.PageSize_)
    {
      bool foundCorruption = false;
      // scan every byte in the left pad
      for (size_t i = 0; i < config_.PadBytes_; i++)
      {
        // is this byte valid?
        if (current[i] != PAD_PATTERN)
        {
          fn(current, stats_.ObjectSize_);
          corruption++;
          foundCorruption = true;
          break;
        }
      }
      current += config_.PadBytes_ + stats_.ObjectSize_;

      // scan every byte in the right block
      for (size_t i = 0; i < config_.PadBytes_; i++)
      {
        if (foundCorruption)
          break;

        if (current[i] != PAD_PATTERN)
        {
          fn(current, stats_.ObjectSize_);
          corruption++;
          break;
        }
      }
      current += config_.PadBytes_ + config_.HBlockInfo_.size_;
    }
    page = page->Next;
  }

  return corruption;
}

/******************************************************************************
* brief  : extra credit function - frees empty pages
*
* output :
******************************************************************************/
unsigned ObjectAllocator::FreeEmptyPages(void)
{
  return 0;
}

/******************************************************************************
* brief  : returns the head of the pagelist
*
* output : the pagelist pointer
******************************************************************************/
const void* ObjectAllocator::GetPageList(void) const
{
  return reinterpret_cast<const void*>(pageList_);
}

/******************************************************************************
* brief  : return the stats associated with this memory manager
*
* output : stats block
******************************************************************************/
OAStats ObjectAllocator::GetStats(void) const
{
  return stats_;
}

