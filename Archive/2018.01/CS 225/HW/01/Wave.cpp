#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <time.h>

#define AUTOGEN_ true

using std::cout;
using std::cin;
using std::vector;
using std::string;
using std::istringstream;

void CalcVectors(vector<int>& points)
{
  vector<int> temp;
  for (int i = 0; i < 6; i++)
  {
    int x2 = points[(i + 2) % 6];
    int x = points[i];

    temp.push_back(x2 - x);
  }
  points = temp;
}

void CalcNorms(vector<int>& vectors)
{
  for (int i = 0; i < 6; i += 2)
  {
    int x = vectors[i];
    int y = -vectors[i + 1];
    vectors[i] = y;
    vectors[i + 1] = x;
  }
}

vector<int> CalcC(vector<int>& points)
{
  vector<int> temp;
  for (int i = 0; i < 6; i+=2)
  {
    int x = points[i];
    int y = points[i + 1];
    int x1 = points[(i + 2) % 6];
    int y1 = points[(i + 3) % 6];

    int c = ((y1 - y) * x) - ((x1 - x) * y);
    temp.push_back(c);
  }
  return temp;
}

void TestPoint(vector<int> norms, vector<int> cPoints)
{
  unsigned long exit = 10000000;
  while (exit)
  {
    int x, y;
    int i = rand();
    bool inside = true;
    if (AUTOGEN_)
    {
      x = i++;
      y = i++;
    }
    else
    {
      cout << "enter point to test\n";
      cin >> x;
      cin >> y;
    }
    for (int i = 0, j = 0; i < 6; i += 2, j++)
    {
      int dot = (norms[i] * x) + (norms[i + 1] * y) + cPoints[j];
      if (dot < 0)
      {
        inside = false;
        break;
      }
    }

    if (AUTOGEN_)
    {
      exit--;
    }
    else
    {
      cout << "the point {" << x << "," << y << "}";
      if (inside)
        cout << " is inside\n";
      else
        cout << " is outside\n";
    }
  }
}

void TriArea(vector<int>& points)
{
  double area = 0;

  //find what the area should be
  double actualArea = 
    (points[0] * points[3] +
    points[2] * points[5] +
    points[5] * points[1] -
    points[1] * points[2] -
    points[3] * points[4] -
    points[5] * points[0]) / 2;
}

int main()
{
  time_t start;
  time_t end;
  double seconds;
  
  vector<int> norms;
  vector<int> points;
  vector<int> cPoints;
  vector<int> vectors;

  //autogen points for testing
  if (AUTOGEN_)
  {
    while (norms.size() < 6)
    {
      norms.push_back(rand());
    }
    time(&start);

    testByArea(norms);

    time(&end);
    seconds = difftime(end, start);
    cout << "area completed in " << seconds << "seconds\n";
  }

  //generate points manually
  else
  {
    cout << "enter triangle points\n";

    while (norms.size() < 6)
    {
      int input;
      cin >> input;

      norms.push_back(input);
    }
  }

  time(&start);
  //convert to vectors
  points = norms;
  CalcVectors(norms);

  //convert to norms
  CalcNorms(norms);

  //calculate c values
  cPoints = CalcC(points);

  TestPoint(norms, cPoints);

  time(&end);
  seconds = difftime(end, start);

  cout << "completed in " << seconds << "seconds\n";


  return 0;
}