#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <wchar.h>
#include <locale.h>
#include "board/board.h"
#include "queue/queue.h"

#define for_x for (int x = 0; x < w; x++)
#define for_y for (int y = 0; y < h; y++)
#define for_xy for_x for_y

#define MAX_QUEUE 4
#define OUTPUT_FILENAME "output.csv"
Board* boards;
int t;
int t_i;
int num_boards;

enum signals {LOOP, NOCELLS, NOTIME, SIGNAL, DEFAULT};

void show(int **univ, int w, int h, int t, Board * board)
{
	//printf("\033[2J");
	//printf("\033[H");
	for_y {
		for_x
      printf(" %lc ", univ[y][x] ? (wint_t) 0x25FC : (wint_t) 0x25FB );
		printf("\n");
	}

  /*printf("\033[H");
	for_y {
		for_x printf(univ[y][x] ? "\033[07m  \033[m" : "  ");
		printf("\033[E");
	}
	fflush(stdout);
  */
	printf("[%s, t=%d, alive_cells: %d]\n", board->name, t, board->alive_cells);
	fflush(stdout);

  /*for_xy {
    printf("%X\n", univ[y][x] ? 0x25A0 : 0x25A1);
  }*/
}

Board* find_board(Board* boards, int pid) {
  for (size_t i = 0; i < num_boards; i++) {
    if (boards[i].id == pid) {
      return &boards[i];
    }
  }
  return NULL;
}

void write_exit(Board *board){
	printf("write_exit %s\n", board->name);
	char* board_name = malloc(strlen(board->name)+1);
	strcpy(board_name, board->name);
	char* filename = strcat(board_name, ".txt");
	printf("%s\n", filename);
	FILE *board_file = fopen(filename, "w+");

	if (board_file != NULL) {
		char* str_reason = "";

		switch (board->reason) {
			case 0:
				str_reason = "LOOP";
				break;
			case 1:
				str_reason = "NOCELLS";
				break;
			case 2:
				str_reason = "NOTIME";
				break;
			case 3:
				str_reason = "SIGNAL";
				break;
			case 4:
				str_reason = "DEFAULT";
				break;
		}
		fprintf(board_file, "%s,%d,%d,%s\n", board->name, board->end_time, board->alive_cells, str_reason);
		printf("%s,%d,%d,%s\n", board->name, board->end_time, board->alive_cells, str_reason);
		fclose(board_file);
	}
}

void intHandler() {
  signal(SIGINT, intHandler);
  printf("interrupted\n");
  Board* board = find_board(boards, getpid());
	board->reason = SIGNAL;
  printf("board: %s interrupted\n", board->name);
	write_exit(board);
	exit(0);
}

void generate_report(Board *boards, int num_boards) {
  FILE *output_file = fopen(OUTPUT_FILENAME, "w+");
	char c;
  if (output_file != NULL) {
    for (size_t i = 0; i < num_boards; i++) {
			char *filename = strcat(boards[i].name, ".txt");
			FILE *input_file = fopen(filename, "r");
			if (input_file != NULL) {
				while ((c = fgetc(input_file)) != EOF)
	      	fputc(c, output_file);
				fclose(input_file);
				remove(filename);
			}
    }
    fclose(output_file);
  }
}

// https://stackoverflow.com/a/39211250/5666746
int **copy_matrix(int **mat, int size1, int size2){
  int row;

  int **res = malloc(size1 * sizeof(int *));
  for(row = 0; row < size1; row++) {
      res[row] = malloc(size2 * sizeof(int));
      //memcpy(res[row], mat[row], size2 * sizeof(int));
  }

	for (size_t i = 0; i < size1; i++) {
		for (size_t j = 0; j < size2; j++) {
			res[i][j] = mat[i][j];
		}
	}
  return res;
}

