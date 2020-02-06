#include "perm-lexicographical.h"
#include <algorithm>
#include <iostream>

bool nextPermLexicographical (std::vector<int>& stream) 
{
  // find largest right most character
  int pivotA = -1;
  for (size_t i = 0; i < stream.size() - 1; i++)
  {
    if (stream[i] < stream[i + 1])
      pivotA = i;
  }

  // you are already in reverse order -we're done
  if (pivotA == -1)
    return false;

  // find smallest char larger than pivot to the right
  int pivotB = -1;
  for (size_t i = pivotA + 1; i < stream.size(); i++)
  {
    if (pivotB == -1 || (stream[i] > stream[pivotA] && stream[i] < stream[pivotB]))
      pivotB = i;

  }

  // swap pivots
  int swap = stream[pivotA];
  stream[pivotA] = stream[pivotB];
  stream[pivotB] = swap;

  // sort characters to the right of A
  std::sort(stream.begin() + pivotA + 1, stream.end(), std::less<int>());

  return true;
}
