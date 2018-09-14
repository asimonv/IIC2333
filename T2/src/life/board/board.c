#include <stdio.h>
#include <stdlib.h>
#include "board.h"

Board* init_board(Position* positions, int num_positions, char* name, int A, int B, int C, int size, int id) {
  Board* board = malloc(sizeof(Board));
  board->positions = positions;
  board->num_positions = num_positions;
  board->alive_cells = num_positions;
  board->name = name;
  board->A = A;
  board->B = B;
  board->C = C;
  board->id = id;
  board->reason = 0;
  board->size = size;
  return board;
}