int compare_matrices(int** m1, int**m2, int size){
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			if (m1[i][j] != m2[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

int loop_check(Queue *q, int** actual, int size) {
	node *root = q->list->head;
	while (root) {
		if (compare_matrices(root->item, actual, size)) {
			return 1;
		}
		root = root->next;
	}
	return 0;
}

int check_alive(int **m, int size){
	int count = 0;
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			if (m[i][j] == 1) {
				count += 1;
			}
		}
	}
	return count;
}



void free_array(int** array, int size){
	for (int i = 0; i < size; i++) {
		free(array[i]);
	}

	free(array);
}

void cleanup() {
  for (size_t i = 0; i < num_boards; i++) {
    free_array(boards[i].matrix, boards[i].size);
  }
}

int countLiveNeighbors(int y, int x, int size, int** board)
{
    // The number of live neighbors.
    int value = 0;

    // This nested loop enumerates the 9 cells in the specified cells neighborhood.
    for (int j = -1; j <= 1; j++) {
        // If loopEdges is set to false and y+j is off the board, continue.
        if (y + j < 0 || y + j >= size) {
            continue;
        }

        for (int i = -1; i <= 1; i++) {
            // If loopEdges is set to false and x+i is off the board, continue.
            if (x + i < 0 || x + i >= size) {
                continue;
            }
            // Count the neighbor cell at (h,k) if it is alive.
            value += board[y+j][x+i] ? 1 : 0;
        }
    }

    // Subtract 1 if (x,y) is alive since we counted it as a neighbor.
    return value - (board[y][x] ? 1 : 0);
}

/* https://rosettacode.org/wiki/Conway%27s_Game_of_Life */
void evolve(int **univ, int w, int h, Board* board)
{
	int **new = calloc(h, h * sizeof(int*));
	for (int i = 0; i < w; i++) {
		new[i] = calloc(w, w * sizeof(int));
	}

	for (size_t y = 0; y < h; y++) {
		for (size_t x = 0; x < w; x++) {
			int n = countLiveNeighbors(y, x, w, board->matrix);
			int c = univ[y][x];
			new[y][x] = (c && (n >= board->B && n <= board->C)) || (!c && n == board->A);
		}
	}

	for_y for_x univ[y][x] = new[y][x];
	free_array(new, w);

}

void set_positions(int** board, Board* data_board) {
	//printf("num_positions: %d\n", data_board->num_positions);
	for (size_t i = 0; i < data_board->num_positions; i++) {
		Position *pos = &data_board->positions[i];
		board[data_board->size - pos->y - 1][pos->x] = 1;
	}
}

void game(Board* board, int t)
{
	int w = board->size;
	int h = board->size;
	t_i = t;
	Queue* history = queue_init();
	/*int **univ = calloc(h, h * sizeof(int*));
	for (int i = 0; i < w; i++) {
    univ[i] = calloc(w, w * sizeof(int));
	}*/

	set_positions(board->matrix, board);
  show(board->matrix, w, h, t_i - t, board);
	int has_looped = 0;
	while (t && board->alive_cells && !has_looped) {
		//show(univ, w, h, t_i-t, board);
		evolve(board->matrix, w, h, board);
    //show(board->matrix, w, h, t_i - t, board);
		board->end_time = t_i - t;
		has_looped = loop_check(history, board->matrix, board->size);
		node *play = malloc(sizeof(node));
		play->item = copy_matrix(board->matrix, board->size, board->size);
		queue_push(history, play, true);
		if (history->list->length == MAX_QUEUE + 1) {
			queue_pop(history);
		}
		board->alive_cells = check_alive(board->matrix, board->size);
		//usleep(200000);
		t-=1;
	}

  enum signals reason = DEFAULT;

	if (!board->alive_cells) {
		printf("[%s] no alive cells | t=%d, alive_cells: %d\n", board->name, t_i - t, board->alive_cells);
    reason = NOCELLS;
	} else if (has_looped) {
		printf("[%s] has looped | t=%d, alive_cells: %d\n", board->name, t_i - t, board->alive_cells);
    reason = LOOP;
	} else if(!t) {
    printf("[%s] time ended | t=%d, alive_cells: %d\n", board->name, t_i - t, board->alive_cells);
    reason = NOTIME;
  }
  printf("end\n");
  board->reason = reason;
  board->end_time = t_i - t;
  free_queue(history);
	write_exit(board);
	//exit(0);
}

void intHandlerMain() {
	printf("keep running\n");
}

int main(int argc, char const *argv[]) {

  if (argc < 2) {
    printf("modo de uso: ./scheduler <file> <t>\n");
    exit(1);
  }

	signal(SIGINT, intHandlerMain);

  setlocale(LC_ALL, "");
  char *line = NULL;
  FILE *file;
  size_t len = 0;
  ssize_t read;

  t = atoi(argv[2]);

  file = fopen(argv[1], "r");

  if (file) {
      read = getline(&line, &len, file);
			char* token = strtok(line, " ");
      num_boards = atoi(token);
      boards = malloc(sizeof(Board)*num_boards);
			token = strtok(NULL, " ");
      int A = atoi(token);
			token = strtok(NULL, " ");
      int B = atoi(token);
			token = strtok(NULL, " ");
      int C = atoi(token);
			token = strtok(NULL, " ");
      int D = atoi(token);

			printf("%d, %d, %d, %d, %d\n", num_boards, A, B, C, D);

      for (size_t i = 0; i < num_boards; i++) {
        read = getline(&line, &len, file);
				//printf("%s\n", line);
				token = strtok(line, " ");
        char* t_name = malloc(sizeof(token));
				strcpy(t_name, token);
				//printf("t_name: %s\n", t_name);
				token = strtok(NULL, " ");
        int num_cells = atoi(token);
				//printf("num_cells: %d\n", num_cells);
        Position* positions = malloc(sizeof(Position)*num_cells);
        for (size_t i = 0; i < num_cells; i++) {
					token = strtok(NULL, " ");
          positions[i].x = atoi(token);
					token = strtok(NULL, " ");
          positions[i].y = atoi(token);
        }

        Board* board = init_board(positions, num_cells, t_name, A, B, C, D, -1);
        boards[i] = *board;
      }

      free(line);
			fclose(file);

  } else {
    printf("archivo no encontrado\n");
    exit(1);
  }

  /* https://www.geeksforgeeks.org/create-n-child-process-parent-process-using-fork-c/ */
	// cambiar a num_boards
  for(int i=0;i<num_boards;i++) // loop will run n times (n=5)
    {
        if(fork() == 0)
        {
            signal(SIGINT, intHandler);
            printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
            int w = boards[i].size;
            int h = boards[i].size;
            boards[i].id = getpid();
            int **univ = calloc(h, h * sizeof(int*));
            for (int i = 0; i < w; i++) {
              univ[i] = calloc(w, w * sizeof(int));
            }
            boards[i].matrix = univ;
            game(&boards[i], t);
            exit(0);
        } else {
          // parent
        }
    }

    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, 0))) {
      if (pid == -1 && errno == ECHILD) {
          break;
      } else if (pid == -1) {
          perror("waitpid");
      } else if (WIFEXITED(status)) {
          printf("%d exited, status=%d\n", pid, WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
          printf("%d killed by signal %d\n", pid, WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
          printf("%d stopped by signal %d\n", pid, WSTOPSIG(status));
      } else if (WIFCONTINUED(status)) {
          printf("%d continued\n", pid);
      }
    }

  printf("generating report\n");
  generate_report(boards, num_boards);

  /*for (size_t i = 0; i < num_boards; i++) {
    show(boards[i].matrix, boards[i].size, boards[i].size, t, &boards[i]);
  }*/

  return 0;
}
