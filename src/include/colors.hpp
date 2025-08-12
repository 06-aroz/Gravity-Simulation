#pragma once
#include "definition.hpp"
#include "math.hpp"

#include <random>

/*
color randomColor() {
    static std::random_device rd; // only used once to seed the generator
    // should prob change this later
    static std::mt19937 gen(rd()); // Mersenne Twister RNG
    std::uniform_real_distribution<float> dist(0.25, 1.0);

    color rand_color;
    rand_color.R = dist(gen);
    rand_color.G = dist(gen);
    rand_color.B = dist(gen);
    return rand_color;
}

std::vector<float> weightFunction(float t) {
    const int number_components = 6;
    std::vector<float> weights(7, 0.0f);

    t = std::clamp(t, 0.0f, 1.00f);
    float scaled = t * number_components;
    int component_index = static_cast<int>(scaled);
    float local_t = scaled - component_index;

    if (component_index >= 6)
        component_index = 5, local_t = 1.00f;

    weights[component_index] = 1.0f - local_t;
    weights[component_index + 1] = local_t;

    return weights;
}

color vectorMapper(float t) {
    // Defining our basis Colors in a matrix
    float col_mat[7][3] = {{0.0f, 0.4f, 0.9f}, {0.0f, 0.75f, 0.9f}, {0.0f, 1.0f, 0.8f}, {0.5f, 1.0f, 0.4f},
                           {0.9f, 1.0f, 0.4f}, {1.0f, 0.9f, 0.4f},  {1.0f, 0.4f, 0.4f}};
    std::vector<float> w_vec = weightFunction(t);
    float col_vec[3] = {0.0, 0.0, 0.0};

    // Performing the matrix transformation
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            col_vec[i] += col_mat[j][i] * w_vec[j];
        }
    }

    return {col_vec[0], col_vec[1], col_vec[2]};
}
*/

// Various mapping methods
color mapColors(Vec2 obj_velocity, float max_speed, color (*Mapper)(float)) {
    float obj_speedsq = DotProduct(obj_velocity, obj_velocity);
    float t = obj_speedsq > 4 *max_speed *max_speed ? 1.0f : t = obj_speedsq / (max_speed * max_speed);
    return Mapper(t);
}

color mapPeriodColors(float t, color (*Mapper)(float)) {
    float theta = std::cos(t) * std::cos(t);

    return Mapper(theta);
}

color mapCosine(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    float phase_R = M_PI;
    float phase_G = M_PI_2;
    float phase_B = M_PI_4;

    float ang_R = 1.0f;
    float ang_G = 1.0f;
    float ang_B = 1.0f;

    float amp_R = 1.0f;
    float amp_G = 1.0f;
    float amp_B = 1.0f;

    return {
        0.5f * (1.0f + amp_R * std::cosf(ang_R * M_PI * t - phase_R)),
        0.5f * (1.0f + amp_G * std::cosf(ang_G * M_PI * t - phase_G)),
        0.5f * (1.0f + amp_B * std::cosf(ang_B * M_PI * t - phase_B)),
    };
}
