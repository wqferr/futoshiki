#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "core/futoshiki.h"

#include "struct/bitarray.h"
#include "struct/list.h"

typedef struct Constraint Constriction;
typedef struct Cell Cell;

struct Cell {
    unsigned char val;

    unsigned char row : 4;
    unsigned char col : 4;

    // Cells whose values are greater than this
    Cell *constraints[4];
    unsigned char nConstr : 3;

    BitArray *possibilities;
};

struct Puzzle {
    Cell ***cells;
    unsigned char dim;

    List *constrCells;
};





Cell *cell_new(unsigned char row, unsigned char col, unsigned char puzzleDim) {
    Cell *c = malloc(sizeof(*c));

    c->row = row;
    c->col = col;
    c->val = 0;
    memset(c->constraints, 0, sizeof(c->constraints));
    c->possibilities = bitarray_new(puzzleDim);
    bitarray_setAll(c->possibilities);

    c->nConstr = 0;

    return c;
}

void cell_destroy(Cell *c) {
    bitarray_destroy(c->possibilities);
    free(c);
}


void constr_new(Puzzle *p, Cell *c1, Cell *c2) {
    if (c1->nConstr == 0) // not yet in the list
        list_append(p->constrCells, c1);

    c1->constraints[c1->nConstr] = c2;
    c1->nConstr++;
}


bool cell_checkConstraints(Cell *c) {
    Cell *other;
    unsigned char i;

    for (i = 0; i < c->nConstr; i++) {
        other = c->constraints[i];
        if (other->val > 0
                && c->val >= other->val)
            return false;
    }
    return true;
}

bool _puzzle_checkIndividualConstr(Puzzle *p) {
    ListIterator *iter = list_iterator(p->constrCells);
    Cell *c;
    bool valid = true;

    while (valid && listiter_hasNext(iter)) {
        c = (Cell *) listiter_next(iter);

        valid = c->val == 0 || cell_checkConstraints(c);
    }

    listiter_destroy(iter);

    return valid;
}

bool _puzzle_checkValidState(Puzzle *p) {
    unsigned char i, j;
    bool valid = true;
    BitArray *row, *col;
    Cell *c;

    if (!_puzzle_checkIndividualConstr(p))
        return false;

    row = bitarray_new(p->dim);
    col = bitarray_new(p->dim);

    i = 0;
    while (valid && i < p->dim) {
        j = 0;

        bitarray_clearAll(row);
        bitarray_clearAll(col);

        while (valid && j < p->dim) {
            c = p->cells[i][j];

            if (c->val > 0) {
                if (bitarray_check(row, c->val-1)) { // number already set
                    valid = false;
                } else {
                    bitarray_set(row, c->val-1);
                }
            }


            c = p->cells[j][i];

            if (c->val > 0) {
                if (bitarray_check(col, c->val-1)) { // number already set
                    valid = false;
                } else {
                    bitarray_set(col, c->val-1);
                }
            }


            j++;
        }

        i++;
    }


    bitarray_destroy(row);
    bitarray_destroy(col);

    return valid;
}

Puzzle *puzzle_new(FILE *stream) {
    unsigned char nConstr;
    unsigned char i, j;
    unsigned char r1, c1;
    unsigned char r2, c2;

    Cell *c;
    Puzzle *p = malloc(sizeof(*p));

    fscanf(stream, "%hhu%hhu", &p->dim, &nConstr);

    p->cells = malloc(p->dim * sizeof(*p->cells));

    for (i = 0; i < p->dim; i++) {
        p->cells[i] = malloc(p->dim * sizeof(**p->cells));
        for (j = 0; j < p->dim; j++) {
            c = cell_new(i, j, p->dim);
            fscanf(stream, "%hhu", &c->val);

            p->cells[i][j] = c;
        }
    }

    p->constrCells = list_new();
    for (i = 0; i < nConstr; i++) {
        scanf("%hhu%hhu%hhu%hhu", &r1, &c1, &r2, &c2);
        constr_new(
            p,
            p->cells[r1-1][c1-1],
            p->cells[r2-1][c2-1]
        );
    }

    return p;
}

void puzzle_destroy(Puzzle *p) {
    unsigned char i, j;

    list_destroy(p->constrCells);

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
        for (j = 0; j < p->dim; j++)
            fprintf(stream, "%hhu ", p->cells[i][j]->val);

        fputc('\n', stream);
    }
}
