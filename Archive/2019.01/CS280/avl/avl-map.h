#ifndef AVLMAP_H
#define AVLMAP_H
#pragma once
#include <stack>

namespace CS280
{
  template< typename KEY_TYPE, typename VALUE_TYPE >
  class AVLmap
  {
  public:
    class Node
    {
    public:
      Node(KEY_TYPE key, VALUE_TYPE val, Node* parent, int depth, int weight, Node* left, Node* right);

      Node(const Node&) = delete;
      Node* operator=(const Node&) = delete;

      KEY_TYPE const& Key() const;   // return a const reference
      VALUE_TYPE&     Value();       // return a reference

      Node* decrement();
      Node* last() const;
      Node* first() const;

    private:
      void printNode();
      bool rotateCCW();
      bool rotateCW();
      KEY_TYPE    key;
      VALUE_TYPE  value;
      Node        *parent;
      int         depth, weight; // optional
      int height = 1;
      Node        *left;
      Node        *right;
      bool removing = false;

      friend class AVLmap;
    };

    ///// defined for the provided functions /////
    int getdepth(Node*) const;
    char getedgesymbol(const Node * node) const;




  private:

    ///// Mine /////
    void balance();
    int getHeight(Node* current);

    unsigned totalNodes = 0;
    Node* search(Node* current, const KEY_TYPE& key, int currentDepth = 0)const;
    Node* insert(Node* current, const KEY_TYPE& key, const VALUE_TYPE& value);
    void deleteTree(Node* current);
    void updateEnd();
    std::stack<Node*>path;

    ///// Provided /////

    struct AVLmap_iterator
    {
    public:
      AVLmap_iterator(Node* p = nullptr);
      AVLmap_iterator& operator=(const AVLmap_iterator& rhs);
      AVLmap_iterator& operator++();
      AVLmap_iterator operator++(int);
      Node & operator*();
      Node * operator->();
      bool operator!=(const AVLmap_iterator& rhs);
      bool operator==(const AVLmap_iterator& rhs);
      friend class AVLmap;

    private:
      Node* p_node;
    };

    struct AVLmap_iterator_const
    {
    public:
      AVLmap_iterator_const(Node* p = nullptr);
      AVLmap_iterator_const& operator=(const AVLmap_iterator_const& rhs);
      AVLmap_iterator_const& operator++();
      AVLmap_iterator_const operator++(int);
      Node const& operator*();
      Node const* operator->();
      bool operator!=(const AVLmap_iterator_const& rhs);
      bool operator==(const AVLmap_iterator_const& rhs);
      friend class AVLmap;

    private:
      Node* p_node;
    };

    // AVLmap implementation
    Node* pRoot = nullptr;
    unsigned int size_ = 0;

    // end iterators are same for all AVLmaps, thus static
    // make AVLmap_iterator a friend
    // to allow AVLmap_iterator to access end iterators 
    static AVLmap_iterator end_it;
    static AVLmap_iterator_const const_end_it;


  public:
    //BIG FOUR
    AVLmap();
    AVLmap(const AVLmap& rhs);
    AVLmap& operator=(const AVLmap& rhs);
    virtual ~AVLmap();
    unsigned int size();


    //value setter and getter
    VALUE_TYPE& operator[](KEY_TYPE const& key);
    //next method doesn't make sense
    //because operator[] inserts a non-existing element
    //which is not allowed on const maps
    //VALUE_TYPE operator[](int key) const;

    //standard names for iterator types
    typedef AVLmap_iterator       iterator;
    typedef AVLmap_iterator_const const_iterator;

    //AVLmap methods dealing with non-const iterator 
    AVLmap_iterator begin();
    AVLmap_iterator end();
    AVLmap_iterator find(KEY_TYPE const& key);
    void erase(AVLmap_iterator it);

    //AVLmap methods dealing with const iterator 
    AVLmap_iterator_const begin() const;
    AVLmap_iterator_const end() const;
    AVLmap_iterator_const find(KEY_TYPE const& key) const;
    //do not need this one (why)
    //AVLmap_iterator_const erase(AVLmap_iterator& it) const;

    void print(std::ostream& os, bool print_value = false) const;

    //inner class (AVLmap_iterator) doesn't have any special priveleges
    //in accessing private data/methods of the outer class (AVLmap)
    //so need friendship to allow AVLmap_iterator to access private "AVLmap::end_it"
    //BTW - same is true for outer class accessing inner class private data
    friend struct AVLmap_iterator;
    friend struct AVLmap_iterator_const;
  private:
  };

  //notice that it doesn't need to be friend
  template< typename KEY_TYPE, typename VALUE_TYPE >
  std::ostream& operator<<(std::ostream& os, AVLmap<KEY_TYPE, VALUE_TYPE> const& map);
}

#include "avl-map.cpp"
#endif
