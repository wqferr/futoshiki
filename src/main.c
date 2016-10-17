#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "core/futoshiki.h"

#define ASSIGN_MAX_EXC "Numero de atribuicoes excede limite maximo"

int main(void) {

	unsigned int i, ncases;
    unsigned int success = 0;
    int assignments;
    clock_t t;

	scanf("%d", &ncases);

	for(i = 1; i <= ncases; i++){
	    Puzzle *p = puzzle_new(stdin);
        assignments = 0;

	    printf("%d\n", i);

        t = clock();
	    success += puzzle_solve(p, &assignments);
        t = clock() - t;
        if (assignments < ASSIGN_MAX) {
            puzzle_display(p, stdout);
            printf("atribuicoes: %d\n", assignments);
            printf("tempo aproximado: %.3f segundos\n", ((float) t)/CLOCKS_PER_SEC);
        } else {
            printf("%s\n", ASSIGN_MAX_EXC);
        }

	    puzzle_destroy(p);
	}

    printf("%u casos resolvidos\n", success);
    return 0;
}
