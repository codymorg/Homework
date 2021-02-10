#include "gol.h"
#include <algorithm>
#include <cstdio>  /* sscanf */
#include <fstream> /* ifstream */
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <chrono>

#pragma warning(disable : 4996)

void draw(std::vector<std::tuple<int, int>>& population, int max_x, int max_y)
{
  std::sort(population.begin(), population.end(), [](std::tuple<int, int> const& a, std::tuple<int, int> const& b) {
    return (std::get<1>(a) < std::get<1>(b)) or (std::get<1>(a) == std::get<1>(b) and std::get<0>(a) < std::get<0>(b));
  });

  // draw population          +---------> x
  int old_y = -1; // down       |
  int old_x = -1; // right      V y

#if 0
    // +--------------+ style
    std::cout << std::setw(4) << "+"; // top-left corner
    std::cout << std::setfill( '-' ) << std::setw( max_x+1 ) << "+" << std::setfill( ' ' ); // line
#else
  //  000000000011111111112222222222
  // +012345678901234567890123456789+
  std::cout << std::setw(4) << " "; // top-left corner
  if (max_x >= 12)
  {
    for (int i = 0; i < max_x; ++i)
    {
      std::cout << i / 10;
    }
  }
  std::cout << std::endl << std::setw(4) << " ";
  for (int i = 0; i < max_x; ++i)
  {
    std::cout << i % 10;
  }
  std::cout << std::endl;
  std::cout << std::setw(4) << "+";                                                   // top-left corner
  std::cout << std::setfill('-') << std::setw(max_x + 1) << "+" << std::setfill(' '); // line
#endif

  // notation
  std::cout << "--> x"; // x-axis symbol

  for (auto const& t : population)
  {
    int x, y;
    std::tie(x, y) = t;

    for (int j = old_y; j < y; ++j)
    { // skip rows if y changed
      if (j != -1)
      {                                               // if not the first line
        std::cout << std::setw(max_x - old_x) << "|"; // finish line
      }
      if (j < max_y - 1)
      {                                                         // if not the last line
        std::cout << std::endl << std::setw(3) << j + 1 << "|"; // start of the next line
      }
      old_x = -1;
    }

    // skip columns and place "*"
    std::cout << std::setw(x - old_x) << "*";

    // prepare for next iteration
    old_x = x;
    old_y = y;
  }

  // finish the field
  for (int j = old_y; j < max_y; ++j)
  {                                                            // skip rows if y changed
    std::cout << std::setw(max_x - old_x) << "|" << std::endl; // finish line
    if (j < max_y - 1)
    {                                            // if not the last line
      std::cout << std::setw(3) << j + 1 << "|"; // start of the next line
    }
    old_x = -1;
  }

  std::cout << std::setw(4) << "+"                                                   // bottom-left corner
            << std::setfill('-') << std::setw(max_x + 1) << "+" << std::setfill(' ') // line
            << std::endl
            << std::setw(4) << "|"
            << "\n"
            << std::setw(4) << "V"
            << "  y" << std::endl; // y-axis symbol
}

std::tuple< std::vector< std::tuple<int,int> >, int, int > // population, max_x, max_y
read( char const * infile )
{
    int max_x =0, max_y =0;
    std::vector< std::tuple<int,int> > initial_population;
    std::ifstream file_init_population( infile, std::ifstream::in );
    file_init_population >> max_x >> max_y;

    int x,y;
    file_init_population >> x >> y;
    while ( !file_init_population.eof() ) {
        initial_population.push_back( std::make_tuple( x,y ) );
        file_init_population >> x >> y;
    }

//    for ( auto t : initial_population ) {
//        int x,y;
//        std::tie( x,y ) = t;
//        std::cout << x << " " << y << std::endl;
//    }

    return std::make_tuple( initial_population, max_x, max_y );
}


void test(int testN, char const * infile, int num_iter )
{


    std::vector< std::tuple<int,int> > initial_population;
    int max_x, max_y;
    std::tie( initial_population, max_x, max_y ) = read( infile );

    //draw( initial_population, max_x, max_y );
 
    auto begin = std::chrono::high_resolution_clock::now();
    std::vector< std::tuple<int,int> > final_population = run( initial_population, num_iter, max_x, max_y );
    auto end = std::chrono::high_resolution_clock::now();
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "\nTime: " << ms.count() << "\n";
    std::ofstream out(std::string("my_out")+std::to_string(testN));
    std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    draw( final_population, max_x, max_y );

    std::cout.rdbuf(coutbuf); //reset to standard output again
    draw( final_population, max_x, max_y );
    // print final_population
#if 0
    for ( auto const & t : final_population ) {
        int i,j;
        std::tie( i,j ) = t;
        std::cout << i << " " << j << std::endl;
    }
#endif
}

// in0 - blinker
// in1 - glider on 10x10
// in2 - glider on 25x25
void test0() { test(0, "in0", 10 ); }     
void test1() { test(1, "in0", 11 ); }
void test2() { test(2, "in0", 100 ); }
void test3() { test(3, "in0", 101 ); }
void test4() { test(4, "in1", 10 ); }
void test5() { test(5, "in1", 11 ); }
void test6() { test(6, "in1", 24 ); }
void test7() { test(7, "in2", 84 ); }

void (*pTests[])() = { 
    test0,test1,test2,test3,test4,test5,test6,test7
}; 

int main( int argc, char ** argv ) 
{
  for (auto t : pTests)
  {
    printf("test start\n");
    t();
    printf("\ntest end\n");
  }
  return 0;
}
