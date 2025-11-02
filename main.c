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
    struct Node *next;
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

void cortex_swap_pos (Cortex *cortex, int i, int j) {
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

void cortex_embaralhar (Cortex *cortex, unsigned seed) {
    if (!cortex || cortex -> len <= 1) {
        return;
    }

    int n = cortex -> len;

    Node **v = (Node**)malloc(sizeof(Node*) * n);

    if (!v) {
        return;
    }

    Node *p = cortex -> start;

    for (int i = 0; i < n; i++) {
        v[i] = p;
        p = p -> next;
    }

    srand(seed);

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = v[i] -> card;
        v[i] -> card = v[j] -> card;
        v[j] -> card = temp;
    }
    free(v);
}

int cortex_reveal (Cortex *cortex, int line, int column) {
    if (!cortex) {
        return 0;
    }

    Node *p = cortex -> start;

    while (p) {
        if (!p -> card.isRemoved && p -> card.lines == line && p -> card.colums == column) {
            if (!p -> card.isTurned) {
                p -> card.isTurned = 1;
                p -> card.times_reveled += 1;
            }
            return 1;
        }
        p = p -> next;
    }
    return 0;
}

int main() {
    printf("Echoes of Chaos");
    return 0;
}