#include <iostream>
#include <queue>
#include <stack>

using std::stack;
using std::queue;

#define COUT if(false) std::cout

// static data members
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator        
		CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::end_it        = CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator(nullptr);

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const  
		CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::const_end_it  = CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator_const(nullptr);


///// Helper funcitons /////
/******************************************************************************
* name   - AVL::search
*          
* brief  - look for this key in the map
*          
* input  - current : start looking from this node
*          key     : the sought after value
*          
* output - the node containing the key
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::search(CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* current, const KEY_TYPE& key, int currentD)const
{
  // are you my mom?
  if (!current)
    return nullptr;

  if (current->key == key)
  {
    current->depth = currentD;
    return current;
  }

  // look left for smaller keys
  if (key < current->key)
  {
    current = search(current->left, key, currentD + 1);
  }

  // or right for larger keys
  else
    current = search(current->right, key, currentD + 1);

  // never found the key
  return current;
}

/******************************************************************************
* name   - AVL::insert
*
* brief  - insert a value at a new key
*          assumes that pRoot is Valid and that the key does not already exist
*
* input  - current : start looking from this node
*          key     : look for this key
*          value   : insert thisvalue
* 
* output - the inserted node
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::insert(CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* current, const KEY_TYPE& key, const VALUE_TYPE& value)
{
  COUT << "inserting " << key << "\n";
  while (current)
  {
    current->weight++;
    path.push(current);

    COUT << current->key << " weight :" << current->weight << "\n";
    if (key > current->key)
    {
      // there is a right node - go there
      if (current->right)
      {
        current = current->right;
        COUT << current->key << " weight :" << current->weight << "\n";

      }

      // write this key at current->right
      else
      {
        current->right = new Node(key, value, current, current->depth + 1, 1, nullptr, nullptr);
        current = current->right;

        // update tail pointer
        if(!end_it.p_node || current->key > end_it.p_node->key)
          end_it.p_node = current;

        break;
      }
    }
    else
    {
      if (current->left)
      {
        current = current->left;
        COUT << current->key << " weight :" << current->weight << "\n";
      }
      else
      {
        //if you never go right...set the tail at the head
        if (!end_it.p_node)
          end_it.p_node = current;

        current->left = new Node(key, value, current, current->depth + 1, 1, nullptr, nullptr);
        current = current->left;
        break;
      }
    }
  }

  //update current data
  totalNodes++;

  //check the path for unbalanced nodes
  updateEnd();
  balance();

  // return the node just inserted
  return current;
}

///// Operators /////


/******************************************************************************
* name   - AVL::op[]
*
* brief  - return the ref value at this key
*
* input  - key
*
* output - value ref
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::operator[](const KEY_TYPE& key)
{
  // if pRoot is invalid - make a new head
  if (!pRoot)
  {
    pRoot = new Node(key, VALUE_TYPE(), 0, 0, 1, nullptr, nullptr);
    totalNodes++;
    COUT << key << " : " << totalNodes << " nodes\n";
    return pRoot->value;
  }

  // look for this key
  Node* current = search(pRoot,key);

  // found the key - return it
  if (current)
  {
    return current->value;
  }

  // no key found - create one
  else
  {
    current = insert(pRoot, key, VALUE_TYPE());
    return current->value;
  }
}

/******************************************************************************
* name   - copy AVL operator =
*
* brief  - copy right AVL to left
*
* input  - right AVL
*
* output - left AVL
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>& 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::operator=(const AVLmap& rhs)
{
  queue<Node*> kids;

  //make a new root
  this->pRoot = new Node(rhs.pRoot->key, rhs.pRoot->value, 0, rhs.pRoot->depth, 0, nullptr, nullptr);
  this->totalNodes++;

  if (rhs.pRoot->right)
    kids.push(rhs.pRoot->right);
  if (rhs.pRoot->left)
    kids.push(rhs.pRoot->left);

  while (!kids.empty())
  {
    Node* kid = kids.front();
    kids.pop();
    this->insert(pRoot, kid->key, kid->value);

    // add the kid's kids
    if (kid->right)
      kids.push(kid->right);
    if (kid->left)
      kids.push(kid->left);
  }

  return *this;
}

/******************************************************************************
* name   - AVL::iterator op !=
*
* brief  - compare two iterators
*
* input  - rhs : right hand iterator
*
* output - true if they are the different
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator!=(const AVLmap_iterator & rhs)
{
  return this->p_node != rhs.p_node;
}

/******************************************************************************
* name   - AVL::iterator op ==
*
* brief  - compare two iterators
*
* input  - rhs : right hand iterator
*
* output - true if they are the same
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
inline bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator==(const AVLmap_iterator& rhs)
{
  return this->p_node == rhs.p_node;
}


///// Construction /////

/******************************************************************************
* name   - AVL::copy constructor
*
* brief  - copy a map
*
* input  - rhs : right hand side
*
* output - 
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap(const AVLmap& rhs) : path()
{
  if (!rhs.pRoot)
    return;

  queue<Node*> kids;

  //make a new root
  this->pRoot = new Node(rhs.pRoot->key, rhs.pRoot->value, 0, rhs.pRoot->depth, 0, nullptr, nullptr);
  this->totalNodes++;

  if(rhs.pRoot->right)
    kids.push(rhs.pRoot->right);
  if(rhs.pRoot->left)
    kids.push(rhs.pRoot->left);

  while (!kids.empty())
  {
    Node* kid = kids.front();
    kids.pop();
    this->insert(pRoot, kid->key, kid->value);

    // add the kid's kids
    if(kid->right)
      kids.push(kid->right);
    if(kid->left)
      kids.push(kid->left);
  }
}

/******************************************************************************
* name   - Node constructor
*
* brief  - construct a new node
*
* input  - key    : index value
*        - val    : value
*        - parent : who is your daddy?
*        - depth : what depth is this node
*        - balance: not used
*        - left   : left pointer (less than)
*        - right  : right pointer (greater)
*
* output - 
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::Node(KEY_TYPE key, VALUE_TYPE val, Node* parent, int depth, int balance, Node* left, Node* right):
  key(key), value(val), parent(parent), depth(depth), weight(balance), left(left), right(right)
{

}

/******************************************************************************
* name   - AVLmap constructor
*
* brief  - construct a new binary search tree
*
* input  -
*
* output -
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap():path()
{
  end_it.p_node = pRoot;
}

/******************************************************************************
* name   - AVL::deleteTree
*
* brief  - delete the entire tree (recursive from destructor)
*
* input  - current - what node shall the destruction begin
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::deleteTree(Node* current)
{
  while (current->left)
    deleteTree(current->left);

  while (current->right)
    deleteTree(current->right);

  //clean up pointers
  Node* myParent = current->parent;
  if (myParent)
  {
    if (myParent->left == current)
      myParent->left = nullptr;
    else
      myParent->right = nullptr;
  }

  // delete me
  COUT << "deleting " << current->key << "\n";
  delete current;
  totalNodes--;
}


/******************************************************************************
* name   - AVL destructor
*
* brief  - destroy the entire tree
*
* input  -
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::~AVLmap()
{
  if(pRoot)
    deleteTree(pRoot);
  pRoot = nullptr;
  end_it.p_node = nullptr;
}


/******************************************************************************
* name   -AVL::begin
*
* brief  -
*
* input  -
*
* output -
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator 
CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::begin() 
{
	if (pRoot) 
    return AVLmap<KEY_TYPE,VALUE_TYPE>::AVLmap_iterator(pRoot->first());
	else       
    return end_it;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
char CS280::AVLmap<KEY_TYPE,VALUE_TYPE>::getedgesymbol(const Node* node) const 
{
	const Node* parent = node->parent;
	if ( parent == nullptr) 
    return '-';
	else
    return ( parent->left == node)?'\\':'/';
}

/* this is another "ASCII-graphical" print, but using 
 * iterative function. 
 * Left branch of the tree is at the bottom
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
std::ostream& CS280::operator<<(std::ostream& os, AVLmap<KEY_TYPE,VALUE_TYPE> const& map) 
{
	map.print(os);
	return os;
}

/******************************************************************************
* name   - get depth
*
* brief  - get the depth of this node
*
* input  - node : this node
*
* output - depth
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
inline int CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::getdepth(Node* node) const
{
  Node* current = search(pRoot, node->key);
  return current->depth;
}

/******************************************************************************
* name   - Node::last
* 
* brief  - return the tail of the tree
*
* input  - 
*
* output - tail pointer
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::last() const
{
  return end_it.p_node;
}

/******************************************************************************
* name   - Node::first
*
* brief  - return the head of the tree
*
* input  -
*
* output - head pointer
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::first() const
{
  Node* current = this->left;
  while (current && current->left)
    current = current->left;

  return current;
}


///// Incrementor /////

/******************************************************************************
* name   - deref for iterator
*
* brief  - *it gives you the node
*
* input  -
*
* output - ref to node
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node const& 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::operator*()
{
  return *this->p_node;
}

/******************************************************************************
* name   - iterator constructor
*
* brief  - make an iterator
*
* input  - p : pointer to node
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
inline CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::AVLmap_iterator(Node * p): p_node(p)
{
}

/******************************************************************************
* name   - const iterator construct
*
* brief  - make an iterator
*
* input  - p : pointer to node
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
inline CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator_const::AVLmap_iterator_const(Node * p) : p_node(p)
{
}

/******************************************************************************
* name   - decrement node
*
* brief  - decrement this node
*
* input  -
*
* output - the previous node in the tree
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node*
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::decrement()
{
  // go left and all the way right
  if (left)
  {
    Node* current = this->left;
    while (current->right)
      current = current->right;

    return current;
  }



  // no left - find the ancestor
  else
  {
    Node* ancestor = parent;
    while (ancestor && this->key < ancestor->key)
    {
      ancestor = ancestor->parent;
    }
    return ancestor;
  }
}

/******************************************************************************
* name   - key
*
* brief  - return the key of this node
*
* input  - 
*
* output - key
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
KEY_TYPE const & CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::Key() const
{
  return key;
}

/******************************************************************************
* name   - value
*
* brief  - return the value of this node
*
* input  - 
*
* output - the value
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE& CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::Value()
{
  return value;
}


/******************************************************************************
* name   - pointer deref
*
* brief  - access pointer stuff
*
* input  -
*
* output - the stuff
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator->()
{
  return this->p_node;
}

/******************************************************************************
* name   - ++ operator
*
* brief  - go to the next key in the tree
*
* input  -
*
* output - the next key
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator& 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator++()
{
  //you are already at the end
  if (this->p_node == end_it.p_node)
  {
    this->p_node = nullptr;
    return *this;
  }

  Node* current = this->p_node;

  //you have right kids - find sucessor
  if (current->right)
  {
    current = current->right;
    while (current->left)
    {
      current = current->left;
    }
  }

  //no right kids
  else
  {
    while (current->parent && current->key > current->parent->key)
    {
      current = current->parent;
    }
    if (!current->parent)
      COUT << "warning parent is null\n";
    current = current->parent;
  }

  this->p_node = current;
  return *this;
}


/******************************************************************************
* name   - print tree
*
* brief  - print this tree
*
* input  - os          : outstream
         - print_value : edge character?
*
* output - cout the tree
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::print(std::ostream& os, bool print_value) const
{
  AVLmap<KEY_TYPE, VALUE_TYPE>::Node* lastNode = pRoot->last();
  COUT << "total nodes : " << totalNodes << "\n";
  while (lastNode)
  {
    COUT << "[" << lastNode->key << "]\t";
    COUT << "Paren: [";
    if (lastNode->parent)
    {
      COUT << lastNode->parent->key;
    }
    COUT << "]\tleft : [";
    if (lastNode->left)
    {
      COUT << lastNode->left->key;
    }
    COUT << "]\tright: [";
    if (lastNode->right)
    {
      COUT << lastNode->right->key;
    }
    COUT << "]\n";
    lastNode = lastNode->decrement();
  }
  if (pRoot)
  {
    AVLmap<KEY_TYPE, VALUE_TYPE>::Node* lastNode = pRoot->last();
    while (lastNode)
    {

      int depth = getdepth(lastNode);
      int i;
      /* printf(b); */

      char edge = getedgesymbol(lastNode);
      switch (edge) {
      case '-':
        for (i = 0; i < depth; ++i) std::printf("       ");
        os << lastNode->key;
        if (print_value) { os << " -> " << lastNode->value; }
        os << std::endl;
        break;
      case '\\':
        for (i = 0; i < depth; ++i) std::printf("       ");
        os << edge << std::endl;
        for (i = 0; i < depth; ++i) std::printf("       ");
        os << lastNode->key;
        if (print_value) { os << " -> " << lastNode->value; }
        os << std::endl;
        break;
      case '/':
        for (i = 0; i < depth; ++i) std::printf("       ");
        os << lastNode->key;
        if (print_value) { os << " -> " << lastNode->value; }
        os << std::endl;
        for (i = 0; i < depth; ++i) std::printf("       ");
        os << edge << std::endl;
        break;
      }
      lastNode = lastNode->decrement();
    }
  }
  std::printf("\n");
}

