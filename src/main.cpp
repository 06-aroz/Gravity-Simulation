#include "../include/colors.hpp"
#include "../include/computation.hpp"
#include "../include/controls.hpp"
#include "../include/integrator.hpp"
#include "../include/rendering.hpp"
#include "../include/types.hpp"

#include <GLUT/glut.h>
#include <fstream>
#include <iostream>
#include <vector>

std::ofstream file("energy_output.txt");

void mainLoop(std::vector<Body> &bodies) {
    Screen_Settings.dt = 0.05f;
    color body_col;
    for (auto &body : bodies) {
        body_col = (Screen_Settings.showColor) ? mapColors(body.velocity, mapCosine) : Body_Settings.default_color;

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

        cameraMovement(is_left, is_right, is_up, is_down, body);

        // Printing out data
        // const Vec2 &sum_Energy = TotalEnergy();
        // file << (glutGet(GLUT_ELAPSED_TIME) / 1000.0f) << " " << sum_Energy.x << " " << sum_Energy.y << " " << sum_Energy.x + sum_Energy.y
        //      << std::endl;
    }

    if (!Screen_Settings.isPaused) {
        EulerStepBH(Screen_Settings.dt, bodies);
    }
    // renderCenterMass();
}

void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glClearColor(Screen_Settings.bg_Color.R, Screen_Settings.bg_Color.G, Screen_Settings.bg_Color.B, 0.5);
    glScalef(Screen_Settings.zoom, Screen_Settings.zoom, 1.0f);
    mainLoop(bodies);
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

// FIX THIS LATER?
void StartGLUT(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(Screen_Settings.screenWidth, Screen_Settings.screenHeight);
    glutCreateWindow("Gravity Simulation");
    glutInitWindowPosition(0, 0);

    glMatrixMode(GL_PROJECTION);

    glOrtho(-Screen_Settings.screenWidth, Screen_Settings.screenWidth, -Screen_Settings.screenHeight, Screen_Settings.screenHeight, -1.0f, 1.0f);

    glutDisplayFunc(displayCallback);
    glutIdleFunc(glutPostRedisplay);
    glutReshapeFunc(checkResize);

    glutKeyboardFunc(handleKeypress);
    glutKeyboardUpFunc(handleKeyRelease);
    glutMouseFunc(onClick);
}

// Rewrite all of this
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
    std::cout << "C             : Toggle collisions" << std::endl;
    std::cout << "b             : Toggle border collisions" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "               Visual Controls" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "v             : Show/hide vectors" << std::endl;
    std::cout << "p             : Show/hide paths" << std::endl;
    std::cout << "o             : Show/hide bodies" << std::endl;
    std::cout << "=============================================" << std::endl;
}

int main(int argc, char **argv) {
    showMessage();
    Body_Settings.number_objects = 1000;
    Body_Settings.color_preset = cP_holoPink;
    // fixGen(0);
    bodies = generateBody(Body_Settings.number_objects, Generate_RingStack);
    bodies.push_back(Body({0, 0}, {0, 0}, 5000));
    StartGLUT(argc, argv);
    glutMainLoop();
    file.close();

    return 0;
}
