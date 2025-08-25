#include "../include/math.hpp"

float DotProduct(const Vec2 &V1, const Vec2 &V2) {
    return V1.x * V2.x + V1.y * V2.y;
}

float VectorMagnitude(Vec2 V) {
    return std::sqrt(DotProduct(V, V));
}

float compDist(Vec2 V1, Vec2 V2) {
    float dx = V1.x - V2.x;
    float dy = V1.y - V2.y;
    return std::sqrt(dx * dx + dy * dy);
}

Vec2 ScaleVector(Vec2 V1, float scalar) {
    return {V1.x * scalar, V1.y * scalar};
}

Vec2 Normalize(Vec2 V) {
    return {V.x / VectorMagnitude(V), V.y / VectorMagnitude(V)};
}

Vec2 VectorSum(Vec2 V1, Vec2 V2, bool is_difference) {
    Vec2 sum;
    if (is_difference) {
        V2.x *= -1;
        V2.y *= -1;
    }
    sum.x = V1.x + V2.x;
    sum.y = V1.y + V2.y;
    return sum;
}

float Distance(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    return distance;
}

// Yeah ChatGPT made this, it works so who cares
float randomRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}
