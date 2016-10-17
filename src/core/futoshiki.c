#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "core/futoshiki.h"
#include "struct/list.h"
#include "struct/bitarray.h"

typedef unsigned char uchar;

typedef struct Cell {
    // Valor atual da célula
    uchar val;

    // Posição da célula na grade
    uchar row;
    uchar col;

    // Vetor e número de limitações
    // Cada limitação é uma célula estritamente maior que esta
    uchar nConstr;
    struct Cell *constr[4];

    // Vetor onde cada posição i representa o número de células que impedem
    // o valor i+1 de ser posto nesta célula.
    // Mais adequado que um vetor de valores booleanos pois permite subtrair
    // 1 do valor na volta do backtracking e somar 1 ao prosseguir à próxima
    // célula.
    uchar *restrictedValues;

    // Número de valores que podem ser colocados nesta célula.
    // Equivalente ao número de valores 0 em restrictedValues.
    uchar nPossibilities;

} Cell;

struct Puzzle {
    Cell ***cells;

    // Número de células por lado do jogo
    uchar size;

    // Lista de todas as células que possuem alguma limitação
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

// Incrementa de 1 o índice de restrictedValues adequado de todas as células
// não definidas na linha e coluna especificados, com exceção da própria posi-
// ção.
void _strengthenRestrValues(Puzzle *p, uchar row, uchar col, uchar val) {
    uchar i;
    Cell *otherInRow, *otherInCol;

    if (val > 0) {
        for (i = 0; i < p->size; i++) {
            // Células na linha row
            if (i != col) { // Ignorar própria posição
                otherInRow = p->cells[row][i];
                if (otherInRow->restrictedValues[val-1] == 0) // Menos um 0 no vetor
                    otherInRow->nPossibilities--;
                otherInRow->restrictedValues[val-1]++;
            }

            // Análogo ao código acima, mas para células na coluna col
            if (i != row) {
                otherInCol = p->cells[i][col];
                if (otherInCol->restrictedValues[val-1] == 0)
                    otherInCol->nPossibilities--;
                otherInCol->restrictedValues[val-1]++;
            }
        }
    }
}

// Análogo à função acima, mas decrementa em vez de incrementar.
void _lessenRestrValues(Puzzle *p, uchar row, uchar col, uchar val) {
    uchar i;
    Cell *otherInRow, *otherInCol;

    if (val > 0) {
        for (i = 0; i < p->size; i++) {
            if (i != col) {
                otherInRow = p->cells[row][i];
                otherInRow->restrictedValues[val-1]--;
                if (otherInRow->restrictedValues[val-1] == 0) // Um 0 a mais no vetor
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

// Chamado quando uma célula muda seu valor
// Reduz a restrição do valor atual da célula e aumenta a restrição do novo
// valor.
void _updateRestrictedValues(Puzzle *p, Cell *c, uchar newVal) {
    _lessenRestrValues(p, c->row, c->col, c->val);
    _strengthenRestrValues(p, c->row, c->col, newVal);
}

// Retorna se o tabuleiro ainda pode teoricamente ser resolvido.
// Procura por alguma célula sem valores possíveis.
bool _forwardCheck(Puzzle *p) {
    uchar i, j;

    for (i = 0; i < p->size; i++)
        for (j = 0; j < p->size; j++)
            if (p->cells[i][j]->val == 0 && p->cells[i][j]->nPossibilities == 0)
                return false;
    return true;
}

// Cicla pelos valores possíveis da célula.
// Retorna true se houver um próximo valor, retorna false caso contrário.
// Automaticamente ajusta o valor de volta para 0 se não houver mais valores.
bool cell_nextValue(Puzzle *p, Cell *c, int *assignments) {
    uchar newVal;
#if OPT_LEVEL >= OPT_FORWARD_CHECKING
    // Em caso de forward checking, repetir até que _forwardCheck retorne true
    do {
#endif
        newVal = c->val+1;

        while (newVal <= p->size && c->restrictedValues[newVal-1] > 0) {
            newVal++;
        }

        if (newVal > p->size)
            newVal = 0;

        _updateRestrictedValues(p, c, newVal);
        c->val = newVal;
#if OPT_LEVEL >= OPT_FORWARD_CHECKING
    // Se newVal == 0 não há mais valores a serem checados
    } while (newVal > 0 && !_forwardCheck(p));
#endif

    (*assignments)++;
    return c->val > 0;
}



// Retorna a próxima célula a ser processada pelo algoritmo
Cell *cell_nextInSeq(Puzzle *p, Cell *c) {
    uchar i, j;

#if OPT_LEVEL < OPT_MVR
    // Se a heurística MVR não for utilizada, procurar a próxima célula vazia
    // à direita e abaixo desta.

    // Iniciar a busca na mesma linha e coluna à direita desta
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

    // Se a heurística MVR for utilizada, procurar pela célula com menor valor
    // nPossibilities dentre todas as células em branco.
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

// Retorna o menor valor que a célula pode assumir
uchar cell_smallestPossibility(Puzzle *p, Cell *c) {
    uchar i;
    if (c->val > 0)
        return c->val;
    for (i = 0; i < p->size; i++)
        if (c->restrictedValues[i] == 0)
            return i+1;
    return 0;
}

// Retorna o maior valor que a célula pode assumir
uchar cell_greatestPossibility(Puzzle *p, Cell *c) {
    uchar i;
    if (c->val > 0)
        return c->val;
    for (i = 1; i <= p->size; i++)
        if (c->restrictedValues[p->size-i] == 0)
            return p->size-i + 1;
    return 0;
}

// Atualiza o vetor de possibilidades.
// Retorna se algum vetor foi alterado.
bool _updateIneqRestr(Puzzle *p) {
    ListIterator *iter = list_iterator(p->constrCells);
    Cell *c, *other;
    uchar i, j, lim;
    bool altered = false;

    // Para cada célula com limitações
    while (listiter_hasNext(iter)) {
        c = listiter_next(iter);
        // Para cada limitação
        for (i = 0; i < c->nConstr; i++) {
            other = c->constr[i];

            // Sabe-se que other->val > min(c)
            // Logo, todo valor menor que ou igual o valor mínimo de c é
            // impossível para other, o que implica que o vetor de other tem
            // em, tais posições, valores diferentes de 0.
            lim = cell_smallestPossibility(p, c);
            for (j = 0; j < lim; j++) {
                // Vetor alterado
                if (other->restrictedValues[j] == 0) {
                    other->nPossibilities--;
                    altered = true;
                }
                other->restrictedValues[j] = 5;
            }

            // Análogo, mas partindo do fato de que c->val < max(other)
            lim = cell_greatestPossibility(p, other);
            for (j = lim; j < p->size; j++) {
                if (c->restrictedValues[j] == 0) {
                    c->nPossibilities--;
                    altered = true;
                }
                c->restrictedValues[j] = 5;
            }
        }
    }

    listiter_destroy(iter);
    return altered;
}

// Tenta simplificar o estado inicial do tabuleiro.
void puzzle_simplify(Puzzle *p) {
    bool altered;
    uchar i, j, newVal;
    Cell *c;

    // Repetir simplificação até que nenhuma mudança seja feita.
    do {
        altered = false;

        // Para cada célula
        for (i = 0; i < p->size; i++) {
            for (j = 0; j < p->size; j++) {
                c = p->cells[i][j];
                // Se c só tem um valor possível
                if (c->val == 0 && c->nPossibilities == 1) {
                    // Atribuir aquele valor
                    newVal = cell_smallestPossibility(p, c);
                    // E atualizar restrições
                    _updateRestrictedValues(p, c, newVal);
                    c->val = newVal;
                    altered = true;
                }
            }
        }
        if (_updateIneqRestr(p))
            altered = true;
    } while (altered);
}


// Adicionar uma nova limitação tal que c1 < c2
void puzzle_addConstr(Puzzle *p, Cell *c1, Cell *c2) {
    if (c1->nConstr == 0) // Not yet in the list
        list_append(p->constrCells, c1);

    c1->constr[c1->nConstr] = c2;
    c1->nConstr++;
}

// Cria um novo tabuleiro com os dados formatados segundo o especificado.
// Os dados são lidos da stream de dados passada.
Puzzle *puzzle_new(FILE *stream) {
    uchar nConstr;
    uchar v;
    uchar i, j;
    uchar k, l;

    Puzzle *p = malloc(sizeof(*p));

    // Leitura do tamanho e número de limitações
    fscanf(stream, "%hhu%hhu", &p->size, &nConstr);

    // Alocação da matriz de células
    p->cells = malloc(p->size * sizeof(*p->cells));

    for (i = 0; i < p->size; i++) {
        p->cells[i] = malloc(p->size * sizeof(**p->cells));
        for (j = 0; j < p->size; j++) {
            fscanf(stream, "%hhu", &v);
            p->cells[i][j] = cell_new(p, i, j, v);
        }
    }

    // Leitura e criação das limitações
    p->constrCells = list_new();
    while (nConstr > 0) {
        fscanf(stream, "%hhu%hhu%hhu%hhu", &i, &j, &k, &l);
        puzzle_addConstr(p, p->cells[i-1][j-1], p->cells[k-1][l-1]);
        nConstr--;
    }


    // Atualização dos valores iniciais do vetor de restrições
    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++) {
            v = p->cells[i][j]->val;
            if (v > 0)
                _strengthenRestrValues(p, i, j, v);
        }
    }

#if OPT_LEVEL >= OPT_SIMPLIFY
    // Se nível de otimização permitir, simplificar o tabuleiro anteriormente.
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
    // Vetores que marcam quais valores já foram encontrados na linha/coluna i.
    BitArray *numsInRow = bitarray_new(p->size);
    BitArray *numsInCol = bitarray_new(p->size);
    ListIterator *constrIter;
    Cell *c;
    uchar i, j;
    uchar v;
    bool valid = true;

    // Linha ou coluna atual sendo checada
    i = 0;

    // Iterar até contradição ou fim do tabuleiro
    while (valid && i < p->size) {

        // Posição da linha/coluna atual
        j = 0;

        // Reset do vetor de booleanos
        bitarray_clearAll(numsInRow);
        bitarray_clearAll(numsInCol);

        while (valid && j < p->size) {
            v = p->cells[i][j]->val;

            if (v > 0) {
                if (bitarray_check(numsInRow, v-1)) // Mesmo número já foi encontrado anteriormente na mesma linha
                    valid = false;
                else
                    bitarray_set(numsInRow, v-1); // Setar como já encontrado
            } else {
                valid = false;
            }


            v = p->cells[j][i]->val;

            // Análogo, mas com i representando a coluna e não a linha
            if (v > 0) {
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

    // Verificar se todas as limitações foram respeitadas
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

#if OPT_LEVEL < OPT_MVR
    // Procurar primeira célula vaga
    for (i = 0; i < p->size; i++)
        for (j = 0; j < p->size; j++)
            if (p->cells[i][j]->val == 0)
                return p->cells[i][j];

    return NULL;
#else
    // Procurar célula mais símples
    uchar easiestComplexity = UCHAR_MAX;
    Cell *easiest, *curr;

    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++) {
            curr = p->cells[i][j];
            if (curr->val == 0 && curr->nPossibilities < easiestComplexity) {
                //fprintf(stderr, "easiestComplx: %hhu\n", curr->nPossibilities);
                easiest = curr;
                easiestComplexity = curr->nPossibilities;
            }
        }
    }

    return easiest;
#endif
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
