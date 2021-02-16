#include <stdio.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>   // freeglut.h might be a better alternative, if available.
#include <unistd.h>
#include <math.h>

#define INITIAL_WINDOW_WIDTH 500
#define INITIAL_WINDOW_HEIGHT 500

// Variables to control frame rate
const int target_fps = 4;
float sleep_t = (1.0f / target_fps) * 1000;

// Variables to control arena size
const int grid_size = 10;
const float grid_increment = 1.0f / grid_size;

// For comparing float values
float epsilon = 0.00001;

int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

// Compare two float values for equality within some given epsilon
bool fequal(GLfloat a, GLfloat b) {
	return fabs(a-b) < epsilon;
}

// Generate random number between -1.0 and 1.0 in 1/grid_size increments
GLfloat rand_low() {
	return roundf( (-1.0f + 2.0f*((double)rand() / (double)RAND_MAX)) * (float)grid_size) / (float)grid_size;
}

struct {
	GLsizei window_size[2];
	GLfloat size;
	struct {
		GLfloat x;
		GLfloat y;
	} head;
	GLfloat x[256];
	GLfloat y[256];
	GLint direction;
	GLint length;
} snake;

struct {
	GLfloat x;
	GLfloat y;
	GLfloat size;
} food;

void draw_square(GLfloat x, GLfloat y, GLfloat square_size)
{
    glBegin(GL_POLYGON);
    glColor3f( 0, 1, 0 );
    glVertex2f( x+square_size, y+square_size );

    glColor3f( 0, 1, 0 );
    glVertex2f( x-square_size, y+square_size );

    glColor3f( 0, 1, 0 );
    glVertex2f( x-square_size, y-square_size );

    glColor3f( 0, 1, 0 );
    glVertex2f( x+square_size, y-square_size );
    glEnd();
}

void gen_food()
{
	food.x = rand_low();
	food.y = rand_low();

	for(int i=0; i < snake.length; ++i)
	{
		if( fequal(food.x, snake.x[i]) )
		{
			if( fequal(food.y, snake.y[i]) )
			{
				// Could this be recursive?
				fprintf(stdout, "COLLISION!\n");
				food.x = rand_low();
				food.y = rand_low();
			}
		}
	}
}

void display() {  // Display function will draw the image.

    glClearColor( 0, 0, 0, 1 );  // (In fact, this is the default.)
    glClear( GL_COLOR_BUFFER_BIT );

	draw_square(snake.head.x, snake.head.y, snake.size);
	draw_square(food.x, food.y, food.size);
	for(int i=0; i<snake.length; ++i)
	{
		draw_square(snake.x[i], snake.y[i], snake.size);
	}

    glutSwapBuffers(); // Required to copy color buffer onto the screen.
}

static void keyboard(unsigned char key, int x, int y)
{
	if(key == 'q' || key == 'Q')
	{
		glutLeaveMainLoop();
	}
	if(key == 'w' || key == 'W')
	{
		if(snake.direction != 1 && snake.direction != 0)
		{
			snake.direction = 0;
			fprintf(stdout, "Up, y=%f\n", snake.head.y);
		}
	}
	if(key == 's' || key == 'S')
	{
		if(snake.direction != 0 && snake.direction != 1)
		{
			snake.direction = 1;
			fprintf(stdout, "Down, y=%f\n", snake.head.y);
		}
	}
	if(key == 'a' || key == 'A')
	{
		if(snake.direction != 3 && snake.direction != 2)
		{
			snake.direction = 2;
			fprintf(stdout, "Left, x=%f\n", snake.head.x);
		}
	}
	if(key == 'd' || key == 'D')
	{
		if(snake.direction != 2 && snake.direction != 3)
		{
			snake.direction = 3;
			fprintf(stdout, "Right, x=%f\n", snake.head.x);
		}
	}
}

static void reshape(int w, int h)
{
	snake.window_size[0] = w;
	snake.window_size[1] = h;

	if(h == 0)
		h=1;
	float ratio = 1.0* w/h;
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
}
static void timer(int msec)
{
	if(fequal(snake.head.x, food.x) && fequal(snake.head.y, food.y))
	{
		fprintf(stdout, "Snake x: %f, snake y: %f,  snake length: %d\n", snake.head.x, snake.head.y, snake.length);

		snake.x[snake.length-1] = snake.head.x;
		snake.y[snake.length-1] = snake.head.y;
		snake.length++;
		gen_food();
	}

	// This is really expensive. Should replace it with a 'hold snake.head coords for
	// snake.length steps' setup
	for(int i=snake.length; i >= 0; --i)
	{
		if(i == 0)
		{
			snake.x[0] = snake.head.x;
			snake.y[0] = snake.head.y;
		}
		else
		{
			snake.x[i] = snake.x[i-1];
			snake.y[i] = snake.y[i-1];
		}
	}

	switch(snake.direction) {
		case 0:
			snake.head.y += grid_increment;
			break;
		case 1:
			snake.head.y -= grid_increment;
			break;
		case 2:
			snake.head.x -= grid_increment;
			break;
		case 3:
			snake.head.x += grid_increment;
			break;
	}
	glutPostRedisplay();
	glutTimerFunc(sleep_t, timer, 0);
}

static void update(void)
{
	glutPostRedisplay();
}

int main( int argc, char** argv )
{
	snake.window_size[0] = INITIAL_WINDOW_WIDTH;
	snake.window_size[1] = INITIAL_WINDOW_HEIGHT;
	snake.size = 0.05;
	snake.head.x = 0;
	snake.head.y = 0;
	snake.length = 1;
	snake.direction = 0;
	food.size = 0.02;

    glutInit(&argc, argv);				 // Initialize GLUT and
    glutInitDisplayMode(GLUT_SINGLE);    // Use single color buffer and no depth buffer.
    glutInitWindowSize(snake.window_size[0], snake.window_size[1]);         // Size of display area, in pixels.
    glutInitWindowPosition(100,100);     // Location of window in screen coordinates.
    glutCreateWindow("Snake"); 			 // Parameter is window title.
    glutDisplayFunc(display);            // Called when the window needs to be redrawn.
	glutTimerFunc(sleep_t, timer, 0);
	//glutReshapeFunc(&reshape);
    glutKeyboardFunc(&keyboard);
	//glutIdleFunc(&update);

    glutMainLoop(); // Run the event loop!  This function does not return.
                    // Program ends when user closes the window.
    return 0;

}
