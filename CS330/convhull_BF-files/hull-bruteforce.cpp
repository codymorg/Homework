#include "hull-bruteforce.h"
#include <algorithm>
#include <iostream>
#include <map>
#define DEBUG 1
bool Point::operator==( Point const& arg2 ) const {
    return ( (x==arg2.x) && (y==arg2.y) );
}

std::ostream& operator<< (std::ostream& os, Point const& p) {
	os << "(" << p.x << " , " << p.y << ") ";
	return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
	os >> p.x >> p.y;
	return os;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//return value is (on left, on right)
std::pair<bool,bool> get_location(
		float const& p1x, //check which side of the line (p1x,p1y)-->(p2x,p2y) 
		float const& p1y, //point (qx,qy) is on
		float const& p2x,
		float const& p2y,
		float const& qx,
		float const& qy
		) 
{
	Point dir   = {p2x-p1x,p2y-p1y};
	Point norm  = {dir.y, -dir.x};
	Point point = {qx-p1x,qy-p1y};
	//scalar product is positive if on right side
	float scal_prod = norm.x*point.x + norm.y*point.y;
	return std::make_pair( (scal_prod<0), (scal_prod>0));
}

// returns -1 for left, 1 for right and 0 for on the line
int IsOnRight(const Point& lineA, const Point& lineB, const Point& point)
{
  std::pair<bool, bool> side = get_location(lineA.x, lineA.y, lineB.x, lineB.y, point.x, point.y);
  bool isLeft = side.first;
  bool isRight = side.second;

  if      (isRight) return  1;
  else if (isLeft)  return -1;
  else              return  0;
}

bool AreAllPointsOnTheSameSide(const Point& lineA, 
                               const Point& lineB, 
                               const std::vector<Point>& points, 
                               bool mustBeRight = false)
{
  int side = -2;
  bool wereAllOnTheSameSideHere = true;
  if (mustBeRight)
    side = 1;

  for (size_t i = 0; i < points.size(); i++)
  {
    if (lineA == points[i] || lineB == points[i])
      continue;

    int mySide = IsOnRight(lineA, lineB, points[i]);

    // set the global side from the first point checked
    if (side == -2)
      side = mySide;
    else if (side != mySide)
    {
      wereAllOnTheSameSideHere = false;
      break;
    }
  }

  return wereAllOnTheSameSideHere;
}

size_t MinPoint(const std::vector <Point>& points)
{
  float min = INT8_MAX;
  size_t index = 0;

  for (size_t i = 0; i < points.size(); i++)
  {
    if (points[i].x < min)
    {
      min = points[i].x;
      index = i;
    }
  }

  return index;
}

//returns a set of indices of points that form convex hull
std::set<int> hullBruteForce (const std::vector<Point>& points) 
{
	int num_points = points.size();
	//std::cout << "number of points " << num_points << std::endl;
	if ( num_points < 3 ) throw "bad number of points";

	std::set<int> hull_indices;
	std::set<int> touched;
  std::map<size_t, int> contacts;

  // for every point
  for (size_t pointIndex = 0; pointIndex < points.size(); pointIndex++)
  {
    // make a line with every point in touched
    for (size_t touch = 0; touch < touched.size(); touch++)
    {
      // if all points are on one side
      if (AreAllPointsOnTheSameSide(points[pointIndex], points[touch], points))
      {
        // add to hull, contacts
        hull_indices.insert(pointIndex);
        hull_indices.insert(touch);

        // remove all values with 2 connections
        if (contacts.find(pointIndex) != contacts.end())
          contacts.erase(pointIndex);
        else
          contacts[pointIndex] = 1;

        if (contacts.find(touch) != contacts.end())
          contacts.erase(touch);
        else
          contacts[touch] = 1;

        // if contacts is empty - early out
        if (contacts.size() == 0)
          return hull_indices;
      }
    }

    //add point to touched
    touched.insert(pointIndex);
  }
		
  // this should never be hit
  throw "Early out never hit";
	return hull_indices;
}

std::vector<int> hullBruteForce2 ( std::vector< Point > const& points ) 
{
  int num_points = points.size();
	if ( num_points < 3 ) throw "bad number of points";

	std::vector<int> hull_indices;
  std::map<size_t, int> contacts;

  size_t min = MinPoint(points);
  hull_indices.push_back(min);
  contacts[min] = 0;

  // keep looking until the hull is complete
  while (contacts.size() > 0)
  {
    // find a line that puts all points on the right
    for (size_t i = 0; i < points.size(); i++)
    {
      // dont draw lines to ourselves
      if (int(i) == hull_indices.back())
        continue;

      if (AreAllPointsOnTheSameSide(points[i], points[hull_indices.back()], points, true))
      {
        // add that point to hull
        hull_indices.push_back(i);

        // remove all values with 2 connections
        if (contacts.find(i) != contacts.end())
          contacts.erase(i);
        else
          contacts[i] = 1;

        int index = hull_indices[hull_indices.size()-2];
        if (contacts.find(index) != contacts.end() && contacts[index] != 0)
          contacts.erase(index);
        else
          contacts[index] = 1;

        if (contacts.size() == 0)
        {
          hull_indices.erase(hull_indices.end()-1);
          break;
        }
      }
    }
  }
		
	return hull_indices;
}
