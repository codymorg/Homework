#include <iostream>
#include <vector>
#include <algorithm>

using std::vector;
using std::cout;
using std::for_each;

#define BRANCHING_FACTOR 2
#define FIRST_INDEX 0

template<typename T>
bool Add(vector<T> const& set, vector<T> sub, int index)
{
  if (index < int(set.size()))
  {
    sub.push_back(set[index]);
    Add(set, sub, index + 1);

    int indexR = index;
    while (indexR < int(set.size()))
    {
      Add(set, sub, indexR + 2);
      indexR++;
    }

    //print that shit
    cout << "{ ";
    for_each(sub.begin(), sub.end(), [&](T elem) {cout << elem << " "; });
    cout << "}\n";
      
    // this is a valid index
    return true;
  }

  //not a valid index
  else
  {
    //cout << index << " not valid\n";
    return false;
  }
}

// actual recursive function
template< typename T >
void subset_rec(vector<T> const& set, int index ) 
{
  vector<T> sub;

  // explore the left side fo the tree
  Add(set, sub, index + 1);

  //all done tree
  if (index >= int(set.size()) - BRANCHING_FACTOR)
  {
    // dont forget the emptry set
    cout << "{ }\n";

    return;
  }
  else
  {
    //explore the next major branch of the tree
    subset_rec(set, index + 1);
  }
}



// kickstart
template< typename T >
void subsets(vector<T> const& set)
{
  // need to start one before the beginning of the array
  subset_rec(set, FIRST_INDEX - 1);
}
