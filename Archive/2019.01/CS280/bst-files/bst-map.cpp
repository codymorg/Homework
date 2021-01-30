#include <iostream>
#include <queue>

using std::queue;

#define COUT if(false) std::cout

// static data members
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator        
		CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::end_it        = CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(nullptr);

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const  
		CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::const_end_it  = CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const(nullptr);


///// Helper funcitons /////

/******************************************************************************
* name   - BST::search
*          
* brief  - look for this key in the map
*          
* input  - current : start looking from this node
*          key     : the sought after value
*          
* output - the node containing the key
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::search(CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* current, const KEY_TYPE& key)const
{
  // are you my mom?
  if (!current)
    return nullptr;

  if (current->key == key)
  {
    return current;
  }

  // look left for smaller keys
  if(key < current->key)
    current = search(current->left, key);

  // or right for larger keys
  else
    current = search(current->right, key);

  // never found the key
  return current;
}

/******************************************************************************
* name   - BST::insert
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::insert(CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* current, const KEY_TYPE& key, const VALUE_TYPE& value)
{
  while (current)
  {
    if (key > current->key)
    {
      // there is a right node - go there
      if (current->right)
        current = current->right;

      // write this key at current->right
      else
      {
        current->right = new Node(key, value, current, current->height + 1, 0, nullptr, nullptr);
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
        current = current->left;
      else
      {
        //if you never go right...set the tail at the head
        if (!end_it.p_node)
          end_it.p_node = current;

        current->left = new Node(key, value, current, current->height + 1, 0, nullptr, nullptr);
        current = current->left;
        break;
      }
    }
  }

  //update current data
  totalNodes++;
  COUT << key << " : "  << totalNodes << "\n";
  // return the node just inserted
  return current;
}

///// Operators /////


/******************************************************************************
* name   - BST::op[]
*
* brief  - return the ref value at this key
*
* input  - key
*
* output - value ref
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE& CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::operator[](const KEY_TYPE& key)
{
  // if pRoot is invalid - make a new head
  if (!pRoot)
  {
    pRoot = new Node(key, VALUE_TYPE(), 0, 0, 0, nullptr, nullptr);
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
* name   - copy BST operator =
*
* brief  - copy right BST to left
*
* input  - right BST
*
* output - left BST
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>& 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::operator=(const BSTmap& rhs)
{
  queue<Node*> kids;

  //make a new root
  this->pRoot = new Node(rhs.pRoot->key, rhs.pRoot->value, 0, rhs.pRoot->height, 0, nullptr, nullptr);
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
* name   - BST::iterator op !=
*
* brief  - compare two iterators
*
* input  - rhs : right hand iterator
*
* output - true if they are the different
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator!=(const BSTmap_iterator & rhs)
{
  return this->p_node != rhs.p_node;
}

/******************************************************************************
* name   - BST::iterator op ==
*
* brief  - compare two iterators
*
* input  - rhs : right hand iterator
*
* output - true if they are the same
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
inline bool CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator==(const BSTmap_iterator& rhs)
{
  return this->p_node == rhs.p_node;
}


///// Construction /////

/******************************************************************************
* name   - BST::copy constructor
*
* brief  - copy a map
*
* input  - rhs : right hand side
*
* output - 
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap(const BSTmap& rhs)
{
  if (!rhs.pRoot)
    return;

  queue<Node*> kids;

  //make a new root
  this->pRoot = new Node(rhs.pRoot->key, rhs.pRoot->value, 0, rhs.pRoot->height, 0, nullptr, nullptr);
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
*        - height : what depth is this node
*        - balance: not used
*        - left   : left pointer (less than)
*        - right  : right pointer (greater)
*
* output - 
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Node(KEY_TYPE key, VALUE_TYPE val, Node* parent, int height, int balance, Node* left, Node* right):
  key(key), value(val), parent(parent), height(height), balance(balance), left(left), right(right)
{

}

/******************************************************************************
* name   - BSTmap constructor
*
* brief  - construct a new binary search tree
*
* input  -
*
* output -
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap()
{
}

/******************************************************************************
* name   - BST::deleteTree
*
* brief  - delete the entire tree (recursive from destructor)
*
* input  - current - what node shall the destruction begin
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::deleteTree(Node* current)
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
* name   - BST destructor
*
* brief  - destroy the entire tree
*
* input  -
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::~BSTmap()
{
  if(pRoot)
    deleteTree(pRoot);
  pRoot = nullptr;
  end_it.p_node = nullptr;
}


/******************************************************************************
* name   -BST::begin
*
* brief  -
*
* input  -
*
* output -
******************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator 
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::begin() 
{
	if (pRoot) 
    return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(pRoot->first());
	else       
    return end_it;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
char CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::getedgesymbol(const Node* node) const 
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
std::ostream& CS280::operator<<(std::ostream& os, BSTmap<KEY_TYPE,VALUE_TYPE> const& map) 
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
inline int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::getdepth(Node* node) const
{
  return node->height;
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::last() const
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::first() const
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node const& 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator*()
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
inline CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::BSTmap_iterator(Node * p): p_node(p)
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
inline CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::BSTmap_iterator_const(Node * p) : p_node(p)
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node*
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::decrement()
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
KEY_TYPE const & CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Key() const
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
VALUE_TYPE& CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Value()
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator->()
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator++()
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
void CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::print(std::ostream& os, bool print_value ) const
{
  BSTmap<KEY_TYPE, VALUE_TYPE>::Node* lastNode = pRoot->last();
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
		BSTmap<KEY_TYPE,VALUE_TYPE>::Node* lastNode = pRoot->last();
		while ( lastNode ) 
    { 
      
			int depth = getdepth(lastNode);
			int i;
			/* printf(b); */

			char edge = getedgesymbol(lastNode);
			switch (edge) {
				case '-': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << lastNode->key;
                    if ( print_value ) { os << " -> " << lastNode->value; }
                    os << std::endl;
					break;
				case '\\': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << lastNode->key;
                    if ( print_value ) { os << " -> " << lastNode->value; }
                    os << std::endl;
					break;
				case '/': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << lastNode->key;
                    if ( print_value ) { os << " -> " << lastNode->value; }
                    os << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					break;
			}
			lastNode = lastNode->decrement();
		}
	}
	std::printf("\n");
}

