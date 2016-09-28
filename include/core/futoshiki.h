#ifndef _FUTOSHIKI_H_
#define _FUTOSHIKI_H_ 1

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

bool _puzzle_checkValidState(Puzzle *);


/**
 * Solves the given Puzzle.
 */
bool puzzle_solve(Puzzle *);

/**
 * Displays the Puzzle to the given output stream.
 */
void puzzle_display(const Puzzle *, FILE *);

#endif /* ifndef _FUTOSHIKI_H_ */
