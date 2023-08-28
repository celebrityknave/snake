#include <stdio.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <unistd.h>
#include <math.h>
#include "framework/definitions.h"

int msleep(unsigned int tms)
{
	return usleep(tms * 1000);
}

// Compare two float values for equality within some given epsilon
bool fequal(GLfloat a, GLfloat b)
{
	return fabs(a-b) < EPSILON;
}

bool grid[4096]; // TODO dynamically allocate grid size based on user input

int rand_grid()
{
	return (rand() % GRID_SIZE);
}

struct world {
	GLsizei window_size[2];
	int x[256];			// TODO dynamically allocate co-ordinate limits based on user input
	int y[256];
} world;

struct snake {
	GLfloat size;
	int x_position;
	int y_position;
	GLint direction;
	GLint length;
	int body_x[256];
	int body_y[256];
} snake;

struct food {
	int x_position;
	int y_position;
	GLfloat size;
} food;

float convertGridToFloat(int gridPoint)
{
	if(gridPoint - GRID_SIZE != 0)
	{
		return (float)(gridPoint - GRID_SIZE / 2) / (GRID_SIZE / 2);
	}

	return 0.0f;
}

void draw_square(int grid_x, int grid_y, GLfloat square_size)
{
	GLfloat x;
	GLfloat y;

	// Convert grid co-ordinates to floating point values.
	x = convertGridToFloat(grid_x);
	y = convertGridToFloat(grid_y);

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

void generateFood()
{
	food.x_position = rand_grid();
	food.y_position = rand_grid();
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

	// Draw food
	draw_square(food.x_position, food.y_position, food.size);

	// Draw snake
	for(int i = 0; i < GRID_SIZE; ++i)
	{
		for(int j = 0; j < GRID_SIZE; ++j)
		{
			if(grid[GRID_SIZE * i + j])
				draw_square(i, j, snake.size);
		}
	}

    glutSwapBuffers(); // Required to copy color buffer onto the screen.
}

static void readUserInput(unsigned char key, int x, int y)
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

static void reshape(int width, int height)
{
	world.window_size[0] = width;
	world.window_size[1] = height;

	// To prevent a division by 0
	if(height == 0)
		height = 1;

	float aspectRatio = 1.0 * width / height;
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(45, aspectRatio, 1, 1000);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
}

bool snakeHasEatenFood()
{
	if(snake.x_position == food.x_position && snake.y_position == food.y_position)
	{
		return true;
	}
	return false;
}

void parseUserInput()
{
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
}

void clearGrid()
{
	// Initialise grid to have no collision
	for(int i=0; i < GRID_SIZE; ++i)
	{
		for(int j=0; j < GRID_SIZE; ++j)
		{
			grid[GRID_SIZE * i + j] = 0;
		}
	}
}

void writeSnakeCoordinates()
{
	for(int i=0; i < snake.length; ++i)
	{
		grid[GRID_SIZE * world.x[i] + world.y[i]] = 1;
	}
}

void embiggenSnake()
{
		snake.body_x[snake.length-1] = snake.x_position;
		snake.body_y[snake.length-1] = snake.y_position;
		snake.length++;
}

static void timer(int msec)
{
	// Check whether snake has eaten a piece of food
	if(snakeHasEatenFood())
	{
		embiggenSnake();s
		generateFood();
	}

	parseUserInput();

	// This is really expensive. Should replace it with a 'hold position coords for
	// snake.length steps' setup
	for(int i=snake.length; i >= 0; --i)
	{
		if(i == 0)
		{
			world.x[0] = snake.x_position;
			world.y[0] = snake.y_position;
		}
		else
		{
			world.x[i] = world.x[i - 1];
			world.y[i] = world.y[i - 1];
		}
	}

	clearGrid();

	writeSnakeCoordinates();

	glutPostRedisplay();
	glutTimerFunc(SLEEP_T, timer, 0);
}

static void update(void)
{
	glutPostRedisplay();
}

void init()
{
	clearGrid();
	generateFood();

	// The snake starts in the middle of the arena.
	snake.x_position = GRID_SIZE / 2;
	snake.y_position = GRID_SIZE / 2;

	world.window_size[0] = INITIAL_WINDOW_WIDTH;
	world.window_size[1] = INITIAL_WINDOW_HEIGHT;
	snake.size = CELL_SIZE;
	snake.length = 1;
	snake.direction = UP;
	food.size = CELL_SIZE / 2;
}

int main( int argc, char** argv )
{
	init();

    glutInit(&argc, argv);				 									// Initialise
    glutInitDisplayMode(GLUT_SINGLE);    								// single color buffer and no depth buffer.
    glutInitWindowSize(world.window_size[0], world.window_size[1]);	// Size of display area
    glutInitWindowPosition(100,100);											// Location of window in screen coordinates.
    glutCreateWindow(WINDOW_TITLE);												// Window title.
    glutDisplayFunc(display);												// Called when redrawing window.
	glutTimerFunc(SLEEP_T, timer, 0);
	//glutReshapeFunc(&reshape);
    glutKeyboardFunc(&readUserInput);

    glutMainLoop(); 					 											// Run the event loop

	//free(grid);
    return 0;
}
