#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

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

    uchar *restrictedValues;
    uchar nPossibilities;

} Cell;

struct Puzzle {
    Cell ***cells;
    uchar size;
    List *constrCells;
};


Cell *cell_new(Puzzle *p, uchar row, uchar col, uchar val) {
    Cell *c = malloc(sizeof(*c));

    c->val = val;
    c->row = row;
    c->col = col;
    c->nConstr = 0;
    c->restrictedValues = calloc(p->size, sizeof(*c->restrictedValues));
    c->nPossibilities = p->size;

    return c;
}

void cell_destroy(Cell *c) {
    free(c->restrictedValues);
    free(c);
}

void _strengthenRestrValues(Puzzle *p, uchar row, uchar col, uchar val) {
    uchar i;
    Cell *otherInRow, *otherInCol;

    if (val > 0) {
        for (i = 0; i < p->size; i++) {
            if (i != col) {
                otherInRow = p->cells[row][i];
                if (otherInRow->restrictedValues[val-1] == 0)
                    otherInRow->nPossibilities--;
                otherInRow->restrictedValues[val-1]++;
            }
            if (i != row) {
                otherInCol = p->cells[i][col];
                if (otherInCol->restrictedValues[val-1] == 0)
                    otherInCol->nPossibilities--;
                otherInCol->restrictedValues[val-1]++;
            }
        }
    }
}

void _lessenRestrValues(Puzzle *p, uchar row, uchar col, uchar val) {
    uchar i;
    Cell *otherInRow, *otherInCol;

    if (val > 0) {
        for (i = 0; i < p->size; i++) {
            if (i != col) {
                otherInRow = p->cells[row][i];
                otherInRow->restrictedValues[val-1]--;
                if (otherInRow->restrictedValues[val-1] == 0)
                    otherInRow->nPossibilities++;
            }

            if (i != row) {
                otherInCol = p->cells[i][col];
                otherInCol->restrictedValues[val-1]--;
                if (otherInCol->restrictedValues[val-1] == 0)
                    otherInCol->nPossibilities++;
            }
        }
    }
}

void _updateRestrictedValues(Puzzle *p, Cell *c, uchar newVal) {
    _lessenRestrValues(p, c->row, c->col, c->val);
    _strengthenRestrValues(p, c->row, c->col, newVal);
}

bool cell_nextValue(Puzzle *p, Cell *c, int *assignments) {
#if OPT_LEVEL < OPT_FORWARD_CHECKING

    c->val = (c->val+1) % (p->size+1);

#else

    uchar newVal = c->val+1;

    while (newVal <= p->size && c->restrictedValues[newVal-1] > 0) {
        newVal++;
    }

    if (newVal > p->size)
        newVal = 0;

    _updateRestrictedValues(p, c, newVal);
    c->val = newVal;
#endif

    (*assignments)++;
    return c->val > 0;
}



Cell *cell_nextInSeq(Puzzle *p, Cell *c) {
    uchar i, j;

#if OPT_LEVEL < OPT_MVR
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
#else
    Cell *easiest = NULL;
    uchar easiestComplexity = UCHAR_MAX;
    uchar currComplexity;
    i = 0;
    j = 0;

    while (i < p->size) {
        j = 0;
        while (j < p->size) {
            c = p->cells[i][j];
            if (c->val == 0) {
                currComplexity = c->nPossibilities;
                if (currComplexity < easiestComplexity) {
                    easiest = c;
                    easiestComplexity = currComplexity;
                }
            }
            j++;
        }
        i++;
    }

    return easiest;
#endif
}

uchar cell_smallestPossibility(Puzzle *p, Cell *c) {
    uchar i;
    if (c->val > 0)
        return c->val;
    for (i = 0; i < p->size; i++)
        if (c->restrictedValues[i] == 0)
            return i+1;
    return 0;
}

uchar cell_greatestPossibility(Puzzle *p, Cell *c) {
    uchar i;
    if (c->val > 0)
        return c->val;
    for (i = 1; i <= p->size; i++)
        if (c->restrictedValues[p->size-i] == 0)
            return p->size-i + 1;
    return 0;
}

void _updateIneqRestr(Puzzle *p) {
    ListIterator *iter = list_iterator(p->constrCells);
    Cell *c, *other;
    uchar i, j, lim;

    while (listiter_hasNext(iter)) {
        c = listiter_next(iter);
        for (i = 0; i < c->nConstr; i++) {
            other = c->constr[i];

            lim = cell_smallestPossibility(p, c);
            for (j = 0; j < lim; j++)
                other->restrictedValues[j]++;

            lim = cell_greatestPossibility(p, other);
            for (j = lim; j < p->size; j++)
                c->restrictedValues[j]++;
        }
    }

    listiter_destroy(iter);
}

void puzzle_simplify(Puzzle *p) {
    bool altered;
    uchar i, j, newVal;
    Cell *c;

    do {
        altered = false;
        for (i = 0; i < p->size; i++) {
            for (j = 0; j < p->size; j++) {
                c = p->cells[i][j];
                if (c->val == 0 && c->nPossibilities == 1) {
                    newVal = cell_smallestPossibility(p, c);
                    _updateRestrictedValues(p, c, newVal);
                    c->val = newVal;
                    altered = true;
                }
            }
        }
        _updateIneqRestr(p);
    } while (altered);
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


    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++) {
            v = p->cells[i][j]->val;
            if (v > 0)
                _strengthenRestrValues(p, i, j, v);
        }
    }

#if OPT_LEVEL >= OPT_SIMPLIFY
    puzzle_simplify(p);
#endif

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

bool _backtrack(Puzzle *p, Cell *c, int *assignments) {
    if (c == NULL)
        return puzzle_checkSolved(p);
    if (*assignments >= ASSIGN_MAX)
        return false;

    while (cell_nextValue(p, c, assignments)) {
        if (_backtrack(p, cell_nextInSeq(p, c), assignments))
            return true;
    }

    return false;
}

bool puzzle_solve(Puzzle *p, int *assignments) {
    return _backtrack(p, _firstCell(p), assignments);
}

void puzzle_display(const Puzzle *p, FILE *stream) {
    uchar i, j;

    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++)
            fprintf(stream, "%hhu ", p->cells[i][j]->val);

        fputc('\n', stream);
    }
}
