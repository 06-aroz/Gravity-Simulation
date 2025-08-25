#include "../include/computation.hpp"

void detectBorder(Body &one) {
    if (one.position.y + one.radius >= Screen_Settings.screenHeight / Screen_Settings.zoom) {
        one.position.y = (Screen_Settings.screenHeight / Screen_Settings.zoom - one.radius);
        one.velocity.y *= -Body_Settings.mu;
    }

    if (one.position.y - one.radius <= -Screen_Settings.screenHeight / Screen_Settings.zoom) {
        one.position.y = (-Screen_Settings.screenHeight / Screen_Settings.zoom + one.radius);
        one.velocity.y *= -Body_Settings.mu;
    }

    if (one.position.x + one.radius >= Screen_Settings.screenWidth / Screen_Settings.zoom) {
        one.position.x = (Screen_Settings.screenWidth / Screen_Settings.zoom - one.radius);
        one.velocity.x *= -Body_Settings.mu;
    }

    if (one.position.x - one.radius <= -Screen_Settings.screenWidth / Screen_Settings.zoom) {
        one.position.x = (-Screen_Settings.screenWidth / Screen_Settings.zoom + one.radius);
        one.velocity.x *= -Body_Settings.mu;
    }
}

Vec2 GravAccel(Body &one, Body &two) {
    Vec2 acceleration;

    float dist = std::max(Body_Settings.max_distance, Distance(one, two));
    acceleration.x = -Screen_Settings.G * two.mass * (one.position.x - two.position.x) / (dist * dist * dist);
    acceleration.y = -Screen_Settings.G * two.mass * (one.position.y - two.position.y) / (dist * dist * dist);
    return acceleration;
}

// Energy computation
float PotentialEnergy(Body &one, Body &two) {
    return -Screen_Settings.G * two.mass * one.mass / Distance(one, two);
}

float KineticEnergy(Body &one) {
    return 0.5f * one.mass * DotProduct(one.velocity, one.velocity);
}

Vec2 TotalEnergy() {
    float U_sum = 0.0f;
    float K_sum = 0.0f;
    size_t n = bodies.size();

    for (size_t i = 0; i < n; i++) {
        K_sum += KineticEnergy(bodies[i]);
        for (size_t j = 0; j < n; j++) {
            if (j != i) {
                U_sum += PotentialEnergy(bodies[i], bodies[j]);
            }
        }
    }
    return {U_sum, K_sum};
}

bool CheckCollision(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float rad_sum = one.radius + two.radius;
    return delta_x * delta_x + delta_y * delta_y <= rad_sum * rad_sum + 0.5 * Body_Settings.max_distance;
}

Matrix ElasticCheck(Body &one, Body &two) {
    Matrix velocity_matrix;

    float mass_difference = one.mass - two.mass;
    float mass_sum = one.mass + two.mass;

    Vec2 velocity_difference = {one.velocity.x - two.velocity.x, one.velocity.y - two.velocity.y};
    Vec2 position_difference = {one.position.x - two.position.x, one.position.x - two.position.x};

    float vector_scale = 2.0f / mass_sum * DotProduct(velocity_difference, position_difference) /
                         (position_difference.x * position_difference.x + position_difference.y * position_difference.y);

    Vec2 velocity1 = {one.velocity.x - position_difference.x * two.mass * vector_scale,
                      one.velocity.y - position_difference.y * two.mass * vector_scale};
    Vec2 velocity2 = {two.velocity.x + position_difference.x * one.mass * vector_scale,
                      two.velocity.y + position_difference.y * one.mass * vector_scale};

    velocity_matrix.V1 = velocity1;
    velocity_matrix.V2 = velocity2;

    return velocity_matrix;
}

Matrix PositionCollide(Body &one, Body &two) {
    Matrix position_matrix;
    float dX = one.position.x - two.position.x;
    float dY = one.position.y - two.position.y;
    float min_dist = one.radius + two.radius;
    float distsq = dX * dX + dY * dY;

    if (distsq < min_dist * min_dist) {
        float dist = std::sqrt(distsq);
        Vec2 Normal_dr = ScaleVector({dX, dY}, 1.0 / dist); // normalizing the position vector
        float mass_ratio = 1.0 / (1.0 + (two.radius / one.radius) * (two.radius / one.radius));
        float delta = 0.5f * (min_dist - dist);

        // Mass ratio allows objects of larger size to move less
        one.position.x += Normal_dr.x * (1 - mass_ratio) * delta;
        one.position.y += Normal_dr.y * (1 - mass_ratio) * delta;
        two.position.y -= Normal_dr.x * mass_ratio * delta;
        two.position.y -= Normal_dr.y * mass_ratio * delta;
    }

    position_matrix.V1 = one.position;
    position_matrix.V2 = two.position;

    return position_matrix;
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

void renderCenterMass() {
    float mass_sum = 0.0f;
    Vec2 CM_position = {0.0f, 0.0f};

    for (auto &bodies : bodies) {
        mass_sum += bodies.mass;
        CM_position.x += bodies.position.x * bodies.mass;
        CM_position.y += bodies.position.y * bodies.mass;
    }

    if (mass_sum != 0.0f) {
        CM_position.x /= mass_sum;
        CM_position.y /= mass_sum;
    }

    float CM_radius = 10.0f / Screen_Settings.zoom;
    if (Screen_Settings.centerCamera) {
        if (Screen_Settings.showCM) {
            DrawCircle(CM_position, CM_radius, {0.0, 1.0, 0.2});
        }
        for (auto &b : bodies) {
            b.addPosition(ScaleVector(CM_position, -1.0f));
        }
    } else {
        if (Screen_Settings.showCM)
            DrawCircle(CM_position, CM_radius, {1.0, 1.0, 1.0});
    }
}
