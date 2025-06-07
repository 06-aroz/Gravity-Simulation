# Another $N$-Body Gravity Simulation
An $N$-Body simulation predicts the motion of a system of $N$ particles under the influence of gravity. These simulations typically model gravity using Newton's Law of Gravitation.

$$ \textbf{F}_G=-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^2}\cdot\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|} $$

Newtonian gravity is more than suitable enough for most simulation and computation. This project details how I went about modelling gravitational physics——this README should provide guidance for anyone who wishes to create something similar. The simulation is largely based in C/C++ and uses the OpenGL/GLUT API for visualization. 

# Direct Approach
The basic approach that we're going to take involves an acceleration algorithm of the form. 
```C++
void applyAcceleration(float dt) {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            bodies[i].updateVelocity(GravAccel(bodies[i], bodies[j]), dt);
            bodies[j].updateVelocity(GravAccel(bodies[j], bodies[i]), dt);
        }
    }
}
```
In the actual code this is written a bit differently from here——but the basic idea is still going to be the same. We check for each individual body the acceleration that is applied from all the other bodies and each other body being applied the same equal but opposite acceleeration, then we increment the loop for each body in the system. This is the easiest way to apply the accelerations but that comes at the cost of speed. Despite slow performance, this algorithm is perfectly acceptible for low body simulations which are equally as important for study, so I will proceed with how I implemented [this].

## Implementation
There are various ways to track the position of a given body, the approach I went with is defining an individual `Body` object. 
```C++
class Body {
  public:
    Vec2 position;
    Vec2 velocity;
    float radius;
    float mass;

    Body(Vec2 position, Vec2 velocity, float mass) {
        this->position = position;
        this->velocity = velocity;
        this->mass = mass;

        this->radius = std::max(2.0f, std::sqrt(mass) * 0.5f); // You can choose however method to assign the radius of the object
    }

    void updateVelocity(Vec2 acceleration, float dt) {
        this->velocity += acceleration * dt;
    }

    void updatePos(float dt) {
        this->position += this->velocity * dt;
    }
    ...
};
```
The above code defines our body with a distinct `position`, `velocity`, `mass`, and `radius` that we can access. I have defined a custom `Vec2` struct to easily handle computation, however it's perfectly valid to do this via arrays. 
```C++
struct Vec2 {
    float x;
    float y;
    // Below I have defined [constructors] to redefine basic operators.
    ...
};
```
For later on we also defined a $2\times 2$ matrix struct.
```C++
struct Matrix {
    Vec2 V1;
    Vec2 V2;
};
```
If you look at the `Body` class, our method to update the position of each body is [Euler's Method](https://en.wikipedia.org/wiki/Euler_method). 

$$ \textbf{x}_{n+1} = \textbf{x}_n+\left(\frac{d\textbf{x}_n}{dt}\right)\Delta t=\textbf{x}_n+\textbf{v}_n\Delta t $$

$$ \textbf{v}_{n+1} = \textbf{v}_n+\left(\frac{d\textbf{v}_n}{dt}\right)\Delta t = \textbf{v}_n+\textbf{a}\Delta t$$

Euler's Method is prone to numerical instability [after a while], but it will work for now. Later we can implement [Verlet Integration](https://en.wikipedia.org/wiki/Verlet_integration) which would be more suitable given that our acceleration is only position dependent. Our time step $\Delta t$ will be determined by GLUT's `glutGet(GLUT_ELAPSED_TIME)` function, the method by which we got $\Delta t$ is as follows.
```C++
    float dt = 0.0f;
    float scale_time; // global parameter to control our time step
    int last_time = 0;
    ...
    void updateTime() {
        int cur_time = glutGet(GLUT_ELAPSED_TIME);
        dt = scale_time * static_cast<float>(cur_time - last_time) / 1000.0f;
        last_time = cur_time;
    }
```
Just choose $\Delta t$ to be a small number in general. In `math.hpp` I have defined several functions which would help in the vector maths. 

### Applying Accelerations
Now that we can track the position of two bodies with one another, we can now go about computing the gravitational acceleration for each body. To do this we use Newton's equation of Gravitation.

$$ \textbf{F}_G=-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^2}\cdot\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|} $$

