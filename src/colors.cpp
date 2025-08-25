#include "../include/colors.hpp"

// Yeah ChatGPT generated this, it works so whatever
color randomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.25, 1.0);

    color rand_color;
    rand_color.R = dist(gen);
    rand_color.G = dist(gen);
    rand_color.B = dist(gen);
    return rand_color;
}

color mapColors(Vec2 obj_velocity, color (*Mapper)(float)) {
    float max_speed = Body_Settings.max_speed;
    float obj_speedsq = DotProduct(obj_velocity, obj_velocity);
    float t = obj_speedsq > 4 *max_speed *max_speed ? 1.0f : t = obj_speedsq / (max_speed * max_speed);
    return Mapper(t);
}

color mapPeriodColors(float t, color (*Mapper)(float)) {
    float theta = std::cos(t) * std::cos(t);
    return Mapper(theta);
}

color mapCosine(float t) {
    // Force t to be from 0 to 1
    t = std::clamp(t, 0.0f, 1.0f);
    float phase[3], ang[3], amp[3];

    switch (Body_Settings.color_preset) {

    case cP_mono:
        phase[0] = phase[1] = phase[2] = 0.0f;
        amp[0] = amp[1] = amp[2] = -1.0f;
        ang[0] = ang[1] = ang[2] = 1.0f;
        break;
    case cP_Chroma:
        amp[0] = amp[1] = amp[2] = 1.0f;
        ang[0] = ang[1] = ang[2] = 1.0f;
        phase[0] = 1.0f, phase[1] = 0.5f, phase[2] = 0.0f;
        break;
    case cP_RedBlue:
        amp[0] = amp[1] = amp[2] = 1.0f;
        ang[0] = ang[1] = ang[2] = 1.0f;
        phase[0] = 1.0f, phase[1] = -0.5f, phase[2] = 0.0f;
        break;
    case cP_holoPink:
        amp[0] = amp[1] = 1.0f, amp[2] = -1.0f;
        ang[0] = ang[1] = ang[2] = 1.0f;
        phase[0] = 0.5f, phase[1] = 1.0f, phase[2] = -0.1f;
        break;
    default:
        cP_mono;
    }

    return {
        0.5f * (1.0f + amp[0] * std::cosf(M_PI * (ang[0] * t - phase[0]))),
        0.5f * (1.0f + amp[1] * std::cosf(M_PI * (ang[1] * t - phase[1]))),
        0.5f * (1.0f + amp[2] * std::cosf(M_PI * (ang[2] * t - phase[2]))),
    };
}
