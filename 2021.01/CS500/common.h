#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

#include <Eigen/Dense>
#include <Eigen/StdVector>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Geometry/AlignedBox.h>
#include <Eigen/src/Geometry/Quaternion.h>

typedef Eigen::Matrix3f             Mat3;
typedef Eigen::Vector3f             Vec3;
typedef Eigen::Vector4f             Vec4;
typedef Eigen::AlignedBox<float, 3> Bbox; // The BV type provided by Eigen
typedef Eigen::Quaternionf          Quat;
#include <string>
using std::string;
#include <iostream>
using std::cout;
#include <vector>
using std::vector;

const float Radians = 3.14159265358979323846f / 180.0f; // Convert degrees to radians
