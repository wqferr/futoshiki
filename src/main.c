#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "core/futoshiki.h"

int main(void) {

	int i, ncases;
	scanf("%d", &ncases);

	for(i = 0; i < ncases; i++){
	    Puzzle *p = puzzle_new(stdin);

	    puzzle_solve(p);
	    printf("%d\n", i);
	    puzzle_display(p, stdout);

	    puzzle_destroy(p);
	}
    return 0;
}
