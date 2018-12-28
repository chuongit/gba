/*
 * tetris.c
 *      Author: Chuong Nguyen Thien
 */

#include "common/gba.h"
#include "common/screenmode.h"
#include "common/dma.h"
#include "common/sprite.h"
#include "common/keypad.h"
#include "common/timers.h"
#include "common/interrupt.h"
#include "tetris.h"
#include "Background.raw.c"
#include "Tetrominoes.raw.c"
#include "Cells.raw.c"
#include "Numbers.raw.c"
#include "master.pal.c"

u8 board[BOARD_HEIGHT][BOARD_WIDTH];
Tetromino playingTet;
Tetromino waitingTet;
boolean isPlaying = false;
boolean isGameOver = false;
u8 level = 1;
u16 score = 0;
boolean isActionTime = false;
u8 keys_pressed = 0xFF;

extern u16* videoBuffer;
extern u16* FrontBuffer;
extern u16* BackBuffer;

void printLog(char *s) {
	asm volatile("mov r0, %0;"
			"swi 0xff0000;"
			: // no output
			: "r" (s)
			: "r0");
}
void debuglog(char *s) {
	char buffer[100];
	sprintf(buffer, "DEBUG: %s\n", s);
	printLog(buffer);
}
void debuglogInt(int i) {
	char buffer[100];
	sprintf(buffer, "DEBUG: %d\n", i);
	printLog(buffer);
}

void WaitForVsync() {
	while ((volatile u16) REG_VCOUNT != 160) {
	}
}

//void sleep(int i) {
//	int x, y;
//	int c;
//	for (y = 0; y < i; y++) {
//		for (x = 0; x < 30000; x++)
//			c = c + 2; // do something to slow things down
//	}
//}

void loadPalette() {
	DMAcopy(master_Palette, OBJPaletteMem, sizeof(master_Palette) / sizeof(u16));
	DMAcopy(master_Palette, BGPaletteMem, sizeof(master_Palette) / sizeof(u16));
}

void loadBackground() {
	DMAcopy(Background_Bitmap, FrontBuffer,
			sizeof(Background_Bitmap) / sizeof(u16));
	//	DMAcopy(Background_Bitmap, BackBuffer,
	//			sizeof(Background_Bitmap) / sizeof(u16));
}

//void testSprites() {
//	u16 loop;
//	u16 x;
//	u16 y;
//	x = 8;
//	y = 8;
//	for (loop = 0; loop < TETROMINO_SHAPES; loop++) {
//		sprites[loop].attribute0 = COLOR_256 | SQUARE | y; //256 colors, shape and y-coord
//		sprites[loop].attribute1 = SIZE_32 | x; //size 32x32 and x-coord
//		sprites[loop].attribute2 = loop * 32; //pointer to tile where sprite starts
//		x += 40;
//		if (x > 200) {
//			x = 8;
//			y += 40;
//		}
//	}
//}

void loadSprites() {
	InitializeSprites(); //set all sprites off screen (stops artifact)
	u16 count = TETROMINO_SHAPES * 32 * 32 / 2;
	//DMAcopy(Tetrominoes_Bitmap, OAMData, count);
	DMAcopy(Tetrominoes_Bitmap, OAMDataBMP, count); // bitmap mode
}

void loadSpriteTetromino(Tetromino tetromino) {
	sprites[tetromino.OAMSpriteIndex].attribute0 = COLOR_256 | SQUARE
			| tetromino.y; //256 colors, shape and y-coord
	sprites[tetromino.OAMSpriteIndex].attribute1 = SIZE_32 | tetromino.x; //size 32x32 and x-coord
	sprites[tetromino.OAMSpriteIndex].attribute2 = 512
			+ tetromino.frames[tetromino.activeFrame]; //pointer to tile where sprite starts
}

void loadPlayingTetromino() {
	loadSpriteTetromino(playingTet);
}

void loadWaitingTetromino() {
	loadSpriteTetromino(waitingTet);
}

void startTimer(u16 level) {
	REG_TM0CNT = 0; // Turn off timer 0

	REG_TM0D = -0x4000 + (level - 1) * 1000; // -1s + level...
	REG_TM0CNT = TIME_FREQUENCY_1024 | TIME_IRQ_ENABLE | TIME_ENABLE;
}

void stopTimer() {
	REG_TM0CNT = 0; // Turn off timer 0
}

void interrupt_handler() {
	REG_IME = 0; //disable interrupts
	//handleKeys(); // slow and issue

	if ((REG_IF & INT_TIMER0) == INT_TIMER0) {
		isActionTime = true;
	}
	REG_IF |= INT_TIMER0;

	REG_IME = 1; //enable interrupts

}

