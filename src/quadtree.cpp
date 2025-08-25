#include "../include/quadtree.hpp"

// Barnes-Hut Algorithm
BHNode::BHNode(Vec2 _min, Vec2 _max, BHNode *_parent)
    : parent(_parent), mass(0.0f), min(_min), max(_max), center({0.5f * (max.x + min.x), 0.5f * (max.y + min.y)}), CM_pos({0.0f, 0.0f}),
      num_bodies(0), is_subdivided(false) {
    quadNode[0] = quadNode[1] = quadNode[2] = quadNode[3] = nullptr;
}

BHNode::~BHNode() {
    for (int i = 0; i < 4; i++) {
        delete quadNode[i];
        quadNode[i] = nullptr;
    }
}

bool BHNode::isRoot() {
    return (parent == nullptr);
}

bool BHNode::isExternal() {
    return (quadNode[0] == nullptr && quadNode[1] == nullptr && quadNode[2] == nullptr && quadNode[3] == nullptr);
}

void BHNode::resetTree(Vec2 _min, Vec2 _max) {
    if (!isRoot()) {
        throw std::runtime_error("ERROR! Only the ROOT can reset the tree!");
    }

    for (int i = 0; i < 4; i++) {
        delete quadNode[i];
        quadNode[i] = nullptr;
    }

    min = _min;
    max = _max;

    center = {0.5f * (max.x + min.x), 0.5f * (max.y + min.y)};
    CM_pos = {0.0f, 0.0f};
    mass = 0.0f;
    renegades.clear();
}

BHNode::quad_type BHNode::getQuad(Vec2 pos) {
    if (pos.x <= center.x && pos.y <= center.y) {
        return SW;
    }
    if (pos.x >= center.x && pos.y <= center.y) {
        return SE;
    }
    if (pos.x <= center.x && pos.y >= center.y) {
        return NW;
    }
    if (pos.x >= center.x && pos.y >= center.y) {
        return NE;

    } else if (pos.x > max.x || pos.x < min.x || pos.y > max.y || pos.y < min.y) {
        std::cout << "Quadrant cannot be determined" << std::endl;
        std::cout << "Particle at: (" << pos.x << ", " << pos.y << ")" << std::endl;
        std::cout << "Quadrant min: (" << min.x << ", " << min.y << ")" << std::endl;
        std::cout << "Quadrant max: (" << max.x << ", " << max.y << ")" << std::endl;
        std::exit(1);
    } else {
        std::cout << "Quadrant cannot be determined" << std::endl;
        std::exit(1);
    }
}

BHNode *BHNode::createQuadNode(quad_type loc) {
    switch (loc) {
    case SW:
        return new BHNode(min, center, this);
    case NW:
        return new BHNode({min.x, center.y}, {center.x, max.y}, this);
    case NE:
        return new BHNode(center, max, this);
    case SE:
        return new BHNode({center.x, min.y}, {max.x, center.y}, this);
    default:
        std::cout << "Quadrant cannot be determined" << std::endl;
    }
}

// Shoutout to my glorious king beltoforian
void BHNode::compMassDist() {
    if (num_bodies == 1) {
        mass = body.mass;
        CM_pos = body.position;
    } else {
        mass = 0.0f;
        CM_pos = {0.0f, 0.0f};
        for (int i = 0; i < 4; i++) {
            if (quadNode[i]) {
                quadNode[i]->compMassDist();
                mass += quadNode[i]->mass;
                CM_pos.x += quadNode[i]->CM_pos.x * quadNode[i]->mass;
                CM_pos.y += quadNode[i]->CM_pos.y * quadNode[i]->mass;
            }
        }
    }

    CM_pos.x /= mass;
    CM_pos.y /= mass;
}

