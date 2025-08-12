#pragma once
#include "GLUT/glut.h"
#include <vector>

struct Vec2 {
    float x;
    float y;
};

struct Matrix {
    Vec2 V1;
    Vec2 V2;
};

struct color {
    float R;
    float G;
    float B;
};

class SimParameters {
  public:
    bool showVectors = false;
    bool showPaths = true;
    bool showObjs = true;
    bool applyGravity = true;
    bool isPaused = false;
    bool applyCollisions = false;
    bool applyBorder = false;
    bool centerCamera = false;
    bool showCM = true;

    float screenWidth;
    float screenHeight;
    float zoom = 1.0f;

    float G = 1.0f;
    float dt = 0.0f;
    float scale_time;
    int last_time = 0;

    const color bg_Color = {0.2, 0.2, 0.25};
    const color velocityColor = {0.2, 0.0, 1.0};
    const color accelerationColor = {1.0, 0.0, .25};

    SimParameters(float screenWidth, float scale_time) {
        this->screenWidth = screenWidth;
        this->screenHeight = 0.75 * screenWidth;
        this->scale_time = scale_time;
    }

    void updateTime() {
        int cur_time = glutGet(GLUT_ELAPSED_TIME);
        dt = scale_time * static_cast<float>(cur_time - last_time) / 1000.0f;
        last_time = cur_time;
    }
};

class BodyParameters {
  public:
    int path_length = 1000;
    int number_objects;
    float max_distance;
    const float mu = 0.5f; // dampening factor

    BodyParameters(int number_objects, float max_distance) {
        this->number_objects = number_objects;
        this->max_distance = max_distance;
    }
};
// Define the bodies class with position and velocity info

class Body {
  public:
    Vec2 position;
    Vec2 velocity;
    float radius;
    float mass;
    color color;

    std::vector<Vec2> position_list;

    Body(Vec2 position, Vec2 velocity, float mass) {
        this->position = position;
        this->velocity = velocity;
        this->mass = mass;

        this->radius = 0.1f * std::sqrt(mass);
        this->position_list.push_back(position);
    }

    void updateVelocity(Vec2 acceleration, float dt) {
        this->velocity.x += acceleration.x * dt;
        this->velocity.y += acceleration.y * dt;
    }

    void updatePos(float dt, int path_length) {
        this->position.x += this->velocity.x * dt;
        this->position.y += this->velocity.y * dt;
        this->position_list.push_back(position);

        // Limit trail length
        if (this->position_list.size() > path_length) {
            this->position_list.erase(this->position_list.begin());
        }
    }

    void addPosition(Vec2 update_vector) {
        this->position.x += update_vector.x;
        this->position.y += update_vector.y;
    }
};

SimParameters Screen_Settings = SimParameters(800, 60.0f);
BodyParameters Body_Settings = BodyParameters(2, 1e-9f);
std::vector<Body> bodies;
