# How to Install
This project uses GLUT/freeGLUT as the graphics API. You will first need to install GLUT or [freeGLUT](https://freeglut.sourceforge.net) if not already installed, from there you can follow the steps on the [installation page](https://freeglut.sourceforge.net/docs/install.php). If you're on macOS it will likely already be pre-installed but if not, run:
```bash
    brew install freeglut
```
After GLUT/freeGLUT has been installed, enter the main directly and run the make file:
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
# Project Details
This project is an $n$-body gravity simulator which uses the Barnes-Hut algorithm to simulate the motion of [the system], and the regular brute-force algorithm for . The rest of the README will go over how I went about making this project and hopefully will serve as a guide to anyone who wants to do something similar. An $n$-body simulator numerically solves the [$n$-body problem](https://en.wikipedia.org/wiki/N-body_problem), which is a system of nonlinear ODEs of the following.\
```math
\left( \sum_{k=1}^n a_k b_k \right)^2 \leq \left( \sum_{k=1}^n a_k^2 \right) \left( \sum_{k=1}^n b_k^2 \right)
```