void setInterupt() {
	REG_IME = 0;
	REG_INTERUPT = (u32) interrupt_handler;
	//REG_P1CNT = KEY_INTERRUPT | KEY_A | KEY_B | KEY_LEFT | KEY_RIGHT | KEY_DOWN;
	REG_IE |= INT_TIMER0;
	REG_IME = 1;
}

Tetromino randomTetromino() {
	//int xrand = REG_TM0D % TETROMINOES + 1;
	int xrand = rand() % TETROMINOES + 1;
	Tetromino tet;
	switch (xrand) {
	case 1:
		tet = Tet_O;
		break;
	case 2:
		tet = Tet_I;
		break;
	case 3:
		tet = Tet_S;
		break;
	case 4:
		tet = Tet_Z;
		break;
	case 5:
		tet = Tet_T;
		break;
	case 6:
		tet = Tet_L;
		break;
		//	case 7:
		//		tet = Tet_J;
		//		break;
	}
	return tet;
}

void newWaitingTetromino() {
	waitingTet = randomTetromino();
	waitingTet.x = WAITING_X;
	waitingTet.y = WAITING_Y;
	waitingTet.activeFrame = 0;
	waitingTet.OAMSpriteIndex = SPRITE_WAITING_TETROMINO;
	loadWaitingTetromino();
}

void newPlayingTetromino() {
	playingTet = waitingTet;
	playingTet.x = PLAYING_X;
	playingTet.y = PLAYING_Y;
	playingTet.activeFrame = 0;
	playingTet.OAMSpriteIndex = SPRITE_PLAYING_TETROMINO;
	loadPlayingTetromino();
	newWaitingTetromino();
}

void nextTetromino() {
	newPlayingTetromino();
	newWaitingTetromino();
}

void newBoard() {
	u8 i, j;
	for (i = 0; i < BOARD_HEIGHT; i++) {
		for (j = 0; j < BOARD_WIDTH; j++) {
			board[i][j] = 0;
		}
	}
}

void newGame() {
	level = 1;
	score = 0;
	startTimer(level);
	newBoard();
	loadBackground();
	newWaitingTetromino();
	newPlayingTetromino();
	showScore();
	isPlaying = true;
	isGameOver = false;
}

void showScore() {
	if (score < 10) {
		numbers(SCORE_X, SCORE_Y, 0);
		numbers(SCORE_X + 4, SCORE_Y, 0);
		numbers(SCORE_X + 8, SCORE_Y, score);
	} else if (score < 100) {
		numbers(SCORE_X, SCORE_Y, 0);
		numbers(SCORE_X + 4, SCORE_Y, score / 10);
		numbers(SCORE_X + 8, SCORE_Y, score % 10);
	} else if (score < 1000) {
		numbers(SCORE_X, SCORE_Y, score / 100);
		numbers(SCORE_X + 4, SCORE_Y, score / 10 % 10);
		numbers(SCORE_X + 8, SCORE_Y, score % 10);
	} else if (score < 10000) {
		numbers(SCORE_X, SCORE_Y, score / 1000);
		numbers(SCORE_X + 4, SCORE_Y, score / 100 % 10);
		numbers(SCORE_X + 8, SCORE_Y, score / 10 % 10);
		numbers(SCORE_X + 12, SCORE_Y, score % 10);
	}

	if (level < 10) {
		numbers(LEVEL_X, LEVEL_Y, 0);
		numbers(LEVEL_X + 4, LEVEL_Y, 0);
		numbers(LEVEL_X + 8, LEVEL_Y, level);
	} else if (level < 100) {
		numbers(LEVEL_X, LEVEL_Y, 0);
		numbers(LEVEL_X + 4, LEVEL_Y, level / 10);
		numbers(LEVEL_X + 8, LEVEL_Y, level % 10);
	}
}

void playingGame() {
	if (isActionTime && isPlaying) {
		if (!moveDown()) {
			setOnBoard(playingTet);
			checkScore();
			nextTetromino();
			if (!isValidOnBoard(playingTet)) {
				gameOver();
			}
		} else {
		}
	}
	isActionTime = false;
}

void gameOver() {
	isPlaying = false;
	isGameOver = true;
}

