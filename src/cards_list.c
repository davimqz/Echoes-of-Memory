#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "../include/cards_list.h"

// Cria um novo nó de carta
CardNode* create_card_node(int id) {
    CardNode *node = (CardNode*)malloc(sizeof(CardNode));
    if (node == NULL) {
        printf("Erro: falha na alocação de memória\n");
        return NULL;
    }
    
    node->id = id;
    node->revealed = false;
    node->matched = false;
    node->next = NULL;
    
    return node;
}

// Adiciona uma carta no final da lista
void append_card(CardNode **head, int id) {
    CardNode *new_node = create_card_node(id);
    if (new_node == NULL) return;
    
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    
    CardNode *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

// Retorna o nó na posição especificada (0-indexado)
CardNode* node_at(CardNode *head, int index) {
    if (index < 0 || head == NULL) return NULL;
    
    CardNode *current = head;
    int count = 0;
    
    while (current != NULL && count < index) {
        current = current->next;
        count++;
    }
    
    return current;
}

// Retorna o tamanho da lista
int list_length(CardNode *head) {
    int count = 0;
    CardNode *current = head;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

// Imprime a lista (para debug)
void print_list(CardNode *head) {
    CardNode *current = head;
    printf("Lista: ");
    
    while (current != NULL) {
        printf("[%d:%s%s] ", 
               current->id, 
               current->revealed ? "R" : "H",
               current->matched ? "M" : "");
        current = current->next;
    }
    printf("\n");
}

// Libera toda a memória da lista
void free_card_list(CardNode **head) {
    CardNode *current = *head;
    CardNode *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    *head = NULL;
}

// Embaralha um array usando algoritmo Fisher-Yates
void shuffle_array(int *array, int size) {
    srand(time(NULL));
    
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Troca array[i] e array[j]
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Inicializa as cartas do jogo (cria pares embaralhados)
void inicializarCartas(CardNode **head, int boardSize) {
    // Libera lista anterior se existir
    if (*head != NULL) {
        free_card_list(head);
    }
    
    int totalCards = boardSize * boardSize;
    int numPairs = totalCards / 2;
    
    // Cria array com os pares
    int *cardTypes = (int*)malloc(totalCards * sizeof(int));
    
    // Preenche com pares (cada tipo aparece 2 vezes)
    for (int i = 0; i < numPairs; i++) {
        cardTypes[i * 2] = i;
        cardTypes[i * 2 + 1] = i;
    }
    
    // Embaralha as cartas
    shuffle_array(cardTypes, totalCards);
    
    // Cria a lista encadeada com as cartas embaralhadas
    for (int i = 0; i < totalCards; i++) {
        append_card(head, cardTypes[i]);
    }
    
    free(cardTypes);
    
    printf("Cartas inicializadas: %d cartas, %d pares\n", totalCards, numPairs);
}

// Escolhe uma carta na posição especificada
CardNode* escolherCarta(CardNode *head, int index) {
    CardNode *card = node_at(head, index);
    
    if (card == NULL) {
        printf("Erro: carta na posição %d não encontrada\n", index);
        return NULL;
    }
    
    if (card->matched) {
        printf("Carta na posição %d já foi encontrada\n", index);
        return NULL;
    }
    
    // Revela a carta temporariamente
    card->revealed = true;
    
    return card;
}

// Verifica se duas cartas formam um par
int verificarPar(CardNode *a, CardNode *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    
    if (a == b) {
        printf("Erro: mesma carta selecionada duas vezes\n");
        return 0;
    }
    
    if (a->id == b->id) {
        // É um par! Marca ambas como encontradas
        a->matched = true;
        b->matched = true;
        a->revealed = false; // Remove revelação temporária
        b->revealed = false;
        
        printf("Par encontrado! Tipo: %d\n", a->id);
        return 1;
    } else {
        // Não é par, esconde as cartas
        a->revealed = false;
        b->revealed = false;
        
        printf("Não é par: %d != %d\n", a->id, b->id);
        return 0;
    }
}

// Troca dois nós adjacentes na lista encadeada
void swap_adjacent_nodes(CardNode **head, CardNode *prev, CardNode *a, CardNode *b) {
    if (a == NULL || b == NULL || a->next != b) {
        return; // Não são adjacentes ou são NULL
    }
    
    // Ajusta o ponteiro anterior
    if (prev == NULL) {
        // 'a' é o primeiro nó da lista
        *head = b;
    } else {
        prev->next = b;
    }
    
    // Faz a troca
    a->next = b->next;
    b->next = a;
}

// Implementa Bubble Sort na lista encadeada
void ordenarCartas(CardNode **head) {
    if (*head == NULL || (*head)->next == NULL) {
        return; // Lista vazia ou com um elemento
    }
    
    bool swapped;
    
    do {
        swapped = false;
        CardNode *prev = NULL;
        CardNode *current = *head;
        
        while (current != NULL && current->next != NULL) {
            // Compara current->id com current->next->id
            if (current->id > current->next->id) {
                // Precisa trocar
                CardNode *next = current->next;
                swap_adjacent_nodes(head, prev, current, next);
                
                // Atualiza ponteiros após a troca
                // Agora 'next' está na posição de 'current'
                prev = next;
                swapped = true;
            } else {
                // Não troca, apenas avança
                prev = current;
                current = current->next;
            }
        }
    } while (swapped);
    
    printf("Cartas reordenadas pela IA (CORTEX)\n");
}

// IA simples que sugere uma jogada
void jogadaIA(CardNode *head, int *sugestao_index) {
    *sugestao_index = -1; // Inicializa sem sugestão
    
    CardNode *current = head;
    int index = 0;
    
    // Estratégia simples: procura a primeira carta não matched
    while (current != NULL) {
        if (!current->matched && !current->revealed) {
            *sugestao_index = index;
            printf("IA CORTEX sugere: posição %d (tipo %d)\n", index, current->id);
            return;
        }
        current = current->next;
        index++;
    }
    
    printf("IA CORTEX: nenhuma sugestão disponível\n");
}

// Wrapper para liberarMemoria (compatibilidade com especificação)
void liberarMemoria(CardNode **head) {
    free_card_list(head);
    printf("Memória liberada\n");
}