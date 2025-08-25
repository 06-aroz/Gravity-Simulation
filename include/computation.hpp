#ifndef _COMPUTATUION_H
#define _COMPUTATUION_H

#include "math.hpp"
#include "rendering.hpp"
#include "types.hpp"

#include <GLUT/glut.h>

void detectBorder(Body &one);
Vec2 GravAccel(Body &one, Body &two);
float PotentialEnergy(Body &one, Body &two);
float KineticEnergy(Body &one);
Vec2 TotalEnergy();
bool CheckCollision(Body &one, Body &two);
Matrix ElasticCheck(Body &one, Body &two);
Matrix PositionCollide(Body &one, Body &two);
void detectCollision();
void renderCenterMass();

// Integration
Vec2 compNetAccel(size_t i);

// Leapfrog method
void halfStep(Body &b, Vec2 acc, float dt);
void LeapFrogStep(float dt);
void EulerMethod(float dt, Body &body);

#endif // !_COMPUTATUION_H
