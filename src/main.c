#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "core/futoshiki.h"


/*
 * Test cases 1-7
 * int main(void) {
 *     Puzzle *p = puzzle_new(stdin);
 * 
 *     printf("%d\n", _puzzle_checkValidState(p));
 * 
 *     puzzle_destroy(p);
 * 
 *     return 0;
 * }
*/

int main(void) {
    Puzzle *p = puzzle_new(stdin);
    clock_t t;

    t = clock();
    printf("%d\n", puzzle_solve(p));
    fprintf(stderr, "%f\n", ((float)clock() - t) / CLOCKS_PER_SEC);
    puzzle_display(p, stdout);

    puzzle_destroy(p);
    return 0;
}
