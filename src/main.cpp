#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <random>

#include "include/generation.h"
#include "include/linmath.h"
#include "include/render.h"

float screenHeight = 800.0f;
float screenWidth = screenHeight;
float G = 1.0f;

const color velocityColor = {0.2, 0.0, 1.0};
const color accelerationColor = {1.0, 0.0, .25};
const float mu = 0.5f; // dampening factor

bool showVectors = false;
bool showPaths = true;
bool showObjs = true;
bool applyGravity = true;
bool isPaused = false;
bool applyCollisions = false;
bool applyBorder = false;
bool centerCamera = false;
bool showCM = true;

bool is_up = false, is_down = false, is_left = false, is_right = false;

float max_distance = 20.0f;

float dt = 0.0f;
float scale_time = 60.0f;

int path_length = 500;
int last_time = 0;
int number_objects = 100;

// Delta time implementation
void updateTime() {
    int cur_time = glutGet(GLUT_ELAPSED_TIME);
    dt = scale_time * static_cast<float>(cur_time - last_time) / 1000.0f;
    last_time = cur_time;
}

color randomColor() {
    static std::random_device rd;  // only used once to seed the generator
    // should prob change this later
    static std::mt19937 gen(rd()); // Mersenne Twister RNG
    std::uniform_real_distribution<float> dist(0.25, 1.0);

    color rand_color;
    rand_color.R = dist(gen);
    rand_color.G = dist(gen);
    rand_color.B = dist(gen);
    return rand_color;
}

// Define the bodies class with position and velocity info
class Body {
public:
    Vec2 position; 
    Vec2 velocity; 
    float radius;
    float mass;
    color bodycol;

    std::vector<Vec2> position_list;

    Body(Vec2 position, Vec2 velocity, float mass) {
        this -> position = position;
        this -> velocity = velocity;
        this -> mass = mass;

        if (mass < 50.0f) {
            this -> radius = 4.0f;
        } else {
            this -> radius = std::sqrtf(mass) * 0.5f;
        }

        this -> position_list.push_back(position);
        this -> bodycol = randomColor();
    }

    void updateVelocity(Vec2 acceleration) {
        this -> velocity.x += acceleration.x * dt;
        this -> velocity.y += acceleration.y * dt;
    }

    void updatePos() {
        this -> position.x += this -> velocity.x * dt;
        this -> position.y += this -> velocity.y * dt;
        this -> position_list.push_back(position);

        // Limit trail length
        const size_t maxTrailLength = path_length;
        if (this->position_list.size() > maxTrailLength) {
            this->position_list.erase(this -> position_list.begin());
        }
    }

    void addPosition(Vec2 update_vector) {
        this -> position.x += update_vector.x;
        this -> position.y += update_vector.y;
    }
};

// Drawing the paths for each body
void DrawPath(Body one, color path_col) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int i = 0;
    size_t n = one.position_list.size();
    for (const auto& position : one.position_list) {
        float alpha = static_cast<float>(i) / n;
        glBegin(GL_LINE_STRIP);
        glColor4f(path_col.R, path_col.G, path_col.B, alpha);
        glVertex2f(position.x, position.y);
        i++;
    }
    glEnd();
}

float Distance(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float distance = std::sqrtf(delta_x * delta_x + delta_y * delta_y);
    return distance;
}

bool CheckCollision(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float rad_sum = one.radius + two.radius;
    return delta_x * delta_x + delta_y * delta_y <= rad_sum * rad_sum;
}

Matrix ElasticCheck(Body &one, Body &two) {
    Vec2 velocity1;
    Vec2 velocity2;
    Matrix velocity_matrix;

    float mass_difference = one.mass - two.mass;
    float mass_sum = one.mass + two.mass;

    Vec2 velocity_difference1 = VectorSum(one.velocity, two.velocity, true);
    Vec2 velocity_difference2 = VectorSum(two.velocity, one.velocity, true);

    Vec2 position_difference1 = VectorSum(one.position, two.position, true);
    Vec2 position_difference2 = VectorSum(two.position, one.position, true);

    float vector_scale1 = (2.0f * two.mass)/mass_sum * DotProduct(velocity_difference1, position_difference1) / 
        (position_difference1.x * position_difference1.x + position_difference1.y * position_difference1.y);
    float vector_scale2 = (2.0f * one.mass)/mass_sum * DotProduct(velocity_difference2, position_difference2) /
        (position_difference2.x * position_difference2.x + position_difference2.y * position_difference2.y);

    velocity1.x = one.velocity.x - position_difference1.x * vector_scale1;
    velocity1.y = one.velocity.y - position_difference1.y * vector_scale1;
    velocity2.x = two.velocity.x - position_difference2.x * vector_scale2;
    velocity2.y = two.velocity.y - position_difference2.y * vector_scale2;

    velocity_matrix.V1 = velocity1;
    velocity_matrix.V2 = velocity2;

    return velocity_matrix;
}

