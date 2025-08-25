#pragma once

#include "math.hpp"
#include "types.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

// Barnes-Hut Algorithm
class BHNode {
  private:
    float theta = 0.1f;
    float theta_sq = theta * theta;
    float epsilon = Body_Settings.max_distance;

  public:
    Body body;
    float mass;

    Vec2 CM_pos;
    Vec2 min;
    Vec2 max;
    Vec2 center;

    BHNode *parent;
    BHNode *quadNode[4];
    int num_bodies;
    bool is_subdivided;

    std::vector<Body> renegades;

    enum quad_type { NE, NW, SW, SE };

    // Constuctors
    BHNode(Vec2 _min, Vec2 _max, BHNode *_parent);
    ~BHNode();

    bool isRoot();
    bool isExternal();
    void resetTree(Vec2 _min, Vec2 _max);
    quad_type getQuad(Vec2 pos);
    BHNode *createQuadNode(quad_type loc);

    void compMassDist();

    Vec2 compAcc(Body &one, Body &two);
    Vec2 compTreeAcc(Body &one);
    Vec2 calcAccOut(Body &one);

    void insertBody(Body &one, int level);
};
