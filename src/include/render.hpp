#pragma once
#include "colors.hpp"
#include "computation.hpp"
#include "math.hpp"

#include <GLUT/glut.h>

void DrawCircle(Vec2 position, float radius, color circle_col) {
    int resolution = 16;
    glColor3d(circle_col.R, circle_col.G, circle_col.B);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(position.x, position.y);
    for (int i = 0; i <= resolution; ++i) {
        float angle = 2.0f * M_PI * (static_cast<float>(i) / resolution); // Converts the ints i and resolution to be
                                                                          // floats safely
        float x = position.x + radius * std::cos(angle);
        float y = position.y + radius * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

// Drawing the Vec2 arrow for the velocity
void DrawVector(Vec2 vect, bool isNormalized, color col, Vec2 position, float radius) {
    float addy, addx;
    float scale = 2.0f;
    float scale2 = 500.0f;
    if (isNormalized == true) {
        addx = 0.0f;
        addy = 0.0f;
    } else {
        addx = scale2 * vect.x;
        addy = scale2 * vect.y;
    }
    Vec2 normalizedV = Normalize(vect);
    float arrowLength = scale * radius;

    // The first ends of the Vec2 head
    float endX = position.x + arrowLength * normalizedV.x + addx;
    float endY = position.y + arrowLength * normalizedV.y + addy;

    glColor3f(col.R, col.G, col.B);
    glBegin(GL_LINES);
    // Draws main body
    glVertex2f(position.x, position.y);
    glVertex2f(endX, endY);
    glEnd();

    // Arrowhead parameters
    float arrowSize = radius * 0.4f;                        // length of the arrowhead lines
    float angle = std::atan2(normalizedV.y, normalizedV.x); // direction of the vector

    // Angles for arrowhead lines (in radians)
    float leftAngle = angle - M_PI * 3.0f / 4.0f;  // 135 degrees
    float rightAngle = angle + M_PI * 3.0f / 4.0f; // -135 degrees

    float leftX = endX + arrowSize * std::cos(leftAngle);
    float leftY = endY + arrowSize * std::sin(leftAngle);

    float rightX = endX + arrowSize * std::cos(rightAngle);
    float rightY = endY + arrowSize * std::sin(rightAngle);

    float enX1 = endX + 0.5 * radius * normalizedV.x;
    float enY1 = endY + 0.5 * radius * normalizedV.y;

    // Draw arrowhead
    glColor3f(col.R, col.G, col.B);
    glBegin(GL_TRIANGLES);
    glVertex2f(endX, endY);
    glVertex2f(enX1, enY1);
    glVertex2f(leftX, leftY);
    glEnd();

    glColor3f(col.R, col.G, col.B);
    glBegin(GL_TRIANGLES);
    glVertex2f(endX, endY);
    glVertex2f(enX1, enY1);
    glVertex2f(rightX, rightY);
    glEnd();
}

// Drawing the paths for each body
void DrawPath(Body one, color path_col) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int i = 0;
    size_t n = one.position_list.size();
    for (const auto &position : one.position_list) {
        float alpha = static_cast<float>(i) / n;
        glBegin(GL_LINE_STRIP);
        glColor4f(path_col.R, path_col.G, path_col.B, alpha);
        glVertex2f(position.x, position.y);
        i++;
    }
    glEnd();
}

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
    int ring_pop = static_cast<int>(number_objects / number_rings);
    int k = 0;

    // List of 2x2 matrices for each inital condition for each object
    std::vector<Matrix> Initial_Conditions(number_objects);
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

            // V1 is the first vector for position, and velocity would be V2 in
            // the matrix
            Initial_Conditions[j + k].V1 = gen_position;
            Initial_Conditions[j + k].V2 = gen_velocity;
        }
        k += ring_pop;
    }
    return Initial_Conditions;
}

// Making the bodies and creating init positions
std::vector<Body> generateBody(int number_objects, std::vector<Matrix> (*genFunction)(int number_objects)) {
    const float ring_dM = 100.0f;
    std::vector<Body> bodys;
    for (int i = 0; i < number_objects; i++) {
        color bod_col = randomColor();
        Body one = Body(genFunction(number_objects)[i].V1, genFunction(number_objects)[i].V2, ring_dM);
        bodys.push_back(one);
    }
    bodys.push_back(Body({0.0f, 0.0f}, {0.0f, 0.0f}, 1000.0f));
    return bodys;
}
