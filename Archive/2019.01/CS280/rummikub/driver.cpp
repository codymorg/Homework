#include "rummikub.h"
#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>

using std::ifstream;
using std::cout;
using std::cin;

void test0() // solvable
{
  std::vector< Tile > tiles;
  RummiKub rks;

  ifstream stream;
  stream.open("C:/Users/Cody/Documents/~Homework/SVN/19 Spring/CS280/rummikub/tiles.txt");
  char input[10];
  if (stream.is_open())
  {
    cout << "solving for these tiles:\n";
    while (!stream.eof())
    {
      stream >> input;
      Color color;
      switch (input[1])
      {
      case 'd'://diamond
        color = Color::Red;
        break;
      case 's'://spade
        color = Color::Green;
        break;
      case 'h'://heart
        color = Color::Blue;
        break;
      case 'c'://clubs
        color = Color::Yellow;
        break;
      default:
        cout << "invalid tile" << input[0] << " " <<input[1] << "\n";
        break;
      }
      int denom = input[0] - 48;
      Tile tile;
      tile.denomination = denom;
      tile.color = color;
      tiles.push_back(tile);
    }
  }


  cout << "\n\nsolving...\n\n\n";
  tiles.pop_back();

  for (size_t i = 0; i < tiles.size(); i++)
  {
    cout << tiles[i] << " ";
  }
  cout << "\n";
  for (size_t i = 0; i < tiles.size(); i++)
  {
    rks.Add(tiles[i]);
  }

  rks.Solve();
  rks.printSolution();
}

int main( int argc, char *argv[] )
{

    test0();

<<<<<<< .mine
||||||| .r203
    std::random_shuffle( tiles.begin(), tiles.end() );
   
    for ( auto const& t : tiles ) {
        rks.Add( t );
    }

    rks.Solve();
    std::vector< std::vector< Tile > > runs     = rks.GetRuns();
    std::vector< std::vector< Tile > > groups   = rks.GetGroups();
    
    if ( runs.size()==0 && groups.size()==0 ) { // there is NO solution to this problem
        std::cout << "Solved correctly (no solution)\n";
    } else {
        std::cout << "Solved incorrectly (non-existing solution found)\n";
    }
}

#include <random>
std::vector< Tile >
GenerateRandomSolvable( 
        int num_runs, int max_run_length=13, 
        int num_groups = 0 
        )
{
    std::random_device                      rd;
    std::mt19937                            gen( rd() );
    std::uniform_int_distribution<int>      dis_group_denomination( 0,12 );  // random denomination
    std::uniform_int_distribution<int>      dis_group_length( 3,4 );         // random length 3 or 4
    std::uniform_int_distribution<int>      dis_run_color( 0,3 );            // random run color
    std::uniform_int_distribution<int>      dis_run_start( 0,10 );           // random run start
    std::uniform_int_distribution<int>      dis_rand( 0,10000000 );          // random number to be used in other cases

    std::vector< Tile > tiles;

    // create groups
    for ( int i=0; i<num_groups; ++i ) {
        int denomination    = dis_group_denomination( gen );
        int length          = dis_group_length( gen ); // 3 or 4
        if( length == 4 ) { // all 4 colors
            tiles.push_back( {denomination,Red} );
            tiles.push_back( {denomination,Green} );
            tiles.push_back( {denomination,Blue} );
            tiles.push_back( {denomination,Yellow} );
        } else { // one color to skip
            int color_to_skip = dis_rand( gen ) % 4;
            if ( Red    != color_to_skip ) { tiles.push_back( {denomination,Red} );     }
            if ( Green  != color_to_skip ) { tiles.push_back( {denomination,Green} );  }
            if ( Blue   != color_to_skip ) { tiles.push_back( {denomination,Blue} );    }
            if ( Yellow != color_to_skip ) { tiles.push_back( {denomination,Yellow} );  }
        }
    }
    //create runs
    for ( int i=0; i<num_runs; ++i ) {
        int start   = dis_run_start( gen );
        int end     = start + 3 + dis_rand( gen ) %  ( std::min(13,start+max_run_length)+1-3-start);
        //std::cerr << "create run from " << start << " to " << end << " of len " << end-start << std::endl;
        Color col   = static_cast<Color>( dis_run_color( gen ) );
        for ( int d=start; d<end; ++d ) {
            tiles.push_back( {d,col} );
        }
    }
    return tiles;
}

void test3() 
    // about 2*3.5 + 2*3.5 = 14 tiles on average 
    // two 2-tile or 4-tile groups
    // two 2-tile or 4-tile runs
    // expected time is 
{
    RummiKub rks;
    std::vector< Tile> tiles = GenerateRandomSolvable( 2, 4, 2);
    std::random_shuffle( tiles.begin(), tiles.end() );
   
    for ( auto const& t : tiles ) {
        rks.Add( t );
    }

    rks.Solve();
    //std::cout << "After Solve " << rks << std::endl;
    if ( CheckSolution( rks, tiles ) ) {
        std::cout << "Solved correctly\n";
    } else {
        std::cout << "Solved incorrectly\n";
    }
}


