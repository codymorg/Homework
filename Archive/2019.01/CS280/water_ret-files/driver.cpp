#include <iostream>
#include "water.h"


void test0() { std::cout << waterret( "input0" ) << std::endl; }
void test1() { std::cout << waterret( "input1" ) << std::endl; }
void test2() { std::cout << waterret( "input2" ) << std::endl; }
void test3() { std::cout << waterret( "input3" ) << std::endl; }
void test4() { std::cout << waterret( "input4" ) << std::endl; }
void test5() { std::cout << waterret( "input5" ) << std::endl; }
void test6() { std::cout << waterret( "input6" ) << std::endl; }
void test7() { std::cout << waterret( "input7" ) << std::endl; }
void test8() { std::cout << waterret( "input8" ) << std::endl; }

void (*pTests[])(void) = { 
	test0, test1, test2, test3, test4, 
	test5, test6, test7, test8
};
void test_all() {
	for (size_t i = 0; i<sizeof(pTests)/sizeof(pTests[0]); ++i)
		pTests[i]();
}

#include <cstdio> /* sscanf */
int main(int argc, char *argv[] ) {
    if (argc >1) {
        int test = 0;
        std::sscanf(argv[1],"%i",&test);
        try {
            pTests[test]();
        } catch( const char* msg) {
            std::cerr << msg << std::endl;
        }
    } else { // no argument provided
        try {
            test_all();
        } catch( const char* msg) {
            std::cerr << msg << std::endl;
        }
    }

    return 0;
}
