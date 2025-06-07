# Another $N$-Body Gravity Simulation
An $N$-Body simulation predicts the motion of a system of $N$ particles under the influence of gravity. These simulations typically model gravity using Newton's Law of Gravitation.

$$ \textbf{F}_G=-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^3}\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|} $$

This project details how I went about modelling gravitational physics---this README should provide guidance for anyone who wishes to create something similar. 
The simulation is largely based in C/C++ and uses the OpenGL/GLUT API for visualization. 
