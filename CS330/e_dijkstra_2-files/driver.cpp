#include "e-dijkstra-solver.h"
#include <iostream>
#include <iomanip>

#include <string>
#include <map> 

void test0() { std::cout << e_dijkstra_solver( "in0" ) << std::endl; }
void test1() { std::cout << e_dijkstra_solver( "in1" ) << std::endl; }
void test2() { std::cout << e_dijkstra_solver( "in2" ) << std::endl; }
void test3() { std::cout << e_dijkstra_solver( "in3" ) << std::endl; }
void test4() { std::cout << e_dijkstra_solver( "in4" ) << std::endl; }
void test5() { std::cout << e_dijkstra_solver( "in5" ) << std::endl; }
void test6() { std::cout << e_dijkstra_solver( "in6" ) << std::endl; }
void test7() { std::cout << e_dijkstra_solver( "in7" ) << std::endl; }

void (*pTests[])( ) = { 
    test0,test1,test2,test3,test4,
    test5,test6,test7
}; 

#include <cstdio> // sscanf
int main(int argc, char ** argv) {
  for(auto i : pTests)
  {
    i();
  }

    return 0;
}
