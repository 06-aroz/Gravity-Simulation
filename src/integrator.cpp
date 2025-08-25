#include "../include/integrator.hpp"

// Integration
Vec2 compNetAccel(size_t i) {
    Vec2 net_acc = {0.0f, 0.0f};
    for (size_t j = 0; j < bodies.size(); j++) {
        if (j != i) {
            Vec2 acc_ij = GravAccel(bodies[i], bodies[j]);
            net_acc.x += acc_ij.x;
            net_acc.y += acc_ij.y;

            if (Screen_Settings.showVectors) {
                DrawVector(acc_ij, true, Screen_Settings.accelerationColor, bodies[i].position, bodies[i].radius);
                DrawVector(GravAccel(bodies[j], bodies[i]), true, Screen_Settings.accelerationColor, bodies[j].position, bodies[j].radius);
            }
        }
    }
    return net_acc;
}

// Leapfrog method
void halfStep(Body &b, Vec2 acc, float dt) {
    b.velocity.x += acc.x * (0.5f * dt);
    b.velocity.y += acc.y * (0.5f * dt);
}

void LeapFrogStep(float dt) {
    size_t n = bodies.size();
    std::vector<Vec2> acc(n);

    // First compute initial acceleration
    for (size_t i = 0; i < n; i++)
        acc[i] = compNetAccel(i);

    // First half step
    for (size_t i = 0; i < n; i++)
        halfStep(bodies[i], acc[i], dt);

    // Update position
    for (size_t i = 0; i < n; i++)
        bodies[i].updatePos(dt);

    // Next step
    for (size_t i = 0; i < n; i++)
        acc[i] = compNetAccel(i);

    for (size_t i = 0; i < n; i++)
        halfStep(bodies[i], acc[i], dt);
}

void EulerMethod(float dt) {
    // Acceleration algorithm, euler method
    for (size_t i = 0; i < bodies.size(); i++) {
        Vec2 net_acc = compNetAccel(i);
        bodies[i].updateVelocity(net_acc, dt);
        bodies[i].updatePos(dt);
    }
}

Vec2 min = {-100.0f * Screen_Settings.screenWidth / Screen_Settings.zoom, -100.0f * Screen_Settings.screenHeight / Screen_Settings.zoom};
Vec2 max = {100.0f * Screen_Settings.screenWidth / Screen_Settings.zoom, 100.0f * Screen_Settings.screenHeight / Screen_Settings.zoom};
BHNode root = BHNode(min, max, nullptr);

void buildTree(std::vector<Body> &bodies) {
    root.resetTree(min, max);

    for (auto &body : bodies) {
        root.insertBody(body, 0);
    }

    root.compMassDist();
}

// Significantly slower since we build the tree twice
void LeapFrogStepBH(float dt, std::vector<Body> &bodies) {

    // First compute initial acceleration
    buildTree(bodies);
    for (auto &body : bodies) {
        body.acc = root.calcAccOut(body);
    }

    // First half step
    for (auto &body : bodies)
        halfStep(body, body.acc, dt);

    // Update position
    for (auto &body : bodies)
        body.updatePos(dt);

    // Next step
    buildTree(bodies);
    for (auto &body : bodies) {
        body.acc = root.calcAccOut(body);
    }

    // First half step
    for (auto &body : bodies) {
        halfStep(body, body.acc, dt);
    }
}

void EulerStepBH(float dt, std::vector<Body> &bodies) {
    // Acceleration algorithm, euler method
    buildTree(bodies);
    for (auto &body : bodies) {
        Vec2 acc = root.calcAccOut(body);
        body.updateVelocity(acc, dt);
        body.updatePos(dt);
    }
}
