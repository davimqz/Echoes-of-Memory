#ifndef CARDS_LIST_H
#define CARDS_LIST_H

// Estrutura de um nó da lista encadeada de cartas
typedef struct CardNode {
    int id;                  // identificador do símbolo (0..N-1)
    int revealed;            // está temporariamente revelada
    int matched;             // já foi combinada/encontrada
    struct CardNode *next;   // ponteiro para o próximo nó
} CardNode;

// Funções para manipulação da lista de cartas
CardNode* create_card_node(int id);
void append_card(CardNode **head, int id);
CardNode* node_at(CardNode *head, int index);
int list_length(CardNode *head);
void free_card_list(CardNode **head);

// Funções do jogo da memória
void inicializarCartas(CardNode **head, int boardSize, int availableTypes);
CardNode* escolherCarta(CardNode *head, int index);
void jogadaIA(CardNode *head, int *sugestao_index);

#endif