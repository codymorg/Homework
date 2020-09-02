#include "perm-jt.h"
#include <algorithm>
#include <cstdio>

PermJohnsonTrotter::PermJohnsonTrotter(int size) : current_(),direction_()
{
  int i = 1;
  while (size--)
  {
    current_.push_back(i++);
    direction_.push_back(-1);
  }
}

bool PermJohnsonTrotter::Next()
{
  int maxIndex = 0;
  std::vector<int> touched;
  do
  {
    int max = 0;

    // find largest 
    for (unsigned i = 0; i < current_.size(); i++)
    {
      if (max < current_[i] && !vectorContains(touched, current_[i]))
      {
        max = current_[i];
        maxIndex = i;
      }
    }

    // keep track of attempted values
    touched.push_back(max);

    if (touched.size() >= current_.size())
      break;
  } while (isStuck(maxIndex)); // if cant change its direction and repeat

  // swap in its direction
  swap(maxIndex);

  return isNext();
}

std::vector<int> const& PermJohnsonTrotter::Get() const
{
  return current_;
}

// find out if this value can leglly be swapped (true) and changes its direciton (false)
bool PermJohnsonTrotter::isStuck(int largestIndex)
{
  bool stuck = false;
  if (largestIndex + direction_[largestIndex] < 0 || 
      largestIndex + direction_[largestIndex] >= int(current_.size()) || 
      current_[largestIndex] < current_[largestIndex + direction_[largestIndex]])
  {
    stuck = true;
    direction_[largestIndex] *= -1;
  }

  return stuck;
}

void PermJohnsonTrotter::swap(int index)
{
  int dir = direction_[index];
  int temp = current_[index];
  current_[index] = current_[index + dir];
  current_[index + dir] = temp;

  // dont forget to move the direction with the value
  temp = direction_[index];
  direction_[index] = direction_[index + dir];
  direction_[index + dir] = temp;
}

bool PermJohnsonTrotter::isNext()
{
  bool isNext = true;
  unsigned count = 0;
  for (size_t i = current_.size()-1; i > 0; i--)
  {
    if (current_[i] == current_[i - 1] + 1)
    {
      count++;
    }
  }
  if (count == current_.size()-1)
    isNext = false;

  return isNext;
}

bool PermJohnsonTrotter::vectorContains(std::vector<int> const& vector, int value)
{
  bool doesContain = false;
  for (unsigned i = 0; i < vector.size(); i++)
  {
    int v = vector[i];
    if (v == value)
    {
      doesContain = true;
      break;
    }
  }

  return doesContain;
}
