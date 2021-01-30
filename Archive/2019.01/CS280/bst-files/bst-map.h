#ifndef BSTMAP_H
#define BSTMAP_H
#pragma once

namespace CS280 
{
  template< typename KEY_TYPE, typename VALUE_TYPE >
  class BSTmap 
  {
    public:
    class Node 
    {
      public:
      Node( KEY_TYPE key, VALUE_TYPE val, Node* parent, int height, int balance, Node* left, Node* right);

      Node(const Node&)               = delete;
      Node* operator=(const Node&)    = delete;

      KEY_TYPE const& Key() const;   // return a const reference
      VALUE_TYPE&     Value();       // return a reference

      Node* decrement();
      Node* last() const;
      Node* first() const;

      private:
      void printNode();

      KEY_TYPE    key;
      VALUE_TYPE  value;
      Node        *parent;
      int         height,balance; // optional
      Node        *left;
      Node        *right;

      friend class BSTmap;
    };

    ///// defined for the provided functions /////
    int getdepth(Node*) const;
    char getedgesymbol(const Node * node) const;




    private:

    ///// Mine /////
    unsigned totalNodes = 0;
    Node* search(Node* current, const KEY_TYPE& key)const;
    Node* insert(Node* current, const KEY_TYPE& key, const VALUE_TYPE& value);
    void deleteTree(Node* current);
    void rotateCCW(Node* deleteMe);
    void updateEnd();

    ///// Provided /////

    struct BSTmap_iterator 
    {
      public:
      BSTmap_iterator(Node* p=nullptr);
      BSTmap_iterator& operator=(const BSTmap_iterator& rhs);
      BSTmap_iterator& operator++();
      BSTmap_iterator operator++(int);
      Node & operator*();
      Node * operator->();
      bool operator!=(const BSTmap_iterator& rhs);
      bool operator==(const BSTmap_iterator& rhs);
      friend class BSTmap;

      private:
      Node* p_node;
    };

    struct BSTmap_iterator_const 
    {
      public:
      BSTmap_iterator_const(Node* p=nullptr);
      BSTmap_iterator_const& operator=(const BSTmap_iterator_const& rhs);
      BSTmap_iterator_const& operator++();
      BSTmap_iterator_const operator++(int);
      Node const& operator*();
      Node const* operator->();
      bool operator!=(const BSTmap_iterator_const& rhs);
      bool operator==(const BSTmap_iterator_const& rhs);
      friend class BSTmap;

      private:
      Node* p_node;
    };

    // BSTmap implementation
    Node* pRoot = nullptr;
    unsigned int size_ = 0;

    // end iterators are same for all BSTmaps, thus static
    // make BSTmap_iterator a friend
    // to allow BSTmap_iterator to access end iterators 
    static BSTmap_iterator end_it;
    static BSTmap_iterator_const const_end_it;


    public:
    //BIG FOUR
    BSTmap();
    BSTmap(const BSTmap& rhs);
    BSTmap& operator=(const BSTmap& rhs);
    virtual ~BSTmap();
    unsigned int size();


    //value setter and getter
    VALUE_TYPE& operator[](KEY_TYPE const& key);
    //next method doesn't make sense
    //because operator[] inserts a non-existing element
    //which is not allowed on const maps
    //VALUE_TYPE operator[](int key) const;

    //standard names for iterator types
    typedef BSTmap_iterator       iterator;
    typedef BSTmap_iterator_const const_iterator;

    //BSTmap methods dealing with non-const iterator 
    BSTmap_iterator begin();
    BSTmap_iterator end();
    BSTmap_iterator find(KEY_TYPE const& key);
    void erase(BSTmap_iterator it);

    //BSTmap methods dealing with const iterator 
    BSTmap_iterator_const begin() const;
    BSTmap_iterator_const end() const;
    BSTmap_iterator_const find(KEY_TYPE const& key) const;
    //do not need this one (why)
    //BSTmap_iterator_const erase(BSTmap_iterator& it) const;

    void print(std::ostream& os, bool print_value = false) const;

    //inner class (BSTmap_iterator) doesn't have any special priveleges
    //in accessing private data/methods of the outer class (BSTmap)
    //so need friendship to allow BSTmap_iterator to access private "BSTmap::end_it"
    //BTW - same is true for outer class accessing inner class private data
    friend struct BSTmap_iterator;
    friend struct BSTmap_iterator_const;
    private:
  };

  //notice that it doesn't need to be friend
  template< typename KEY_TYPE, typename VALUE_TYPE >
  std::ostream& operator<<(std::ostream& os, BSTmap<KEY_TYPE, VALUE_TYPE> const& map);
}

#include "bst-map.cpp"
#endif
