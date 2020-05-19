#include "overlap.h"
using std::map;
#include <vector>
using std::vector;
#include <iostream>
#include <fstream>
using std::fstream;
using std::ofstream;
#include <algorithm>
using std::sort;
#include <iomanip>
#include <list>
using std::list;
#include <iterator>

#define COUT if(false)std::cout

enum Type
{
  None = -1,
  Start,
  End,
  Mixed
};

struct Slice
{
  Slice(int end, Type t) : endY(end), type(t)
  {}

  int endY = 0;
  Type type = None;
};

struct Rectangle 
{
  int x1,y1,x2,y2;
};

static map<int, list<Rectangle>> rectangles; // all rectangles
static map<int, list<Slice>> xMap;           // active rectangles
static map<int, int> totals;                 // total amount of overlapping

static int max;
static int MOD = 7001; // for modulo counting
static int recCount;

struct R {
  int x1, y1, x2, y2;
};

struct Line {
  int x1, x2, y;
  bool start;     // true - start. false - end
  bool operator< (Line const& other) const { return y < other.y; }
};

////////////////////////////////////////////////////////////////////////////////
//   START BRUTE FORCE
////////////////////////////////////////////////////////////////////////////////
struct BruteForce {
  std::vector< std::vector<int> > field;
  std::map<int, int> freq;

  BruteForce(int dim) : field(dim, std::vector<int>(dim, 0)), freq() { }

  void AddRectangle(int x1, int y1, int x2, int y2) {
    for (int i = x1; i <= x2; ++i) {    // note "<=" !!!
      for (int j = y1; j <= y2; ++j) { // note "<=" !!!
        ++field[i][j];
      }
    }
  }

  void Process() {
    for (unsigned i = 0; i < field.size(); ++i) {
      for (unsigned j = 0; j < field.size(); ++j) {
        ++freq[field[i][j]];
        freq[field[i][j]] %= MOD; // modulo so no overflow
      }
    }
  }
};



static void LoadRectangles(char const* filename)
{
  // clear data
  rectangles.clear();
  xMap.clear();
  totals.clear();

  // load recs
  fstream in(filename);
  if (in.fail())
    throw "Cannot open input file";
  in >> max;
  in >> recCount;
  for (int i = 0; i < recCount; i++)
  {
    Rectangle r;
    in >> r.x1;
    in >> r.y1;
    in >> r.x2;
    in >> r.y2;
    rectangles[r.y1].push_back(r);
  }
}

static void AddRectangle(int y)
{
  if(rectangles.find(y) != rectangles.end())
  {
    for(Rectangle& r : rectangles[y])
    {
      xMap[r.x1].push_back(Slice(r.y2, Start));
      xMap[r.x2].push_back(Slice(r.y2, End));

      // take out of reserve rec list
    }
      rectangles.erase(y);
  }
}

static Type ProcessPoint(list<Slice>& points, int* startCount, int* endCount, int y)
{
  Type type = None;
  list<Slice>::iterator it = points.begin();
  list<Slice>::iterator del = it;
  while(it != points.end())
  {
    // determine what type of point set we mave - all S all E or mixed 
    if (type == None)          type = it->type;
    else if (type != it->type) type = Mixed;

    // tally these so we dont have to later
    if(startCount && it->type == Start) (*startCount)++;
    else if(endCount)                   (*endCount)++;

    list<Slice>::iterator del = it;
    it++;
    if (startCount && del->endY == y)
      points.erase(del);
  }

  return type;
}

static int Adjust(Type a, Type b)
{
  if (a == Start || a == None)
  {
    if(b == End) return 1;
    else         return 0;
  }
  else
  {
    if(b == End) return  0;
    else         return -1;
  }
}

static void ProcessLayer(int y)
{
  int overlapping = 0;
  map<int, list<Slice>>::iterator pList = xMap.begin();
  map<int, list<Slice>>::iterator next = pList;
  map<int, list<Slice>>::iterator delMe = xMap.end();
  next++;

  // empty
  if(pList == xMap.end())
  {
    return;
  }

  // single point
  if(next == xMap.end())
  {
    int starts = 0;
    int ends = 0;
    ProcessPoint(pList->second, &starts, &ends, y);
    totals[starts]++;
    return;
  }

  COUT << "::: layer " << y << "\n";
  COUT <<"";
  for (pList; next != xMap.end(); next++, pList++)
  {
    // delete old rectangles
    if(delMe != xMap.end())
    {
      xMap.erase(delMe);
      delMe = xMap.end();
    }
    int starts = 0;
    int ends = 0;

    Type myType = ProcessPoint(pList->second, &starts, &ends, y);
    Type nextType = ProcessPoint(next->second, nullptr, nullptr, y);

    // 1. add to the overlapp count
    overlapping += starts; 
       
    // 2. if mixed add one for extra overlap
    if(myType == Mixed)    
    {
      totals[overlapping]++;
      totals[overlapping] %= MOD;
      COUT << overlapping << "[1] ";
    }
    
    // 3. lower count for ended rectangles
    overlapping -= ends;

    // 4. add the count
    int total = next->first - pList->first + Adjust(myType, nextType);
    totals[overlapping] += total;
    totals[overlapping] %= MOD;
    COUT << overlapping << "[" << total << "]\n";
  }
}