void handleKeys() {
	// Key Down: move down
	if (!((*KEYS) & KEY_DOWN)) { // key Down down
		keys_pressed &= (~MY_KEY_DOWN_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_DOWN_UP)) { // key Down up and was down
		keys_pressed |= MY_KEY_DOWN_UP; // set bit
		if (isPlaying) {
			moveDown();
		}
	}

	// Key Left: move left
	if (!((*KEYS) & KEY_LEFT)) { // key Left down
		keys_pressed &= (~MY_KEY_LEFT_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_LEFT_UP)) { // key Left up and was down
		keys_pressed |= MY_KEY_LEFT_UP; // set bit
		if (isPlaying) {
			moveLeft();
		}
	}

	// Key Right: move right
	if (!((*KEYS) & KEY_RIGHT)) { // key Right down
		keys_pressed &= (~MY_KEY_RIGHT_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_RIGHT_UP)) { // key Right up and was down
		keys_pressed |= MY_KEY_RIGHT_UP; // set bit
		if (isPlaying) {
			moveRight();
		}
	}

	// Key Up: rotate right
	if (!((*KEYS) & KEY_UP)) { // key Up down
		keys_pressed &= (~MY_KEY_UP_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_UP_UP)) { // key Up up and was down
		keys_pressed |= MY_KEY_UP_UP; // set bit
		if (isPlaying) {
			rotate(true);
		}
	}
	// Key A: rotate right
	if (!((*KEYS) & KEY_A)) { // key A down
		keys_pressed &= (~MY_KEY_A_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_A_UP)) { // key A up and was down
		keys_pressed |= MY_KEY_A_UP; // set bit
		if (isPlaying) {
			rotate(true);
		}
	}

	// Key B: rotate left
	if (!((*KEYS) & KEY_B)) { // key B down
		keys_pressed &= (~MY_KEY_B_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_B_UP)) { // key B up and was down
		keys_pressed |= MY_KEY_B_UP; // set bit
		if (isPlaying) {
			rotate(false);
		}
	}

	//	if (!(*KEYS & KEY_L)) {
	//	}
	//	if (!(*KEYS & KEY_R)) {
	//	}


	// Key Start: Pause, resume, or new game
	if (!((*KEYS) & KEY_START)) { // key Start down
		keys_pressed &= (~MY_KEY_START_UP); // clear bit
	} else if (!(keys_pressed & MY_KEY_START_UP)) { // key Start up and was down
		keys_pressed |= MY_KEY_START_UP; // set bit
		if (isPlaying) {
			isPlaying = false; // pause
		} else if (!isGameOver) {
			isPlaying = true; // resume
		} else {
			newGame();
		}
	}

	if (!(*KEYS & KEY_SELECT)) {
	}
}

boolean rotate(boolean right) {
	Tetromino tet = playingTet;
	if (tet.shape == Shape_O) {
		// no rotate
		return true;
	} else if (tet.shape == Shape_I || tet.shape == Shape_S || tet.shape
			== Shape_Z) { // 2 variants
		tet.activeFrame = 1 - tet.activeFrame;
	} else if (tet.shape == Shape_T || tet.shape == Shape_L /*|| tet.shape
	 == Shape_J*/) { // 4 variants
		if (right) {
			tet.activeFrame += 1;
			if (tet.activeFrame > 3) {
				tet.activeFrame = 0;
			}
		} else {
			if (tet.activeFrame == 0) {
				tet.activeFrame = 3;
			} else {
				tet.activeFrame -= 1;
			}
		}
	}
	if (isValidOnBoard(tet)) {
		playingTet.activeFrame = tet.activeFrame;
		loadPlayingTetromino();
		return true;
	}
	return false;
}

boolean moveLeft() {
	Tetromino tet = playingTet;
	tet.x -= TILE_CELL_SIZE;
	if (isValidOnBoard(tet)) {
		playingTet.x = tet.x;
		loadPlayingTetromino();
		return true;
	}
	return false;
}

boolean moveRight() {
	Tetromino tet = playingTet;
	tet.x += TILE_CELL_SIZE;
	if (isValidOnBoard(tet)) {
		playingTet.x = tet.x;
		loadPlayingTetromino();
		return true;
	}
	return false;
}

boolean moveDown() {
	Tetromino tet = playingTet;
	tet.y += TILE_CELL_SIZE;
	if (isValidOnBoard(tet)) {
		playingTet.y = tet.y;
		loadPlayingTetromino();
		return true;
	}
	return false;
}

boolean isValidCell(s16 ix, s16 iy) {
	return (ix >= 0 && ix < BOARD_WIDTH && iy >= 0 && iy < BOARD_HEIGHT);
}

boolean isValidOnBoard(Tetromino tet) {
	u8 invalid = 0;
	u8 valid = 0;
	ShapeMatrix* matrix = Matrix_Shapes[tet.shape][tet.activeFrame];
	s16 bx = (tet.x - BOARD_X) / TILE_CELL_SIZE;
	s16 by = BOARD_HEIGHT - ((tet.y - BOARD_Y) / TILE_CELL_SIZE) - 1;
	u8 i, j;
	for (i = 0; i < 4; i++) {
		s16 iy = by - i;
		for (j = 0; j < 4; j++) {
			s16 ix = bx + j;
			if (!isValidCell(ix, iy)) { // skip invalid cell
				if ((*matrix)[i][j] > 0) {
					++invalid;
					break;
				} else {
					continue;
				}
			} else if (board[iy][ix] > 0 && (*matrix)[i][j] > 0) { // overwrite cell
				++invalid;
				break;
			} else {
				++valid;
			}
		}
		if (invalid > 0) {
			break;
		}
	}
	return (invalid == 0 && valid > 0);
}

