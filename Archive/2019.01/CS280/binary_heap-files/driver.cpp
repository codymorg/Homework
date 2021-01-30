#include "binary_heap.h"
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>
#include <algorithm> // shuffle, sort
#include <random>

void test0() // basic functionality
{
    BinaryHeap< int, int > bh;
    std::cout << "Empty heap: " << bh << std::endl;
    std::tuple <int,int> t = std::make_tuple( 9,9 );
    bh.push( t );
    std::cout << bh << std::endl;
    t = std::make_tuple( 5,5 );
    bh.push( t );
    std::cout << bh << std::endl;
    t = std::make_tuple( 3,3 );
    bh.push( t );
    std::cout << bh << std::endl;
    t = std::make_tuple( 7,7 );
    bh.push( t );
    std::cout << bh << std::endl;
    t = std::make_tuple( 4,4 );
    bh.push( t );
    std::cout << bh << std::endl;
    t = std::make_tuple( 2,2 );
    bh.push( t );
    std::cout << bh << std::endl; // position 5 is the last 
    bh.decrease_key( 5, 1 ); // decrease key at index 5 (0-based indexing) to new key 1
    std::cout << bh << std::endl;

    for ( int i=0; i<6; ++i ) {
        t = bh.top();
        std::cout << "top=" << std::get<0>( t ) << " heap: " << bh << std::endl;
        bh.pop();
        std::cout << "after pop" << " heap: " << bh << std::endl;
    }

}

void test1() // basic functionality
{
    BinaryHeap< int, int > bh;
    std::cout << "Empty heap: " << bh << std::endl;
    std::tuple <int,int> t = std::make_tuple( 10,10 );
    bh.push( t );

    for ( int i=0; i<16; ++i ) { // 1 pop, 2 pushes 
        t = bh.top();
        std::cout << "top=" << std::get<0>( t ) << " heap: " << bh << std::endl;
        bh.pop();
        std::cout << "after pop" << " heap: " << bh << std::endl;
        std::get<0>( t )-=1;
        bh.push( t );
        std::get<0>( t )+=2;
        bh.push( t );
    }
}


