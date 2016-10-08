#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "core/futoshiki.h"

int main(void) {

	int i, ncases;
	scanf("%d", &ncases);

	for(i = 1; i <= ncases; i++){
	    Puzzle *p = puzzle_new(stdin);

	    printf("%d\n", i);
	    printf("%d\n", puzzle_solve(p));
	    puzzle_display(p, stdout);

	    puzzle_destroy(p);
	}
    return 0;
}
