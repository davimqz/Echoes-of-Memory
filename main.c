#include <stdio.h>
#include <stdlib.h>

typedef struct Card {
    int id;
    int isTurned;
    int isRemoved;
    int times_reveled;
    int lines;
    int colums;
} Card;

typedef struct Node {
    Card card;
    struct No *next;
} Node;

typedef struct {
    Node *start;
    int len;
} Cortex;

typedef struct {
    int lines;
    int colums;
    int cell_width;
    int cell_height;
    int margin;
} BoardView;

static int cortex_len (const Cortex* cortex) {
    if (cortex == NULL) {
        return 0;
    } else {
        return cortex -> len;
    }
}

Node *createNode (Card card) {
    Node *novo = (Node*)malloc(sizeof(Node));
    if (!novo) {
        return NULL;
    }

    novo -> card = card;
    novo -> next = NULL;
    return novo; 
}

void cortex_add_init (Cortex* cortex, Card card) {
    if (!cortex) {
        return;
    }

    Node *novo = createNode(card);

    if (!cortex) {
        return;
    }

    novo -> next = cortex -> start;
    cortex -> start = novo;
    cortex -> len += 1;
}

Node *cortex_search_pos (Cortex* cortex, int pos) {
    if (!cortex || pos < 0) {
        return NULL;
    }

    Node *init = cortex -> start;

    int i = 0;

    while (init && i < pos) {
        init = init -> next;
        i++;
    }
    return init;
}

void cortex_embaralhar (Cortex *cortex, int i, int j) {
    if (!cortex || i == j || i < 0 || j < 0) {
        return;
    }

    Node *a = cortex_search_pos(cortex, i);
    Node *b = cortex_search_pos(cortex, j);

    if (!a || !b) {
        return;
    }

    Card temp = a -> card;
    a -> card = b -> card;
    b -> card = temp;
}