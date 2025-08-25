#ifndef _RENDERING_H
#define _RENDERING_H

#include "math.hpp"
#include "types.hpp"

#include <GLUT/glut.h>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

void DrawCircle(Vec2 position, float radius, color circle_col);
void DrawVector(Vec2 vect, bool isNormalized, color col, Vec2 position, float radius);
void DrawPath(Body one, color path_col);

// Object generation
void spawnBody(Vec2 position, float mass);
void despawnBody();
float OrbitalSpeed(float radius, float mass, bool normalize);

// Initial conditions generation functions
std::vector<Matrix> Generate_CurlRing(int number_objects);
std::vector<Matrix> Generate_RingStack(int number_objects);

bool isEqual(const Vec2 &a, const Vec2 &b, double eps);
std::vector<Vec2> randVec(int num_vec, std::vector<Vec2> vec_list);

std::vector<Matrix> Generate_NormalDist(int number_objects);

// Placing the bodies into a vector list based on a generation function
std::vector<Body> generateBody(int number_objects, std::vector<Matrix> (*genFunction)(int number_objects));
void fixGen(int index);

#endif // !_RENDERING_H
