#pragma once
#include <Eigen/StdVector>
typedef Eigen::Vector3f Vec3;
typedef Eigen::Vector4f Vec4;
#include <Eigen/src/Geometry/AlignedBox.h>
typedef Eigen::AlignedBox<float, 3> Bbox; // The BV type provided by Eigen
typedef Eigen::Matrix<float,3, 3> Mat3;
#include <string>
using std::string;
#include <iostream>
using std::cout;
#include <cmath>;
