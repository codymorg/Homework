#include <random>
#include <vector>
#include <algorithm>    // shuffle
#include <iostream>
#include <sstream>      // std::stringstream
#include "wb_tree.h"

void test0() // build tree from sorted array using O(n) algorithm
// this is practice - will not be used in grading
{
    int size = 40;
    int * a = new int[ size ];
    for ( int i=0; i<size; ++i ) {
        a[i] = i;
    }

    TreeNode * root = sorted_array2tree( a, size );

    print_inorder( root );
    print_padded( root, 0,'-');
    std::ofstream os( "pic1.dot" );
    print_dot( root, os );
    delete [] a;

    erase_tree( root );
}

void test1() // build tree from random array 0...n-1
// balancing is not tested
{
    int size = 40;
    std::vector<int> v;
    v.reserve( size );
    for ( int i=0; i<size; ++i ) {
        v.push_back( i );
    }
    
    std::random_device rd;
    std::mt19937 gen( rd() );
    std::shuffle( v.begin(), v.end(), gen );

    TreeNode * root = nullptr;
    float alpha =0.25; // no rebalancing at all
    for ( int i=0; i<size; ++i ) {
        insert( &root, v[i], alpha );
    }
    print_inorder( root );

    // un-comment if you want to see the actual tree structure
    // print_padded( root, 0, '-' ); // insert order dependent
    // or
    // std::stringstream ss;
    // std::ofstream os( "pic1.dot" );
    // print_dot( root, os );

    erase_tree( root );
}

void test2() // build tree from random array 0...n-1 and test using find
// balancing is not tested
{
    int size = 40;
    std::vector<int> v;
    v.reserve( size );
    for ( int i=0; i<size; ++i ) {
        v.push_back( i );
    }
    
    std::random_device rd;
    std::mt19937 gen( rd() );
    std::shuffle( v.begin(), v.end(), gen );

    TreeNode * root = nullptr;
    float alpha = 1; // no rebalancing at all
    for ( int i=0; i<size; ++i ) {
        insert( &root, v[i], alpha );
    }

    //print_inorder( root );
    
    // un-comment if you want to see the actual tree structure
    // print_padded( root, 0, '-' ); // insert order dependent
    // or
    // std::stringstream ss;
    // std::ofstream os( "pic1.dot" );
    // print_dot( root, os );
    
    // find existing elements
    for ( int i=0; i<size; ++i ) {
        TreeNode * p = find( root, v[i] );
        if ( p && p->data == v[i] ) {}
        else {
            std::cout << "Err\n";
        }
    }

    // find non-existing elements
    for ( int i=0; i<size; ++i ) {
        TreeNode * p = find( root, v[i]+size );
        if ( p ) {
            std::cout << "Err\n";
        }
        else { }
    }

    erase_tree( root );
}

void test3() // build tree from random array 0...n-1 and test using find (random values)
{
    int size = 40;
    std::vector<int> v;
    v.reserve( size );
    for ( int i=0; i<size; ++i ) {
        v.push_back( i );
    }
    
    std::random_device rd;
    std::mt19937 gen( rd() );
    std::shuffle( v.begin(), v.end(), gen );

    TreeNode * root = nullptr;
    float alpha = 1; // no rebalancing at all
    for ( int i=0; i<size; ++i ) {
        insert( &root, v[i], alpha );
    }
    
    print_inorder( root );

    // un-comment if you want to see the actual tree structure
    // print_padded( root, 0, '-' ); // insert order dependent
    // or
    // std::stringstream ss;
    // std::ofstream os( "pic1.dot" );
    // print_dot( root, os );
    

    std::uniform_int_distribution<> dis( -size, size );

    for ( int i=0; i<1000; ++i ) {
        int val = dis( gen );
        std::vector<int>::const_iterator it = std::find( v.begin(), v.end(), val );
        TreeNode * p_node = find( root, val );
        if ( it != v.end() && p_node != nullptr && *it == p_node->data )    {}
        else if ( it == v.end() && p_node == nullptr )                      {}
        else {
            std::cout << "Err\n";
        }
    }

    erase_tree( root );
}

