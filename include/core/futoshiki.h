#ifndef _FUTOSHIKI_H_
#define _FUTOSHIKI_H_ 1

#define OPT_NONE 0
#define OPT_BASIC 1
#define OPT_MVR 2

#define OPT_LEVEL OPT_NONE

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
bool puzzle_solve(Puzzle *);

/**
 * Displays the Puzzle to the given output stream.
 */
void puzzle_display(const Puzzle *, FILE *);

#endif /* ifndef _FUTOSHIKI_H_ */
