#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/ai_gemini.h"

// Configuração global da IA
static GeminiConfig g_config = {0};

// Inicializa a IA GEMINI (carrega configurações de variáveis de ambiente)
int gemini_init(GeminiConfig *config) {
    // Carrega chave da API de variável de ambiente
    const char *api_key = getenv("GEMINI_API_KEY");
    const char *endpoint = getenv("GEMINI_ENDPOINT");
    
    if (api_key == NULL) {
        printf("AVISO: GEMINI_API_KEY não encontrada. Usando IA local.\n");
        config->enabled = 0;
        return 0;
    }
    
    // Configura valores padrão
    strncpy(config->api_key, api_key, sizeof(config->api_key) - 1);
    config->api_key[sizeof(config->api_key) - 1] = '\0';
    
    if (endpoint != NULL) {
        strncpy(config->endpoint, endpoint, sizeof(config->endpoint) - 1);
    } else {
        // Endpoint padrão do GEMINI (exemplo)
        strcpy(config->endpoint, "https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateContent");
    }
    config->endpoint[sizeof(config->endpoint) - 1] = '\0';
    
    config->enabled = 1;
    config->timeout_seconds = 10;
    
    memcpy(&g_config, config, sizeof(GeminiConfig));
    
    printf("IA GEMINI inicializada com sucesso!\n");
    printf("Endpoint: %s\n", config->endpoint);
    return 1;
}

// Função stub para sugestão via GEMINI (implementação futura)
int gemini_suggest_move(CardNode *head, int *suggested_index, char *reasoning) {
    if (!g_config.enabled) {
        return 0; // GEMINI não disponível
    }
    
    // STUB: Em uma implementação real, aqui seria feita a chamada HTTP para a API GEMINI
    // Exemplo de prompt que seria enviado:
    /*
     * Prompt para GEMINI:
     * "Você é a superinteligência CORTEX em um jogo de memória futurístico. 
     *  Analise o estado atual das cartas (lista de IDs) e sugira a próxima jogada
     *  para dificultar o jogador humano. Estado atual: [lista de cartas reveladas/matched]
     *  Responda apenas com o índice da carta sugerida e uma breve justificativa."
     */
    
    printf("CORTEX (GEMINI): Analisando estado do jogo...\n");
    
    // Simulação de resposta da API (implementação real faria parsing de JSON)
    CardNode *current = head;
    int index = 0;
    int available_positions[16];
    int available_count = 0;
    
    // Encontra posições disponíveis (não matched, não revealed)
    while (current != NULL) {
        if (!current->matched && !current->revealed) {
            available_positions[available_count++] = index;
        }
        current = current->next;
        index++;
    }
    
    if (available_count == 0) {
        return 0; // Nenhuma posição disponível
    }
    
    // Simula "decisão inteligente" da CORTEX
    srand(time(NULL));
    int choice = rand() % available_count;
    *suggested_index = available_positions[choice];
    
    if (reasoning != NULL) {
        sprintf(reasoning, "CORTEX detectou padrão neuronal na posição %d. Resistência humana prevista.", 
                *suggested_index);
    }
    
    printf("CORTEX (GEMINI): Sugestão - posição %d\n", *suggested_index);
    return 1;
}

// Função híbrida que tenta GEMINI primeiro, depois fallback para IA local
void jogadaIA_avancada(CardNode *head, int *sugestao_index, char *reasoning) {
    *sugestao_index = -1;
    
    // Tenta usar GEMINI primeiro
    if (g_config.enabled) {
        if (gemini_suggest_move(head, sugestao_index, reasoning)) {
            printf("🤖 CORTEX (GEMINI): %s\n", reasoning ? reasoning : "Movimento calculado.");
            return;
        } else {
            printf("CORTEX (GEMINI): Conexão falhou. Usando protocolo local...\n");
        }
    }
    
    // Fallback para IA local
    jogadaIA(head, sugestao_index);
    if (reasoning != NULL && *sugestao_index != -1) {
        sprintf(reasoning, "CORTEX (Local): Protocolo de emergência ativado - posição %d", *sugestao_index);
    }
}

// Cleanup da IA
void gemini_cleanup() {
    printf("IA GEMINI: recursos liberados.\n");
    memset(&g_config, 0, sizeof(GeminiConfig));
}

/*
 * DOCUMENTAÇÃO PARA INTEGRAÇÃO GEMINI:
 * 
 * 1. VARIÁVEIS DE AMBIENTE NECESSÁRIAS:
 *    - GEMINI_API_KEY: Chave da API do Google Gemini
 *    - GEMINI_ENDPOINT: (Opcional) URL customizada da API
 * 
 * 2. CONFIGURAÇÃO NO WINDOWS:
 *    set GEMINI_API_KEY=sua_chave_aqui
 *    set GEMINI_ENDPOINT=https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateContent
 * 
 * 3. CONFIGURAÇÃO NO LINUX/MAC:
 *    export GEMINI_API_KEY="sua_chave_aqui"
 *    export GEMINI_ENDPOINT="https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateContent"
 * 
 * 4. IMPLEMENTAÇÃO COMPLETA (TODO):
 *    - Adicionar biblioteca HTTP (curl ou similar)
 *    - Implementar parsing de JSON para resposta da API
 *    - Adicionar tratamento de erros de rede
 *    - Implementar cache local para reduzir chamadas à API
 *    - Adicionar rate limiting para respeitar limites da API
 * 
 * 5. PRIVACIDADE:
 *    - Nunca commitar chaves de API no repositório
 *    - Usar variáveis de ambiente ou arquivos de configuração locais
 *    - Considerar criptografia local das chaves
 * 
 * 6. EXEMPLO DE USO:
 *    GeminiConfig config;
 *    if (gemini_init(&config)) {
 *        int sugestao;
 *        char reasoning[256];
 *        jogadaIA_avancada(cardList, &sugestao, reasoning);
 *    }
 */