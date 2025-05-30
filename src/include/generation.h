#pragma once
#include <cmath>
#include <vector>
#include "linmath.h"

// Jeez i gotta change this

Vec2 GeneratePositions_Ring(int number_objects, float radius) {
    Vec2 position_gen = {0, 0};
    float angle = 0.0;
    for (int i = 0; i < number_objects; i++) {
        position_gen.x = radius * std::cos(angle);
        position_gen.y = radius * std::sin(angle);
        angle += 2 * M_PI / number_objects;
    }
    return position_gen;
}

Vec2 GenerateVelocity_Curl(int number_objects, float radius) {
    Vec2 velocity_gen = {0, 0};
    float angle = 0.0;
    float v_0 = std::sqrtf(radius);
    for (int i = 0; i < number_objects; i++) {
        velocity_gen.y = v_0 * std::cos(angle);
        velocity_gen.x = v_0 * -std::sin(angle);
        angle += 2 * M_PI / number_objects;
    }
    return velocity_gen;
}

std::vector<Matrix> Generate_RingStack(int number_objects) {
    const float v_0 = 1.0 / 80.0;
    const float ring_spacing = 100.0f;
    float radius = 0.0f;
    float init_radius = 200.0f;

    Vec2 gen_position = {0, 0};
    Vec2 gen_velocity = {0, 0};

    int number_rings = 5;
    int ring_pop = static_cast<int> (number_objects / number_rings);
    int k = 0;

    // List of 2x2 matrices for each inital condition for each object
    std::vector<Matrix> Initial_Conditions (number_objects);
    for (int i = 0; i < number_rings; i++) {
        float angle = 0.0f;
        radius += 100.0f;
        for (int j = 0; j < ring_pop; j++) {
            
            gen_position.x = (radius + init_radius) * std::cos(angle);
            gen_position.y = (radius + init_radius) * std::sin(angle);

            // ehhh change this
            gen_velocity.x = v_0 * (radius + init_radius) * std::sin(angle);
            gen_velocity.y = -v_0 * (radius + init_radius) * std::cos(angle);
            angle += 2 * M_PI / ring_pop;

            // V1 is the first vector for position, and velocity would be V2 in the matrix
            Initial_Conditions[j + k].V1 = gen_position;
            Initial_Conditions[j + k].V2 = gen_velocity;

        }
            k += ring_pop;
    }
    return Initial_Conditions;
}