//void test4() // small rebalancing test - debugging
//// first 5 inserts do not require rebalancing
//// the last insert will rebalance at the root
//{
//    // build perfectly balanced tree - empty
//    TreeNode * root = nullptr;
//
//    float alpha = 0.8; // rebalance if one of the children has 80% or more of the nodes
//    for ( int i=0; i<6; ++i ) {
//        insert( &root, i, alpha );
//        
//        // I'm not looking at this output
//        std::stringstream ss;
//        ss << "pic" << i << ".dot";
//        std::ofstream os( ss.str() );
//        print_dot( root, os );
//       
//        // this output should match mine
//        print_padded( root, 0, '-' );
//        std::cout << "---------------------------------\n";
//    }
//
//    erase_tree( root );
//}
//
void test5() // small rebalancing test - debugging
// start with balanced tree with 1,2,3 ( 2 is the root )
// first 5 inserts (4,5,6,7) do not require rebalancing
// then insert 8 and rebalance similar to test 4 should happen at node 3
// continue inserting: 9, 10 - no re-balancing
// 11 will cause rebalance at root (2)
{
    // build perfectly balanced tree - empty
    TreeNode * root = nullptr;
    float alpha = 0.8; // rebalance if one of the children has 80% or more of the nodes
    
    insert( &root, 2, alpha );
    insert( &root, 1, alpha );
    insert( &root, 3, alpha );

    for ( int i=4; i<12; ++i ) {
        insert( &root, i, alpha );
        
        // I'm not looking at this output
        std::stringstream ss;
        ss << "pic" << i << ".dot";
        std::ofstream os( ss.str() );
        print_dot( root, os );
       
        // this output should match mine
        print_padded( root, 0, '-' );
        std::cout << "---------------------------------\n";
    }

    erase_tree( root );
}

void test6() // small rebalancing test - debugging SAME AS BEFORE BUT SMALLER ALPHA
{
    // build perfectly balanced tree - empty
    TreeNode * root = nullptr;
    float alpha = 0.6; // rebalance if one of the children has 80% or more of the nodes
    
    insert( &root, 2, alpha );
    insert( &root, 1, alpha );
    insert( &root, 3, alpha );

    for ( int i=4; i<12; ++i ) {
        insert( &root, i, alpha );
        
        // I'm not looking at this output
        std::stringstream ss;
        ss << "pic" << i << ".dot";
        std::ofstream os( ss.str() );
        print_dot( root, os );
       
        // this output should match mine
        print_padded( root, 0, '-' );
        std::cout << "---------------------------------\n";
    }

    erase_tree( root );
}

void run_stress( int size, float alpha )
{
    std::vector<int> v;
    v.reserve( size );
    for ( int i=0; i<size; ++i ) {
        v.push_back( i );
    }
    
    std::random_device rd;
    std::mt19937 gen( rd() );
    std::shuffle( v.begin(), v.end(), gen );

    TreeNode * root = nullptr;
    for ( int i=0; i<size; ++i ) {
        insert( &root, v[i], alpha );
    }
    
        std::stringstream ss;
        std::ofstream os( "pic1.dot" );
        print_dot( root, os );

    check_weights( root, alpha );
    if ( root->size != size ) { std::cout << "Err\n"; }

    erase_tree( root );
}

void test7() // stress test
{
    run_stress( 10000, 0.8 );
}

//void test8() // stress test
//{
//    run_stress( 10000, 0.6 );
//}
//
//void test9()
//{
//    int size = 31;
//    int * a = new int[ size ];
//    for ( int i=0; i<size; ++i ) {
//        a[i] = i;
//    }
//
//    TreeNode * root = sorted_array2tree( a, size );
//
//    print_inorder( root );
//    print_padded( root, 0,'-');
//    std::ofstream os( "pic31.dot" );
//    print_dot( root, os );
//    delete [] a;
//
//    float alpha = 0.8; // rebalance if one of the children has 80% or more of the nodes
//    for ( int i=31; i<39; ++i ) {
//        insert( &root, i, alpha );
//        
//        // I'm not looking at this output
//        std::stringstream ss;
//        ss << "pic" << i << ".dot";
//        std::ofstream os( ss.str() );
//        print_dot( root, os );
//       
//        // this output should match mine
//        print_padded( root, 0, '-' );
//        std::cout << "---------------------------------\n";
//    }
//
//    erase_tree( root );
//}



#include <cstdio>   //sscanf
int main(int argc, char** argv)
{
  test6();
  //if (argc > 1) {
  //  int test = 0;
  //  std::sscanf(argv[1], "%i", &test);
  //  try {
  //    pTests[test]();
  //  }
  //  catch (const char* msg) {
  //    std::cerr << msg << std::endl;
  //  }
  //}

  return 0;
}
