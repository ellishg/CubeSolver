//
//  definitions.h
//  Cube
//
//  Created by Ellis Sparky Hoag on 7/2/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

#ifndef definitions_h
#define definitions_h

#define FRONT 0b000
#define RIGHT 0b001
#define BACK 0b010
#define LEFT 0b011
#define UP 0b100
#define DOWN 0b101

#define RED FRONT
#define BLUE RIGHT
#define ORANGE BACK
#define GREEN LEFT
#define WHITE UP
#define YELLOW DOWN

#define CLOCKWISE 1
#define HALF_TURN 2
#define COUNTER_CLOCKWISE 3

#define INVALID_STATE 0xFF
#define INVALID_INDEX -1

static const char * face_to_char[18] = {
    "F ", "R ", "B ", "L ", "U ", "D ",
    "F2 ", "R2 ", "B2 ", "L2 ", "U2 ", "D2 ",
    "F' ", "R' ", "B' ", "L' ", "U' ", "D' "
};

static const int facelet_index[120] = {
    //Corners
    UP, 6,
    LEFT, 2,
    FRONT, 0,
    
    UP, 8,
    RIGHT, 0,
    FRONT, 2,
    
    UP, 2,
    RIGHT, 2,
    BACK, 0,
    
    UP, 0,
    LEFT, 0,
    BACK, 2,
    
    DOWN, 0,
    LEFT, 8,
    FRONT, 6,
    
    DOWN, 2,
    RIGHT, 6,
    FRONT, 8,
    
    DOWN, 8,
    RIGHT, 8,
    BACK, 6,
    
    DOWN, 6,
    LEFT, 6,
    BACK, 8,

    //Edges
    UP, 7,
    -1, -1,
    FRONT, 1,
    
    UP, 3,
    LEFT, 1,
    -1, -1,
    
    UP, 1,
    -1, -1,
    BACK, 1,
    
    UP, 5,
    RIGHT, 1,
    -1, -1,
    
    -1, -1,
    LEFT, 5,
    FRONT, 3,
    
    -1, -1,
    LEFT, 3,
    BACK, 5,
    
    -1, -1,
    RIGHT, 5,
    BACK, 3,
    
    -1, -1,
    RIGHT, 3,
    FRONT, 5,
    
    DOWN, 1,
    -1, -1,
    FRONT, 7,
    
    DOWN, 3,
    LEFT, 7,
    -1, -1,
    
    DOWN, 7,
    -1, -1,
    BACK, 7,
    
    DOWN, 5,
    RIGHT, 7,
    -1, -1
};

static const unsigned int corner_cubie_to_facelet_color[24] = {
    WHITE, GREEN, RED,
    WHITE, RED, BLUE,
    WHITE, BLUE, ORANGE,
    WHITE, ORANGE, GREEN,
    YELLOW, RED, GREEN,
    YELLOW, BLUE, RED,
    YELLOW, ORANGE, BLUE,
    YELLOW, GREEN, ORANGE
};

static const unsigned int edge_cubie_to_facelet_color[24] = {
    WHITE, RED,
    WHITE, GREEN,
    WHITE, ORANGE,
    WHITE, BLUE,
    RED, GREEN,
    ORANGE, GREEN,
    ORANGE, BLUE,
    RED, BLUE,
    YELLOW, RED,
    YELLOW, GREEN,
    YELLOW, ORANGE,
    YELLOW, BLUE
};

static unsigned int front_face_move[40] = {
    4, 0, 2, 3, 5, 1, 6, 7,
    2, 1, 0, 0, 1, 2, 0, 0,
    4, 1, 2, 3, 8, 5, 6, 0, 7, 9, 10, 11,
    1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0
};

static unsigned int left_face_move[40] = {
    3, 1, 2, 7, 0, 5, 6, 4,
    1, 0, 0, 2, 2, 0, 0, 1,
    0, 5, 2, 3, 1, 9, 6, 7, 8, 4, 10, 11,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned int back_face_move[40] = {
    0, 1, 6, 2, 4, 5, 7, 3,
    0, 0, 2, 1, 0, 0, 1, 2,
    0, 1, 6, 3, 4, 2, 10, 7, 8, 9, 5, 11,
    0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0
};

static unsigned int right_face_move[40] = {
    0, 5, 1, 3, 4, 6, 2, 7,
    0, 2, 1, 0, 0, 1, 2, 0,
    0, 1, 2, 7, 4, 5, 3, 11, 8, 9, 10, 6,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned int up_face_move[40] = {
    1, 2, 3, 0, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned int down_face_move[40] = {
    0, 1, 2, 3, 7, 4, 5, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 8,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned int * face_moves[6] = {
    front_face_move,
    right_face_move,
    back_face_move,
    left_face_move,
    up_face_move,
    down_face_move
};

#endif /* definitions_h */
