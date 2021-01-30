#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <tuple>
#include <iostream> // debugging only

#define COUT if(false) std::cout

// key   - binary heap
// value - actual data 

// forward declaration for 1-1 operator<< 
template < typename T_key, typename T_value >
class BinaryHeap;

template < typename T_key, typename T_value >
std::ostream& operator<< (std::ostream& os, BinaryHeap<T_key, T_value> const & bh);


template < typename T_key, typename T_value >
class BinaryHeap
{
    private:
        // indexing starts at 0 - i.e. minimum element is at index 0
        // modify left/right/parent implementations to support that
        std::vector< std::tuple<T_key,T_value> > data = {};
    public:
        BinaryHeap() = default;
        BinaryHeap( std::vector< std::tuple<T_key,T_value> > && data2 ) : data( std::move( data2 ) ) 
        {
            init_heap();
        }
        BinaryHeap( BinaryHeap const& ) = default; // vector copy is OK
        BinaryHeap( BinaryHeap && ) = default; // vector move is OK
        BinaryHeap& operator=( BinaryHeap const& ) = default; // vector assignment is OK
        BinaryHeap& operator=( BinaryHeap && ) = default; // vector move assignment is OK

        friend
        std::ostream& operator<< (std::ostream& os, BinaryHeap<T_key, T_value> const & bh)
        {
            for ( std::tuple< T_key,T_value> const& el : bh.data ) {
                os << std::get<0>( el ) << " "; // does not prunsigned values
            }
            return os;
        }

        void push( std::tuple<T_key,T_value> t )
        {
          data.push_back(t);

          // if you're breaking the heap bubble up
          bubble(t);
        }

        void push( std::tuple<T_key,T_value> && t )
        {
        }

        void pop() 
        {
          swap(0, data.size() - 1);
          data.erase(data.end() - 1);

          if (!data.empty())
          {
            int leftIndex = left(0);
            int rightIndex = right(0);
            int size = data.size();

            // can't go left go right
            if (leftIndex >= size && rightIndex < size)
            {
              sink(data[0]);
            }

            //cant go right go left
            else if (rightIndex >= size && leftIndex < size)
            {
              sinkLeft(data[0]);
            }

            // can't sink
            else if (leftIndex >= size && rightIndex >= size)
            {
              //
            }

            //can go both ways: left > right go left
            else if (std::get<0>(data[1]) < std::get<0>(data[2]))
            {
              sinkLeft(data[0]);
              if (std::get<0>(data[0]) > std::get<0>(data[2]))
                sink(data[0]);
            }

            // do right first
            else
            {
              sink(data[0]);
              if (std::get<0>(data[0]) > std::get<0>(data[1]))
                sinkLeft(data[0]);
            }
          }
        }

        unsigned size() const
        {
            return data.size();
        }

        std::tuple<T_key,T_value> const& top() const 
        {
          return data[0];
        }

        void decrease_key( unsigned index, T_key new_key ) // bubble up
        {
          std::get<0>(data[index]) = new_key;
          bubble(data[index]);
        }


    private:
        int left  (int i ) const { return 2*i+1; }
        int right (int i ) const { return 2*i+2; }
        int parent(int i ) const { return (i-1)/2; }

        std::vector<unsigned> kids = {};
        int find(const T_key key, unsigned current)
        {
          // invalid
          if (current >= data.size() || current < 0)
            return -1;

          // gotchya
          T_key currentKey = std::get<0>(data[current]);
          if (key == currentKey)
            return int(current);

          kids.push_back(right(current));
          kids.push_back(left(current));

          while (!kids.empty())
          {
            unsigned top = kids.back();
            kids.erase(kids.end() - 1);
            if (find(key, top) != -1)
              break;
          }

          return -1;
        }

        // swap 2 keys in the heap
        void swap(unsigned a, unsigned b)
        {
          std::tuple<T_key, T_value> swapper = data[a];
          data[a] = data[b];
          data[b] = swapper;
        }

        // bubble value up if it's smaller than its parent
        void bubble(std::tuple<T_key, T_value> t)
        {
          int me = data.size() - 1;
          unsigned myparent = parent(me);
          while (std::get<0>(t) < std::get<0>(data[myparent]))
          {
            swap(me, myparent);
            me = myparent;
            myparent = parent(me);
          }
        }

        void sink(std::tuple<T_key, T_value> t)
        {
          if (data.size() == 1)
            return;
          T_key myKey = std::get<0>(t);
          int myIndex = find(myKey, 0);
          if (myIndex == -1)
            return;

          // get the index of the child we're swapping with (priority right)
          unsigned childIndex = right(myIndex);
          if (childIndex >= data.size())
          {
            childIndex = left(myIndex);
            if (unsigned(myIndex) >= data.size())
              return;
          }
          T_key childKey = std::get<0>(data[childIndex]);

          // sink this key
          while (myKey > childKey)
          {
            COUT << "moving " << myKey << " down " << childIndex << "\n";
            swap(myIndex, childIndex);

            // move parent down
            myIndex = childIndex;

            // move child down
            childIndex = right(myIndex);
            if (childIndex >= data.size())
            {
              childIndex = left(myIndex);
              if (childIndex >= data.size())
                break;
            }
            childKey = std::get<0>(data[childIndex]);
          }
        }
        void sinkLeft(std::tuple<T_key, T_value> t)
        {
          T_key myKey = std::get<0>(t);
          int myIndex = find(myKey, 0);
          if (myIndex == -1)
            return;

          // get the index of the child we're swapping with (priority right)
          unsigned childIndex = left(myIndex);
          if (childIndex >= data.size())
          {
            childIndex = right(myIndex);
            if (unsigned(myIndex) >= data.size())
              return;
          }
          T_key childKey = std::get<0>(data[childIndex]);

          // sink this key
          while (myKey > childKey)
          {
            COUT << "moving " << myKey << " down " << childIndex << "\n";
            swap(myIndex, childIndex);

            // move parent down
            myIndex = childIndex;

            // move child down
            childIndex = left(myIndex);
            if (childIndex >= data.size())
            {
              childIndex = right(myIndex);
              if (childIndex >= data.size())
                break;
            }
            childKey = std::get<0>(data[childIndex]);
          }
        }

        void init_heap() 
        {
        }

        void heapify( unsigned index ) // trickle down
            // assume left(index) and right(index) are heaps
            // and overall structure is heap
        {
        }

};

#endif
