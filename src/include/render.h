#pragma  once
#include "linmath.h"
#include <GLUT/glut.h>

void DrawCircle(Vec2 position, float radius, color circle_col) {
    int resolution = 100;
    glColor3d(circle_col.R, circle_col.G, circle_col.B);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(position.x, position.y);
    for (int i = 0; i <= resolution; ++i) {
        float angle = 2.0f * M_PI * (static_cast<float>(i) / resolution); // Converts the ints i and resolution to be floats safely
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
    Vec2 normalizedV = NormalizeVector(vect);
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
    float arrowSize = radius * 0.4f; // length of the arrowhead lines
    float angle = std::atan2(normalizedV.y, normalizedV.x); // direction of the vector

    // Angles for arrowhead lines (in radians)
    float leftAngle = angle - M_PI * 3.0f / 4.0f; // 135 degrees
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

