#pragma once 
#include <cmath>

struct color {
    float R;
    float G;
    float B;
};

struct Vec2 {
    float x;
    float y;
};

struct Matrix {
    Vec2 V1;
    Vec2 V2;
};

float DotProduct(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 VectorSum(Vec2 a, Vec2 b, bool difference) {
    Vec2 Sum;
    if (difference) {
        Sum.x = a.x - b.x;
        Sum.y = a.y - b.y;
    } else {
        Sum.x = a.x + b.x;
        Sum.y = a.y + b.y;
    }
    return Sum;
}

Vec2 ScaleVector(Vec2 a, float scalar) {
    Vec2 Scaled;
    Scaled.x = scalar * a.x;
    Scaled.y = scalar * a.y;
    return Scaled;
}

float VectorMagnitude(Vec2 V) {
    return std::sqrt(V.x * V.x + V.y * V.y);
}

Vec2 NormalizeVector(Vec2 V) {
    float magnitude = VectorMagnitude(V);
    Vec2 NormV;
    if (magnitude == 0) {
        NormV.x = 0;
        NormV.y = 0;
    } else {
        NormV.x = V.x / magnitude;
        NormV.y = V.y / magnitude;
    }
    return NormV;
}


