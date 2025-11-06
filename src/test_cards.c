#include <stdio.h>
#include <stdlib.h>
#include "../include/cards_list.h"

// Teste simples das funções da lista encadeada
int main() {
    printf("=== Teste da Lista Encadeada - Echoes of Memory ===\n\n");
    
    CardNode *head = NULL;
    
    // Teste 1: Inicializar cartas para um tabuleiro 4x4
    printf("1. Inicializando cartas para tabuleiro 4x4...\n");
    inicializarCartas(&head, 4);
    print_list(head);
    printf("Tamanho da lista: %d\n\n", list_length(head));
    
    // Teste 2: Escolher algumas cartas
    printf("2. Escolhendo cartas...\n");
    CardNode *carta1 = escolherCarta(head, 0);
    CardNode *carta2 = escolherCarta(head, 8);
    
    if (carta1) printf("Carta 1 (pos 0): id=%d, revealed=%s\n", carta1->id, carta1->revealed ? "true" : "false");
    if (carta2) printf("Carta 2 (pos 8): id=%d, revealed=%s\n", carta2->id, carta2->revealed ? "true" : "false");
    printf("\n");
    
    // Teste 3: Verificar se formam par
    printf("3. Verificando se formam par...\n");
    int eh_par = verificarPar(carta1, carta2);
    printf("Resultado: %s\n\n", eh_par ? "PAR!" : "Não é par");
    
    // Teste 4: Aplicar Bubble Sort
    printf("4. Estado antes da ordenação:\n");
    print_list(head);
    
    printf("\n5. Aplicando Bubble Sort...\n");
    ordenarCartas(&head);
    
    printf("\n6. Estado após ordenação:\n");
    print_list(head);
    printf("\n");
    
    // Teste 5: Sugestão da IA
    printf("7. Testando IA...\n");
    int sugestao;
    jogadaIA(head, &sugestao);
    printf("\n");
    
    // Teste 6: Simular alguns pares para testar ordenação
    printf("8. Simulando encontrar um par (forçado)...\n");
    CardNode *primeiro = node_at(head, 0);
    CardNode *segundo = node_at(head, 1);
    
    if (primeiro && segundo) {
        // Força mesmo id para testar par
        segundo->id = primeiro->id;
        printf("Forçando cartas 0 e 1 a terem mesmo id (%d)\n", primeiro->id);
        
        escolherCarta(head, 0);
        escolherCarta(head, 1);
        int par = verificarPar(primeiro, segundo);
        
        if (par) {
            printf("Par encontrado! Aplicando reordenação...\n");
            ordenarCartas(&head);
            print_list(head);
        }
    }
    printf("\n");
    
    // Teste 7: Liberar memória
    printf("9. Liberando memória...\n");
    liberarMemoria(&head);
    
    if (head == NULL) {
        printf("Memória liberada com sucesso!\n");
    }
    
    printf("\n=== Teste concluído ===\n");
    
    return 0;
}