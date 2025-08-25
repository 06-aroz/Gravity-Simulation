#ifndef _TYPES_H
#define _TYPES_H

#include <GLUT/glut.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
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

enum Preset {
    cP_mono,
    cP_Chroma,
    cP_RedBlue,
    cP_holoPink,
};

class SimParameters {
  public:
    bool showVectors = false;
    bool showPaths = false;
    bool showObjs = true;
    bool isPaused = false;
    bool applyCollisions = false;
    bool applyBorder = false;
    bool centerCamera = false;
    bool showCM = true;
    bool showColor = false;

    // Are we running a Barnes-Hut sim?
    bool isBH = true;
    bool isEuler = false;
    bool isLeapFrog = !isEuler;

    float screenWidth;
    float screenHeight;
    float zoom = 1.0f;

    const float G = 1.0f;
    float dt = 0.0f;
    float scale_time;
    int last_time = 0;
    int particle_resolution = 16;

    const color bg_Color = {0.1, 0.1, 0.15};
    const color velocityColor = {0.2, 0.0, 1.0};
    const color accelerationColor = {1.0, 0.0, .25};

    SimParameters();
    SimParameters(float screenWidth, float scale_time);

    void updateTime();
};

class BodyParameters {
  public:
    const size_t path_length = 100;
    const float mu = 0.5f; // dampening factor
    const float max_speed = 50.0f;
    int number_objects;
    float max_distance;
    Preset color_preset;
    const color default_color = {1.0f, 1.0f, 1.0f};

    BodyParameters();
    BodyParameters(int number_objects, float max_distance);
};

// Define the bodies class with position and velocity info
class Body {
  private:
    color default_color = {1.0f, 1.0f, 1.0f};
    float min_radius = 0.1f;

  public:
    Vec2 position;
    Vec2 velocity;
    Vec2 acc;
    float radius;
    float mass;
    color color;

    std::vector<Vec2> position_list;

    Body();
    Body(Vec2 position, Vec2 velocity, float mass);

    // Euler's method implementation
    void updateVelocity(Vec2 acceleration, float dt);
    void updatePos(float dt);
    void addPosition(Vec2 update_vector);
};

// Inintialization for each class
inline SimParameters Screen_Settings = SimParameters();
inline BodyParameters Body_Settings = BodyParameters(1000, 1e-9f);
inline std::vector<Body> bodies;

#endif // !_TYPES_H
