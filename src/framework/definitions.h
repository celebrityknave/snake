
// Define window parameters
#define INITIAL_WINDOW_WIDTH 500
#define INITIAL_WINDOW_HEIGHT 500
#define TARGET_FPS 4
#define GRID_SIZE 20

// Define directions
#define UP 		0
#define DOWN	1
#define LEFT 	2
#define RIGHT	3

// Define user-inputs
#define UP_KEY 'w'
#define DOWN_KEY 's'
#define LEFT_KEY 'a'
#define RIGHT_KEY 'd'
#define QUIT_KEY 'q'

// Other values
#define WINDOW_TITLE "snake"
#define EPSILON 1E-5
#define SLEEP_T (1.0f / TARGET_FPS) * 1000
#define CELL_SIZE (1.0f / GRID_SIZE)