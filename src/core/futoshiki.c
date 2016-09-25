#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "core/futoshiki.h"

#include "struct/bitarray.h"

typedef struct Restraint Restraint;
typedef struct Cell Cell;

struct Restraint {
    enum {
        GREATER,
        LESSER
    } type;
    Cell *other;
};

struct Cell {
    unsigned char val;

    unsigned char row : 4;
    unsigned char col : 4;

    Restraint restraints[4];
    unsigned char nRestr : 3;

    BitArray *possibilities;
};

struct Puzzle {
    Cell ***cells;
    unsigned char dim;
};





Cell *cell_new(unsigned char row, unsigned char col, unsigned char puzzleDim) {
    Cell *c = malloc(sizeof(*c));

    c->row = row;
    c->col = col;
    c->val = 0;
    memset(c->restraints, 0, sizeof(c->restraints));
    c->possibilities = bitarray_new(puzzleDim);
    bitarray_setAll(c->possibilities);

    c->nRestr = 0;

    return c;
}

void cell_destroy(Cell *c) {
    bitarray_destroy(c->possibilities);
    free(c);
}


void restr_new(Cell *c1, Cell *c2) {
    Restraint *r = &c1->restraints[c1->nRestr];
    r->type = LESSER;
    r->other = c2;

    r = &c2->restraints[c2->nRestr];
    r->type = GREATER;
    r->other = c1;
}


Puzzle *puzzle_new(FILE *stream) {
    unsigned char nRestr;
    unsigned char i, j;
    unsigned char r1, c1;
    unsigned char r2, c2;

    Puzzle *p = malloc(sizeof(*p));
    Cell *c;

    fscanf(stream, "%hhu%hhu", &p->dim, &nRestr);

    p->cells = malloc(p->dim * sizeof(*p->cells));

    for (i = 0; i < p->dim; i++) {
        p->cells[i] = malloc(p->dim * sizeof(**p->cells));
        for (j = 0; j < p->dim; j++) {
            c = cell_new(i, j, p->dim);
            fscanf(stream, "%hhu", &c->val);

            p->cells[i][j] = c;
        }
    }

    for (i = 0; i < nRestr; i++) {
        scanf("%hhu%hhu%hhu%hhu", &r1, &c1, &r2, &c2);
        restr_new(
            p->cells[r1-1][c1-1],
            p->cells[r2-1][c2-1]
        );
    }

    return p;
}

void puzzle_destroy(Puzzle *p) {
    unsigned char i, j;
    for (i = 0; i < p->dim; i++) {
        for (j = 0; j < p->dim; j++)
            cell_destroy(p->cells[i][j]);
        free(p->cells[i]);
    }
    free(p->cells);
    free(p);
}

void puzzle_display(const Puzzle *p, FILE *stream) {
    unsigned char i, j;

    for (i = 0; i < p->dim; i++) {
        for (j = 0; j < p->dim; j++) {
            fprintf(stream, "%hhu ", p->cells[i][j]->val);
        }
        puts("");
    }
}
