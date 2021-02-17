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
//bool** grid;

int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

// Compare two float values for equality within some given epsilon
bool fequal(GLfloat a, GLfloat b) {
	return fabs(a-b) < epsilon;
}

bool grid[64][64];

// Generate random number between -1.0 and 1.0 in 1/grid_size increments
GLfloat rand_low() {
	return roundf( (-1.0f + 2.0f*((double)rand() / (double)RAND_MAX)) * (float)grid_size) / (float)grid_size;
}
int rand_grid() {
return (rand() % grid_size * 2);
}

struct {
	GLsizei window_size[2];
	GLfloat size;
	int x_position;
	int y_position;
	int x[256];
	int y[256];
	GLint direction;
	GLint length;
} snake;

struct {
	int x_position;
	int y_position;
	GLfloat size;
} food;

void draw_square(int grid_x, int grid_y, GLfloat square_size)
{
	GLfloat x = (float)(grid_x - grid_size) / (grid_size);
	GLfloat y = (float)(grid_y - grid_size) / (grid_size);

	if(grid_x - grid_size == 0)
		x = 0.0f;
	if(grid_y - grid_size == 0)
		y = 0.0f;

	//fprintf(stdout, "position: %f, %f\n", x, y);

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
	food.x_position = rand_grid();
	food.y_position = rand_grid();

	// TODO remove this loop and write new function that checks unfilled contents of grid[]
	for(int i=0; i < snake.length; ++i)
	{
		if( food.x_position == snake.x[i] )
		{
			if( food.y_position == snake.y[i])
			{
				// Could this be recursive?
				fprintf(stdout, "COLLISION!\n");
				food.x_position = rand_grid();
				food.y_position = rand_grid();
			}
		}
	}
}

void display() {  // Display function will draw the image.

    glClearColor( 0, 0, 0, 1 );  // (In fact, this is the default.)
    glClear( GL_COLOR_BUFFER_BIT );

	//draw_square(snake.x_position, snake.y_position, snake.size);
	draw_square(food.x_position, food.y_position, food.size);

	for(int i=0; i < grid_size*2; ++i)
	{
		for(int j=0; j < grid_size*2; ++j)
		{
			if(grid[i][j])
				draw_square(i, j, snake.size);
		}
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
			fprintf(stdout, "Up, y=%d\n", snake.y_position);
		}
	}
	if(key == 's' || key == 'S')
	{
		if(snake.direction != 0 && snake.direction != 1)
		{
			snake.direction = 1;
			fprintf(stdout, "Down, y=%d\n", snake.y_position);
		}
	}
	if(key == 'a' || key == 'A')
	{
		if(snake.direction != 3 && snake.direction != 2)
		{
			snake.direction = 2;
			fprintf(stdout, "Left, x=%d\n", snake.x_position);
		}
	}
	if(key == 'd' || key == 'D')
	{
		if(snake.direction != 2 && snake.direction != 3)
		{
			snake.direction = 3;
			fprintf(stdout, "Right, x=%d\n", snake.x_position);
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
	if(snake.x_position == food.x_position && snake.y_position == food.y_position)
	{
		snake.x[snake.length-1] = snake.x_position;
		snake.y[snake.length-1] = snake.y_position;
		snake.length++;
		gen_food();

		fprintf(stdout, "Snake x: %d, snake y: %d,  snake length: %d\n", snake.x_position, snake.y_position, snake.length);
	}

	switch(snake.direction) {
		case 0:
			snake.y_position++;
			break;
		case 1:
			snake.y_position--;
			break;
		case 2:
			snake.x_position--;
			break;
		case 3:
			snake.x_position++;
			break;
	}

	// This is really expensive. Should replace it with a 'hold position coords for
	// snake.length steps' setup
	for(int i=snake.length; i >= 0; --i)
	{
		if(i == 0)
		{
			snake.x[0] = snake.x_position;
			snake.y[0] = snake.y_position;
		}
		else
		{
			snake.x[i] = snake.x[i-1];
			snake.y[i] = snake.y[i-1];
		}
	}

	// This could be more efficient
	for(int i=0; i < grid_size*2; ++i)
	{
		for(int j=0; j < grid_size*2; ++j)
		{
			grid[i][j] = 0;
		}
	}

	for(int i=0; i < snake.length; ++i)
	{
		grid[snake.x[i]][snake.y[i]] = 1;
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
	printf("test");

	//bool ** grid = (bool**)malloc(grid_size*2 * sizeof(bool));

	for(int i=0; i < grid_size*2; ++i)
	{
		for(int j=0; j < grid_size*2; ++j)
		{
			grid[i][j] = 0;
		}
	}

	// The snake starts in the middle of the arena, therefore:
	snake.x_position = grid_size;
	snake.y_position = grid_size;

	food.x_position = rand_grid();
	food.y_position = rand_grid();

	//grid[snake.x_position][snake.y_position] = 1;
	snake.window_size[0] = INITIAL_WINDOW_WIDTH;
	snake.window_size[1] = INITIAL_WINDOW_HEIGHT;
	snake.size = grid_increment / 2;
	snake.length = 1;
	snake.direction = 0;
	food.size = grid_increment / 5;

    glutInit(&argc, argv);				 // Initialize GLUT and
    glutInitDisplayMode(GLUT_SINGLE);    // Use single color buffer and no depth buffer.
    glutInitWindowSize(snake.window_size[0], snake.window_size[1]);         // Size of display area, in pixels.
    glutInitWindowPosition(100,100);     // Location of window in screen coordinates.
    glutCreateWindow("Snake"); 			 // Parameter is window title.
    glutDisplayFunc(display);            // Called when the window needs to be redrawn.
	glutTimerFunc(sleep_t, timer, 0);
	//glutReshapeFunc(&reshape);
    glutKeyboardFunc(&keyboard);

    glutMainLoop(); // Run the event loop!  This function does not return.
                    // Program ends when user closes the window.

	//free(grid);
    return 0;
}
