#include <ncurses.h>
#include <time.h>

#define FIELD_SIZE 15
#define SNAKE_INIT_SIZE 3
#define SNAKE_MAX_SIZE 200

typedef struct {
	int x;
	int y;
}Position;

typedef struct {
	Position pos;
}SnakeBodyPart;

typedef struct {
	Position pos;
}Food;

const char SNAKE_HEAD_CHAR = 'H';
const char SNAKE_CHAR = 'S';
const char FOOD_CHAR = 'F';

enum direction { 
	right = 0,
	up = 1,
	left = 2, 
	down = 3, 
};
/*
	-1 -> SNAKE
	0 -> NOTHING
	1 -> FOOD
*/
int field[FIELD_SIZE][FIELD_SIZE];
SnakeBodyPart snake[SNAKE_MAX_SIZE];
int snakeSize = SNAKE_INIT_SIZE;
enum direction snakeDirection;
Food food;
int points = 0;

int isDirectionAllowed(enum direction direction) {
	if ( direction == left && snakeDirection == right) {
		return false;
	}
	if ( direction == right && snakeDirection == left) {
		return false;
	}
	if ( direction == up && snakeDirection == down) {
		return false;
	}
	if ( direction == down && snakeDirection == up) {
		return false;
	}
	return true;
}

void changeSnakeDirection() {
	int keyPressed;

	keyPressed = getch();

	
	if ( keyPressed == KEY_UP ) {
		if ( isDirectionAllowed(up) ) {
			snakeDirection = up;
		}
	}
	if ( keyPressed == KEY_DOWN ) {
		if ( isDirectionAllowed(down) ) {
			snakeDirection = down;
		}
	}
	if ( keyPressed == KEY_LEFT ) {
		if ( isDirectionAllowed(left) ) {
			snakeDirection = left;
		}
	}
	if ( keyPressed == KEY_RIGHT ) {
		if ( isDirectionAllowed(right) ) {
			snakeDirection = right;
		}
	}
	snake[0].pos.x = (snake[0].pos.x + 15) % FIELD_SIZE;
	snake[0].pos.y = (snake[0].pos.y + 15) % FIELD_SIZE;
	/*if ( snake[0].pos.x < 0 ) {
		snake[0].pos.x += FIELD_SIZE;
	}
	if ( snake[0].pos.y < 0 ) {
		snake[0].pos.y += FIELD_SIZE;
	}*/
}

void snakeMove() {
	Position tmpPosition2, tmpPosition;
	int i;
	field[snake[snakeSize-1].pos.x][snake[snakeSize-1].pos.y] = 0;
	tmpPosition = snake[0].pos;

	//initialHeadPosition.x = snake[0].pos.x;
	//initialHeadPosition.y = snake[0].pos.y;
	//snake head move
	switch (snakeDirection) {
		case up:
			snake[0].pos.x--;
			break;
		case down:
			snake[0].pos.x++;
			break;
		case left:
			snake[0].pos.y--;
			break;
		case right:
			snake[0].pos.y++;
			break;
	}

	for ( i = 1; i < snakeSize; i++ ) {
		tmpPosition2 = snake[i].pos;
		snake[i].pos = tmpPosition;
		tmpPosition = tmpPosition2;
	}
}

int checkFoodCol() {
	int i;

	for ( i = 0; i < snakeSize; i++ ) {
		if ( food.pos.x == snake[i].pos.x && food.pos.y == snake[i].pos.y ) {
			return true;
		}
	}
	return false;
}

//generates food at random position
void generateFood() {
	int i;
	field[food.pos.x][food.pos.y] = 0;
	do {
		food.pos.x = rand()%FIELD_SIZE;
		food.pos.y = rand()%FIELD_SIZE;
	}while ( checkFoodCol() );
}

void checkWallCol() {
}

int checkSnakeCol() {
	int i;

	for ( i = 1; i < snakeSize; i++ ) {
		if ( snake[0].pos.x == snake[i].pos.x && snake[0].pos.y == snake[i].pos.y ) {
			return true;
		}
	}
	return false;
}

void snakeGrowth() {
	if ( snakeSize != SNAKE_MAX_SIZE ) {
		snakeSize++;
	}
}

void gameInit() {
	int i, j;

	snakeSize = 3;
	points = 0;
	for ( i = 0; i < FIELD_SIZE; ++i) 	{
		for ( j = 0; j < FIELD_SIZE; j++ ) {
			field[i][j] = 0;
		}
	}
	snakeDirection = right;
	for ( i = 0; i < SNAKE_INIT_SIZE; i++ ) {
		snake[i].pos.y = 4 - i;
		snake[i].pos.x = 1;
	}

	generateFood();
}

/*
	iterates through the field and checks if the snake
	is at that position, if so, then sets the field value to 0
*/
void recreateField() {
	int posX, posY, k;

	for ( posX = 0; posX < FIELD_SIZE; posX++ ) {
		for ( posY = 0; posY < FIELD_SIZE; posY++ ) {
			if ( posX == food.pos.x && posY == food.pos.y ) {
				field[posX][posY] = 1;
			}
			for ( k = 0; k < snakeSize; k++ ) {
				if ( posX == snake[k].pos.x && posY == snake[k].pos.y ) {
					field[posX][posY] = -1;
				}
			}
		}
	}
}

void increasePoints(int increaseAmount) {
	points += increaseAmount;
}

void printField() {
	int i, j;
	printw(" ------------------------------\n");
	for ( i = 0; i < FIELD_SIZE; i++ ) {
		printw("|");
		for ( j = 0; j < FIELD_SIZE; j++ ) {
			if ( field[i][j] == 0 ) {
				printw("  ");
			}
			else if ( field[i][j] == -1 ) {
				printw("S ");
			}
			else if ( field[i][j] == 1 ) {
				printw("F ");
			}
		}
		printw("|\n");
	}
	printw(" ------------------------------\n");
	printw("Points: %i", points);
	refresh();
}

int gameLoop() {
	while(!checkSnakeCol()){
		clear();
		recreateField();
		printField();
		if ( checkFoodCol() ) {
			snakeGrowth();
			generateFood();
			increasePoints(9);
		}
		changeSnakeDirection();
		snakeMove();
		usleep(200000);
	}
	printw("\nYou have lost. You have earned %i points. You ROCK MAN !!\n", points);
}

int main() {
	int ch;

	srand(time(0));

	initscr();			/* Start curses mode 		  */
	keypad(stdscr, TRUE);
	//makes sure that getchar doesn't wait for input
	//if a key is pressed it reads it
	nodelay(stdscr, TRUE);
	noecho();
	while ( ch != 'q') {
		gameInit();
		gameLoop();
		printw("Press q to quit, or anyhting else to play again !");
		nodelay(stdscr, FALSE);
		ch = getch();
		nodelay(stdscr, TRUE);
	}
	endwin();			/* End curses mode		  */

	return 0;
}
