#ifndef _FUTOSHIKI_H_
#define _FUTOSHIKI_H_ 1

#define OPT_NONE 0
#define OPT_FORWARD_CHECKING 1
#define OPT_MVR 2
#define OPT_SIMPLIFY 3

#define OPT_LEVEL OPT_SIMPLIFY
#define ASSIGN_MAX 1e6

#include <stdio.h>
#include <stdbool.h>

typedef struct Puzzle Puzzle;

/**
 * Creates a new puzzle given the input stream.
 */
Puzzle *puzzle_new(FILE *);

/**
 * Frees any memory associated with the Puzzle.
 */
void puzzle_destroy(Puzzle *);


/**
 * Solves the given Puzzle.
 * Retruns wether or not the puzzle has a solution.
 * If not, the state of the cells are not changed.
 */
bool puzzle_solve(Puzzle *, int *);

/**
 * Displays the Puzzle to the given output stream.
 */
void puzzle_display(const Puzzle *, FILE *);

#endif /* ifndef _FUTOSHIKI_H_ */