Vec2 GravAccel(Body &one, Body &two) {
    Vec2 acceleration;
    
    float dist = std::max(max_distance, Distance(one, two));
    acceleration.x = -G * two.mass * (one.position.x - two.position.x) / powf(dist, 3);
    acceleration.y = -G * two.mass * (one.position.y - two.position.y) / powf(dist, 3);
    return acceleration;
}

// Making the bodies and creating init positions
std::vector<Body> generateBody(int number_objects, std::vector<Matrix> (*genFunction)(int number_objects)) {
    const float ring_dM = 100.0f;
    std::vector<Body> bodys;
    for (int i = 0; i < number_objects; i++) {
        Body one = Body(genFunction(number_objects)[i].V1, genFunction(number_objects)[i].V2, ring_dM);
        bodys.push_back(one);
    }
    bodys.push_back(Body({0.0f, 0.0f}, {0.0f, 0.0f}, 1000.0f));
    return bodys;
}

std::vector<Body> bodys = generateBody(number_objects, Generate_RingStack);

void spawnBody(Vec2 position, float mass) {
    Body spawned_body(position, {0, 0}, mass);
    bodys.push_back(spawned_body);
}

void despawnBody() {
    if (!bodys.empty()) {
        bodys.erase(bodys.end() - 1);
    }
}

void detectBorder(Body &body, float mu) {
    // Border detection
    if (body.position.y + body.radius >= screenHeight) {
        body.position.y = screenHeight - body.radius;
        body.velocity.y *= -mu;
    } else if (body.position.y - body.radius <= -screenHeight) {
        body.position.y = -screenHeight + body.radius;
        body.velocity.y *= -mu;
    }

    if (body.position.x + body.radius >= screenWidth) {
        body.position.x = screenWidth - body.radius;
        body.velocity.x *= -mu;
    } else if (body.position.x - body.radius <= -screenWidth) {
        body.position.x = -screenWidth + body.radius;
        body.velocity.x *= -mu;
    }
}

void detectCollision() {
    for (size_t i = 0; i < bodys.size(); i++) {
        for (size_t j = i + 1; j < bodys.size(); j++) {
            if (CheckCollision(bodys[i], bodys[j])) {
                Matrix updatedvelocity = ElasticCheck(bodys[i], bodys[j]);
                bodys[i].velocity = updatedvelocity.V1;
                bodys[j].velocity = updatedvelocity.V2;
            }
        }
    }
}

// Acceleration algorithm
void applyAcceleration(bool applyGravity) {
    if (applyGravity) {
        for (size_t i = 0; i < bodys.size(); i++) {
            for (size_t j = i + 1; j < bodys.size(); j++) {
                bodys[i].updateVelocity(GravAccel(bodys[i], bodys[j]));
                bodys[j].updateVelocity(GravAccel(bodys[j], bodys[i]));
                if (showVectors) {
                    DrawVector(GravAccel(bodys[i], bodys[j]), false, accelerationColor, bodys[i].position, bodys[i].radius);
                    DrawVector(GravAccel(bodys[j], bodys[i]), false, accelerationColor, bodys[j].position, bodys[j].radius);
                }
            }
        }
    }
}

void centreMass(std::vector<Body> bodys) {

}
// Mouse click logic
void onClick(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                float mouse_x = static_cast<float>(x);
                float mouse_y = static_cast<float>(y);
                float true_x = 2.0f * mouse_x - screenWidth;
                float true_y = -2.0f * mouse_y + screenHeight;
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

bool G_key_down = false;

void cameraMovement(bool is_left, bool is_right, bool is_up, bool is_down, Body &one) {
    float cam_amount = 10.0f;
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

// Keypress logic
void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC
            std::cout << "Exiting program." << "\n";
            exit(0);

        case 'r':
            bodys.clear();
            break;

        case 'v':
            showVectors = !showVectors;
            break;

        case 'p':
            showPaths = !showPaths;
            break;

        case 'o':
            showObjs = !showObjs;
            break;

        case 'g':
            applyGravity = !applyGravity;
            break;

        case ' ':
            isPaused = !isPaused;
            break;

        case 'q':
            showCM = !showCM;
            break;

        case 'Q':
            centerCamera = !centerCamera;
            break;

        case 'b':
            applyBorder = !applyBorder;
            break;

        case 'c':
            applyCollisions = !applyCollisions;
            break;

        case 'R':
            bodys.clear();
            bodys = generateBody(number_objects, Generate_RingStack);
            break;

        case 'G':
            G_key_down = true;
            G += 0.5f;
            break;

        // Camera movement flags
        case 'd': is_right = true; break;
        case 'a': is_left  = true; break;
        case 'w': is_up    = true; break;
        case 's': is_down  = true; break;
    }

    if (key != 'G') {
        G = 1.0f;
    }
}

