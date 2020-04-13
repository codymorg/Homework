#include "overlap.h"
using std::map;
#include <vector>
using std::vector;
#include <iostream>
#include <fstream>
using std::fstream;
#include <algorithm>
using std::sort;



struct Slice
{
  Slice(int x, bool isStart, int lifeTime) : xValue(x), start(isStart), ttl(lifeTime)
  {}

  bool start = false;
  int xValue = 0;
  int ttl = 0;
};

struct Rectangle 
{
  int x1,y1,x2,y2;
};

static vector<Rectangle>rectangles; // all rectangles
static int max;
static int MOD = 7001; // for modulo counting
static int recCount;

static void LoadRectangles(char const* filename)
{

  // load recs
  fstream in(filename);
  if (in.fail())
    throw "Cannot open input file";
  in >> max;
  in >> recCount;
  rectangles.resize(recCount);
  for (int i = 0; i < recCount; i++)
  {
    in >> rectangles[i].x1;
    in >> rectangles[i].y1;
    in >> rectangles[i].x2;
    in >> rectangles[i].y2;
  }
  sort(rectangles.begin(), rectangles.end(), [&](auto a, auto b){return a.y1 < b.y1;});
}


////////////////////////////////////////////////////////////////////////////////
std::map<int,int> overlap(char const* filename) 
{
  vector<Slice>slices;         // current x slice at y depth
  map<int, int> totals;
  int currentRectangle = 0;

  LoadRectangles(filename);

  for(int y = rectangles[currentRectangle].y1; y < max; ++y)
  {
    // check for rects for every y
    while(currentRectangle < rectangles.size() && rectangles[currentRectangle].y1 == y)
    {
      // add recs to rec list and slices
      int ttl = rectangles[currentRectangle].y2 - rectangles[currentRectangle].y1 + 1;
      slices.push_back(Slice(rectangles[currentRectangle].x1, true, ttl));
      slices.push_back(Slice(rectangles[currentRectangle].x2, false, ttl));
      currentRectangle++;
      sort(slices.begin(), slices.end(),[&](Slice& a, Slice& b) {return a.xValue < b.xValue;});
    }

    // process x slice
    int count = 0;
    if (!slices.empty())
    {
      for(size_t x = 0; x < slices.size() - 1; ++x)
      {
        // count up for starting rects, down for ending
        if(slices[x].start)
          count++;
        else
          count--;

        // add one if the pair points are different (if they're the same this extra point is picked up elsewhere)
        int total = slices[x + 1].xValue - slices[x].xValue;
        total += slices[x].start != slices[x + 1].start;

        // keep track of total points counted this slice
        totals[count] += total;
        totals[count] %= MOD;

        // kill spent slices
        slices[x].ttl--;
        if (slices[x].ttl <= 0)
        {
          slices.erase(slices.begin() + x);
          x--;
        }
      }
      slices.back().ttl--;
    }
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

