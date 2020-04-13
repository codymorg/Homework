#include "overlap.h"
#include <iostream>

void run( char const * filename, bool print = true ) {
    std::map<int,int> const& f = overlap( filename );
    if ( print ) {
        for( auto const & e: f ) {
            if ( e.first != 0 ) std::cout << e.first << " --> " << e.second << std::endl;
        }
    }
}

// debug, see pictures of in0, in1, in2
void test0() { run( "in0" ); }
void test1() { run( "in1" ); }
void test2() { run( "in2" ); }
// 
void test3() { run( "in3" ); }
void test4() { run( "in4" ); }
void test5() { run( "in5" ); }
void test6() { run( "in6" ); }
void test7() { run( "in7" ); }
void test8() { run( "in8" ); }

#include <chrono>
void test9() { // checking whether SCAN was implemented
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds1, elapsed_seconds2;

    start = std::chrono::system_clock::now();
    run( "in_scan", false ); // base 
    end = std::chrono::system_clock::now();
    elapsed_seconds1 = end-start;
//    std::cerr << "in_scan   " << elapsed_seconds1.count() << std::endl;

    start = std::chrono::system_clock::now();
    run( "in_scan_10", false ); // base 
    end = std::chrono::system_clock::now();
    elapsed_seconds2 = end-start;
//    std::cerr << "in_scan_10 " << elapsed_seconds2.count() << std::endl;

//    std::cout << "Ratio time(in_scan_10) / time( in_scan ) = " 
//        << elapsed_seconds2.count()/elapsed_seconds1.count() << std::endl;
    auto r = elapsed_seconds2.count()/elapsed_seconds1.count();

    if ( r < 3 ) {
        std::cout << "same time\n";         // should get extra credit on some of the tests 6,7,8
    } else if ( r<20 ) {
        std::cout << "linear\n";            // scan is in one coordinate only expected
    } else {
        std::cout << "looks quadratic\n";   // no scan implemented -80
    }
}

void test9_pontus() { // checking whether SCAN was implemented
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds1, elapsed_seconds2;

    start = std::chrono::system_clock::now();
    run( "in_scan", false ); // base 
    end = std::chrono::system_clock::now();
    elapsed_seconds1 = end-start;

    start = std::chrono::system_clock::now();
    run( "in_scan_10", false ); // base 
    end = std::chrono::system_clock::now();
    elapsed_seconds2 = end-start;

    auto r = elapsed_seconds2.count()/elapsed_seconds1.count();

    if ( r<20 ) {
        std::cout << "same time or linear\n";   // scan is in one coordinate only expected
    } else {
        std::cout << "looks quadratic\n";       // no scan implemented -80
    }
}

void (*pTests[])( ) = { 
    test0,test1,test2,test3,test4,
    test5,test6,test7,test8,test9,test9_pontus
};

#include <cstdio>
int main(int argc, char ** argv) {
    if ( argc == 2 ) { // if 1 argument is provided, assume it is test number
        int test = 0;
        std::sscanf(argv[1],"%i",&test);
        pTests[test]();
    } else {
        std::cerr << "Usage ./" << argv[0] << " <test number>\n";
    }
}
