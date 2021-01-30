#include <iostream>
#include <iomanip>      // setw
#include <sstream>      // std::stringstream
#include "wb_tree.h"
#include <stack>
#define DEBUG true
#define COUT if(DEBUG) std::cout

using std::stack;

// check the whole tree 
void check_weights( TreeNode * pRoot, float alpha ) 
// assume pRoot is not NULL
{
    // counts match?
    int total_count = pRoot->size;

    // balances
    if ( pRoot->left ) {
        if ( static_cast<float>( pRoot->left->size ) > alpha*pRoot->size ) {
            std::cout << "left subtree is too heavy at node " << pRoot->data << std::endl;
        }
        total_count -= pRoot->left->size;
        check_weights( pRoot->left, alpha );
    }
    if ( pRoot->right ) {
        if ( static_cast<float>( pRoot->right->size ) > alpha*pRoot->size ) {
            std::cout << "right subtree is too heavy at node " << pRoot->data << std::endl;
        }

        total_count -= pRoot->right->size;
        check_weights( pRoot->right, alpha );
    }

    if ( total_count != 1 ) {
        std::cout << "count mismatch at node " << pRoot->data << "\n";
    }
}

void print_inorder( TreeNode const* pRoot ) {
    if ( pRoot == nullptr ) { return; }

    print_inorder( pRoot->left );
    std::cout << pRoot->data << std::endl;
    print_inorder( pRoot->right );
}

/* this is a "ASCII-graphical" print
 * recursive function. 
 * Left branch of the tree is at the top !!!
 */
void print_padded( TreeNode const* pRoot, int padding, char edge ) {
    if ( pRoot == nullptr ) { return; }

    print_padded( pRoot->left, padding + 5,'/');
    std::cout << std::setw( padding ) << " " << edge << " " << pRoot->data  << "(" << pRoot->size << ")" << std::endl;
    print_padded( pRoot->right, padding + 5,'\\');
}

void print_dot_null( int data, int size, int nullcount, std::ostream & os )
{
    os << "    null" << nullcount << " [shape=point];\n";
    os << "    \"" << data << "(" << size << ")\" -> null" << nullcount << ";\n";
}

void print_dot_aux( TreeNode const* pRoot, std::ostream & os, int & nullcount )
{
    if (pRoot->left) {
        os << "    \"" << pRoot->data << "(" << pRoot->size << ")\" -> \"" << pRoot->left->data << "(" << pRoot->left->size << ")\";\n";
        print_dot_aux( pRoot->left, os, nullcount );
    } else {
        if (pRoot->right) {
            print_dot_null( pRoot->data, pRoot->size, ++nullcount, os );
        }
    }

    if (pRoot->right) {
        os << "    \"" << pRoot->data << "(" << pRoot->size << ")\" -> \"" << pRoot->right->data << "(" << pRoot->right->size << ")\";\n";
        print_dot_aux( pRoot->right, os, nullcount );
    } else {
        if (pRoot->left) {
            print_dot_null( pRoot->data, pRoot->size, ++nullcount, os );
        }
    }
}

void print_dot( TreeNode const* pRoot, std::ostream & os ) {
    os << "digraph {\n";
    os << "    node [fontname=\"Arial\"];\n";

    if ( !pRoot ) {
        os << "\n";
    } else if ( !pRoot->right && !pRoot->left ) {
        os << "    " << pRoot->data << "\n";
    } else {
        int terminal_node_counter = 0;
        print_dot_aux( pRoot, os, terminal_node_counter );
    }

    os << "}\n";
}

static int newCount = 1;
void BuildTree(int left, int right, int* sorted, TreeNode* tree)
{
  // fill data
  int index = (left + right) / 2;
  COUT << "checking index: " << index << "\n";
  tree->data = sorted[index];
  tree->size = (right - left);

  // rec call for left
  if (right - left == 1)
  {
    COUT << "no more kids left \n";
    return;
  }
  else
  {
    COUT << "making left " << index << "\n";
    tree->left = new TreeNode();
    newCount++;
    BuildTree(left, index, sorted, tree->left);
  }

  // rec call for right
  if (right - index == 1)
  {
    COUT << "no more kids right\n";
    return;
  }
  else
  {
    COUT << "making right " << index << "\n";
    tree->right = new TreeNode();
    newCount++;
    BuildTree(index + 1, right, sorted, tree->right);
  }

}

TreeNode* sorted_array2tree(int * a, int size)
{
  TreeNode* tree = new TreeNode();
  BuildTree(0, size, a, tree);
  return tree;
}

