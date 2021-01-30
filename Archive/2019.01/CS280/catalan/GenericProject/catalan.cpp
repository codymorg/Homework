#include "catalan.h"

int catalan2( int size ) 
{
    if ( size < 2 ) return 1;
    
    int n = 0;
    for ( int s=0; s<size; ++s ) 
    {
        //    left           right    (-1 is for root)
        //    subtree        subtree
        n += ( catalan2(s) * catalan2( size-1 -s ) );
    }

    return n;
}

int catalan3(int size)
{
  if (size < 2) return 1;

  int n = 0;
  for (int i = 0; i < size; i++)
  {
    for (int s = 0; s < size - i; ++s)
    {
      //    left           right    (-1 is for root)
      //    subtree        subtree
      n += (catalan3(s) * catalan3(i) * catalan3(size - 1 - s - i));
    }
  }

  return n;
}

int catalan4(int size)
{
  if (size < 2) return 1;

  int n = 0;
  
  for (int i = 0; i < size; i++)
  {
    for (int s = 0; s < size - i; ++s)
    {
      for (int j = 0; j < size - i - s; j++)
      {
        //    left           right    (-1 is for root)
        //    subtree        subtree
        n += (catalan4(s) * catalan4(i) * catalan4(j) * catalan4(size - 1 - s - i - j));
      }
    }
  }

  return n;
}
int catalan(int size, int b)
{
  return size + b;
}
