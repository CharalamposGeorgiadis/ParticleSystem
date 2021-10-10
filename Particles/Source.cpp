#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>

// Particle state
struct Particle
{
	double p[3]; // position vector
	double v[3]; // velocity vector
	double c[3]; // particle color
	double m; // mass of the marticle
	bool inactive = true; // status
};

// Particle system
#define numParticles 10000
struct Particle particles[numParticles];

double e = 0.5; // Coefficient of restitution
double lastTime = 0;

// Generates a random number between lower and upper
double random(double lower, double upper)
{
	double r = (double)(rand() % 1000) / 1000.0;
	return lower + (upper - lower) * r;
}

// Creates a particle from a set position in space at random initial velocities and colors 
void createParticle()
{
	// Finding a non-alive particle
	for (int i = 0; i < numParticles; i++)
	{
		if (particles[i].inactive)
		{
			// Setting random velocity and color
			particles[i].p[0] = 0.0;
			particles[i].p[1] = 2.0;
			particles[i].p[2] = 0.0;
			particles[i].v[0] = random(-1.0, 1.0);
			particles[i].v[1] = random(-1.0, 1.0);
			particles[i].v[2] = random(-1.0, 1.0);
			particles[i].c[0] = random(0.0, 1.0);
			particles[i].c[1] = random(0.0, 1.0);
			particles[i].c[2] = random(0.0, 1.0);
			particles[i].m = random(0.5, 2.0);
			particles[i].inactive = false;
			return;
		}
	}
}

// Updates particle states
void updateParticles()
{
	// Current time
	double time = glutGet(GLUT_ELAPSED_TIME) * 0.0004;

	// Vertical gravity and horizontal wind acceleration
	double gravity = -7.5;
	double wind = 2.5;

	for (int i = 0; i < numParticles; i++)
	{
		double x, y, z, vy;
		if (particles[i].inactive) continue;

		// Updating horizontal velocity due to wind
		particles[i].v[0] += wind * (time - lastTime);

		// Updating vertical velocity due to gravity
		particles[i].v[1] += gravity * particles[i].m * (time - lastTime);

		// Updating position due to velocity
		for (int j = 0; j < 3; j++)
			particles[i].p[j] += particles[i].v[j] * (time - lastTime);

		// Particle coordinates and vertical velocity
		x = particles[i].p[0];
		y = particles[i].p[1];
		z = particles[i].p[2];
		vy = particles[i].v[1];
		
		// Collision detection with floor
		if (fabs(x) < 1 && y < 0 && fabs(z) < 1 && vy < 0)
		{
			// Collision handling: bouncing
			particles[i].p[1] = 0;
			particles[i].v[1] = - e * particles[i].v[1];
		}
		// Killing fallen particles
		if (y < -1)
			particles[i].inactive = true;
	}
	lastTime = time;
}

// GLUT callback for window resizing
void reshape(int width, int height)
{
	// Updating viewport
	glViewport(0, 0, width, height);

	// Setting projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

// GLUT callback for window drawing
void display(void)
{
	// Constantly creating particles
	for (int i = 0; i < 10; i++)
		createParticle();
	
	// Clear window
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Setting camera
	glLoadIdentity();
	gluLookAt(
		0, 1, 3,  // camera position
		0, 1, 0,  // scene center
		0, 1, 0); // up direction

	// Drawing floor
	glColor3d(0.4, 0.5, 0.6);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3d(-1, 0, 1);
	glVertex3d(1, 0, 1);
	glVertex3d(-1, 0, -1);
	glVertex3d(1, 0, -1);
	glEnd();

	// Drawing particles
	glPointSize(5);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glBegin(GL_POINTS);
	for (int i = 0; i < numParticles; i++)
	{
		if (particles[i].inactive) continue;
		glColor3dv(particles[i].c);
		glVertex3dv(particles[i].p);
	}
	glEnd();
	
	// Updating particles
	updateParticles();

	glutSwapBuffers();
	glutPostRedisplay();
}

// Main
int main(int argc, char** argv)
{
	// Creating the main window
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Particles");

	// Setting callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	// Starting the event processing loop
	glutMainLoop();
	return 0;
}
