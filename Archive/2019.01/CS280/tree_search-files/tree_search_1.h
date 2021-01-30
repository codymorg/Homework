#ifndef TREE_SEARCH_1_H
#define TREE_SEARCH_1_H
#include <iostream>
#include <vector>
#include <deque>

#define COUT if(false)std::cout

using std::deque;

template <typename T>
struct TNode
{
    T data = T();
    std::vector< TNode<T>* > children = {};
};

// recursive implementation of DFS
template <typename T>
TNode<T> const*        // returns first found node that is equal to val
tree_depth_first_search_rec( TNode<T> const* root, T const& val )
{
  COUT << " has " << root->data << "\n";
  
  // we got a match!
  if(root->data == val)
  {
    return root;
  }

  
  // dig deep
  for(unsigned i = 0; i < root->children.size(); i++)
  {
    COUT << "child " << i;

    const TNode<T>* found = tree_depth_first_search_rec(root->children[i], val);

    // found it!
    if (found)
      return found;
  }
  
  // no math found
  return nullptr;
}

// iterative implementation of DFS
// use stack (push,top,pop) or vector(push_back,back,pop_back)
// when inserting children, make sure they will be dicovered in the same
// order as in TNode's array children
template <typename T>
TNode<T> const*        // returns first found node that is equal to val
tree_depth_first_search_iter( TNode<T> const* root, T const& val )
{
  return tree_depth_first_search_rec(root, val);
}



// iterative implementation of BFS
// simple change from tree_depth_first_search_iter (previous)
// change container type to deque
template <typename T>
TNode<T> const*        // returns first found node that is equal to val
tree_breadth_first_search_iter( TNode<T> const* root, T const& val )
{
  deque<TNode<T> const*> kids;
  kids.push_back(root);

  while (kids.size() > 0) 
  {
    TNode<T> const* popped = kids[0];
    kids.pop_front();
    if (popped->data == val) 
      return popped; // success
    for (unsigned i = 0; i < popped->children.size(); i++)
    {
      kids.push_back(popped->children[i]);
    }
  }
    return nullptr; // failed
}

#endif
