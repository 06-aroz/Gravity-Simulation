# How to Install
This project uses GLUT/freeGLUT as the graphics API. You will first need to install GLUT or [freeGLUT](https://freeglut.sourceforge.net) if not already installed, from there you can follow the steps on the [installation page](https://freeglut.sourceforge.net/docs/install.php). If you're on macOS it will likely already be pre-installed but if not, run:
```bash
    brew install freeglut
```
After GLUT/freeGLUT has been installed, enter the main directory and run the make file:
```bash
    make run
```
## Changes to be Made
I'm more likely to implement changes to the actual computation as opposed to graphical (optimization) changes since I have more motivation to learn computation optimization than the intricacies of the OpenGL window. The following sections are ordered in priority, and is incomplete as I continue working on the project.
### Optimize Circle Rendering
As of writing this, the current 
### Migrate to OpenGL
GLUT is largely deprecated (hence the many warnings upon running) and OpenGL is basically the standard for graphics APIs. It was only used for this project out of convenience but I wouldn't imagine it being overly difficult to migrate to OpenGL. 
### Easier to use UI
Currently in order for the user to actually change anything, they would have to go directly into the source code and make changes there. I'm hoping to implement some text on screen and an easy way for the user to look up the controls. 
### Optimization
While the Barnes-Hut algorithm works and does improve performance, as of writing I'm only able to get to around ~2000 particles running at a reasonable framerate. Here's what I plan to implement in order to better optimization.
* Multi-threading
  * Long-term goal mainly because I don't know how to do it, but is the clear next major step in improving performance.
* Further graphical optimizations
  * The previous section detailed better circle rendering, but I'll likely have to do more.
  * Implement proper shaders (?)
### 3D sibling 
Translating this project to a seperate 3D version isn't entirely difficult——at least for the math/computation——the main challenge is with the 3D rendering. Until I learn how to render spheres in 3D this will likely not happen. The only thing that would need to happen computation-wise is translating Vec2 to Vec3 and to translate the Barnes-Hut tree to an [Octree](https://en.wikipedia.org/wiki/Octree). 
# Project Details
This project is an $n$-body gravity simulator which uses the Barnes-Hut algorithm to simulate the motion of [the system]. The rest of the README will go over how I went about making this project and hopefully will serve as a guide to anyone who wants to do something similar. An $n$-body simulator numerically solves the [N-body problem](https://en.wikipedia.org/wiki/N-body_problem), which is a system of nonlinear second-order ODEs of the following.
```math
m_i\frac{d^{2}\textbf{r}_i}{dt^{2}}=\sum_{j=1, j\neq i}^{n}\frac{Gm_{i}m_{j}}{\|\textbf{r}_j-\textbf{r}_i\|^{3}}\left(\textbf{r}_{j}-\textbf{r}_{i}\right)
```
Which is just Newton's Second Law combined with [Newton's Law of Universal Gravitation](https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation). Admittedly the above equation can be a bit hard to read, so a simpler case would just be if $n=2$——being the [two-body problem](https://en.wikipedia.org/wiki/Two-body_problem). 
```math
\frac{d^{2}\textbf{r}_1}{dt^{2}}=\frac{Gm_{2}}{\|\textbf{r}_2-\textbf{r}_1\|^{3}}\left(\textbf{r}_2-\textbf{r}_1\right)
```
```math
\frac{d^{2}\textbf{r}_2}{dt^{2}}=\frac{Gm_{1}}{\|\textbf{r}_1-\textbf{r}_2\|^{3}}\left(\textbf{r}_1-\textbf{r}_2\right)
```
Where we just divided out the masses. The two-body problem does have an exact solution using polar coordinates:
```math
r=\frac{a(1-e^2)}{1+e\cos{\phi}}
```
But, for the general $n$-body case no such solution exists, so we have to use numerical methods. 
# Structure
There are many ways to do an $n$-body simulation, the way I went about it was using object-orientated programming since it would be easier to scale/add new features if I wanted to. We will want to define a vector and body class:
```C++
struct Vec2 {
    float x;
    float y;
};
...
class Body {
  public:
    Vec2 position;
    Vec2 velocity;
    float mass;

    Body();
    Body(Vec2 position, Vec2 velocity, float mass);
};
```
This is at minimum what is required. I also ended up defining a "matrix" class which stores two vectors. 
# Brute-Force Computation
We will first go over a simple method to solve a system of second-order ODEs. Newton's Second Law states.
```math
\sum\textbf{F}_{ij}=m_{i}\ddot{\textbf{r}}
```
Ultimately what we actually care about is the acceleration of each individual body since that's going to be what determines the next position of the object. To see how this is true, just recall the definition for acceleration and velocity.
```math
\frac{\Delta \textbf{v}}{\Delta t}=\textbf{a}\imples \Delta\textbf{v}=\textbf{a}\Delta t
```
```math
\frac{\Delta \textbf{x}}{\Delta t}=\textbf{v}\imples \Delta\textbf{x}=\textbf{v}\Delta t
```
So for sufficiently small $\Delta t$, we can get better and better approximations for the change in position. If we couple the above equations together we can compute $\Delta x$ given just our acceleration, this is (Euler's method)[https://en.wikipedia.org/wiki/Euler_method]. So a basic implementation for Euler's method using a `Body` class may be of the form.
```C++
void Body::updateVelocity(Vec2 acceleration, float dt) {
    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;
}

void Body::updatePos(float dt) {
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
}
```
From here you will want to implement a `for` loop which calls both methods for each body in the system. 
```C++
for (auto &body : bodies) {
/*
    It is your choice to compute the net acceleration within the loop, or in its seperate (nested) for loop.
    If outside this loop, consider adding an "acc" attribute to the Body class setting Body.acc = computeNetAcc(...);
*/
    Vec2 acc = computeNetAcc(...);
    ...
    body.updateVelocity(acc, dt);
    body.updatePos(dt);
}
```
Euler's method is by far the easiest integration scheme you can implement, but is prone to numerical instability which can be seen if we graph the total energy over time:
[Show a graph]
Another method which is more suited for gravity simulations would be [Leapfrog integration](https://en.wikipedia.org/wiki/Leapfrog_integration). 
[Talk about leapfrog integration]
## Computation of Net Forces
So far we have assumed that the acceleration of each body can be easily computed, in this section we will go about actually computing the net acceleration. In general an algorithm to compute net forces will look like:
```C++
Vec2 compAcc(Body &one, Body &two) {
    Vec2 acc;
    return acc;
}

Vec2 computeNetAcc(std::vector<Body> &bodies) {
    Vec2 acc_ij;
    Vec2 net_acc = {0, 0};
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j != i) {
                acc_ij = compAcc(bodies[i], bodies[j]);
                net_acc.x += acc_ij.x;
                net_acc.y += acc_ij.y;
            }
        }
    }
    return net_acc;
}
```
If you notice, we have a nested `for` loop. Nested `for` loops aren't neccesarily bad for performance if they are not called frequently——for example for the inital position generation functions some of the functions there use a nested for loop——but in this case it results in an algorithm that is $O(N^2)$ in time complexity which significantly limits the number of bodies we can reasonably render. If we wish to increase $N$, we will have to use an different algorithm. For low $N$ however, this method is entirely fine, below I have included pseudocode for what a basic integrator may look like. 

# The Barnes-Hut Algorithm
In the previous section we came up with the following algorithm.
```C++
Vec2 compAcc(Body &one, Body &two) {
    Vec2 acc;
    return acc;
}

Vec2 computeNetAcc(std::vector<Body> &bodies) {
    Vec2 acc_ij;
    Vec2 net_acc = {0, 0};
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j != i) {
                acc_ij = compAcc(bodies[i], bodies[j]);
                net_acc.x += acc_ij.x;
                net_acc.y += acc_ij.y;
            }
        }
    }
    return net_acc;
}
```
Resulting in $O(N^2)$ time complexity. The Barnes-Hut algorithm allows us to reduce the time complexity down to $O(N\log(N)$ using a Quadtree.
# Explanation of Each Source File
Should probably put this as a comment in those files, but for now. 
