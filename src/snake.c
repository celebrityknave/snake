#include <stdio.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <unistd.h>
#include <math.h>
#include "framework/definitions.h"

// Variables to control frame rate
float sleep_t = (1.0f / TARGET_FPS) * 1000;

// Variables to control arena size
const float grid_increment = 1.0f / GRID_SIZE;

// For comparing float values
float epsilon = 0.00001;

int msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

// Compare two float values for equality within some given epsilon
bool fequal(GLfloat a, GLfloat b) {
	return fabs(a-b) < epsilon;
}

bool grid[4096]; // TODO dynamically allocate grid size based on user input

// Generate random number between -1.0 and 1.0 in 1/grid_size increments
GLfloat rand_low() {
	return roundf( (-1.0f + 2.0f*((double)rand() / (double)RAND_MAX)) * (float)GRID_SIZE) / (float)GRID_SIZE;
}
int rand_grid() {
	return (rand() % GRID_SIZE * 2);
}

struct {
	GLsizei window_size[2];
	GLfloat size;
	int x_position;
	int y_position;
	int x[256];			// TODO dynamically allocate co-ordinate limits based on user input
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
	GLfloat x;
	GLfloat y;

	// Convert grid co-ordinates to floating point values.
	// TODO: make this conversion its own function
	if(grid_x - grid_size != 0)
		x = (float)(grid_x - grid_size) / (grid_size);
	else
		x = 0.0f;

	if(grid_y - grid_size != 0)
		y = (float)(grid_y - grid_size) / (grid_size);
	else
		y = 0.0f;

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
	int open_grid[(GRID_SIZE * 2)^2];
	int open_grid_index;
	for(int i=0; i < GRID_SIZE * 2; ++i)
	{
		for(int j=0; j < GRID_SIZE * 2; ++j)
		{
			if(!grid[(GRID_SIZE * 2) * i + j])
			{
				// Save co-ordinates of grid not currently occupied
				open_grid[open_grid_index] = GRID_SIZE * 2 * i + j;
			}
		}
	}
	int food_position = rand() % (GRID_SIZE * 2 ^ 2);
	// TODO: x_position and y_position should be replaced with a single index value
	//food.x_position = rand_grid();
	//food.y_position = rand_grid();

}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

	// Draw food
	draw_square(food.x_position, food.y_position, food.size);

	// Draw snake
	for(int i = 0; i < GRID_SIZE * 2; ++i)
	{
		for(int j = 0; j < GRID_SIZE * 2; ++j)
		{
			if(grid[(GRID_SIZE * 2) * i + j])
				draw_square(i, j, snake.size);
		}
	}

    glutSwapBuffers(); // Required to copy color buffer onto the screen.
}

static void keyboard(unsigned char key, int x, int y)
{
	if(key == QUIT_KEY)
	{
		glutLeaveMainLoop();
	}
	if(key == UP_KEY)
	{
		// FIXME: this check can sometimes let users reverse the snake
		// in place with quick enough input.
		if(snake.direction != DOWN && snake.direction != UP)
		{
			snake.direction = UP;
			fprintf(stdout, "Up, y=%d\n", snake.y_position);
		}
	}
	if(key == DOWN_KEY)
	{
		if(snake.direction != UP && snake.direction != DOWN)
		{
			snake.direction = DOWN;
			fprintf(stdout, "Down, y=%d\n", snake.y_position);
		}
	}
	if(key == LEFT_KEY)
	{
		if(snake.direction != RIGHT && snake.direction != LEFT)
		{
			snake.direction = LEFT;
			fprintf(stdout, "Left, x=%d\n", snake.x_position);
		}
	}
	if(key == RIGHT_KEY)
	{
		if(snake.direction != LEFT && snake.direction != RIGHT)
		{
			snake.direction = RIGHT;
			fprintf(stdout, "Right, x=%d\n", snake.x_position);
		}
	}
}

static void reshape(int w, int h)
{
	snake.window_size[0] = w;
	snake.window_size[1] = h;

	// To prevent a division by 0
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
	// Check whether snake has eaten a piece of food
	if(snake.x_position == food.x_position && snake.y_position == food.y_position)
	{
		snake.x[snake.length-1] = snake.x_position;
		snake.y[snake.length-1] = snake.y_position;
		snake.length++;
		gen_food();
	}

	// Check snake direction
	switch(snake.direction) {
		case UP:
			snake.y_position++;
			break;
		case DOWN:
			snake.y_position--;
			break;
		case LEFT:
			snake.x_position--;
			break;
		case RIGHT:
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

	// Zero all grid locations and write out snake coordinates
	// This could be more efficient
	for(int i=0; i < grid_size*2; ++i)
	{
		for(int j=0; j < grid_size*2; ++j)
		{
			grid[(grid_size*2)*i + j] = 0;
		}
	}

	for(int i=0; i < snake.length; ++i)
	{
		grid[(grid_size*2)*snake.x[i] + snake.y[i]] = 1;
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
	// Initialise grid to have no collision
	for(int i=0; i < GRID_SIZE * 2; ++i)
	{
		for(int j=0; j < GRID_SIZE * 2; ++j)
		{
			grid[(GRID_SIZE * 2)*i + j] = 0;
		}
	}

	// The snake starts in the middle of the arena.
	// The size of the grid is (grid_size * 2)^2 therefore:
	snake.x_position = GRID_SIZE;
	snake.y_position = GRID_SIZE;

	food.x_position = rand_grid();
	food.y_position = rand_grid();

	snake.window_size[0] = INITIAL_WINDOW_WIDTH;
	snake.window_size[1] = INITIAL_WINDOW_HEIGHT;
	snake.size = grid_increment / 2;
	snake.length = 1;
	snake.direction = UP;
	food.size = grid_increment / 5;

    glutInit(&argc, argv);				 									// Initialise
    glutInitDisplayMode(GLUT_SINGLE);    								// single color buffer and no depth buffer.
    glutInitWindowSize(snake.window_size[0], snake.window_size[1]);	// Size of display area
    glutInitWindowPosition(100,100);											// Location of window in screen coordinates.
    glutCreateWindow("Snake");												// Window title.
    glutDisplayFunc(display);												// Called when redrawing window.
	glutTimerFunc(sleep_t, timer, 0);
	//glutReshapeFunc(&reshape);
    glutKeyboardFunc(&keyboard);

    glutMainLoop(); 					 // Run the event loop

	//free(grid);
    return 0;
}
