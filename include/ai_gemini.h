#ifndef AI_GEMINI_H
#define AI_GEMINI_H

#include "cards_list.h"

// Configuração da IA GEMINI
typedef struct {
    char api_key[256];
    char endpoint[512];
    int enabled;
    int timeout_seconds;
} GeminiConfig;

// Funções da IA GEMINI
int gemini_init(GeminiConfig *config);
int gemini_suggest_move(CardNode *head, int *suggested_index, char *reasoning);
void gemini_cleanup();

// Função híbrida que usa IA local ou GEMINI
void jogadaIA_avancada(CardNode *head, int *sugestao_index, char *reasoning);

#endif // AI_GEMINI_H