#include "../position/position.h"

typedef struct board {
  Position* positions;
  int num_positions;
  int alive_cells;
  char* name;
  int size;
  int reason;
  int end_time;
  int **matrix;
  int id;
  int A;
  int B;
  int C;
} Board;

Board* init_board(Position* positions, int num_positions, char* name, int A, int B, int C, int size, int id);