Vec2 BHNode::compAcc(Body &one, Body &two) {
    // If things break, add a new member to body being acceleration?
    Vec2 acc = {0.0f, 0.0f};

    if (&one == &two) {
        return acc;
    }

    float dist = std::max(Body_Settings.max_distance, Distance(one, two));
    acc.x += -Screen_Settings.G * two.mass * (one.position.x - two.position.x) / (dist * dist * dist);
    acc.y += -Screen_Settings.G * two.mass * (one.position.y - two.position.y) / (dist * dist * dist);

    return acc;
}

Vec2 BHNode::compTreeAcc(Body &one) {
    Vec2 acc = {0.0f, 0.0f};
    float box_dist = 0.0f;
    float dist = 0.0f;

    if (num_bodies == 1) {
        acc = compAcc(one, body);
    } else {
        float dx = one.position.x - CM_pos.x;
        float dy = one.position.y - CM_pos.y;
        float dist_sq = dx * dx + dy * dy;
        box_dist = max.x - min.x;

        if (box_dist * box_dist < theta_sq * dist_sq) {
            dist = std::max(epsilon, std::sqrt(dist_sq));
            is_subdivided = false;
            acc.x = -dx * Screen_Settings.G * mass / (dist * dist * dist);
            acc.y = -dy * Screen_Settings.G * mass / (dist * dist * dist);
        } else {
            // Criteron failed so now compute the child nodes
            is_subdivided = true;
            Vec2 quad_acc = {0.0f, 0.0f};
            for (int i = 0; i < 4; i++) {
                if (quadNode[i] != nullptr) {
                    quad_acc = quadNode[i]->compTreeAcc(one);
                    acc.x += quad_acc.x;
                    acc.y += quad_acc.y;
                }
            }
        }
    }

    return acc;
}

Vec2 BHNode::calcAccOut(Body &one) {
    Vec2 acc = compTreeAcc(one);

    if (renegades.size()) {
        for (size_t i = 0; i < renegades.size(); i++) {
            Vec2 out_acc = compAcc(one, renegades[i]);
            acc.x += out_acc.x;
            acc.y += out_acc.y;
        }
    }

    return acc;
}

void BHNode::insertBody(Body &one, int level) {
    // If the body is outside our range
    if (one.position.x < min.x || one.position.x > max.x || one.position.y < min.y || one.position.y > max.y) {
        std::cout << "ERROR! Body is out of bounds" << std::endl;
        std::cout << "Body Position:" << "(" << one.position.x << ", " << one.position.y << ")" << std::endl;
        std::cout << "Node Min:" << "(" << min.x << ", " << min.y << ")" << std::endl;
        std::cout << "Node Max:" << "(" << max.x << ", " << max.y << ")" << std::endl;
        // std::exit(1);
    }

    if (num_bodies == 0) {
        body = one;
    }

    else if (num_bodies > 1) {
        quad_type quad_pos = getQuad(one.position);
        if (!quadNode[quad_pos]) {
            quadNode[quad_pos] = createQuadNode(quad_pos);
        }

        quadNode[quad_pos]->insertBody(one, level + 1);

    } else if (num_bodies == 1) {
        if (isExternal() || isRoot()) {
            Body two = body;
            if (one.position.x == two.position.x && one.position.y == two.position.y) {
                std::cout << "TWO PARTICLES IN THE SAME POSITION!" << std::endl;
                renegades.push_back(one);
            } else {
                quad_type quad_pos = getQuad(two.position);
                if (quadNode[quad_pos] == nullptr)
                    quadNode[quad_pos] = createQuadNode(quad_pos);
                quadNode[quad_pos]->insertBody(body, level + 1);

                quad_pos = getQuad(one.position);
                if (!quadNode[quad_pos]) {
                    quadNode[quad_pos] = createQuadNode(quad_pos);
                }
                quadNode[quad_pos]->insertBody(one, level + 1);
            }

        } else {
            std::cout << "ERROR! Not an external node or ROOT!" << std::endl;
            std::exit(1);
        }
    }

    num_bodies++;
}
