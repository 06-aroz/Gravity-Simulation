#include "../include/types.hpp"

// Constructors

// ORDER MATTERS HERE
SimParameters::SimParameters() : screenWidth(800.0f), screenHeight(800.0f), scale_time(60.0f) {}
SimParameters::SimParameters(float screenWidth, float scale_time) : screenWidth(screenWidth), scale_time(scale_time) {
    screenHeight = 0.75f * screenWidth;
}

BodyParameters::BodyParameters() : number_objects(0), max_distance(10e-9), color_preset(cP_mono) {}
BodyParameters::BodyParameters(int number_objects, float max_distance)
    : number_objects(number_objects), max_distance(max_distance), color_preset(cP_mono) {}

Body::Body() : position({0.0f, 0.0f}), velocity({0.0f, 0.0f}), acc({0.0f, 0.0f}), mass(0.0f), radius(0.0f), color({0.0f, 0.0f, 0.0f}) {}
Body::Body(Vec2 pos, Vec2 vel, float m) : position(pos), velocity(vel), mass(m), acc({0.0f, 0.0f}), color(default_color) {
    radius = std::max(min_radius, min_radius * std::sqrt(mass));
}

// Methods
void SimParameters::updateTime() {
    int cur_time = glutGet(GLUT_ELAPSED_TIME);
    dt = scale_time * static_cast<float>(cur_time - last_time) / 1000.0f;
    last_time = cur_time;
}

void Body::updateVelocity(Vec2 acceleration, float dt) {
    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;
}

void Body::updatePos(float dt) {
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (Screen_Settings.showPaths) {
        position_list.push_back(position);

        // Limit trail length
        if (position_list.size() > Body_Settings.path_length) {
            position_list.erase(position_list.begin());
        }
    } else {
        position_list.clear();
    }
}

void Body::addPosition(Vec2 update_vector) {
    position.x += update_vector.x;
    position.y += update_vector.y;
}
