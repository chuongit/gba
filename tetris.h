/*
 * tetris.h
 *      Author: Chuong Nguyen Thien
 */

#ifndef TETRIS_H_
#define TETRIS_H_

typedef unsigned char boolean;
#define false 0
#define true (!false)

#define BOARD_WIDTH 11
#define BOARD_HEIGHT 18

//exceed 512 bytes in bitmap mode, skip 1 shape
#define TETROMINOES 6
#define TETROMINO_SHAPES 15

#define TILE_CELL_SIZE 8
#define SPRITE_SIZE 32

#define BOARD_X 30
#define BOARD_Y 10
#define PLAYING_X BOARD_X+40
#define PLAYING_Y BOARD_Y
#define WAITING_X 180
#define WAITING_Y 20
#define LEVEL_X 210
#define LEVEL_Y 60
#define SCORE_X 210
#define SCORE_Y 90

#define SPRITE_PLAYING_TETROMINO 0
#define SPRITE_WAITING_TETROMINO 1
//#define SPRITE_SCORE 2
//#define SPRITE_LEVEL 3

#define MY_KEY_START_UP 64
#define MY_KEY_LEFT_UP 32
#define MY_KEY_RIGHT_UP 16
#define MY_KEY_DOWN_UP 8
#define MY_KEY_UP_UP 4
#define MY_KEY_A_UP 2
#define MY_KEY_B_UP 1

#define MAX_LEVEL 16

enum Shape {
	Shape_O = 0, Shape_I = 1, Shape_S = 2, Shape_Z = 3, Shape_T = 4, Shape_L = 5/*, Shape_J = 6*/
};

typedef struct {
	s16 x;
	s16 y;
	u16 frames[4];
	u8 activeFrame;
	u8 OAMSpriteIndex;
	enum Shape shape;
} Tetromino;

Tetromino Tet_O = { 240, 160, { 0 }, 0, 0, Shape_O };
Tetromino Tet_I = { 240, 160, { 32, 64 }, 0, 0, Shape_I };
Tetromino Tet_S = { 240, 160, { 96, 128 }, 0, 0, Shape_S };
Tetromino Tet_Z = { 240, 160, { 160, 192 }, 0, 0, Shape_Z };
Tetromino Tet_T = { 240, 160, { 224, 256, 288, 320 }, 0, 0, Shape_T };
Tetromino Tet_L = { 240, 160, { 352, 384, 416, 448 }, 0, 0, Shape_L };
//Tetromino Tet_J = { 240, 160, { 480, 512, 544, 576 }, 0, 0, Shape_J };


typedef u8 ShapeMatrix[4][4];

#define MATRIX_O  {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}
#define MATRIX_I1 {{0,0,0,0},{2,2,2,2},{0,0,0,0},{0,0,0,0}}
#define MATRIX_I2 {{0,0,2,0},{0,0,2,0},{0,0,2,0},{0,0,2,0}}
#define MATRIX_S1 {{0,0,0,0},{0,3,3,0},{3,3,0,0},{0,0,0,0}}
#define MATRIX_S2 {{3,0,0,0},{3,3,0,0},{0,3,0,0},{0,0,0,0}}
#define MATRIX_Z1 {{0,0,0,0},{4,4,0,0},{0,4,4,0},{0,0,0,0}}
#define MATRIX_Z2 {{0,0,4,0},{0,4,4,0},{0,4,0,0},{0,0,0,0}}
#define MATRIX_T1 {{0,0,0,0},{5,5,5,0},{0,5,0,0},{0,0,0,0}}
#define MATRIX_T2 {{0,5,0,0},{5,5,0,0},{0,5,0,0},{0,0,0,0}}
#define MATRIX_T3 {{0,0,0,0},{0,5,0,0},{5,5,5,0},{0,0,0,0}}
#define MATRIX_T4 {{0,5,0,0},{0,5,5,0},{0,5,0,0},{0,0,0,0}}
#define MATRIX_L1 {{0,0,0,0},{6,6,6,0},{6,0,0,0},{0,0,0,0}}
#define MATRIX_L2 {{6,6,0,0},{0,6,0,0},{0,6,0,0},{0,0,0,0}}
#define MATRIX_L3 {{0,0,0,0},{0,0,6,0},{6,6,6,0},{0,0,0,0}}
#define MATRIX_L4 {{0,6,0,0},{0,6,0,0},{0,6,6,0},{0,0,0,0}}
//#define MATRIX_J1 {{0,0,0,0},{7,7,7,0},{0,0,7,0},{0,0,0,0}}
//#define MATRIX_J2 {{0,7,0,0},{0,7,0,0},{7,7,0,0},{0,0,0,0}}
//#define MATRIX_J3 {{0,0,0,0},{7,0,0,0},{7,7,7,0},{0,0,0,0}}
//#define MATRIX_J4 {{0,7,7,0},{0,7,0,0},{0,7,0,0},{0,0,0,0}}

const ShapeMatrix Matrix_Shapes[TETROMINOES][4] =
	{{MATRIX_O,MATRIX_O,{},{}},
	{MATRIX_I1, MATRIX_I2,{},{}},
	{MATRIX_S1, MATRIX_S2,{},{}},
	{MATRIX_Z1, MATRIX_Z2,{},{}},
	{MATRIX_T1, MATRIX_T2, MATRIX_T3, MATRIX_T4},
	{MATRIX_L1, MATRIX_L2, MATRIX_L3, MATRIX_L4}};
//	{MATRIX_J1, MATRIX_J2, MATRIX_J3, MATRIX_J4}};

#endif /* TETRIS_H_ */
