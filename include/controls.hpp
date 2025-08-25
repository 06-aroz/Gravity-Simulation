#ifndef _CONTROLS_H
#define _CONTROLS_H

#include "math.hpp"
#include "rendering.hpp"
#include "types.hpp"
#include <GLUT/glut.h>
#include <iostream>

// Mouse click logic
void onClick(int button, int state, int x, int y);

// Should use an enum here
inline bool is_up = false, is_down = false, is_left = false, is_right = false;
void cameraMovement(bool is_left, bool is_right, bool is_up, bool is_down, Body &one);
void handleKeyRelease(unsigned char key, int x, int y);

// Keypress logic
void handleKeypress(unsigned char key, int x, int y);

#endif // _CONTROLS_H
