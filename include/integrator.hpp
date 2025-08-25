#include "computation.hpp"
#include "quadtree.hpp"

Vec2 compNetAccel(size_t i);
void halfStep(Body &b, Vec2 acc, float dt);
void LeapFrogStep(float dt);
void EulerMethod(float dt);

// BH algorithm
void buildTree(std::vector<Body> &bodies);
void LeapFrogStepBH(float dt, std::vector<Body> &bodies);
void EulerStepBH(float dt, std::vector<Body> &bodies);
