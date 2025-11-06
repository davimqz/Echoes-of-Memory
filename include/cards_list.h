#ifndef CARDS_LIST_H
#define CARDS_LIST_H

#include <stdbool.h>

// Estrutura do nó da lista encadeada representando uma carta
typedef struct CardNode {
    int id;                  // identificador do símbolo (0..N-1)
    bool revealed;           // está temporariamente revelada
    bool matched;            // já foi combinada/encontrada
    struct CardNode *next;   // ponteiro para o próximo nó
} CardNode;

// Funções principais da lista encadeada
CardNode* create_card_node(int id);
void append_card(CardNode **head, int id);
CardNode* node_at(CardNode *head, int index);
int list_length(CardNode *head);
void print_list(CardNode *head);
void free_card_list(CardNode **head);

// Funções específicas do jogo da memória
void inicializarCartas(CardNode **head, int boardSize);
CardNode* escolherCarta(CardNode *head, int index);
int verificarPar(CardNode *a, CardNode *b);
void liberarMemoria(CardNode **head);

// Funções de ordenação
void swap_adjacent_nodes(CardNode **head, CardNode *prev, CardNode *a, CardNode *b);
void ordenarCartas(CardNode **head);

// Função de IA
void jogadaIA(CardNode *head, int *sugestao_index);

#endif // CARDS_LIST_H