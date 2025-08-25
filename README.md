# How to Install
This project uses GLUT/freeGLUT as the graphics API. You will first need to install GLUT or [freeGLUT](https://freeglut.sourceforge.net) if not already installed, from there you can follow the steps on the [installation page](https://freeglut.sourceforge.net/docs/install.php). If you're on macOS it will likely already be pre-installed but if not, run:
```bash
    brew install freeglut
```
After GLUT/freeGLUT has been installed, enter the main directly and run the make file:
```bash
    make run
```
# TODO
* Change graphics API.
  * GLUT is largely deprecated and OpenGL is basically the standard for graphics APIs. It was only used for this project out of convenience.
* Optimize circle rendering.
