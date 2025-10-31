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

Node *createNode (Card card) {
    Node *novo = (Node*)malloc(sizeof(Node));
    if (!novo) {
        return NULL;
    }

    novo -> card = card;
    novo -> next = NULL;
    return novo; 
}

