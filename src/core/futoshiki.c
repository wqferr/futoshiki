#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "core/futoshiki.h"
#include "struct/list.h"
#include "struct/bitarray.h"

typedef unsigned char uchar;

typedef struct Cell {
    uchar val;
    uchar row;
    uchar col;

    uchar nConstr;
    struct Cell *constr[4];

} Cell;

struct Puzzle {
    Cell ***cells;
    uchar size;
    List *constrCells;
};


Cell *cell_new(Puzzle *p, uchar row, uchar col, uchar val) {
    Cell *c = malloc(sizeof(*c));

    (void) p;

    c->val = val;
    c->row = row;
    c->col = col;
    c->nConstr = 0;

    return c;
}

void cell_destroy(Cell *c) {
    free(c);
}

bool cell_nextValue(Puzzle *p, Cell *c) {
    c->val = (c->val+1) % (p->size+1);
    return c->val;
}

Cell *cell_nextInSeq(Puzzle *p, Cell *c) {
    uchar i, j;
    i = c->row;
    j = c->col + 1;

    while (i < p->size) {
        while (j < p->size) {
            if (p->cells[i][j]->val == 0)
                return p->cells[i][j];
            j++;
        }

        j = 0;
        i++;
    }
    return NULL;
}


void puzzle_addConstr(Puzzle *p, Cell *c1, Cell *c2) {
    if (c1->nConstr == 0) // Not yet in the list
        list_append(p->constrCells, c1);

    c1->constr[c1->nConstr] = c2;
    c1->nConstr++;
}

Puzzle *puzzle_new(FILE *stream) {
    uchar nConstr;
    uchar v;
    uchar i, j;
    uchar k, l;

    Puzzle *p = malloc(sizeof(*p));

    fscanf(stream, "%hhu%hhu", &p->size, &nConstr);
    p->cells = malloc(p->size * sizeof(*p->cells));

    for (i = 0; i < p->size; i++) {
        p->cells[i] = malloc(p->size * sizeof(**p->cells));
        for (j = 0; j < p->size; j++) {
            fscanf(stream, "%hhu", &v);
            p->cells[i][j] = cell_new(p, i, j, v);
        }
    }

    p->constrCells = list_new();
    while (nConstr > 0) {
        fscanf(stream, "%hhu%hhu%hhu%hhu", &i, &j, &k, &l);
        puzzle_addConstr(p, p->cells[i-1][j-1], p->cells[k-1][l-1]);
        nConstr--;
    }

    return p;
}

void puzzle_destroy(Puzzle *p) {
    uchar i, j;
    list_destroy(p->constrCells);

    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++)
            cell_destroy(p->cells[i][j]);
        free(p->cells[i]);
    }
    free(p->cells);
    free(p);
}

bool puzzle_checkSolved(Puzzle *p) {
    BitArray *numsInRow = bitarray_new(p->size);
    BitArray *numsInCol = bitarray_new(p->size);
    ListIterator *constrIter;
    Cell *c;
    uchar i, j;
    uchar v;
    bool valid = true;

    // Current board line being iterated
    i = 0;

    // Iterate until contradiction or end of board
    while (valid && i < p->size) {

        // Index in the given line being processed
        j = 0;

        bitarray_clearAll(numsInRow);
        bitarray_clearAll(numsInCol);

        while (valid && j < p->size) {
            v = p->cells[i][j]->val;

            if (v > 0) {
                // Same number was set previously in row
                if (bitarray_check(numsInRow, v-1))
                    valid = false;
                else
                    bitarray_set(numsInRow, v-1);
            } else {
                valid = false;
            }


            v = p->cells[j][i]->val;

            if (v > 0) {
                // Same number was set previously in column
                if (bitarray_check(numsInCol, v-1))
                    valid = false;
                else
                    bitarray_set(numsInCol, v-1);
            } else {
                valid = false;
            }

            j++;
        }

        i++;
    }

    bitarray_destroy(numsInRow);
    bitarray_destroy(numsInCol);

    constrIter = list_iterator(p->constrCells);

    while (valid && listiter_hasNext(constrIter)) {
        c = listiter_next(constrIter);
        if (c->val > 0) {
            for (i = 0; i < c->nConstr; i++)
                if (c->val > c->constr[i]->val)
                    valid = false;
        } else {
            valid = false;
        }
    }

    listiter_destroy(constrIter);

    return valid;
}



Cell *_firstCell(Puzzle *p) {
    uchar i, j;

    for (i = 0; i < p->size; i++)
        for (j = 0; j < p->size; j++)
            if (p->cells[i][j]->val == 0)
                return p->cells[i][j];

    return NULL;
}

bool _backtrack(Puzzle *p, Cell *c) {
    if (c == NULL)
        return puzzle_checkSolved(p);

    while (cell_nextValue(p, c)) {
        if (_backtrack(p, cell_nextInSeq(p, c)))
            return true;
    }

    return false;
}

bool puzzle_solve(Puzzle *p) {
    return _backtrack(p, _firstCell(p));
}

void puzzle_display(const Puzzle *p, FILE *stream) {
    uchar i, j;

    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++)
            fprintf(stream, "%hhu ", p->cells[i][j]->val);

        fputc('\n', stream);
    }
}
