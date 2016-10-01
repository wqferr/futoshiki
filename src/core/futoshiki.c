#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "core/futoshiki.h"

#include "struct/bitarray.h"
#include "struct/list.h"
#include "struct/heap.h"

typedef struct Constraint Constriction;
typedef struct Cell Cell;

struct Cell {
    unsigned char val;

    unsigned char row;
    unsigned char col;

    // Cells whose values must be greater than this
    Cell *constraints[4];
    unsigned char nConstr;

#if OPT_LEVEL >= OPT_BITMAP
    BitArray *possibilities;
#endif

    Cell *nextInOrder;
};

struct Puzzle {
    Cell ***cells;
    unsigned char dim;

    List *constrCells;
    Cell *first;
};

Cell *_firstEmptyCell(Puzzle *);


#if OPT_LEVEL >= OPT_MVR
int cell_compareComplexity(const void *r1, const void *r2) {
    Cell *c1 = (Cell *) r1,
         *c2 = (Cell *) r2;

    unsigned int i, n1, n2;
    n1 = 0;
    n2 = 0;

    for (i = 0; i < bitarray_getNBits(c1->possibilities); i++) {
        n1 += bitarray_check(c1->possibilities, i);
        n2 += bitarray_check(c2->possibilities, i);
    }

    return n1 - n2;
}
#endif


Cell *cell_new(unsigned char row, unsigned char col, unsigned char puzzleDim) {
    Cell *c = malloc(sizeof(*c));

    c->row = row;
    c->col = col;
    c->val = 0;
    memset(c->constraints, 0, sizeof(c->constraints));

#if OPT_LEVEL >= OPT_BITMAP
    c->possibilities = bitarray_new(puzzleDim);
    bitarray_setAll(c->possibilities);
#else
    (void) puzzleDim;
#endif

    c->nextInOrder = NULL;
    c->nConstr = 0;

    return c;
}

void cell_destroy(Cell *c) {
#if OPT_LEVEL >= OPT_BITMAP
    bitarray_destroy(c->possibilities);
#endif

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

Cell *_nextEmptyCell(Puzzle *p, Cell *c) {
    unsigned char row, col;
    Cell *other;

    row = c->row;
    col = c->col + 1;
    while (row < p->dim) {
        while (col < p->dim) {
            other = p->cells[row][col];
            if (other->val == 0)
                return other;

            col++;
        }
        col = 0;
        row++;
    }

    return false;
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

#if OPT_LEVEL >= OPT_BITMAP
    unsigned char k, v;
#endif

#if OPT_LEVEL >= OPT_MVR
    Heap *h;
#endif

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

#if OPT_LEVEL >= OPT_BITMAP
    for (i = 0; i < p->dim; i++) {
        for (j = 0; j < p->dim; j++) {
            v = p->cells[i][j]->val;

            if (v > 0) {
                for (k = 0; k < p->dim; k++) {
                    c = p->cells[i][k];
                    bitarray_clear(c->possibilities, v-1);

                    c = p->cells[k][j];
                    bitarray_clear(c->possibilities, v-1);
                }
            }
        }
    }

#   if OPT_LEVEL >= OPT_INEQ

    // TODO

#       if OPT_LEVEL >= OPT_CHAINS

    // TODO

#       endif
#   endif
#endif

#if OPT_LEVEL < OPT_MVR
    c = p->cells[0][0];
    while (c != NULL) {
        c->nextInOrder = _nextEmptyCell(p, c);
        c = c->nextInOrder;
    }
    p->first = _firstEmptyCell(p);
#else
    h = heap_new(&cell_compareComplexity);

    for (i = 0; i < p->dim; i++)
        for (j = 0; j < p->dim; j++)
            if (p->cells[i][j]->val == 0)
                heap_push(h, p->cells[i][j]);

    p->first = heap_pop(h);

    c = p->first;
    while (!heap_isEmpty(h)) {
        c->nextInOrder = heap_pop(h);
        c = c->nextInOrder;
    }

    heap_destroy(h);
#endif

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

bool _checkValid(Puzzle *p, Cell *c, unsigned char v) {
#if OPT_LEVEL >= OPT_CHECK_ROWS
    unsigned char i;
#   if OPT_LEVEL >= OPT_BITMAP

    if (!bitarray_check(c->possibilities, v-1))
        return false;
#   endif

    for (i = 0; i < p->dim; i++) {
        if (p->cells[i][c->col]->val == v && i != c->row)
            return false;
        if (p->cells[c->row][i]->val == v && i != c->col)
            return false;
    }
#else
    (void) p;
    (void) c;
    (void) v;
#endif

    return true;
}

unsigned char _nextValue(Puzzle *p, Cell *c) {
#if OPT_LEVEL >= OPT_BITMAP
    unsigned char i;

    for (i = c->val+1; i <= p->dim; i++) {
        if (_checkValid(p, c, i)) {
            c->val = i;
            return i;
        }
    }

    c->val = 0;
    return 0;
#else
    c->val = (c->val+1) % (p->dim+1);
    return c->val;
#endif
}

Cell *_firstEmptyCell(Puzzle *p) {
    Cell *c = p->cells[0][0];

    if (c->val != 0)
        c = _nextEmptyCell(p, c);

    return c;
}

bool _backtrack(Puzzle *p, Cell *c) {
    bool isLast = c->nextInOrder == NULL;

    // Update cell's value and repeat until all values
    // are exhausted
    while (_nextValue(p, c) != 0) {
        // Check for any inconsistencies for this particular value
        if (isLast) {
            if (_puzzle_checkValidState(p))
                return true;
        } else {
            // There are more cells ahead
            // Try to fill'em up
            //
#if OPT_LEVEL > OPT_NONE
            // Only check further if this value is valid so far
            if (_puzzle_checkValidState(p)) {
#endif
                if (_backtrack(p, c->nextInOrder)) // Everything OK
                    return true;

#if OPT_LEVEL > OPT_NONE
            }
#endif
        }

        // Try next value
    }

    // All values were tried
    return false;
}

bool puzzle_solve(Puzzle *p) {
    // TODO add simplification process

    return _backtrack(p, p->first);
}

void puzzle_display(const Puzzle *p, FILE *stream) {
    unsigned char i, j;

    for (i = 0; i < p->dim; i++) {
        for (j = 0; j < p->dim; j++)
            fprintf(stream, "%hhu ", p->cells[i][j]->val);

        fputc('\n', stream);
    }
}