It's important to note that $\textbf{r}=(x, y, z)$, since we're working with a 2D simulation the $z$ component can be ignored. Let's expand out this equation to get a better idea for what we need to code. Let us denote the distance between each body to be $r = |\textbf{r}_2-\textbf{r}_1|$. And we're going to then directly apply Newton's Second Law.

$$\textbf{F}_{12}=-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^{2}}\cdot\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|}= -\frac{Gm_1 m_2}{r^3} \cdot \begin{bmatrix}
    x_2 - x_1 \\
    y_2 - y_1  
\end{bmatrix}$$

$$\implies \textbf{a}_1=-\frac{G m_2}{r^3} \cdot \begin{bmatrix}
    x_2 - x_1 \\
    y_2 - y_1  
\end{bmatrix}$$

As we can see here, all that is needed is the immidiate body-to-body distance and the body-to-body displacement vector. It's important to note that $G\approx 10^{-11}$, unless if we kept the masses incredibly large it's more convienent to define $G\equiv1$. To begin with the code, we can start by defining a function to compute the distance between the two bodies. Alternatively you can just compute the dot product for the displacement vector with itself. 
```C++
float Distance(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float distance = std::sqrtf(delta_x * delta_x + delta_y * delta_y);
    return distance;
}
```
And below is a basic function that takes in two bodies and computes the acceleration directly using Newton's Law. 
```C++
Vec2 GravAccel(Body &one, Body &two) {
    Vec2 acceleration;

    float dist = std::max(Body_Settings.max_distance, Distance(one, two));
    acceleration.x = -Screen_Settings.G * two.mass * (one.position.x - two.position.x) / (dist * dist * dist);
    acceleration.y = -Screen_Settings.G * two.mass * (one.position.y - two.position.y) / (dist * dist * dist);
    return acceleration;
}
```
Note that we clamp the distance between the actual distance between the two bodies and a maximum distance we choose. Since $|\textbf{F}_G|\propto\frac{1}{r^2}$, when the distances get very small the acceleration as a result gets very large. While this is natural for the simulation, if we want to scale to many more bodies however this behavior is going to be an issue, so we restrict the distances in some way. Now we can come up with our main algorithm to apply these accelerations. 
```C++
std::vector<Body> bodies = generateBody(...);
...
void applyAcceleration(float dt) {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            const Vec2 &G_Accel = GravAccel(bodies[i], bodies[j]);
            bodies[i].updateVelocity(G_Accel, dt);
            bodies[j].updateVelocity(-G_Accel, dt);

        }
    }
}
```
We first started with defining essentially an array––in this case a vector––containing $N$ bodies with all our initial conditions for each body. We then go through the list and for each indvidual body we compute the acceleration applied to it, and every other body recieves its associated acceleration [rewrite this].  
## Collisions
To employ basic elastic collisions for this simulation we use the following [equation](https://en.wikipedia.org/wiki/Elastic_collision).

$$ \textbf{v}_{1}'=\textbf{v}_1-\frac{2m_2}{m_1 + m_2}\frac{(\textbf{v}_1-\textbf{v}_2)\cdot(\textbf{x}_1-\textbf{x}_2)}{|\textbf{x}_1-\textbf{x}_2|^2}(\textbf{x}_1-\textbf{x}_2)$$

$$ \textbf{v}_{2}'=\textbf{v}_2-\frac{2m_1}{m_1 + m_2}\frac{(\textbf{v}_2-\textbf{v}_1)\cdot(\textbf{x}_2-\textbf{x}_1)}{|\textbf{x}_2-\textbf{x}_1|^2}(\textbf{x}_2-\textbf{x}_1)$$

[Derivation](https://stackoverflow.com/a/35212639) for these equations. These equations are most convienent since they are indepedent of the angle and work entirely based on the known components for each object's position and velocity. We can also note that the equation for $\textbf{v}_2'$ is symmetric to $\textbf{v}_1'$, so we can make the following adjustment. 

$$ \textbf{v}_{2}'=\textbf{v}_2+\frac{2m_1}{m_1 + m_2}\frac{(\textbf{v}_1-\textbf{v}_2)\cdot(\textbf{x}_1-\textbf{x}_2)}{|\textbf{x}_1-\textbf{x}_2|^2}(\textbf{x}_1-\textbf{x}_2)$$

Most of the same terms follow so we can encapsulate the dot products into a singular variable, let's call it $Q$.

$$ Q = \frac{2}{m_{1} + m_{2}} \cdot \frac{(\textbf{v}_1 -\textbf{v}_2) \cdot (\textbf{x}_1 - \textbf{x}_2)}{(\textbf{x}_1-\textbf{x}_2)\cdot(\textbf{x}_1-\textbf{x}_2)} $$

Thus we can write.

$$ \textbf{v}_{1}' = \textbf{v}_1 - m_2 Q(\textbf{x}_1-\textbf{x}_2) $$ 

$$ \textbf{v}_{2}' = \textbf{v}_1 + m_1 Q(\textbf{x}_1-\textbf{x}_2) $$

Below I've included my implementation of computing elastic collisions.
```C++
Matrix ElasticCheck(Body &one, Body &two) {
    Matrix velocity_matrix;

    float mass_difference = one.mass - two.mass;
    float mass_sum = one.mass + two.mass;

    Vec2 velocity_difference = one.velocity - two.velocity;
    Vec2 position_difference = one.position - two.position;

    float vector_scale = 2.0f / mass_sum * DotProduct(velocity_difference, position_difference) /
                         (position_difference.x * position_difference.x + position_difference.y * position_difference.y);

    const Vec2 &velocity1 = one.velocity - position_difference * two.mass * vector_scale;
    const Vec2 &velocity2 = two.velocity + position_difference * one.mass * vector_scale;

    velocity_matrix.V1 = velocity1;
    velocity_matrix.V2 = velocity2;

    return velocity_matrix;
}
...
void detectCollision() {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            if (CheckCollision(bodies[i], bodies[j])) {
                Matrix updatedvelocity = ElasticCheck(bodies[i], bodies[j]);

                bodies[i].velocity = updatedvelocity.V1;
                bodies[j].velocity = updatedvelocity.V2;
            }
        }
    }
}
```
We haven't included any criteria for detecting collisions yet, if we did you may encounter a common issue where the bodies may clip through each other. A bandaid fix is to directly adjust the object's positions after collision based on each body's radii. 
```C++
bool CheckCollision(Body &one, Body &two) {
    float delta_x = one.position.x - two.position.x;
    float delta_y = one.position.y - two.position.y;
    float rad_sum = one.radius + two.radius;
    return delta_x * delta_x + delta_y * delta_y <= rad_sum * rad_sum + 0.5 * Body_Settings.max_distance;
}
...
Matrix PositionCollide(Body &one, Body &two) {
    Matrix position_matrix;
    const float &dX = one.position.x - two.position.x;
    const float &dY = one.position.y - two.position.y;
    float min_dist = one.radius + two.radius;
    const float &distsq = dX * dX + dY * dY;

    if (distsq < min_dist * min_dist) {
        float dist = std::sqrt(distsq);
        Vec2 Normal_dr = ScaleVector({dX, dY}, 1.0 / dist); // normalizing the position vector
        float mass_ratio = 1.0 / (1.0 + (two.radius / one.radius) * (two.radius / one.radius));
        float delta = 0.5f * (min_dist - dist);

        // Mass ratio allows objects of larger size to move less
        one.position += Normal_dr * (1 - mass_ratio) * delta;
        two.position -= Normal_dr * mass_ratio * delta;
    }

    position_matrix.V1 = one.position;
    position_matrix.V2 = two.position;

    return position_matrix;
}
```
[Explain the above]. It's also better to check the condition $\mathrm{dist}^2<(r_1+r_2)^2$ as opposed to $\mathrm{dist}<(r_1+r_2)$ since we don't have to perform any square root calculations. Using both the elastic collision computation and adjusting the position we can now model elastic collisions between the objects.
```C++
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
```
Once again, this has a time complexity of $\mathcal{O}(n^2)$ which isn't ideal, regardless this gets our desired result. 
# Barnes-Hut Algorithm
From the previous **Direct Approach** our algorithm takes the form:
```C++
void applyAcceleration(float dt) {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            bodies[i].updateVelocity(GravAccel(bodies[i], bodies[j]), dt);
            bodies[j].updateVelocity(GravAccel(bodies[j], bodies[i]), dt);
        }
    }
}
```
This algorithm as it stands has time complexity of $\mathcal{O}(n^2)$, this is incredibly slow for when we scale up the number of bodies——going from $N=10$ to $N=20$ bodies results in computation running 10x slower. [...]

