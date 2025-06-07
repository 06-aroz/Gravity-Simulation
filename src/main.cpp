#include <GLUT/glut.h>
#include <iostream>
#include <vector>

#include "include/colors.hpp"
#include "include/computation.hpp"
#include "include/definition.hpp"
#include "include/math.hpp"
#include "include/render.hpp"

std::vector<Body> bodies = generateBody(Body_Settings.number_objects, Generate_RingStack);

void spawnBody(Vec2 position, float mass) { bodies.push_back(Body(position, {0, 0}, mass)); };

void despawnBody() {
    if (!bodies.empty()) {
        bodies.erase(bodies.end() - 1);
    }
}

void detectCollision() {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            if (CheckCollision(bodies[i], bodies[j])) {
                Matrix updatedvelocity = ElasticCheck(bodies[i], bodies[j]);

                bodies[i].velocity = updatedvelocity.V1;
                bodies[j].velocity = updatedvelocity.V2;

                Matrix updatedposition = PositionCollide(bodies[i], bodies[j]);
                bodies[i].position = updatedposition.V1;
                bodies[j].position = updatedposition.V2;
            }
        }
    }
}

// Acceleration algorithm
// IF BREAKS MAKE DT GLOBAL
void applyAcceleration(float dt) {
    if (Screen_Settings.applyGravity) {
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                bodies[i].updateVelocity(GravAccel(bodies[i], bodies[j]), dt);
                bodies[j].updateVelocity(GravAccel(bodies[j], bodies[i]), dt);

                if (Screen_Settings.showVectors) {
                    DrawVector(GravAccel(bodies[i], bodies[j]), false, Screen_Settings.accelerationColor, bodies[i].position, bodies[i].radius);
                    DrawVector(GravAccel(bodies[j], bodies[i]), false, Screen_Settings.accelerationColor, bodies[j].position, bodies[j].radius);
                }
            }
        }
    }
}

// Mouse click logic
void onClick(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            float mouse_x = static_cast<float>(x);
            float mouse_y = static_cast<float>(y);
            float true_x = (2.0f * mouse_x - Screen_Settings.screenWidth) / Screen_Settings.zoom;
            float true_y = (-2.0f * mouse_y + Screen_Settings.screenHeight) / Screen_Settings.zoom;
            spawnBody({true_x, true_y}, 1000);
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
            despawnBody();
        }
        break;
    }
}

void cameraMovement(bool is_left, bool is_right, bool is_up, bool is_down, Body &one) {
    float cam_amount = 20.0f / Screen_Settings.zoom;
    if (is_left) {
        one.addPosition({-cam_amount, 0});
    }
    if (is_right) {
        one.addPosition({cam_amount, 0});
    }
    if (is_up) {
        one.addPosition({0, cam_amount});
    }
    if (is_down) {
        one.addPosition({0, -cam_amount});
    }
}

bool is_up = false, is_down = false, is_left = false, is_right = false;

void handleKeyRelease(unsigned char key, int x, int y) {
    switch (key) {
    case 'G':
        Screen_Settings.G = 1.0f;
        break;

    case 'd':
        is_right = false;
        break;
    case 'a':
        is_left = false;
        break;
    case 'w':
        is_up = false;
        break;
    case 's':
        is_down = false;
        break;

    case 'D':
        is_right = false;
        break;
    case 'A':
        is_left = false;
        break;
    case 'W':
        is_up = false;
        break;
    case 'S':
        is_down = false;
        break;
    }
}

