#pragma once
#include "definition.hpp"
#include "math.hpp"

#include <GLUT/glut.h>
#include <cmath>

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
    if (isNormalized) {
        addx = 0.0f;
        addy = 0.0f;
    } else {
        addx = scale2 * vect.x / Screen_Settings.zoom;
        addy = scale2 * vect.y / Screen_Settings.zoom;
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

// Object generation
void spawnBody(Vec2 position, float mass) {
    bodies.push_back(Body(position, {0, 0}, mass));
};

void despawnBody() {
    if (!bodies.empty()) {
        bodies.erase(bodies.end() - 1);
    }
}

float OrbitalSpeed(float radius, float mass, bool normalize) {
    // Checks if we normalize the speed in terms of AU. Speed in km/s
    return normalize == true ? 30.0 / std::sqrt(radius) : std::sqrt(Screen_Settings.G * mass / radius);
}

std::vector<Matrix> Generate_CurlRing(int number_objects) {
    const float ring_spacing = 10.0f;
    float radius = 0.0f;
    float angle = 0.0;
    float init_radius = 800.0f;

    Vec2 gen_position;
    Vec2 gen_velocity;
    std::vector<Matrix> Initial_Conditions(number_objects);
    for (int i = 0; i < number_objects; i++) {
        float v_0 = OrbitalSpeed(radius + init_radius, 2000, false);
        gen_position.x = (radius + init_radius) * std::cos(angle);
        gen_position.y = (radius + init_radius) * std::sin(angle);
        gen_velocity.x = v_0 * -std::sin(angle);
        gen_velocity.y = v_0 * std::cos(angle);
        radius += ring_spacing;
        angle += 2 * M_PI / number_objects;

        Initial_Conditions[i].V1 = gen_position;
        Initial_Conditions[i].V2 = gen_velocity;
    }
    return Initial_Conditions;
}

std::vector<Matrix> Generate_RingStack(int number_objects) {
    const float ring_spacing = 200.0f;
    float radius = 250.0f;
    Vec2 gen_position;
    Vec2 gen_velocity;

    int number_rings = 2;
    int ring_pop = static_cast<int>(number_objects / number_rings);
    int k = 0;

    // List of 2x2 matrices for each inital condition for each object
    std::vector<Matrix> Initial_Conditions(number_objects);
    for (int i = 0; i < number_rings; i++) {
        float angle = 0.0f;
        radius += ring_spacing;
        for (int j = 0; j < ring_pop; j++) {

            // Fix b/c if number_objects % number_rings !=0 then we get obj at centre
            gen_position.x = radius * std::cos(angle);
            gen_position.y = radius * std::sin(angle);
            float v_0 = 0.005f;
            // ehhh change this
            gen_velocity.x = radius * v_0 * std::sin(angle);
            gen_velocity.y = -v_0 * radius * std::cos(angle);
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

/* FIGURE OUT HOW TO NORMALIZE ALL POSITIONS/FORCES IN TERMS OF AU
std::vector<Body> GenerateSS() {
    std::vector<Body> Planets;
    float P_radii[8] = {0.45, 0.72, 1.0, 1.5, 5.2, 9.5, 19.0, 30.0};         // In AU
    float P_masses[8] = {0.055, 0.815, 1.0, 0.107, 317.8, 95.2, 14.5, 17.1}; // In Earth masses
    float P_speeds[8] = {0};

    float angle = 0.0;
    Vec2 P_velocity[8];
    Vec2 P_position[8];

    // Normalize the radii
    for (int i = 0; i < 8; i++) {
        // Define 1 AU = 200 units
        P_radii[i] *= 200;
        P_speeds[i] = OrbitalSpeed(P_radii[i], P_masses[i], true);
        angle = randomRange(0, 2 * M_PI);

        // Generate orbital velocities and positions at a random angle
        P_velocity[i].x = P_speeds[i] * std::sin(angle);
        P_velocity[i].y = -P_speeds[i] * std::cos(angle);

        P_position[i].x = P_radii[i] * std::cos(angle);
        P_position[i].y = P_radii[i] * std::sin(angle);

        Planets.push_back(Body(P_position[i], P_velocity[i], P_masses[i]));
        Planets[i].radius = 15.0f;
    }

    // Randomize the angle for which the planets are aligned

    const Body &Sun = Body({0, 0}, {0, 0}, 330000.0);
    Planets.push_back(Sun);

    return Planets;
}
*/

// Making the bodies and creating init positions
std::vector<Body> generateBody(int number_objects, std::vector<Matrix> (*genFunction)(int number_objects)) {
    const float ring_dM = 100.0f;
    std::vector<Body> bodies;
    for (int i = 0; i < number_objects; i++) {
        Body one = Body(genFunction(number_objects)[i].V1, genFunction(number_objects)[i].V2, ring_dM);
        bodies.push_back(one);
    }
    return bodies;
}

// Defining a fixed array of known periodic initial conditions
std::array<std::array<Body, 3>, 4> Stable_Period_IC = {
    {// 1. Figure-8 Orbit
     {{Body({-0.97000436f, 0.24308753f}, {-0.46620368f, -0.43236573f}, 1.0f), Body({0.97000436f, -0.24308753f}, {-0.46620368f, -0.43236573f}, 1.0f),
       Body({0.0f, 0.0f}, {0.93240737f, 0.86473146f}, 1.0f)}},

     // 2. Broucke A1 (symmetric collinear)
     {{Body({-1.0f, 0.0f}, {0.0f, -0.347111f}, 1.0f), Body({1.0f, 0.0f}, {0.0f, -0.347111f}, 1.0f), Body({0.0f, 0.0f}, {0.0f, 0.694222f}, 1.0f)}},

     // 3. Butterfly Orbit
     {{Body({-0.1339746f, 0.0f}, {0.0f, 0.24999999f}, 1.0f), Body({0.0669873f, 0.11547005f}, {0.21650635f, -0.125f}, 1.0f),
       Body({0.0669873f, -0.11547005f}, {-0.21650635f, -0.125f}, 1.0f)}},

     // 4. Lagrange Equilateral Triangle Orbit
     {{Body({-0.5f, 0.0f}, {0.0f, -0.28867513f}, 1.0f), Body({0.5f, 0.0f}, {0.0f, -0.28867513f}, 1.0f),
       Body({0.0f, 0.8660254f}, {0.0f, 0.57735026f}, 1.0f)}}}};

void fixGen(int index) {
    for (int i = 0; i < 3; i++) {
        bodies.push_back(Stable_Period_IC[index][i]);
    }
}