/******************************************************************************
* name   - end of bst
*
* brief  - return the end of the tree
*
* input  -
*
* output - last node
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::end()
{
  return nullptr;
}


/******************************************************************************
* name   - BST::find
*
* brief  - find a key
*
* input  - key : find me
*
* output - the iterator to that key
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const & key)
{
  Node* current = search(pRoot, key);
  return BSTmap_iterator (current);
}

/******************************************************************************
* name   - BST::Node::print
*
* brief  - print this node
*
* input  - 
*
* output - COUT output
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::printNode()
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


/******************************************************************************
* name   - BST::rotateCCW
*
* brief  - rotate a tree CCW then delete the provided root
*
* input  - deleteMe : delete this node after rotating
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::rotateCCW(Node* deleteMe)
{
  if (!deleteMe)
    return;

  Node* myParent = deleteMe->parent;

  // connect the root
  if (myParent)
  {
    myParent->left = deleteMe->right;
    deleteMe->right->parent = myParent;
  }
  else
  {
    if (deleteMe->right)
      pRoot = deleteMe->right;
    else
      pRoot = deleteMe->left;

    //cleanup opointers
    deleteMe->parent = nullptr;
    if (deleteMe->right)
    {
      deleteMe->right->parent = pRoot;
    }
    if (deleteMe->left)
    {
      deleteMe->left->parent = pRoot;
    }
    if (pRoot)
      pRoot->parent = nullptr;
  }
  
  Node* connect = deleteMe->right;

  //connect the deleted node's branch
  while (connect && connect->left)
  {
    connect = connect->left;
  }
  if(connect)
    connect->left = deleteMe->left;

  deleteMe->printNode();
  delete deleteMe;
  totalNodes--;
}

/******************************************************************************
* name   - BST::rotateCW
*
* brief  - rotate a tree CW then delete the provided root
*
* input  - deleteMe : delete this node after rotating
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::updateEnd()
{
  Node* current = pRoot;
  while (current->right)
    current = current->right;
  end_it.p_node = current;
}


/******************************************************************************
* name   - BST::erase
*
* brief  - delete this node 
*
* input  - it : iterator
*
* output - 
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::erase(BSTmap_iterator it)
{
  //validate node
  if (!it.p_node)
    return;

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

  //if(end_it.p_node->height < 0)
    this->updateEnd();
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
unsigned int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::size()
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node& 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator*()
{
  return *p_node;
}

/******************************************************************************
* name   - assignment for bst
*
* brief  - copy one bst to another
*
* input  - rhs : right hand side
*
* output -
******************************************************************************/
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& 
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator=(const BSTmap_iterator& rhs)
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
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator++(int)
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