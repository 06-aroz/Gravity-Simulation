#include "../include/controls.hpp"

// Mouse click logic
void onClick(int button, int state, int x, int y) {
    // Spawn body on click, despawn on right click
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            float mouse_x = static_cast<float>(x);
            float mouse_y = static_cast<float>(y);
            float true_x = (2.0f * mouse_x - Screen_Settings.screenWidth) / Screen_Settings.zoom;
            float true_y = (-2.0f * mouse_y + Screen_Settings.screenHeight) / Screen_Settings.zoom;
            spawnBody({true_x, true_y}, randomRange(100.0f, 5000.0f));
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
            despawnBody();
        break;
    }
}

// REALLY should fix this for BH algorithm
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

void handleKeyRelease(unsigned char key, int x, int y) {
    switch (key) {
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
        bodies.clear();
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
        Screen_Settings.showColor = !Screen_Settings.showColor;
        break;

    case 'C':
        Screen_Settings.applyCollisions = !Screen_Settings.applyCollisions;
        break;

    case 'R':
        // Reset function
        bodies.clear();
        // fixGen(0);
        // bodies = generateBody(500, Generate_RingStack);
        // bodies.push_back(Body({0, 0}, {0, 0}, 5000));
        bodies = generateBody(Body_Settings.number_objects, Generate_NormalDist);
        break;

    // For three body presets
    case '1':
        fixGen(0);
        break;

    case '2':
        fixGen(1);
        break;

    case '3':
        fixGen(2);
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
}