/******************************************************************************
* name   - end of AVL
*
* brief  - return the end of the tree
*
* input  -
*
* output - last node
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::end()
{
  return nullptr;
}


/******************************************************************************
* name   - AVL::find
*
* brief  - find a key
*
* input  - key : find me
*
* output - the iterator to that key
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const & key)
{
  Node* current = search(pRoot, key);
  return AVLmap_iterator (current);
}

/******************************************************************************
* name   - AVL::Node::print
*
* brief  - print this node
*
* input  - 
*
* output - COUT output
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::printNode()
{
  Node* node = this;
  if(node->parent)
    COUT << "Parent: [" << node->parent->key << "]\n";
  COUT << "Me    : [" << node->key << "]\n";
  if (node->left)
    COUT << "Left  : [" << node->left->key << "]  ";
  if(node->right)
    COUT << "Right : [" << node->right->key << "]\n\n";
}


template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::updateEnd()
{
  Node* current = pRoot;
  while (current->right)
    current = current->right;
  end_it.p_node = current;
}


/******************************************************************************
* name   - AVL::erase
*
* brief  - delete this node 
*
* input  - it : iterator
*
* output - 
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::erase(AVLmap_iterator it)
{
  //validate node
  if (!it.p_node)
    return;

  // build path for balance
  Node* current = it.p_node->parent;
  while (current)
  {
    current->removing = true;
    path.push(current);
    current = current->parent;
  }
    
  int kids = 0;
  Node* node = it.p_node;
  Node* s = nullptr;

  // get node info
  if (node->right)
    kids++;
  if (node->left)
    kids++;
  bool findSucessor = false;
  totalNodes--;

  //if we're deleting the root
  if (pRoot == node)
  {
    switch (kids)
    {
      // there are no children and this is root
    case 0:
      delete pRoot;
      pRoot = nullptr;
      end_it.p_node = nullptr;
      return;

      // only one child, but this is root
    case 1:
      //move the root
      if (pRoot->left)
        pRoot = pRoot->left;
      else
        pRoot = pRoot->right;
      pRoot->parent = nullptr;
      delete node;
      node = nullptr;
      break;

      // 2 kids
    case 2:
      findSucessor = true;
      break;

    default:
      break;
    }
  }

  // this is not the root (or it is the root with two kids)
  if((node && pRoot != node) || findSucessor)
  {
    switch (kids)
    {
      // just a leaf
    case 0:
      //clean up pointers
      if (node->parent->left == node)
        node->parent->left = nullptr;
      else
        node->parent->right = nullptr;

      delete node;
      break;

      // only have 1 kid
    case 1:
      // which side are you on?
      if (node->parent->left == node)
      {
        //this node is on the left - I am guarenteed one kid - where is he?
        if (node->left)
        {
          node->left->parent = node->parent;
          node->parent->left = node->left;
        }
        else
        {
          node->right->parent = node->parent;
          node->parent->left = node->right;
        }
      }

      // I am on the right 
      else
      {
        // i have child - which side
        if (node->left)
        {
          node->left->parent = node->parent;
          node->parent->right = node->left;
        }
        else
        {
          node->right->parent = node->parent;
          node->parent->right = node->right;
        }
      }


      delete node;
      break;

      // you have 2 kids
    case 2:
      //Find sucessor right is guarenteed
      s = node->right;
      while (s->left)
      {
        s = s->left;
      }

      //Copy s into node
      node->key = s->key;
      node->value = s->value;

      // if the sucessor is a child of the node AND sucessor has no children - null this conneciton
      if (node == s->parent && !s->right)
      {
        node->right = nullptr;
        end_it.p_node = node;
      }

      // if there are any nodes to my right (there will never be any to the left)
      else if (s->right)
      {
        // put those nodes on the parent's left (that's where sucessor was)
        if (s->parent->left == s)
        {
          s->parent->left = s->right;
          s->right->parent = s->parent;
        }

        //or the right
        else
        {
          s->parent->right = s->right;
          s->right->parent = s->parent;
        }
      }

      // there are no nodes under sucessor
      else
      s->parent->left = nullptr;

      delete s;
      break;

    default:
      break;
    }
  }

  //if(end_it.p_node->depth < 0)
  this->updateEnd();
  balance();
}

/******************************************************************************
* name   - size
*
* brief  - how big is yo tree
*
* input  -
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
unsigned int CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::size()
{
  return totalNodes;
}

/******************************************************************************
* name   - * operator
*
* brief  - deref the node attatched to this iterator
*
* input  -
*
* output - the node in the tree
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node& 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator*()
{
  return *p_node;
}

/******************************************************************************
* name   - assignment for AVL
*
* brief  - copy one AVL to another
*
* input  - rhs : right hand side
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator& 
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator=(const AVLmap_iterator& rhs)
{
  this->p_node = rhs.p_node;
  return *this;
}

/******************************************************************************
* name   - post incr
*
* brief  - incr after callsing
*
* input  -
*
* output - ref to node
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator
CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::AVLmap_iterator::operator++(int)
{
  //you are already at the end
  if (this->p_node == end_it.p_node)
  {
    this->p_node = nullptr;
    return *this;
  }

  Node* current = this->p_node;

  //you have right kids - find sucessor
  if (current->right)
  {
    current = current->right;
    while (current->left)
    {
      current = current->left;
    }
  }

  //no right kids
  else
  {
    while (current->parent && current->key > current->parent->key)
    {
      current = current->parent;
    }
    if (!current->parent)
      COUT << "warning parent is null\n";
    current = current->parent;
  }

  this->p_node = current;
  Node* returnMe = current->parent;
  return returnMe;
}

/******************************************************************************
* name   - AVL::balance
*
* brief  - check if nodes along the path of insertion are balanced or not
*
* input  - refs AVL::path
*
* output - 
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::balance()
{
  while (!path.empty())
  {
    // check nodes in reverse order
    Node* current = path.top();
    COUT << "checking " << current->key << "\n";
    path.pop();

    int leftHeight = 0;
    int rightHeight = 0;

    if (current->left)
    {
      leftHeight = getHeight(current->left);
      COUT << "height of " << current->left->key << " " << leftHeight;
    }
    if (current->right)
    {
      rightHeight = getHeight(current->right);
      COUT << "height of " << current->right->key << " " << rightHeight;

    }

    //rotate ccw
    bool moveRoot = false;
    if (rightHeight - leftHeight > 1)
    {
      moveRoot = current->rotateCCW();
    }

    //rot cw
    else if (leftHeight - rightHeight > 1)
    {
     moveRoot = current->rotateCW();
    }

    // the root was rotated - fix it;
    if (moveRoot)
    {
      pRoot = current->parent;
    }

    //no rot
    else
    {
    }

  }
}

/******************************************************************************
* name   - AVL::rotate ccw
*
* brief  - rotate the tree from this node ccw
*
* input  - 
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::rotateCCW()
{
  bool onRight = false;
  bool onLeft = false;
  bool pivot = false;

  // where am i?
  if (!parent)
    pivot = true;
  else
  {
    // im on the left
    if (parent->left == this)
      onLeft = true;

    // im on the left
    else
      onRight = true;
  }

  // is this a complex rotation?
  if (right && right->left && !removing)
  {
      right->rotateCW();
  }

  // rotate CCW
  Node* rightLeft = right->left; // hold this node

  if (onRight)
    parent->right = right;         // my parent's right is now my right
  else if (onLeft)
    parent->left = right;

  right->parent = parent;        // vice versa
  right->left = this;            // i am now my child's left node
  parent = right;                // my parent is my right child
  right = rightLeft;             // my right is what i replaced
  if (rightLeft)                 // if there is a node here we need to pdate its parent
    rightLeft->parent = this;    

  removing = false;
  return pivot;
}

/******************************************************************************
* name   - AVL::rotate cw
*
* brief  - rotate the tree from this node cw
*
* input  -
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::Node::rotateCW()
{
  COUT << "\nrotating CW: " << this->key << "\n";
  bool onRight = false;
  bool onLeft = false;
  bool pivot = false;

  if (!parent)
    pivot = true;
  else
  {
    // im on the left
    if (parent->left == this)
      onLeft = true;

    // im on the left
    else
      onRight = true;
  }

  // is this a complex rotation?
  if (left && left->right && !removing)
  {
    left->rotateCCW();
  }


  // rotate CW 
  Node* leftRight = left->right; 

  if (onRight)
    parent->right = left;       
  else if (onLeft)
    parent->left = left;

  left->parent = parent;
  left->right = this;
  parent = left;
  left = leftRight;
  if(leftRight)
    leftRight->parent = this;

  return pivot;
}

template<typename KEY_TYPE, typename VALUE_TYPE>
int CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::getHeight(Node* current)
{
  if (!current)
    return 0;

  int leftHeight = getHeight(current->left);
  int rightHeight = getHeight(current->right);

  return (std::max(leftHeight, rightHeight) + 1);
}
