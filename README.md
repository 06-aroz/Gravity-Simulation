# Another $N$-Body Gravity Simulation
An $N$-Body simulation predicts the motion of a system of $N$ particles under the influence of gravity. These simulations typically model gravity using Newton's Law of Gravitation.

$$ \textbf{F}_G=-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^2}\cdot\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|} $$

This project details how I went about modelling gravitational physics——this README should provide guidance for anyone who wishes to create something similar. The simulation is largely based in C/C++ and uses the OpenGL/GLUT API for visualization. 

# Basic Approach
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
In the actual code this is written a bit differently from here––and the actual implementation may be entirely different——but the basic idea is still going to be the same. We check for each individual body the acceleration that is applied from all the other bodies and each other body being applied the same equal but opposite acceleeration, then we increment the loop for each body in the system. This is the easiest way to apply the accelerations but that comes at the cost of speed. This algorithm as it stands runs at $O(n^2)$, which is incredibly slow for when we want to scale the number of particles. Despite slow performance, this algorithm is perfectly acceptible for low body simulations which are equally as important for study, so I will proceed with how I implemented [this].

## Implementation

[...]. Now that we can track the position of two bodies with one another, we can now go about computing the gravitational acceleration for each body. To do this we use Newton's equation of Gravitation.

$$ \textbf{F}_G=-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^2}\cdot\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|} $$

It's important to note that $\textbf{r}=(x, y, z)$, since we're working with a 2D simulation the $z$ component can be ignored. Let's expand out this equation to get a better idea for what we need to code. Let us denote the distance between each body to be $r_{21} = |\textbf{r}_2-\textbf{r}_1|$.

$$-\frac{Gm_1 m_2}{|\textbf{r}_2-\textbf{r}_1|^2}\cdot\frac{\textbf{r}_2-\textbf{r}_1}{|\textbf{r}_2-\textbf{r}_1|} $$
$$ = -\frac{Gm_1 m_2}{r} \cdot \begin{bmatrix}
    x_2 - x_1 \\
    y_2 - y_1  
\end{bmatrix}$$

As we can see here, all that is needed is the immidiate body-to-body distance and the body-to-body displacement as a vector. 



