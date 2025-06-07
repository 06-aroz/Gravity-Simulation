#pragma once

#include "definition.hpp"
#include "math.hpp"

#include <GLUT/glut.h>

void detectBorder(Body &one) {
    if (one.position.y + one.radius >= Screen_Settings.screenHeight / Screen_Settings.zoom) {
        one.position.y = (Screen_Settings.screenHeight / Screen_Settings.zoom - one.radius);
        one.velocity.y *= -Body_Settings.mu;
    }

    if (one.position.y - one.radius <= -Screen_Settings.screenHeight / Screen_Settings.zoom) {
        one.position.y = (-Screen_Settings.screenHeight / Screen_Settings.zoom + one.radius);
        one.velocity.y *= -Body_Settings.mu;
    }

    if (one.position.x + one.radius >= Screen_Settings.screenWidth / Screen_Settings.zoom) {
        one.position.x = (Screen_Settings.screenWidth / Screen_Settings.zoom - one.radius);
        one.velocity.x *= -Body_Settings.mu;
    }

    if (one.position.x - one.radius <= -Screen_Settings.screenWidth / Screen_Settings.zoom) {
        one.position.x = (-Screen_Settings.screenWidth / Screen_Settings.zoom + one.radius);
        one.velocity.x *= -Body_Settings.mu;
    }
}

float Distance(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float distance = std::sqrtf(delta_x * delta_x + delta_y * delta_y);
    return distance;
}

Vec2 GravAccel(Body &one, Body &two) {
    Vec2 acceleration;

    float dist = std::max(Body_Settings.max_distance, Distance(one, two));
    acceleration.x = -Screen_Settings.G * two.mass * (one.position.x - two.position.x) / (dist * dist * dist);
    acceleration.y = -Screen_Settings.G * two.mass * (one.position.y - two.position.y) / (dist * dist * dist);
    return acceleration;
}
bool CheckCollision(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float rad_sum = one.radius + two.radius;
    return delta_x * delta_x + delta_y * delta_y <= rad_sum * rad_sum + 0.5 * Body_Settings.max_distance;
}

Matrix ElasticCheck(Body &one, Body &two) {
    Matrix velocity_matrix;

    float mass_difference = one.mass - two.mass;
    float mass_sum = one.mass + two.mass;

    Vec2 velocity_difference = one.velocity - two.velocity;
    Vec2 position_difference = one.position - two.position;

    float vector_scale = 2.0f / mass_sum * DotProduct(velocity_difference, position_difference) /
                         (position_difference.x * position_difference.x + position_difference.y * position_difference.y);

    const Vec2 &velocity1 = one.velocity - position_difference * two.mass * vector_scale;
    const Vec2 &velocity2 = two.velocity + position_difference * one.mass * vector_scale;

    velocity_matrix.V1 = velocity1;
    velocity_matrix.V2 = velocity2;

    return velocity_matrix;
}

Matrix PositionCollide(Body &one, Body &two) {
    Matrix position_matrix;
    const float &dX = one.position.x - two.position.x;
    const float &dY = one.position.y - two.position.y;
    float min_dist = one.radius + two.radius;
    const float &distsq = dX * dX + dY * dY;

    if (distsq < min_dist * min_dist) {
        float dist = std::sqrt(distsq);
        Vec2 Normal_dr = ScaleVector({dX, dY}, 1.0 / dist); // normalizing the position vector
        float mass_ratio = 1.0 / (1.0 + (two.radius / one.radius) * (two.radius / one.radius));
        float delta = 0.5f * (min_dist - dist);

        // Mass ratio allows objects of larger size to move less
        one.position += Normal_dr * (1 - mass_ratio) * delta;
        two.position -= Normal_dr * mass_ratio * delta;
    }

    position_matrix.V1 = one.position;
    position_matrix.V2 = two.position;

    return position_matrix;
}
