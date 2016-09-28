#include <stdio.h>
#include <stdlib.h>

#include "core/futoshiki.h"

int main(void) {
    Puzzle *p = puzzle_new(stdin);

    printf("%d\n", _puzzle_checkValidState(p));

    puzzle_destroy(p);

    return 0;
}
