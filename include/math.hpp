#ifndef _MATH_H
#define _MATH_H

#include "types.hpp"

#include <cmath>
#include <random>

float DotProduct(const Vec2 &V1, const Vec2 &V2);
float VectorMagnitude(Vec2 V);
float compDist(Vec2 V1, Vec2 V2);
Vec2 ScaleVector(Vec2 V1, float scalar);
Vec2 Normalize(Vec2 V);
Vec2 VectorSum(Vec2 V1, Vec2 V2, bool is_difference);
float Distance(Body &one, Body &two);
float randomRange(float min, float max);

#endif // !_MATH_H