std::map<int, int> brute_force(char const* filename)
{
  std::ifstream in(filename); // closed automatically
  if (in.fail()) throw "Cannot open input file";

  std::vector<R> rects;
  int dim, N;
  in >> dim >> N;
  for (int r = 0; r < N; ++r) {
    int x1, y1, x2, y2;
    in >> x1 >> y1 >> x2 >> y2;
    rects.push_back(R{ x1,y1,x2,y2 });
  }

  BruteForce brf(dim);

  for (auto const& r : rects) {
    brf.AddRectangle(r.x1, r.y1, r.x2, r.y2);
  }

  brf.Process();
  vector<int>totals;
  totals.resize(20);
  for(int v = 0; v < brf.field.size(); v++)
  {
    auto vec = brf.field[v];
    int current = 1;
    for (int i = 1; i < 20; i++)
    {
      int total = 0;
      for(auto num : vec)
      {
        if (num == current)
        {
          total++;
        }
      }
      totals[current] += total;
      totals[current] %= MOD;
      COUT << std::setw(5);
      COUT << totals[current];
      current++;
    }
    COUT << "\n";

  }
  ofstream  fileout;

  fileout.open("C:/Users/Cody/source/repos/Homework/CS330/overlap-files/test.txt");
  bool s = true;
  for(auto vec : brf.field)
  {
    if (s)
    {
      s = false;
      continue;
    }
    for(auto n : vec)
    {
      if(n !=0)
        fileout << n;
    }
    fileout << "\n";
  }
  return brf.freq;
}

////////////////////////////////////////////////////////////////////////////////
std::map<int,int> overlap(char const* filename) 
{
  // load all of the rectangles in a Y sorted map
  LoadRectangles(filename);

  // for every depth 
  for (int y = (*rectangles.begin()).first; y < max; y++)
  {
    // add rectangles as they start
    AddRectangle(y);


    // for every x at this depth
    ProcessLayer(y);
    map<int, list<Slice>>::iterator yit = xMap.begin();

    // delete rectangles as they finish
    while(yit != xMap.end() && yit->second.empty())
    {
      xMap.erase(yit++);
    }

    // early out
    if(rectangles.empty() && xMap.empty())
      break;
  }

  return totals;
}

// for debugging - see pictures
// in0
// in1
// in2
//
// simple tests ( less than 0.1s)
// in3    100   rectangles  gen.exe 1000 100 50 > in3
// in4    500   rectangles  gen.exe 2000 500 50 > in4
// in5    1000  rectangles  gen.exe 5000 1000 50 > in5
//
// algorithm correctness test: (80% of the grade)
// in_scan    500  rectangles  gen.exe 1000 500 50 > in_scan
// in_scan_10 is exactly as the above but scaled 10 times
// brute-force can solve in_scan in 0.1s
// but it will take brute-force 10^2 times longer to solve in_scan_10
// if scan is implemented for only Y coordinates (as described in the handout), it will be 10 times slower
// if scan is implemented for both coordinates, it will be the same time as in_scan
// 
// bigger test
// in6    5000  rectangles  gen.exe 50000 5000 50 > in6
//
// stress test
// in7    10000 rectangles  gen.exe 100000 10000 50 > in7
// in8    10000 rectangles  gen.exe 100000 10000 1 > in8





////////////////////////////////////////////////////////////////////////////////
//   START BRUTE FORCE
////////////////////////////////////////////////////////////////////////////////
//struct BruteForce {
//  std::vector< std::vector<int> > field;
//  std::map<int, int> freq;
//
//  BruteForce(int dim) : field(dim, std::vector<int>(dim, 0)), freq() { }
//
//  void AddRectangle(int x1, int y1, int x2, int y2) {
//    for (int i = x1; i <= x2; ++i) {    // note "<=" !!!
//      for (int j = y1; j <= y2; ++j) { // note "<=" !!!
//        ++field[i][j];
//      }
//    }
//  }
//
//  void Process() {
//    for (unsigned i = 0; i < field.size(); ++i) {
//      for (unsigned j = 0; j < field.size(); ++j) {
//        ++freq[field[i][j]];
//        freq[field[i][j]] %= MOD; // modulo so no overflow
//      }
//    }
//  }
//};
//
//std::map<int, int> brute_force(char const* filename)
//{
//  std::ifstream in(filename); // closed automatically
//  if (in.fail()) throw "Cannot open input file";
//
//  std::vector<R> rects;
//  int dim, N;
//  in >> dim >> N;
//  for (int r = 0; r < N; ++r) {
//    int x1, y1, x2, y2;
//    in >> x1 >> y1 >> x2 >> y2;
//    rects.push_back(R{ x1,y1,x2,y2 });
//  }
//
//  BruteForce brf(dim);
//
//  for (auto const& r : rects) {
//    brf.AddRectangle(r.x1, r.y1, r.x2, r.y2);
//  }
//
//  brf.Process();
//
//  return brf.freq;
//}
//////////////////////////////////////////////////////////////////////////////////
////   END BRUTE FORCE
//////////////////////////////////////////////////////////////////////////////////