// Keypress logic
void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC
        std::cout << "Exiting program." << std::endl;
        exit(0);

    case 'r':
        bodies.clear();
        break;

    case 'v':
        Screen_Settings.showVectors = !Screen_Settings.showVectors;
        break;

    case 'p':
        Screen_Settings.showPaths = !Screen_Settings.showPaths;
        break;

    case 'o':
        Screen_Settings.showObjs = !Screen_Settings.showObjs;
        break;

    case 'g':
        Screen_Settings.applyGravity = !Screen_Settings.applyGravity;
        break;

    case ' ':
        Screen_Settings.isPaused = !Screen_Settings.isPaused;
        break;

    case 'q':
        Screen_Settings.showCM = !Screen_Settings.showCM;
        break;

    case 'Q':
        Screen_Settings.centerCamera = !Screen_Settings.centerCamera;
        break;

    case 'b':
        Screen_Settings.applyBorder = !Screen_Settings.applyBorder;
        break;

    case 'c':
        Screen_Settings.applyCollisions = !Screen_Settings.applyCollisions;
        break;

    case 'R':
        Screen_Settings.zoom = 1.0;
        bodies.clear();
        bodies = generateBody(Body_Settings.number_objects, Generate_RingStack);
        break;

    case 'G':
        Screen_Settings.G += 0.5f;
        break;
    case '+':
        Screen_Settings.zoom *= 1.5f;
        break;
    case '_':
        Screen_Settings.zoom /= 1.5f;
        break;

    // Camera movement flags
    case 'd':
        is_right = true;
        break;
    case 'a':
        is_left = true;
        break;
    case 'w':
        is_up = true;
        break;
    case 's':
        is_down = true;
        break;

    case 'D':
        is_right = true;
        break;
    case 'A':
        is_left = true;
        break;
    case 'W':
        is_up = true;
        break;
    case 'S':
        is_down = true;
        break;
    }

    if (key != 'G') {
        Screen_Settings.G = 1.0f;
    }
}
void mainDisplay(std::vector<Body> &bodies) {
    Screen_Settings.updateTime();
    float elapsed_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    for (auto &body : bodies) {
        color body_col = mapColors(body.velocity, 10);
        if (Screen_Settings.showObjs) {
            DrawCircle(body.position, body.radius, body_col);
        }

        if (Screen_Settings.showVectors) {
            DrawVector(body.velocity, true, Screen_Settings.velocityColor, body.position, body.radius);
        }

        if (Screen_Settings.showPaths) {
            DrawPath(body, body_col);
        }

        if (Screen_Settings.applyCollisions) {
            detectCollision();
        }

        if (Screen_Settings.applyBorder) {
            detectBorder(body);
        }

        if (!Screen_Settings.isPaused) {
            applyAcceleration(Screen_Settings.dt);
            body.updatePos(Screen_Settings.dt, Body_Settings.path_length);
        }

        cameraMovement(is_left, is_right, is_up, is_down, body);
    }

    float mass_sum = 0.0f;
    Vec2 CM_position = {0.0f, 0.0f};

    // MAKE INTO FUNCTION???
    for (auto &bodies : bodies) {
        mass_sum += bodies.mass;
        CM_position.x += bodies.mass * bodies.position.x;
        CM_position.y += bodies.mass * bodies.position.y;
    }

    if (mass_sum != 0.0f) {
        CM_position.x /= mass_sum;
        CM_position.y /= mass_sum;
    }

    if (Screen_Settings.centerCamera) {
        DrawCircle(CM_position, 5.0f, {0.0, 1.0, 0.2});
        for (auto &b : bodies) {
            b.addPosition(ScaleVector(CM_position, -1.0f));
        }
    } else {
        DrawCircle(CM_position, 5.0f, {1.0, 1.0, 1.0});
    }
}

void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glClearColor(Screen_Settings.bg_Color.R, Screen_Settings.bg_Color.G, Screen_Settings.bg_Color.B, 0.5);
    glScalef(Screen_Settings.zoom, Screen_Settings.zoom, 1.0f);
    mainDisplay(bodies);
    glutSwapBuffers();
}

void checkResize(int new_width, int new_height) {
    Screen_Settings.screenWidth = new_width;
    Screen_Settings.screenHeight = new_height;
    glViewport(0, 0, new_width, new_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-new_width, new_width, -new_height, new_height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay(); // refresh window.
}

// FIX THIS LATER
void StartGLUT(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(Screen_Settings.screenWidth, Screen_Settings.screenHeight);
    glutCreateWindow("Gravity Simulation");
    glutInitWindowPosition(100, 100);

    glMatrixMode(GL_PROJECTION);

    glOrtho(-Screen_Settings.screenWidth, Screen_Settings.screenWidth, -Screen_Settings.screenHeight, Screen_Settings.screenHeight, -1.0f, 1.0f);

    glutDisplayFunc(displayCallback);
    glutIdleFunc(glutPostRedisplay);
    glutReshapeFunc(checkResize);

    glutKeyboardFunc(handleKeypress);
    glutKeyboardUpFunc(handleKeyRelease);
    glutMouseFunc(onClick);
}

void showMessage() {
    std::cout << "=============================================" << std::endl;
    std::cout << "              Simulation Started" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "LC to add bodies, RC to remove bodies" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "                  Controls" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "WASD          : Move camera" << std::endl;
    std::cout << "ESC           : Exit" << std::endl;
    std::cout << "R             : Reset simulation" << std::endl;
    std::cout << "r             : Clear all bodies" << std::endl;
    std::cout << "SPACE         : Pause/unpause" << std::endl;
    std::cout << "g             : Toggle gravity" << std::endl;
    std::cout << "Hold G        : Increase gravity strength" << std::endl;
    std::cout << "c             : Toggle collisions" << std::endl;
    std::cout << "b             : Toggle border collisions" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "               Visual Controls" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "v             : Show/hide vectors" << std::endl;
    std::cout << "p             : Show/hide paths" << std::endl;
    std::cout << "o             : Show/hide bodies" << std::endl;
    std::cout << "q             : Show/hide centre of mass" << std::endl;
    std::cout << "Q             : Center camera to CM" << std::endl;
    std::cout << "=============================================" << std::endl;
}

int main(int argc, char **argv) {
    showMessage();
    StartGLUT(argc, argv);
    glutMainLoop();

    return 0;
}
