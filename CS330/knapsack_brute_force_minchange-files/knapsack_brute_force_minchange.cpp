#include "knapsack_brute_force_minchange.h"
using std::vector;
using std::string;
using std::pair;
#include <algorithm>
using std::reverse;
#include <iostream>
using std::cout;
#include <string>

#define DEBUG 0

GreyCode::GreyCode(int s) : master_()
{
  // 's' should be unsigned no?
  if (s <= 0)
    return;
  
  // create 2^(s-1) strings
  unsigned size = 2 << (s - 1);
  master_.resize(size);

  // write until 0 count runs out
  int fCount = size;
  int tCount = size;
  int currentFalse = fCount >> 1;
  int currentTrue = tCount;
  for (size_t i = 0; fCount > 1; i++)
  {
    // hit the end of this iteration
    if (i >= size)
    {
      fCount >>= 1;
      tCount >>= 1;
      currentFalse = fCount >> 1; // false starts with half its value
      currentTrue = tCount;

      i = -1;
    }

    // write zeros
    else if (currentFalse-- > 0)
    {
      master_[i].push_back(0);
    }

    // write ones
    else if(currentTrue-- > 0)
    {
      master_[i].push_back(1);
    }

    // reset pattern
    else if (i < size)
    {
      currentFalse = fCount;
      currentTrue = tCount;
      i--;
    }
  }

  currentIndex_ = 1;
}

////////////////////////////////////////////////////////////////////////////////
std::pair<bool, std::pair<bool, int>> GreyCode::Next() 
{
  int  pos = getPosModified();              // which position is modified 
  bool add = master_[currentIndex_++][master_[0].size() - pos - 1]; // is true if new value is 1 (add item), false otherwise 
  bool last = isLast();                     // is this the last permutation 
  
  pair<bool, int> data(add, pos);
  pair<bool, std::pair<bool, int>> isLast(last, data);

  return isLast;
}

std::vector<bool> GreyCode::getPermutation(int i)
{
  vector<bool>clone = master_[i];
  reverse(clone.begin(), clone.end());
  return clone;
}

void GreyCode::printMaster()
{
#if(DEBUG)
  cout << "== Start ==\n";
  for (size_t i = 0; i < master_.size(); i++)
  {
    for (size_t j = 0; j < master_[i].size(); j++)
    {
      cout << (int)master_[i][j];
    }
    cout << "\n";
  }
#endif
}

int GreyCode::getPosModified()
{
  int index = 0;
  for (int i = master_[currentIndex_].size() - 1; i >= 0 ; i--)
  {
    if (master_[currentIndex_][i] != master_[currentIndex_ - 1][i])
      break;
    else
      index++;
  }

  return index;
}

bool GreyCode::isLast()
{
  return unsigned(currentIndex_) < master_.size();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<bool> knapsack_brute_force( std::vector<Item> const& items, Weight const& W )
{
  // generate greycode
  GreyCode gray(items.size());
  pair<bool, pair<bool, int>> perm; // <are more? <is adding?, index>>
  perm.first = true;
  Sack haversack(W);
  int index = 1;

  // while there are permutations
  while (perm.first)
  {
    // add if we're adding - subtract otherwise
    perm = gray.Next();
    if (perm.second.first)
    {
      haversack.add(items[perm.second.second], index++);
    }
    else
    {
      haversack.remove(items[perm.second.second], index++);
    }
  }

  // return the permutation with the greatest value
  return gray.getPermutation(haversack.maxPermutation);

}

Sack::Sack(Weight const& maxWeight) : maxWeight_(maxWeight), currentWeight_(), filledWeight()
{}

void Sack::add(Item const& item, int perm)
{
  currentValue_ += item.GetValue();
  currentWeight_ += item.GetWeight();

  validate(perm);
}

void Sack::remove(Item const& item, int perm)
{
  currentValue_ -= item.GetValue();
  currentWeight_ -= item.GetWeight();

  validate(perm);
}

void Sack::validate(int perm)
{
  // check that this item isnt' too heavy
  if (currentValue_ > maxValue_ && currentWeight_ <= maxWeight_)
  {
    maxValue_ = currentValue_;
    filledWeight = currentWeight_;
    maxPermutation = perm;
  }
#if(DEBUG)
  printf("%i: sack: $%i #%i maxValue: $%i\n", perm, currentValue_, currentWeight_, maxValue_);
#endif
}
