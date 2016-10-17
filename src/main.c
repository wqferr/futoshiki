#include <stdio.h>
#include <stdlib.h>

#include "core/futoshiki.h"

#define ASSIGN_MAX_EXC "Numero de atribuicoes excede limite maximo"

int main(void) {

	unsigned int i, ncases;
    unsigned int success = 0;
    int assignments;
	scanf("%d", &ncases);

	for(i = 1; i <= ncases; i++){
	    Puzzle *p = puzzle_new(stdin);
        assignments = 0;

	    printf("%d\n", i);
	    success += puzzle_solve(p, &assignments);
        if (assignments < ASSIGN_MAX) {
            puzzle_display(p, stdout);
        } else {
            printf("%s\n", ASSIGN_MAX_EXC);
        }

	    puzzle_destroy(p);
	}
    printf("%.2f%% dos jogos foram resolvidos\n", (100.0f*success)/ncases);
    return 0;
}
