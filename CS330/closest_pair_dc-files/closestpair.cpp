#include "closestpair.h"
using std::vector;
#include <algorithm>
using std::sort;
#include <limits>
#include <cmath>
#include <iostream>
#include <utility>

#ifndef INT_MAX
#define INT_MAX       2147483647
#endif

std::ostream& operator<< (std::ostream& os, Point const& p) {
	os << "(" << p.x << " , " << p.y << ") ";
	return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
	os >> p.x >> p.y;
	return os;
}

static float Distance(const Point* left, const Point* right, bool square = false)
{
	float x = left->x - right->x;
	float y = left->y - right->y;
	float dist = x * x + y * y;

	if(!square)
		dist = sqrt(dist);

	return dist;
}

////////////////////////////////////////////////////////////////////////////////
float closestPair ( std::vector< Point > const& points ) 
{
	// prep temp containers
	vector<const Point*> xSorted;
	vector<const Point*> ySorted;
	xSorted.reserve(points.size());
	ySorted.reserve(points.size());

	// sort points based on x and y
	for(int i = 0; i < int(points.size()); i++)
	{
		xSorted.push_back(&(points[i]));
		ySorted.push_back(&(points[i]));
	}
	sort(xSorted.begin(), xSorted.end(),[&](const Point* left, const Point* right){return left->x < right->x;});
	sort(ySorted.begin(), ySorted.end(),[&](const Point* left, const Point* right) {return left->y < right->y; });

	// determine the closest pair
	float min = INT_MAX;
	for(int i = 0; i < int(points.size()) - 1; i++)
	{
		float xDist = Distance(xSorted[i + 1], xSorted[i], true);
		float yDist = Distance(ySorted[i + 1], ySorted[i], true);
		min = std::min(min,std::min(xDist, yDist));
	}

	return sqrt(min);
}

