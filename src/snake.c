#include <stdio.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <unistd.h>
#include <math.h>
#include "framework/definitions.h"

// TODO : Move some of these to definitions.h, Create typedefs.
// TODO : Add toggle for debug logging
struct world {
	GLsizei window_height;
	GLsizei window_width;
} world;

struct point
{
	int x;
	int y;
};

typedef struct point point;

struct snake
{
	GLfloat size;
	point position;
	point body[256];
	GLint direction;
	GLint length;
} snake;

struct food {
	struct point position;
	GLfloat size;
} food;

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

struct point randGridPoint()
{
	point randPoint;
	randPoint.x = rand() % GRID_SIZE;
	randPoint.y = rand() % GRID_SIZE;

	return randPoint;
}

// TODO : Slight refactor to call this a single time on a given point to return a tuple
float convertGridToFloat(int gridPoint)
{
	if(gridPoint - GRID_SIZE != 0)
	{
		return (float)(gridPoint - GRID_SIZE / 2.0) / (GRID_SIZE / 2.0);
	}

	return 0.0f;
}

void drawSquare(point point, GLfloat square_size)
{
	GLfloat x;
	GLfloat y;

	// Convert grid co-ordinates to floating point values.
	x = convertGridToFloat(point.x);
	y = convertGridToFloat(point.y);

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

	fprintf(stdout, "draw square at, x=%d,y=%d\n", point.x, point.y);
}

void generateFood()
{
	food.position = randGridPoint();
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

	// Draw food
	drawSquare(food.position, food.size);

	// Draw snake
	for(int i = 0; i < GRID_SIZE; ++i)
	{
		for(int j = 0; j < GRID_SIZE; ++j)
		{
			point snakeBodyPosition = {i, j};
			if(grid[GRID_SIZE * i + j])
				drawSquare(snakeBodyPosition, snake.size);
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
			fprintf(stdout, "Up, x=%d,y=%d\n", snake.position.x, snake.position.y);
		}
	}
	if(key == DOWN_KEY)
	{
		if(snake.direction != UP && snake.direction != DOWN)
		{
			snake.direction = DOWN;
			fprintf(stdout, "Down, x=%d,y=%d\n", snake.position.x, snake.position.y);
		}
	}
	if(key == LEFT_KEY)
	{
		if(snake.direction != RIGHT && snake.direction != LEFT)
		{
			snake.direction = LEFT;
			fprintf(stdout, "Left, x=%d,y=%d\n", snake.position.x, snake.position.y);
		}
	}
	if(key == RIGHT_KEY)
	{
		if(snake.direction != LEFT && snake.direction != RIGHT)
		{
			snake.direction = RIGHT;
			fprintf(stdout, "Right, x=%d,y=%d\n", snake.position.x, snake.position.y);
		}
	}
}

static void reshape(int width, int height)
{
	world.window_width = width;
	world.window_height = height;

	// To prevent a division by 0
	if(height == 0)
	{
		height = 1;
	}

	float aspectRatio = 1.0 * width / height;
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(45, aspectRatio, 1, 1000);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
}

bool snakeHasEatenFood()
{
	if(snake.position.x == food.position.x && snake.position.y == food.position.y)
	{
		return true;
	}
	return false;
}

bool snakeHasEatenSelf()
{
	for(int i = 1; i < 256; i++)
	{
		if(snake.position.x == snake.body[i].x && snake.position.y == snake.body[i].y)
		{
			return true;
		}
	}
	return false;
}

void parseUserInput()
{
	// Check snake direction
	if(snake.direction == UP)
		snake.position.y++;
	if(snake.direction == DOWN)
		snake.position.y--;
	if(snake.direction == LEFT)
		snake.position.x--;
	if(snake.direction == RIGHT)
		snake.position.x++;

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

void init()
{
	clearGrid();
	generateFood();

	// The snake starts in the middle of the arena.
	snake.position.x = GRID_SIZE / 2;
	snake.position.y = GRID_SIZE / 2;

	world.window_width = INITIAL_WINDOW_WIDTH;
	world.window_height = INITIAL_WINDOW_HEIGHT;
	snake.size = CELL_SIZE;
	snake.length = 1;
	snake.direction = UP;
	food.size = CELL_SIZE / 2;
}

void writeSnakeCoordinates()
{
	for(int i=0; i < snake.length; ++i)
	{
		grid[GRID_SIZE * snake.body[i].x + snake.body[i].y] = 1;
	}
}

void embiggenSnake()
{
		// TODO : remove the next two lines. Make them unnecessary by keeping coordinates static until they time out.
		snake.body[snake.length-1].x = snake.position.x;
		snake.body[snake.length-1].y = snake.position.y;
		snake.length++;
}

static void timer(int msec)
{

	if(snakeHasEatenSelf())
	{
		// TODO : create game over screen
		init();
	}

	// Check whether snake has eaten a piece of food
	if(snakeHasEatenFood())
	{
		embiggenSnake();
		generateFood();
	}

	parseUserInput();

	// This is really expensive. Should replace it with a 'hold position coords for
	// snake.length steps' setup
	for(int i=snake.length; i >= 0; --i)
	{
		if(i == 0)
		{
			snake.body[0].x = snake.position.x;
			snake.body[0].y = snake.position.y;
		}
		else
		{
			snake.body[i].x = snake.body[i - 1].x;
			snake.body[i].y = snake.body[i - 1].y;
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

int main( int argc, char** argv )
{
	init();

    glutInit(&argc, argv);				 									// Initialise
    glutInitDisplayMode(GLUT_SINGLE);    								// single color buffer and no depth buffer.
    glutInitWindowSize(world.window_width, world.window_height);	// Size of display area
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
