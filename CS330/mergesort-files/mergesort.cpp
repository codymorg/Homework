/*
 * Name : Cody Morgan
 * Class: CS 330 Algorithms
 * Assn : MergeSort
 * Data : 16 MAR 2020
*/


#include "mergesort.h"
#include <vector>
using std::vector;
static vector<int>leftHalf;
static vector<int>rightHalf;
static int size = -1;
void merge(int* data, int left, int middle, int right)
{
  // block out some memory
  if(leftHalf.capacity() == 0)
  {
    leftHalf.reserve(size);
    rightHalf.reserve(size);
  }
  else
  {
  leftHalf.clear();
  rightHalf.clear();
  }

  int leftPivot = middle - left + 1;
  int rightPivot = right - middle;

  // copy data
  for (auto i = 0; i < leftPivot; i++)
  { 
    if (left + i < size)
      leftHalf.push_back(data[left + i]);
  }
  for (auto i = 0; i < rightPivot; i++)
  {
    if(middle + i + 1 < size)
     rightHalf.push_back(data[middle + i + 1]);
  }

  int i = 0;
  int j = 0;
  int k = left;

  // merge data
  while(i < leftPivot && j < rightPivot && j < rightHalf.size())
  {
    if(leftHalf[i] <= rightHalf[j])
    {
      data[k] = leftHalf[i++];
    }
    else
    {
      data[k] = rightHalf[j++];
    }
    k++;
  }

  while(i < leftPivot)
  {
    data[k++] = leftHalf[i++];
  }
  while(j < rightPivot && j < rightHalf.size())
  {
    data[k++] = rightHalf[j++];
  }
}

void mergesort(int* data, unsigned left, unsigned right)
{
  if (right > left)
  {
    // find middle
    int middle = (left + right) / 2;
    if(size == -1)
      size = right;

    // recursive on first half
    mergesort(data, left, middle);

    // recursive on last half
    mergesort(data, middle + 1, right);

    // merge both together
    merge(data, left, middle, right);
  }
}