void (*pTests[])(void) = { 
	test0, test1, test2, test3
};

void test_all() {
	for (size_t i = 0; i<sizeof(pTests)/sizeof(pTests[0]); ++i)
		pTests[i]();
}

#include <cstdio> /* sscanf */
int main( int argc, char *argv[] ) {
    //if (argc >1) {
    //    int test = 0;
    //    std::sscanf(argv[1],"%i",&test);
    //    try {
    //        pTests[test]();
    //    } catch( const char* msg) {
    //        std::cerr << msg << std::endl;
    //    }
    //} else {
    //    try {
    //        test_all();
    //    } catch( const char* msg) {
    //        std::cerr << msg << std::endl;
    //    }
    //}

  test_all();
=======
    std::random_shuffle( tiles.begin(), tiles.end() );
   
    for ( auto const& t : tiles ) {
        rks.Add( t );
    }

    rks.Solve();
    std::vector< std::vector< Tile > > runs     = rks.GetRuns();
    std::vector< std::vector< Tile > > groups   = rks.GetGroups();
    
    if ( runs.size()==0 && groups.size()==0 ) { // there is NO solution to this problem
        std::cout << "Solved correctly (no solution)\n";
    } else {
        std::cout << "Solved incorrectly (non-existing solution found)\n";
    }
}

#include <random>
std::vector< Tile >
GenerateRandomSolvable( 
        int num_runs, int max_run_length=13, 
        int num_groups = 0 
        )
{
    std::random_device                      rd;
    std::mt19937                            gen( rd() );
    std::uniform_int_distribution<int>      dis_group_denomination( 0,12 );  // random denomination
    std::uniform_int_distribution<int>      dis_group_length( 3,4 );         // random length 3 or 4
    std::uniform_int_distribution<int>      dis_run_color( 0,3 );            // random run color
    std::uniform_int_distribution<int>      dis_run_start( 0,10 );           // random run start
    std::uniform_int_distribution<int>      dis_rand( 0,10000000 );          // random number to be used in other cases

    std::vector< Tile > tiles;

    // create groups
    for ( int i=0; i<num_groups; ++i ) {
        int denomination    = dis_group_denomination( gen );
        int length          = dis_group_length( gen ); // 3 or 4
        if( length == 4 ) { // all 4 colors
            tiles.push_back( {denomination,Red} );
            tiles.push_back( {denomination,Green} );
            tiles.push_back( {denomination,Blue} );
            tiles.push_back( {denomination,Yellow} );
        } else { // one color to skip
            int color_to_skip = dis_rand( gen ) % 4;
            if ( Red    != color_to_skip ) { tiles.push_back( {denomination,Red} );     }
            if ( Green  != color_to_skip ) { tiles.push_back( {denomination,Green} );  }
            if ( Blue   != color_to_skip ) { tiles.push_back( {denomination,Blue} );    }
            if ( Yellow != color_to_skip ) { tiles.push_back( {denomination,Yellow} );  }
        }
    }
    //create runs
    for ( int i=0; i<num_runs; ++i ) {
        int start   = dis_run_start( gen );
        int end     = start + 3 + dis_rand( gen ) %  ( std::min(13,start+max_run_length)+1-3-start);
        //std::cerr << "create run from " << start << " to " << end << " of len " << end-start << std::endl;
        Color col   = static_cast<Color>( dis_run_color( gen ) );
        for ( int d=start; d<end; ++d ) {
            tiles.push_back( {d,col} );
        }
    }
    return tiles;
}

void test3() 
    // about 2*3.5 + 2*3.5 = 14 tiles on average 
    // two 2-tile or 4-tile groups
    // two 2-tile or 4-tile runs
    // expected time is 
{
    RummiKub rks;
    std::vector< Tile> tiles = GenerateRandomSolvable( 2, 4, 2);
    std::random_shuffle( tiles.begin(), tiles.end() );
   
    for ( auto const& t : tiles ) {
        rks.Add( t );
    }

    rks.Solve();
    //std::cout << "After Solve " << rks << std::endl;
    if ( CheckSolution( rks, tiles ) ) {
        std::cout << "Solved correctly\n";
    } else {
        std::cout << "Solved incorrectly\n";
    }
}


void (*pTests[])(void) = { 
	test0, test1, test2, test3
};

void test_all() {
	for (size_t i = 0; i<sizeof(pTests)/sizeof(pTests[0]); ++i)
		pTests[i]();
}

#include <cstdio> /* sscanf */
int main( int argc, char *argv[] ) {
  test_all();
>>>>>>> .r204
    return 0;
}
