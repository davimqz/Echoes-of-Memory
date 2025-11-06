#include <stdio.h>
#include <stdlib.h>
#include "../include/cards_list.h"
#include "../include/ai_gemini.h"

int main() {
    printf("=== Teste da IA GEMINI - Echoes of Memory ===\n\n");
    
    // Teste 1: Inicializar IA
    printf("1. Inicializando IA GEMINI...\n");
    GeminiConfig config;
    int gemini_ok = gemini_init(&config);
    
    if (gemini_ok) {
        printf("✅ IA GEMINI inicializada com sucesso\n");
        printf("   API Key: %s\n", config.api_key[0] ? "***configurada***" : "não encontrada");
        printf("   Endpoint: %s\n", config.endpoint);
    } else {
        printf("⚠️  IA GEMINI não disponível, usando IA local\n");
    }
    printf("\n");
    
    // Teste 2: Criar lista de cartas
    printf("2. Criando lista de cartas...\n");
    CardNode *head = NULL;
    inicializarCartas(&head, 4);
    print_list(head);
    printf("\n");
    
    // Teste 3: Testar sugestão da IA
    printf("3. Testando sugestão da IA...\n");
    int sugestao;
    char reasoning[256] = {0};
    
    jogadaIA_avancada(head, &sugestao, reasoning);
    
    if (sugestao != -1) {
        printf("✅ IA sugeriu posição: %d\n", sugestao);
        printf("   Raciocínio: %s\n", reasoning);
    } else {
        printf("❌ IA não conseguiu fazer sugestão\n");
    }
    printf("\n");
    
    // Teste 4: Simular algumas jogadas
    printf("4. Simulando jogadas...\n");
    CardNode *carta1 = escolherCarta(head, 0);
    CardNode *carta2 = escolherCarta(head, 8);
    
    printf("Carta 1 (pos 0): id=%d\n", carta1 ? carta1->id : -1);
    printf("Carta 2 (pos 8): id=%d\n", carta2 ? carta2->id : -1);
    
    int eh_par = verificarPar(carta1, carta2);
    if (eh_par) {
        printf("✅ Par encontrado! Aplicando reordenação...\n");
        ordenarCartas(&head);
        print_list(head);
        
        // Nova sugestão após reordenação
        printf("\n5. Nova sugestão da IA após reordenação...\n");
        jogadaIA_avancada(head, &sugestao, reasoning);
        if (sugestao != -1) {
            printf("✅ Nova sugestão: posição %d\n", sugestao);
            printf("   Raciocínio: %s\n", reasoning);
        }
    } else {
        printf("❌ Não é par. Cartas voltaram ao estado original.\n");
    }
    printf("\n");
    
    // Teste 5: Cleanup
    printf("6. Limpeza...\n");
    gemini_cleanup();
    liberarMemoria(&head);
    printf("✅ Recursos liberados\n");
    
    printf("\n=== Teste concluído ===\n");
    printf("\nDICAS PARA ATIVAR GEMINI:\n");
    printf("- Configure: set GEMINI_API_KEY=sua_chave_aqui\n");
    printf("- Execute novamente o teste\n");
    printf("- A IA usará GEMINI em vez da IA local\n");
    
    return 0;
}