void erase_tree(TreeNode* pRoot)
{
  TreeNode* deleteMe = pRoot;

  // walk all the way left
  while (deleteMe->left)
  {
    erase_tree(deleteMe->left);
    deleteMe->left = nullptr;
  }

  // walk all the way right
  while (deleteMe->right)
  {
    erase_tree(deleteMe->right);
    deleteMe->right = nullptr;
  }

  // you have no children
  delete deleteMe;
}

static int index = 0;
void treeToArray(const TreeNode* pRoot, int* array) 
{
  if (pRoot == nullptr) 
  { 
    return; 
  }

  treeToArray(pRoot->left, array);
  COUT << "[" << index << "]" <<" = " << pRoot->data << "\n";
  array[index ++] = pRoot->data;
  treeToArray(pRoot->right, array);
}


static stack<TreeNode*> path;
void insert(TreeNode** ppRoot, int value, float alpha)
{
  //COUT << "inserting " << value << "\n";
  if (!*ppRoot)
  {
    *ppRoot = new TreeNode();
    (*ppRoot)->data = value;
    return;
  }
  TreeNode* current = *ppRoot;

  while (current)
  {
    // are you my mom?
    if (current->data == value)
      break;
    else
      current->size++;

    path.push(current);
    // check if you are  bigger than value
    if (value > current->data)
    {
      //COUT << value << " bigger than " << current->data << " looking right\n";

      // keep digging
      if (current->right)
      {
        current = current->right;
      }

      // there are no children this way, add to right
      else
      {
        //COUT << "no kids right, inserting as right of " << current->data << "\n";
        current->right = new TreeNode;
        current = current->right;
        current->data = value;
      }
    }

    // no? go left then
    else
    {
      //COUT << value << "smaller than " << current->data << " looking left\n";

      if (current->left)
      {
        current = current->left;
      }

      // at value to left
      else
      {
        //COUT << "no kids left, inserting as left of " << current->data << "\n";
        current->left = new TreeNode;
        current = current->left;
        current->data = value;
      }
    }
  }



  while (!path.empty())
  {
    TreeNode* lastNode = path.top();
    COUT << "checking balance of data" << lastNode->data << "\n";
    path.pop();

    // is it unbalanced?
    int left = 0;
    if (lastNode->left)
    {
      left = lastNode->left->size;
    }
    int right = 0;
    if (lastNode->right)
    {
      right = lastNode->right->size;
    }
    int pivot = lastNode->size;

    if (left > alpha * pivot || right > alpha * pivot)
    {
      COUT << "one is unbalanced: " << left << " > " << alpha * pivot << " or " << right << " > " << alpha * pivot << "\n";

      //keep track of which side we're inserting on
      bool isRight = false;
      if (right > alpha * pivot)
        isRight = true;

      int size = lastNode->size;
      int* newArray = new int[size];
      treeToArray(lastNode, newArray);
      index = 0;

      // validate
      if (DEBUG)
      {
        COUT << "{";
        for (int i = 0; i < size; i++)
        {
          COUT << newArray[i] << " ";
        }
        COUT << "}\n";
      }
      if (DEBUG)
      {
        COUT << "BEFOre OUT\n";
        print_padded(*ppRoot, 0, '-');
        check_weights(*ppRoot, alpha);
      }
      erase_tree(lastNode);
      lastNode = sorted_array2tree(newArray, size);
      TreeNode* parent = nullptr;
      if (path.empty())
        parent = *ppRoot;
      else
        parent = path.top();

      // replace the branch that was destroyed
      if (lastNode == (*ppRoot))
      {
        COUT << "parent root\n";
        //(*ppRoot) = lastNode;
      }
      else if (isRight)
        parent->right = lastNode;
      else
        parent->left = lastNode;

      delete[] newArray;
      while (!path.empty())
      {
        path.pop();
      }

      COUT << "[fixed] not ";
    }
    COUT << "balanced: " << left << " < " << alpha * pivot << " and " << right << " < " << alpha * pivot << "\n";
    if (DEBUG)
    {
      COUT << "DEBUG OUT\n";
      print_padded(*ppRoot, 0, '-');
      check_weights(*ppRoot, alpha);
    }
  }
}

TreeNode* find(TreeNode* pRoot, int value)
{
  if (pRoot == nullptr) 
  { 
    COUT << "no more kids this way\n";
    return pRoot; 
  }
  COUT << "does " <<value<< " = " << pRoot->data << "?\n";
  if (pRoot->data == value)
  {
    COUT << "FOUND\n";
    return pRoot;
  }

  TreeNode* found = find(pRoot->left, value);
  if (!found)
  {
    COUT << "looking right\n";
    found = find(pRoot->right, value);
  }
  return found;
}

