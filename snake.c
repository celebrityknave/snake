#include <stdio.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>   // freeglut.h might be a better alternative, if available.
#include <unistd.h>
#include <math.h>

#define INITIAL_WINDOW_WIDTH 500
#define INITIAL_WINDOW_HEIGHT 500

short snake_direction = 0;
const int target_fps = 4;
//const float time_per_frame = 1000 / target_fps;
long current_frame_no = 0;
float sleep_t = (1.0f / target_fps) * 1000;
float epsilon = 0.00001;

int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}
bool fequal(GLfloat a, GLfloat b)
{
	return fabs(a-b) < epsilon;
}

// Generate random number between -1.0 and 1.0 in 0.1-point increments
GLfloat rand_low()
{
	float rand_no = -1.0f + 2.0f*((double)rand() / (double)RAND_MAX);
	float rand_rounded = roundf( rand_no * 10.0f) / 10.0f;

	return rand_rounded;
}

struct {
	GLsizei window_size[2];
	GLfloat size;
	GLfloat x;
	GLfloat y;
	GLfloat direction;
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
}

void display() {  // Display function will draw the image.

    glClearColor( 0, 0, 0, 1 );  // (In fact, this is the default.)
    glClear( GL_COLOR_BUFFER_BIT );

	draw_square(snake.x, snake.y, snake.size);
	draw_square(food.x, food.y, food.size);

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
		//snake_y += 0.1;
		if(snake_direction != 1 && snake_direction != 0)
		{
			snake_direction = 0;
			fprintf(stdout, "Up, y=%f\n", snake.y);
		}
	}
	if(key == 's' || key == 'S')
	{
		//snake_y -= 0.1;
		if(snake_direction != 0 && snake_direction != 1)
		{
			snake_direction = 1;
			fprintf(stdout, "Down, y=%f\n", snake.y);
		}
	}
	if(key == 'a' || key == 'A')
	{
		//snake_x -= 0.1;
		if(snake_direction != 3 && snake_direction != 2)
		{
			snake_direction = 2;
			fprintf(stdout, "Left, x=%f\n", snake.x);
		}
	}
	if(key == 'd' || key == 'D')
	{
		//snake_x += 0.1;
		if(snake_direction != 2 && snake_direction != 3)
		{
			snake_direction = 3;
			fprintf(stdout, "Right, x=%f\n", snake.x);
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
static void timer (int msec)
{
	fprintf(stdout, "Snake x: %f, food x: %f, snake y: %f, food y: %f\n", snake.x, food.x, snake.y, food.y);
	if(fequal(snake.x, food.x) && fequal(snake.y, food.y))
		gen_food();
	switch(snake_direction) {
		case 0:
			snake.y += 0.1f;
			break;
		case 1:
			snake.y -= 0.1f;
			break;
		case 2:
			snake.x -= 0.1f;
			break;
		case 3:
			snake.x += 0.1f;
			break;
	}
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
	snake.x = 0;
	snake.y = 0;
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
	glutIdleFunc(&update);

    glutMainLoop(); // Run the event loop!  This function does not return.
                    // Program ends when user closes the window.
    return 0;

}

