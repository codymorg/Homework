#pragma once
#ifndef SCAPEGOAT_TREE_H
#define SCAPEGOAT_TREE_H
#include <fstream> // std::ostream

struct TreeNode 
{
    int data {0};
    TreeNode* left {nullptr}, * right {nullptr};
    int size {1}; // num nodes in the subtree rooted at this node including node itself
};


void insert( TreeNode ** ppRoot, int value, float alpha );

TreeNode * find( TreeNode * pRoot, int value );

void erase_tree( TreeNode * pRoot );

void BuildTree(int left, int right, int* sorted, TreeNode* tree);

// practice function - accept a sorted array of integers and build 
// almost balanced tree, see test0 (not used in testing/grading)
TreeNode* sorted_array2tree(int * a, int size);

////////////////////////////////////////////////////////////////////////////////
// implemented
// check the whole tree for correct balances

void check_weights( TreeNode * pRoot, float alpha );

void print_inorder( TreeNode const* pRoot );

/* this is a "ASCII-graphical" print
 * recursive function. 
 * Left branch of the tree is at the top !!!
 */
void print_padded( TreeNode const* pRoot, int padding, char edge );

////////////////////////////////////////////////////////////////////////////////
// print_dot ( helper print_dot_aux) 
// output tree in dot-format
// use 
// dot -Tps pic1.dot -o outfile.ps
// to generate PostScript output
void print_dot( TreeNode const* pRoot, std::ostream & os );

#endif