void handleKeyRelease(unsigned char key, int x, int y) {
    switch (key) {
        case 'G':
            G = 1.0f;
            G_key_down = false;
            break;

        case 'd': is_right = false; break;
        case 'a': is_left  = false; break;
        case 'w': is_up    = false; break;
        case 's': is_down  = false; break;
    }
}

void mainDisplay(std::vector<Body> &bodys, bool showVectors) {
    updateTime();

    for (auto& body : bodys) {

        if (showObjs) {
            DrawCircle(body.position, body.radius, body.bodycol);
        }

        if (showVectors) {
            DrawVector(body.velocity, true, velocityColor, body.position, body.radius);
        }

        if (showPaths) {

            DrawPath(body, body.bodycol);
        }

        if (applyCollisions) {
            detectCollision();
        }

        if (applyBorder) {
            detectBorder(body, mu);
        }
        
        if(!isPaused) {
            applyAcceleration(applyGravity);
            body.updatePos();
        }

        cameraMovement(is_left, is_right, is_up, is_down, body);
    }

    float mass_sum = 0.0f;
    Vec2 CM_position = {0.0f, 0.0f};

    for (auto& bodys : bodys) {
        mass_sum += bodys.mass;
        CM_position.x += bodys.mass * bodys.position.x;
        CM_position.y += bodys.mass * bodys.position.y;
    }

    if (mass_sum != 0.0f) {
        CM_position.x /= mass_sum;
        CM_position.y /= mass_sum;
    }

    if (centerCamera) {
        DrawCircle(CM_position, 5.0f, {0.0, 1.0, 0.2});
        for (auto& b : bodys) {
            b.addPosition(ScaleVector(CM_position, -1.0f));
        }
    } else {
        DrawCircle(CM_position, 5.0f, {1.0, 1.0, 1.0});
    }
}

void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT);
    mainDisplay(bodys, showVectors);
    glutSwapBuffers();
}

void idleCallback() {
    glutPostRedisplay(); // Tells GLUT to redraw the screen
    if (G_key_down) {
        G++;
    }
}

void checkResize(int new_width, int new_height) {
    screenWidth = new_width;
    screenHeight = new_height;
    glViewport(0, 0, new_width, new_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-new_width,  new_width, -new_height, new_height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    displayCallback(); // refresh window.
}

void StartGLUT(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("2dGrav");
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-screenWidth, screenWidth, -screenHeight, screenHeight, -1.0f, 1.0f);

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(checkResize);
    glutIdleFunc(idleCallback);

    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(handleKeyRelease);
    glutMouseFunc(onClick);
}

void showMessage() {
    std::cout << "=============================================" << std::endl;
    std::cout << "Simulation Started" << std::endl;
    std::cout << "---------------------------------------------"<< std::endl;
    std::cout << "LC to add bodies, RC to remove bodies" << std::endl;
    std::cout << "---------------------------------------------"<< std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Controls" << std::endl;
    std::cout << "---------------------------------------------"<< std::endl;
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
    std::cout << "Visual Controls" << std::endl;
    std::cout << "---------------------------------------------"<< std::endl;
    std::cout << "v             : Show/hide velocity vectors" << std::endl;
    std::cout << "p             : Show/hide paths" << std::endl;
    std::cout << "o             : Show/hide bodies" << std::endl;
    std::cout << "q             : Show/hide centre of mass" << std::endl;
    std::cout << "C             : Center camera on COM" << std::endl; // reassigned from 'q'
    std::cout << "=============================================" << std::endl;
}

int main(int argc, char **argv) {

    showMessage();
    StartGLUT(argc, argv);
    glutMainLoop();

    return 0;
}

