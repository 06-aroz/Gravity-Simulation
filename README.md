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
### 3D sibling 
Translating this project to a seperate 3D version isn't entirely difficult---at least for the math/computation---the main challenge is with the 3D rendering. Until I learn how to render spheres in 3D this will likely not happen. The only thing that would need to happen computation-wise is translating Vec2 to Vec3 and to translate the Barnes-Hut tree to an [Octree](https://en.wikipedia.org/wiki/Octree). 
# Project Details
This project is an $n$-body gravity simulator which uses the Barnes-Hut algorithm to simulate the motion of [the system]. The rest of the README will go over how I went about making this project and hopefully will serve as a guide to anyone who wants to do something similar. An $n$-body simulator numerically solves the [N-body problem](https://en.wikipedia.org/wiki/N-body_problem), which is a system of nonlinear ODEs of the following.
```math
m_i\frac{d^{2}\textbf{r}_i}{dt^{2}}=\sum_{j=1, j\neq i}^{n}\frac{Gm_{i}m_{j}}{\|\textbf{r}_j-\textbf{r}_i\|^{3}}\left(\textbf{r}_{j}-\textbf{r}_{i}\right)
```
Which is just Newton's Second Law combined with [Newton's Law of Universal Gravitation](https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation). Admittedly the above equation can be a bit hard to read, so a simpler case would just be if $n=2$---being the [two-body problem](https://en.wikipedia.org/wiki/Two-body_problem). 
```math
\frac{d^{2}\textbf{r}_1}{dt^{2}}=\frac{Gm_{2}}{\|\textbf{r}_2-\textbf{r}_1\|^{3}}\left(\textbf{r}_2-\textbf{r}_1\right)
```
```math
\frac{d^{2}\textbf{r}_2}{dt^{2}}=\frac{Gm_{1}}{\|\textbf{r}_1-\textbf{r}_2\|^{3}}\left(\textbf{r}_1-\textbf{r}_2\right)
```
Where we just divided out the masses. The two-body problem does have an exact solution using polar coordinates:
```math
r=\frac{a(1-e^2}{1+e\cos{\phi}}
```
But, for the general $n$-body case no such solution exists, so we have to use numerical methods. 
# Brute-Force Integration