void test2() // test linear init_heap
{
    int size = 1000000;
    std::vector< std::tuple<int,int> > data;
    data.reserve( size );

    // prepare raw data - reverse order
    for ( int i=0; i<size; ++i ) {
        data.push_back( std::make_tuple(size-i, size-i) );
    }

    // randomize raw data
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(data.begin(), data.end(), g); // random shuffle

    // measure time 1-by-1 insert - NlogN
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    BinaryHeap< int, int > bh;
    for ( auto const& el : data ) { // building heap by inserting 1 by 1
        bh.push( std::move( el ) ); // n times log n = O( n log n ) to build a heap
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    //std::cout << "time elapsed " << elapsed_seconds.count() << std::endl;

    // insert "all at once", ctor uses O(N) init_heap
    std::chrono::time_point<std::chrono::system_clock> start2 = std::chrono::system_clock::now();
    BinaryHeap< int, int > bh2( std::move( data ) ); // build heap efficiently in O( N ) time
    // also allow to reuse vector's data
    std::chrono::time_point<std::chrono::system_clock> end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    //std::cout << "time elapsed " << elapsed_seconds2.count() << std::endl;

    // if time are comparable - init_heap is incorrect
    double time_ratio = elapsed_seconds2.count() / elapsed_seconds.count();
    if ( time_ratio > 0.7 ) {
        std::cout << "init_heap is not linear\n";
    } else {
        std::cout << "init_heap seems to be linear\n";
    }
}

// prepare data n, n-1, n-2, .... , 2,1 (worst case insert for min binary heap 
// may be randomize it 
// insert 1 by 1
// top/pop while not empty - checking we get keys in increasing order
void run_NlogN( int size, bool randomize, bool test_heap, bool print_time )
{
    std::vector< std::tuple<int,int> > data;
    data.reserve( size );

    // prepare raw data - reverse order
    for ( int i=0; i<size; ++i ) {
        data.push_back( std::make_tuple(size-i, size-i) );
    }

    if ( randomize ) {
        // randomize raw data
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(data.begin(), data.end(), g); // random shuffle
    }

    // insert 1 by 1 
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    BinaryHeap< int, int > bh;
    for ( auto const& el : data ) { // building heap by inserting 1 by 1
        bh.push( std::move( el ) ); // n times log n = O( n log n ) to build a heap
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    if ( print_time ) {
        std::cout << "time elapsed " << elapsed_seconds.count() << std::endl;
    }

    if ( test_heap ) {
        int prev = std::numeric_limits<int>::min();
        while ( bh.size() > 0 )     // traverse heap, by top-pop
        {
            std::tuple< int, int > t = bh.top();
            int value = std::get<0>( t );
            if ( value < prev ) { std::cout << "Error in heap"  << std::endl; }
            prev = value;
            bh.pop();
        }
    }
}

// prepare data n, n-1, n-2, .... , 2,1 (worst case insert for min binary heap 
// may be randomize it 
// insert the whole array and init it using linear time algorithm
// top/pop while not empty - checking we get keys in increasing order
void run_N( int size, bool randomize, bool test_heap, bool print_time )
{
    std::vector< std::tuple<int,int> > data;
    data.reserve( size );

    // prepare raw data
    for ( int i=0; i<size; ++i ) { // reverse order
        data.push_back( std::make_tuple(size-i, size-i) );
    }

    if ( randomize ) {
        // randomize raw data
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(data.begin(), data.end(), g); // random shuffle
    }

    // insert "all at once", ctor uses O(N) init_heap
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    BinaryHeap< int, int > bh( std::move( data ) ); // build heap efficiently in O( N ) time
    // also allow to reuse vector's data
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    if ( print_time ) {
        std::cout << "time elapsed " << elapsed_seconds.count() << std::endl;
    }

    if ( test_heap ) { // this part is still O( N logN )
        int prev = std::numeric_limits<int>::min();
        while ( bh.size() > 0 )     // traverse heap, by top-pop
        {
            std::tuple< int, int > t = bh.top();
            int value = std::get<0>( t );
            if ( value < prev ) { std::cout << "Error in heap"  << std::endl; }
            prev = value;
            bh.pop();
        }
    }
}
//                           N        rand,   test  time
void test3() { run_NlogN(   100,      true,   true, false ); }
void test4() { run_N(       100,      true,   true, false ); }
void test5() { run_NlogN(   10000,    true,   true, false ); }
void test6() { run_N(       10000,    true,   true, false ); }
void test7() { run_NlogN(   20000000, false,  false,false ); } // see Makefile on pontus for timeouts
void test8() { run_N(       20000000, false,  false,false ); } // see Makefile on pontus for timeouts

// a simple event-based fake simulation
// randomized - no output provided
#include <string>
#include <sstream>      // std::ostringstream
void test9()
{
    int size = 20;
    std::vector< std::tuple<float,std::string> > data; // time -> action 
    data.reserve( size );

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 12.0);
    std::uniform_real_distribution<> dis2(0.0, 12.0);

    // prepare raw data, random fake evens
    for ( int i=0; i<size; ++i ) {
        float time = dis(gen);
        std::ostringstream oss;
        oss << "Action to perform at time " << time;
        std::string action = oss.str();

        data.push_back( std::make_tuple( time, action) );
    }

    // insert "all at once", ctor uses O(N) init_heap
    BinaryHeap< float, std::string > bh( std::move( data ) ); // build heap efficiently in O( N ) time

    // "process" the first 10 events, each event causes 2 more events in the future
    for ( int i=0; i<10; ++i )
    {
        std::tuple< float, std::string > t = bh.top();
        bh.pop();
        float & time          = std::get<0>( t ); // time of current event
        std::string & action  = std::get<1>( t );
        std::cout << "At time " << time << " do " << action << std::endl;

        // action generates 2 new actions
        time += dis2(gen);  // future time for new event
        std::ostringstream oss;
        oss << "Extra action to perform at time " << time;
        action = oss.str();
        bh.push( t );

        time += dis2(gen);  // future time for second new event
        std::ostringstream oss2;
        oss2 << "Extra action to perform at time " << time;
        action = oss2.str();
        bh.push( t );
    }


    while ( bh.size() > 0 )     // traverse heap, by top-pop
    {
        std::tuple< float, std::string > t = bh.top();
        bh.pop();
        float time          = std::get<0>( t );
        std::string action  = std::get<1>( t );
        std::cout << "At time " << time << " do " << action << std::endl;
    }
}

// same weight, same speed. Collsision only modifies the direction.
void test10()
{
    // position direction
    // -1       right   particle 1
    //  0       left    particle 2
    //  2       left    particle 3
    //  .  -1 0  .  2  .  .  .  .  time scale
    //  ---------------------------------------
    //  .  >  <  .  <  .  .  .  .  time 0
    //  .   ><   . <   .  .  .  .  time 0.5 collision positions -0.5 -0.5 1.5
    //  .   <>   . <   .  .  .  .  time 0.5 effect
    //  . <     ><     .  .  .  .  time 1.5 collision
    //  . <     <>     .  .  .  .  time 1.5 effect - no more collisions
    //  no more collisions -- same speed

    //                       pos    dir ( false=left )
    std::vector< std::tuple< float, bool > > particles;
    particles.push_back( std::make_tuple( -1, true ) );
    particles.push_back( std::make_tuple(  0, false ) );
    particles.push_back( std::make_tuple(  2, false ) );

    BinaryHeap< float, std::tuple<size_t,size_t> > bh; // time and pair of indices of colliding particles

    // sort array by position of particles
    std::sort( std::begin(particles), std::end(particles), 
                []( std::tuple< float, bool > const &t1, std::tuple< float, bool > const &t2) {
                return std::get<0>(t1) < std::get<0>(t2);
            } );

    // look at pairs and decide if they MAY collide: directions are RL
    for ( size_t i=1; i<particles.size(); ++i ) {
        if ( std::get<1>( particles[i-1] ) == true          // going right
                && std::get<1>( particles[i] ) == false ) { // going left
            // determine the time of POSSIBLE collisions and add to event
            // collision   x1->    <-x2    time (x2-x1) /2
            float time_collision = ( std::get<0>( particles[i] ) - std::get<0>( particles[i-1] ) ) /2;
            std::tuple< float, std::tuple<size_t,size_t> > event = std::make_tuple( time_collision, std::make_tuple( i-1, i ) ); 
            bh.push( event );
        }
    }

    float time_last_update = 0.0f;
    while ( bh.size() > 0 )
    {
        std::tuple< float, std::tuple<size_t,size_t> > event = bh.top();
        bh.pop();
        float  time   = std::get<0>( event );
        size_t index1 = std::get<0>( std::get<1>( event ) );
        size_t index2 = std::get<1>( std::get<1>( event ) );
        // projected positions
        float position1  = std::get<0>( particles[ index1 ] );
        bool& direction1 = std::get<1>( particles[ index1 ] );
        float position2  = std::get<0>( particles[ index2 ] );
        bool& direction2 = std::get<1>( particles[ index2 ] );
        if ( direction1 == true && direction2 == false ) { // travelling towards each other
            float time_delta = time-time_last_update;
            // calculate new positions for all particles: simple linear update
            float new_pos1 = position1 + time_delta;
            float new_pos2 = position2 - time_delta;
            if ( new_pos1 == new_pos2 ) { // collision
                // update all particles and reset time_last_update
                time_last_update = time;
                for ( std::tuple< float, bool > & p : particles ) {
                    if ( std::get<1>( p ) == true ) { //right
                        std::get<0>( p ) += time_delta;
                    } else {
                        std::get<0>( p ) -= time_delta;
                    }
                }
                // new directions for involved particles
                direction1 = false; // reference
                direction2 = true;  // reference
                // check if new events are possible
                // check index1 -1 and index1
                if ( index1 >0 ) {
                    if ( std::get<1>( particles[index1-1] ) == true          // going right
                            && std::get<1>( particles[index1] ) == false ) { // going left
                        // determine the time of POSSIBLE collisions and add to event
                        // collision   x1->    <-x2    time (x2-x1) /2
                        float time_to_collision = ( std::get<0>( particles[index1] ) - std::get<0>( particles[index1-1] ) ) /2;
                        std::tuple< float, std::tuple<size_t,size_t> > event = std::make_tuple( time+time_to_collision, std::make_tuple( index1-1, index1 ) ); 
                        bh.push( event );
                    }
                }
                // check index2 and index2 +1
                if ( index2 < particles.size()-1 ) {
                    if ( std::get<1>( particles[index2] ) == true          // going right
                            && std::get<1>( particles[index2 +1] ) == false ) { // going left
                        // determine the time of POSSIBLE collisions and add to event
                        // collision   x1->    <-x2    time (x2-x1) /2
                        float time_to_collision = ( std::get<0>( particles[index2 +1] ) - std::get<0>( particles[index2] ) ) /2;
                        std::tuple< float, std::tuple<size_t,size_t> > event = std::make_tuple( time+time_to_collision, std::make_tuple( index2, index2 +1 ) ); 
                        bh.push( event );
                    }
                }
                std::cout << "At time " << time << " collision " << index1 << " and " << index2 << std::endl;
            } else {
                std::cout << "At time " << time << " collision " << index1 << " and " << index2 << " is cancelled" << new_pos1 << " " << new_pos2 << std::endl;
            }
        }
    }

    // process events:
    //      event happens - reverse directions of 2 particles, see if new collisions are possible
    //      event did not happen - do nothing

}

using FLOAT = double;

void print_particles( std::vector< std::tuple< FLOAT, FLOAT, FLOAT > > particles )
{
//    // sort my copy
//    std::sort( std::begin(particles), std::end(particles), 
//                []( std::tuple< FLOAT, FLOAT, FLOAT > const &t1, std::tuple< FLOAT, FLOAT, FLOAT > const &t2) {
//                return std::get<0>(t1) < std::get<0>(t2); // order by time
//            } );

    int count = 0;
    for ( auto const& p : particles ) {
        FLOAT pos     = std::get<0>( p );
        FLOAT mass    = std::get<1>( p );
        FLOAT speed   = std::get<2>( p );
        std::cout << "particale " << count++ << ": position " << pos << " mass " << mass << " speed " << speed << std::endl;
    }
}

void add_collision_event(
    std::vector< std::tuple< FLOAT, FLOAT, FLOAT > > const& particles,
    BinaryHeap< FLOAT, std::tuple<size_t,size_t> > & bh, 
    FLOAT curr_time, size_t i, size_t j  // j is always i+1
    )
{
    // determine the time of a POSSIBLE collision and add to events
    // mass is not used here
    FLOAT speed_towards_each_other = std::get<2>( particles[i] ) - std::get<2>( particles[j] );
    if ( speed_towards_each_other > 0 ) {
        FLOAT time_collision = ( std::get<0>( particles[j] ) - std::get<0>( particles[i] ) ) / speed_towards_each_other;
        std::tuple< FLOAT, std::tuple<size_t,size_t> > event = std::make_tuple( curr_time + time_collision, std::make_tuple( i, j ) ); 
        bh.push( event );
        std::cout << "\tnew event at " << curr_time + time_collision << " collision " << i << " and " << j << std::endl;
    }
    //    } else {
//        if ( std::get<2>( particles[i] ) < 0 &&
//             std::get<2>( particles[j] ) < 0     // same direction - left
//             &&
//             std::get<2>( particles[i] ) > std::get<2>( particles[j] ) ) { // i is is slower
//
//            FLOAT time_collision = ( std::get<0>( particles[j] ) - std::get<0>( particles[i] ) )  //distance
//                / 
//                ( std::get<2>( particles[i] ) - std::get<2>( particles[j] ) ); // both speeds are negative!!!!
//            std::tuple< FLOAT, std::tuple<size_t,size_t> > event = std::make_tuple( curr_time + time_collision, std::make_tuple( i, j ) ); 
//            bh.push( event );
//        } else {
//            if ( std::get<2>( particles[i] ) > 0 &&
//                    std::get<2>( particles[j] ) > 0    // same direction - left
//                    &&
//                    std::get<2>( particles[i] ) > std::get<2>( particles[j] ) ) { // i is is faster
//
//                // note identical code to above
//                FLOAT time_collision = ( std::get<0>( particles[j] ) - std::get<0>( particles[i] ) )  //distance
//                    / 
//                    ( std::get<2>( particles[i] ) - std::get<2>( particles[j] ) ); // both speeds are positive
//                std::tuple< FLOAT, std::tuple<size_t,size_t> > event = std::make_tuple( curr_time + time_collision, std::make_tuple( i, j ) ); 
//                bh.push( event );
//            }        }
//    }
}

void test11()
{
    std::vector< std::tuple< FLOAT, FLOAT, FLOAT > > particles;
#if 0
    // position direction
    // -1       right   particle 1
    //  0       left    particle 2
    //  2       left    particle 3
    //  .  -1 0  .  2  .  .  .  .  time scale
    //  ---------------------------------------
    //  .  >  <  .  <  .  .  .  .  time 0
    //  .   ><   . <   .  .  .  .  time 0.5 collision positions -0.5 -0.5 1.5
    //  .   <>   . <   .  .  .  .  time 0.5 effect
    //  . <     ><     .  .  .  .  time 1.5 collision
    //  . <     <>     .  .  .  .  time 1.5 effect - no more collisions
    //  no more collisions -- same speed

    //                                    pos mass  speed (signed) negative means left
    particles.push_back( std::make_tuple( -1, 1.0,  1.0 ) );
    particles.push_back( std::make_tuple(  0, 1.0, -1.0  ) );
    particles.push_back( std::make_tuple(  2, 1.0, -1.0  ) );
#endif
    particles.push_back( std::make_tuple( -3, 1.0,  1.0 ) );
    particles.push_back( std::make_tuple(  0, 2.0,  0.5  ) );
    particles.push_back( std::make_tuple(  2, 3.0, -2.0  ) ); // will get to 1 before 0-1 collision
    
    BinaryHeap< FLOAT, std::tuple<size_t,size_t> > bh; // time and pair of indices of colliding particles

    // sort array by position of particles - just once
    std::sort( std::begin(particles), std::end(particles), 
                []( std::tuple< FLOAT, FLOAT, FLOAT > const &t1, std::tuple< FLOAT, FLOAT, FLOAT > const &t2) {
                return std::get<0>(t1) < std::get<0>(t2);
            } );
    print_particles( particles );

    // look at pairs and decide if they MAY collide: directions are RL
    for ( size_t i=0; i<particles.size()-1; ++i ) {
        add_collision_event( particles, bh, 0.0f, i, i+1 );
    }

    FLOAT time_last_update = 0.0f;
    while ( bh.size() > 0 )
    {
        std::tuple< FLOAT, std::tuple<size_t,size_t> > event = bh.top();
        bh.pop();
        FLOAT time   = std::get<0>( event );
        size_t   index1 = std::get<0>( std::get<1>( event ) );
        size_t   index2 = std::get<1>( std::get<1>( event ) );
        // old positions
        FLOAT  position1  = std::get<0>( particles[ index1 ] );
        FLOAT& speed1     = std::get<2>( particles[ index1 ] ); // <- reference
        FLOAT  position2  = std::get<0>( particles[ index2 ] );
        FLOAT& speed2     = std::get<2>( particles[ index2 ] ); // <- reference
        FLOAT  time_delta = time-time_last_update;
        
        // calculate new positions for all particles: simple linear update
        FLOAT new_pos1 = position1 + time_delta*speed1;
        FLOAT new_pos2 = position2 + time_delta*speed2;
        
        FLOAT distance = new_pos2-new_pos1;
        if ( distance < 10*std::numeric_limits<FLOAT>::epsilon() 
                &&
             distance > -10*std::numeric_limits<FLOAT>::epsilon()) { // collision
            std::cout << "At time " << time << " collision " << index1 << " and " << index2 << " is processed: " << new_pos1 << " " << new_pos2 << std::endl;
            // update all particles and reset time_last_update
            // TODO keep last_update inside particle, update only 2 particles involved in collision
            for ( std::tuple< FLOAT, FLOAT, FLOAT > & p : particles ) {
                    std::get<0>( p ) += time_delta*std::get<2>( p );
            }
            time_last_update = time;
    
            // new directions for involved particles
            speed1 = -speed1; // <<------- simple collision
            speed2 = -speed2;
            
            // check if new events are possible
            // check index1 -1 and index1
            if ( index1 >0 ) {
                add_collision_event( particles, bh, time, index1-1, index1 );
            }
            // check index2 and index2 +1
            if ( index2 < particles.size()-1 ) {
                add_collision_event( particles, bh, time, index2, index2 +1 );
            }
            print_particles( particles );
        } else {
            std::cout << "At time " << time << " collision " << index1 << " and " << index2 << " is cancelled: " << new_pos1 << " " << new_pos2 << std::endl;
        }
    }

    // process events:
    //      event happens - reverse directions of 2 particles, see if new collisions are possible
    //      event did not happen - do nothing

}


void (*pTests[])(void) = { 
    test0, test1, test2, test3, test4, test5, test6, test7, test8, test9, test10, test11, 
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
    } else {
        try {
            test_all();
        } catch( const char* msg) {
            std::cerr << msg << std::endl;
        }
    }


    return 0;
}
