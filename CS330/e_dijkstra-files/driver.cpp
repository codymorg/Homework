#include "e-dijkstra.h"
#include <iostream>
#include <iomanip>

#include <string>
#include <map> 

std::map<std::string,int> answer; // minimum range that works

bool test( char const * filename, std::map<std::string,int> & answer, int offset ) { 
    return e_dijkstra( filename, answer[filename] + offset ); 
}

bool test0(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in0", answer, -2); } // false
bool test1(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in0", answer, -1); } // false
bool test2(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in0", answer,  0); } // true
bool test3(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in0", answer,  1); } // true
bool test4(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in0", answer,  2); } // true

bool test5(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in1", answer, -2); } // false
bool test6(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in1", answer, -1); } // false
bool test7(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in1", answer,  0); } // true
bool test8(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in1", answer,  1); } // true
bool test9(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in1", answer,  2); } // true

bool test10(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in2", answer, -2); } // false
bool test11(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in2", answer, -1); } // false
bool test12(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in2", answer,  0); } // true
bool test13(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in2", answer,  1); } // true
bool test14(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in2", answer,  2); } // true

bool test15(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in3", answer, -2); } // false
bool test16(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in3", answer, -1); } // false
bool test17(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in3", answer,  0); } // true
bool test18(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in3", answer,  1); } // true
bool test19(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in3", answer,  2); } // true

bool test20(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in4", answer, -2); } // false
bool test21(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in4", answer, -1); } // false
bool test22(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in4", answer,  0); } // true
bool test23(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in4", answer,  1); } // true
bool test24(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in4", answer,  2); } // true

bool test25(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in5", answer, -2); } // false
bool test26(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in5", answer, -1); } // false
bool test27(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in5", answer,  0); } // true
bool test28(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in5", answer,  1); } // true
bool test29(  ) { return test( "C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in5", answer,  2); } // true


bool (*pTests[])( ) = { 
    test0,test1,test2,test3,test4,
    test5,test6,test7,test8,test9,
    test10,test11,test12,test13,test14,
    test15,test16,test17,test18,test19,
    test20,test21,test22,test23,test24,
    test25,test26,test27,test28,test29,
}; 

#include <cstdio> // sscanf
int main(int argc, char ** argv) {
    // correct answers
    answer["C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in0"] = 300;
    answer["C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in1"] = 688;
    answer["C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in2"] = 6;
    answer["C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in3"] = 53;
    answer["C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in4"] = 177;
    answer["C:/Users/codym/source/repos/Homework/CS330/e_dijkstra-files/in5"] = 2843;
    int i = 0;
    test7();
    for (auto fun : pTests)
    {
      if (i % 5 == 0)
        std::cout << "\nbatch " << i / 5 << "\n";
      std::cout << "test " << i++ << " : ";
      std::cout << std::boolalpha << fun() << "\n";
    }
}
//test25
//false
//0m0.014s
//
//test26
//true
//0m0.288s
//
//test27
//true
//0m0.175s
//
//test28
//false
//0m0.009s
//
//test29
//true
//0m0.176s