void setOnBoard(Tetromino tet) {
	ShapeMatrix* matrix = Matrix_Shapes[tet.shape][tet.activeFrame];
	s16 bx = (tet.x - BOARD_X) / TILE_CELL_SIZE;
	s16 by = BOARD_HEIGHT - ((tet.y - BOARD_Y) / TILE_CELL_SIZE) - 1;
	u8 i, j;
	for (i = 0; i < 4; i++) {
		s16 iy = by - i;
		for (j = 0; j < 4; j++) {
			s16 ix = bx + j;
			if (isValidCell(ix, iy) && (*matrix)[i][j] > 0) {
				board[iy][ix] = (*matrix)[i][j];
				drawCell(ix, iy);
			}
		}
	}
}

void drawCell(u8 ix, u8 iy) {
	if (board[iy][ix] > 0) {
		int x = (BOARD_X + ix * TILE_CELL_SIZE) / 2;
		int y = BOARD_Y + (BOARD_HEIGHT - iy - 1) * TILE_CELL_SIZE;
		u8 ii, jj;
		u16 shift = (board[iy][ix] - 1) * TILE_CELL_SIZE * TILE_CELL_SIZE;
		for (ii = 0; ii < TILE_CELL_SIZE; ii++) {
			for (jj = 0; jj < TILE_CELL_SIZE; jj += 2) {
				plotPixel(
						x + jj / 2,
						y + ii,
						((u16*) Cells_Bitmap)[(shift + (ii * TILE_CELL_SIZE)
								+ jj) / 2]);
			}
		}
	}
}

void drawBoard() {
	u8 i, j;
	for (i = 0; i < BOARD_HEIGHT; i++) {
		for (j = 0; j < BOARD_WIDTH; j++) {
			drawCell(j, i);
		}
	}
}

void checkScore() {
	u8 i, j;
	u16 sc = 0;
	for (i = 0; i < BOARD_HEIGHT; i++) {
		u8 check = 0;
		for (j = 0; j < BOARD_WIDTH; j++) {
			if (board[i][j] > 0) {
				++check;
			}
		}
		if (check == BOARD_WIDTH) {
			// add score
			++sc;
			// update board
			u8 ii, jj;
			for (ii = i + 1; ii < BOARD_HEIGHT; ii++) {
				for (jj = 0; jj < BOARD_WIDTH; jj++) {
					board[ii - 1][jj] = board[ii][jj];
				}
			}
			for (jj = 0; jj < BOARD_WIDTH; jj++) {
				board[ii - 1][jj] = 0;
			}
			// recheck this row
			i--;
		}
	}
	if (sc > 0) {
		score += sc;
		checkLevel(score);
		loadBackground();
		showScore();
		drawBoard();
	}
}

void checkLevel(u16 sc) {
	if (sc % 30 == 0 && level < MAX_LEVEL) {
		++level;
	}
}

void numbers(int numx, int numy, int num) {
	int x, y;
	int temp;

	switch (num) {
	case 0:
		temp = 0;
		break;
	case 1:
		temp = 8;
		break;
	case 2:
		temp = 16;
		break;
	case 3:
		temp = 24;
		break;
	case 4:
		temp = 32;
		break;
	case 5:
		temp = 40;
		break;
	case 6:
		temp = 48;
		break;
	case 7:
		temp = 56;
		break;
	case 8:
		temp = 64;
		break;
	case 9:
		temp = 72;
		break;
	}

	for (y = 0; y < 8; y++) {
		for (x = 0; x < (TILE_CELL_SIZE / 2); x++) {
			plotPixel(x + numx, y + numy,
					Numbers_Bitmap[(y + temp) * (TILE_CELL_SIZE / 2) + x]);
		}
	}
}

int main(void) {
	//set mode 4 and enable sprite and 1D mapping
	//Note: Bitmap background modes (modes 3-5) run into sprite memory,
	//so only the second charblock (tiles 512-1023) is available
	SetMode( MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D);

	// load palette, sprites
	loadPalette();
	loadSprites();

	// interrupt
	setInterupt();

	// start new game
	newGame();

	//main loop
	while (1) {
		handleKeys();
		WaitForVsync(); //wait for screen to stop drawing
		playingGame();
		CopyOAM(); //copy sprite into OAM
	}
}
