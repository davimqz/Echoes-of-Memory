#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include <string.h>
#include "memory.h" // Inclui o novo header
#ifdef USE_CURL
#include <curl/curl.h>
#endif
// Implementação local da lista encadeada e funções auxiliares (migradas para memory.c)

typedef struct CardNode {
    int id;                  // identificador do símbolo (0..N-1)
    int revealed;            // está temporariamente revelada
    int matched;             // já foi combinada/encontrada
    struct CardNode *next;   // ponteiro para o próximo nó
} CardNode;

// Cria um novo nó de carta
CardNode* create_card_node(int id) {
    CardNode *node = (CardNode*)malloc(sizeof(CardNode));
    if (node == NULL) {
        printf("Erro: falha na alocação de memória\n");
        return NULL;
    }
    node->id = id;
    node->revealed = 0;
    node->matched = 0;
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
    while (current->next != NULL) current = current->next;
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
    while (current != NULL) { count++; current = current->next; }
    return count;
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

// Embaralha um array usando Fisher-Yates
void shuffle_array(int *array, int size) {
    srand((unsigned)time(NULL));
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i]; array[i] = array[j]; array[j] = temp;
    }
}

// Inicializa as cartas do jogo (cria pares embaralhados)
// availableTypes: número de tipos de frente disponíveis (para indexar texturas)
void inicializarCartas(CardNode **head, int boardSize, int availableTypes) {
    if (*head != NULL) free_card_list(head);
    int totalCards = boardSize * boardSize;
    int requiredPairs = totalCards / 2;

    int *types = (int*)malloc(totalCards * sizeof(int));
    int *available = (int*)malloc(availableTypes * sizeof(int));
    for (int i = 0; i < availableTypes; i++) available[i] = i;

    // shuffle available
    shuffle_array(available, availableTypes);

    int sel = 0;
    // first take unique types up to requiredPairs
    for (int i = 0; i < availableTypes && sel < requiredPairs; i++) {
        types[sel*2] = available[i];
        types[sel*2 + 1] = available[i];
        sel++;
    }
    // if still need pairs, fill by reusing random available types
    while (sel < requiredPairs) {
        int pick = rand() % availableTypes;
        types[sel*2] = available[pick];
        types[sel*2 + 1] = available[pick];
        sel++;
    }

    // shuffle the 2*requiredPairs positions
    shuffle_array(types, totalCards);

    for (int i = 0; i < totalCards; i++) append_card(head, types[i]);

    free(types);
    free(available);
}

// Escolhe uma carta na posição especificada
CardNode* escolherCarta(CardNode *head, int index) {
    CardNode *card = node_at(head, index);
    if (card == NULL) return NULL;
    if (card->matched) return NULL;
    card->revealed = 1;
    return card;
}

// Verifica se duas cartas formam um par
int verificarPar(CardNode *a, CardNode *b) {
    if (a == NULL || b == NULL) return 0;
    if (a == b) return 0;
    if (a->id == b->id) {
        a->matched = 1; b->matched = 1; a->revealed = 0; b->revealed = 0; return 1;
    } else {
        a->revealed = 0; b->revealed = 0; return 0;
    }
}

// Troca dois nós adjacentes na lista encadeada
void swap_adjacent_nodes(CardNode **head, CardNode *prev, CardNode *a, CardNode *b) {
    if (a == NULL || b == NULL || a->next != b) return;
    if (prev == NULL) *head = b; else prev->next = b;
    a->next = b->next; b->next = a;
}

// Implementa Bubble Sort na lista encadeada
void ordenarCartas(CardNode **head) {
    if (*head == NULL || (*head)->next == NULL) return;
    int swapped;
    do {
        swapped = 0;
        CardNode *prev = NULL;
        CardNode *current = *head;
        while (current != NULL && current->next != NULL) {
            if (current->id > current->next->id) {
                CardNode *next = current->next;
                swap_adjacent_nodes(head, prev, current, next);
                prev = next;
                swapped = 1;
            } else { prev = current; current = current->next; }
        }
    } while (swapped);
}

// Busca o índice da primeira carta não revelada/não combinada de um tipo específico
int find_unrevealed_index_of_type(CardNode *head, int type, CardNode *exclude) {
    CardNode *cur = head; int idx = 0;
    while (cur != NULL) {
        if (cur != exclude && !cur->revealed && !cur->matched && cur->id == type) return idx;
        cur = cur->next; idx++;
    }
    return -1;
}

// Gera uma explicação/sugestão simples baseada no lastSeenNodes e estado atual
// Retorna o índice sugerido (0..N-1) ou -1 se sem sugestão. Preenche reason buffer.
int get_ai_suggestion(CardNode *head, CardNode **lastSeenNodes, int frontCount, char *reason, int reasonSize) {
    // 1) Se houver um tipo que já vimos (lastSeenNodes[type]) e ainda existe outra carta desse tipo não combinada,
    //    sugira a posição dessa outra carta (provavelmente formará par).
    for (int t = 0; t < frontCount; t++) {
        CardNode *seen = lastSeenNodes[t];
        if (seen != NULL && !seen->matched) {
            int other = find_unrevealed_index_of_type(head, t, seen);
            if (other >= 0) {
                snprintf(reason, reasonSize, "Sugestão: vire a carta na posição %d — já vimos uma carta do tipo '%d' anteriormente e é provável que forme par.", other+1, t);
                return other;
            }
        }
    }

    // 2) Caso não haja par conhecido, escolha uma posição que ainda não foi revelada/não combinada e que pertença
    //    a um tipo com maior presença restante (heurística simples). Construímos um contador por tipo.
    int counts[64] = {0}; // suporta até 64 tipos, mais do que suficiente aqui
    CardNode *cur = head; int idx = 0; int bestIdx = -1; int bestCount = -1;
    while (cur != NULL) {
        if (!cur->matched && !cur->revealed) counts[cur->id]++;
        cur = cur->next;
    }
    cur = head; idx = 0;
    while (cur != NULL) {
        if (!cur->matched && !cur->revealed) {
            int c = counts[cur->id];
            if (c > bestCount) { bestCount = c; bestIdx = idx; }
        }
        cur = cur->next; idx++;
    }
    if (bestIdx >= 0) {
        snprintf(reason, reasonSize, "Sugestão: vire a carta na posição %d — tipo com maior ocorrência restante (%d cópias), boa para explorar.", bestIdx+1, bestCount);
        return bestIdx;
    }

    // fallback: escolha aleatória disponível
    cur = head; idx = 0; int available[128]; int ac = 0;
    while (cur != NULL) {
        if (!cur->matched && !cur->revealed) available[ac++] = idx;
        cur = cur->next; idx++;
    }
    if (ac == 0) {
        snprintf(reason, reasonSize, "Sem cartas disponíveis para sugerir.");
        return -1;
    }
    int pick = rand() % ac;
    snprintf(reason, reasonSize, "Sugestão aleatória: vire a carta na posição %d para explorar novas informações.", available[pick]+1);
    return available[pick];
}

// --- small helper to call GEMINI (or any endpoint) via libcurl ---
#ifdef USE_CURL
struct MemoryResponse { char *data; size_t size; };
static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryResponse *mem = (struct MemoryResponse*)userp;
    char *ptr = (char*)realloc(mem->data, mem->size + realsize + 1);
    if (ptr == NULL) return 0; // out of memory
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    return realsize;
}

// Call GEMINI-like API: send prompt (plain text) in JSON body {"prompt": "..."}
// Returns a newly-allocated string with the response body (caller must free), or NULL on error.
char *call_gemini_api(const char *api_key, const char *api_url, const char *prompt, int timeout_seconds) {
    if (api_key == NULL || api_url == NULL) return NULL;
    CURL *curl = NULL; CURLcode res;
    struct curl_slist *headers = NULL;
    struct MemoryResponse resp; resp.data = NULL; resp.size = 0;
    char authHeader[512];
    snprintf(authHeader, sizeof(authHeader), "Authorization: Bearer %s", api_key);
    curl = curl_easy_init();
    if (!curl) return NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, authHeader);
    // build JSON body
    size_t bodyLen = strlen(prompt) + 256;
    char *body = (char*)malloc(bodyLen);
    if (!body) { curl_easy_cleanup(curl); curl_slist_free_all(headers); return NULL; }
    snprintf(body, bodyLen, "{\"prompt\": \"%s\"}", prompt);

    curl_easy_setopt(curl, CURLOPT_URL, api_url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&resp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_seconds);
    // perform
    res = curl_easy_perform(curl);
    free(body);
    if (res != CURLE_OK) {
        // cleanup
        if (resp.data) free(resp.data);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return NULL;
    }
    // success: resp.data holds response body
    char *out = resp.data; // transfer ownership
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return out;
}
#else
// If compiled without USE_CURL, provide a stub that returns NULL (no network available)
char *call_gemini_api(const char *api_key, const char *api_url, const char *prompt, int timeout_seconds) {
    (void)api_key; (void)api_url; (void)prompt; (void)timeout_seconds; return NULL;
}
#endif

// IA simples que sugere uma jogada (local)
void jogadaIA(CardNode *head, int *sugestao_index) {
    *sugestao_index = -1; CardNode *current = head; int index = 0; int available_positions[64]; int available_count = 0;
    while (current != NULL) {
        if (!current->matched && !current->revealed) available_positions[available_count++] = index;
        current = current->next; index++; }
    if (available_count == 0) return;
    srand((unsigned)time(NULL)); *sugestao_index = available_positions[rand() % available_count];
}

// Wrapper para liberarMemoria
void liberarMemoria(CardNode **head) { free_card_list(head); }

// Função para exibir o tabuleiro usando a lista encadeada
void exibirTabuleiro(CardNode *head, Texture2D cardBack, Texture2D cardFronts[], 
                     int frontMissing[], const char *frontNames[], 
                     float startX, float startY, float cardW, float cardH, float pad) {
    CardNode *current = head;
    int index = 0;
    
    while (current != NULL) {
        int row = index / 4;
        int col = index % 4;
        
        Rectangle dst;
        dst.x = startX + col * (cardW + pad);
        dst.y = startY + row * (cardH + pad);
        dst.width = cardW;
        dst.height = cardH;
        
        if (current->matched) {
            // carta encontrada - desenha a frente esmaecida
            int t = current->id;
            if (frontMissing[t]) {
                DrawRectangleRec(dst, Fade(LIGHTGRAY, 0.6f));
                DrawText(TextFormat("%s", frontNames[t]), 
                    (int)(dst.x + dst.width/2 - MeasureText(frontNames[t], 14)/2), 
                    (int)(dst.y + dst.height/2 - 7), 14, DARKBLUE);
            } else {
                DrawTexturePro(cardFronts[t], 
                    (Rectangle){0,0, (float)cardFronts[t].width, (float)cardFronts[t].height}, 
                    dst, (Vector2){0,0}, 0.0f, Fade(WHITE, 0.6f));
            }
        } else if (current->revealed) {
            int t = current->id;
            if (frontMissing[t]) {
                DrawRectangleRec(dst, LIGHTGRAY);
                DrawText(TextFormat("%s", frontNames[t]), 
                    (int)(dst.x + dst.width/2 - MeasureText(frontNames[t], 14)/2), 
                    (int)(dst.y + dst.height/2 - 7), 14, DARKBLUE);
            } else {
                DrawTexturePro(cardFronts[t], 
                    (Rectangle){0,0, (float)cardFronts[t].width, (float)cardFronts[t].height}, 
                    dst, (Vector2){0,0}, 0.0f, WHITE);
            }
        } else {
            DrawTexturePro(cardBack, 
                (Rectangle){0,0, (float)cardBack.width, (float)cardBack.height}, 
                dst, (Vector2){0,0}, 0.0f, WHITE);
        }
        
        DrawRectangleLinesEx(dst, 2, BLACK);
        
        current = current->next;
        index++;
    }
}

// NOVO NOME DA FUNÇÃO: AGORA ELA É CHAMADA PELO main.c
int RunMemoryGame(int initialDifficulty) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    // A janela JÁ DEVE ESTAR ABERTA PELO main.c.
    // InitWindow(screenWidth, screenHeight, "Echoes of Memory - Lista Encadeada");
    // SetTargetFPS(60);

    // Carrega texturas (Assumimos que o main.c já carregou, mas vamos carregar aqui para garantir
    // que o módulo seja auto-suficiente caso ele não seja executado na mesma thread do main.c - MELHOR CARREGAR E DESCARREGAR)
    Texture2D cardBack = LoadTexture("./assets/cards/back/card.png");
    Texture2D cardFronts[7];
    // filenames to try (first is preferred, second is ASCII fallback)
    const char *frontFiles[7][2] = {
        {"./assets/cards/front/ball.png", NULL},
        {"./assets/cards/front/bike.png", NULL},
        {"./assets/cards/front/cookie.png", NULL},
        {"./assets/cards/front/dado.png", NULL},
        {"./assets/cards/front/palha\xE7o.png", "./assets/cards/front/palhaco.png"}, // try accented then ascii
        {"./assets/cards/front/pelucia.png", NULL},
        {"./assets/cards/front/videoGame.png", NULL}
    };
    const char *frontNames[7] = {"ball","bike","cookie","dado","palhaco","pelucia","videoGame"};
    int frontCount = 7;
    int frontMissing[7] = {0};

    // Try loading each front; if the preferred name fails and there is a fallback, try it
    for (int i = 0; i < frontCount; i++) {
        const char *f0 = frontFiles[i][0];
        cardFronts[i] = LoadTexture(f0);
        if (cardFronts[i].width == 0 || cardFronts[i].height == 0) {
            // try fallback if provided
            const char *fb = frontFiles[i][1];
            if (fb != NULL) {
                UnloadTexture(cardFronts[i]);
                cardFronts[i] = LoadTexture(fb);
            }
        }
        if (cardFronts[i].width == 0 || cardFronts[i].height == 0) frontMissing[i] = 1;
    }

    // Inicializa a lista encadeada de cartas
    CardNode *cardList = NULL;
    inicializarCartas(&cardList, 4, frontCount); // 4x4 board

    // lastSeenNodes keeps the last seen (revealed) node pointer for each type
    CardNode **lastSeenNodes = (CardNode**)malloc(sizeof(CardNode*) * frontCount);
    for (int i = 0; i < frontCount; i++) lastSeenNodes[i] = NULL;

    // Help (GEMINI) state: allow one help request per round
    int helpUsedThisRound = 0;
    int suggestionIndex = -1;
    char suggestionText[256] = {0};
    float suggestionTimer = 0.0f; // how long to show suggestion overlay
    // Game rules: mistakes / lives
    const int maxMistakes = 5; // player can err this many times
    int mistakes = 0;
    int gameLost = 0; // set when mistakes >= maxMistakes
    float errorFlash = 0.0f; // visual flash timer when wrong
    float lostTimer = 0.0f; // timer to show lost message
    const float lostShowTime = 2.0f; // seconds to show lost message before returning to menu
    // session-only API key storage (entered in-game; not persisted)
    char session_api_key[256] = {0};
    int session_api_key_len = 0;

    float cardW = 120.0f;
    float cardH = 160.0f;
    float pad = 20.0f;
    float startX = (screenWidth - (4 * cardW + 3 * pad)) / 2.0f;
    float startY = (screenHeight - (4 * cardH + 3 * pad)) / 2.0f;

    CardNode *first = NULL, *second = NULL;
    int firstIndex = -1, secondIndex = -1;
    float flipTimer = 0.0f;
    int matchedPairs = 0;
    int gameWon = 0;

    float previewTimer = 5.0f;
    int showingPreview = 1;

    // Simplificamos o estado para ir direto ao jogo
    // typedef enum { STATE_MENU=0, STATE_DIFF=1, STATE_PLAY=2 } AppState;
    // AppState state = STATE_PLAY; 
    int difficulty = initialDifficulty; // usa o parâmetro

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (showingPreview) {
            previewTimer -= GetFrameTime();
            if (previewTimer <= 0.0f) {
                showingPreview = 0;
                CardNode *current = cardList;
                while (current != NULL) {
                    current->revealed = 0;
                    current = current->next;
                }
            } else {
                CardNode *current = cardList;
                while (current != NULL) {
                    if (!current->matched) {
                        current->revealed = 1;
                    }
                    current = current->next;
                }
            }
        }

        // O jogo principal (main.c) deve ter a lógica de menu/dificuldade.
        // A lógica abaixo é o loop do Jogo da Memória em si.

        if (flipTimer > 0) {
            printf("DEBUG: flipTimer atual = %.2f segundos\n", flipTimer);
            flipTimer -= GetFrameTime();
            if (flipTimer <= 0 && first != NULL && second != NULL) {
                printf("DEBUG: Timer expirou! Verificando par...\n");
                int isMatch = verificarPar(first, second);
                if (isMatch) {
                    matchedPairs++;
                    if (matchedPairs >= 8) gameWon = 1;
                    printf("Par encontrado!\n");
                    if (first != NULL) lastSeenNodes[first->id] = NULL;
                }
                else {
                    // wrong attempt: increment mistakes and flash
                    mistakes++;
                    errorFlash = 0.6f;
                    if (mistakes >= maxMistakes) {
                        gameLost = 1;
                        lostTimer = lostShowTime;
                        printf("Game Over: demasiados erros (%d)\n", mistakes);
                    }
                }

                first = second = NULL;
                firstIndex = secondIndex = -1;
                // After resolving a flip pair, we keep helpUsedThisRound as-is (single per entire round)
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && flipTimer <= 0 && !gameLost && !gameWon && !showingPreview) {
            CardNode *current = cardList;
            int index = 0;
            
            while (current != NULL) {
                int row = index / 4;
                int col = index % 4;
                Rectangle cardRect;
                cardRect.x = startX + col * (cardW + pad);
                cardRect.y = startY + row * (cardH + pad);
                cardRect.width = cardW;
                cardRect.height = cardH;
                
                if (CheckCollisionPointRec(mouse, cardRect)) {
                    CardNode *selectedCard = escolherCarta(cardList, index);
                    if (selectedCard != NULL) {
                        // Update last seen pointer for this type
                        CardNode *prevSeen = lastSeenNodes[selectedCard->id];

                        if (first == NULL) {
                            first = selectedCard;
                            firstIndex = index;

                            // If easy mode and we previously saw the matching card (and it's not matched), auto-select it
                            if (difficulty == 0 && prevSeen != NULL && prevSeen != selectedCard && !prevSeen->matched) {
                                second = prevSeen;
                                // reveal the second card so player sees it
                                second->revealed = 1;
                                // set a timer to process match automatically
                                flipTimer = 1.0f;
                            }
                        } else if (second == NULL && selectedCard != first) {
                            second = selectedCard;
                            secondIndex = index;
                            // define tempo para ver as cartas dependendo da dificuldade
                            if (difficulty == 0) {
                                flipTimer = 1.0f; // easy: more time
                                printf("DEBUG: Dificuldade 0 - flipTimer definido para 1.0f\n");
                            }
                            else if (difficulty == 1) {
                                flipTimer = 1.0f; // medium
                                printf("DEBUG: Dificuldade 1 - flipTimer definido para 1.0f\n");
                            }
                            else {
                                flipTimer = 1.0f; // hard
                                printf("DEBUG: Dificuldade 2+ - flipTimer definido para 1.0f\n");
                            }
                        }

                        // record this node as last seen for its type if not matched
                        if (!selectedCard->matched) lastSeenNodes[selectedCard->id] = selectedCard;
                    }
                    break;
                }
                current = current->next;
                index++;
            }
        }

        // Help request: tecla 'H' (uma vez por round)
        if (IsKeyPressed(KEY_H) && !helpUsedThisRound && !gameWon) {
            // ask for suggestion and explanation (local fallback)
            suggestionIndex = get_ai_suggestion(cardList, lastSeenNodes, frontCount, suggestionText, sizeof(suggestionText));

            // If GEMINI env variables are provided, ask the remote model for a richer explanation.
            const char *api_key = getenv("GEMINI_API_KEY");
            const char *api_url = getenv("GEMINI_API_URL");

            // If there is no global env key, allow entering a session-only key via an in-game modal
            if ((api_key == NULL || strlen(api_key) == 0) && session_api_key_len == 0) {
                // open a blocking modal to enter API key for this session (won't be saved)
                int entering = 1;
                while (entering && !WindowShouldClose()) {
                    BeginDrawing();
                    // dim background
                    DrawRectangle(0,0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
                    DrawRectangle(screenWidth/2 - 360, screenHeight/2 - 80, 720, 160, LIGHTGRAY);
                    DrawRectangleLines(screenWidth/2 - 360, screenHeight/2 - 80, 720, 160, DARKGRAY);
                    DrawText("Digite a API key do GEMINI para esta sessão (ENTER para enviar, ESC para cancelar):", screenWidth/2 - 340, screenHeight/2 - 60, 16, DARKBLUE);
                    // show masked input (show last 6 chars for verification)
                    char masked[300];
                    if (session_api_key_len == 0) snprintf(masked, sizeof(masked), "(nenhuma)");
                    else {
                        int show = 6; if (session_api_key_len < show) show = session_api_key_len;
                        int start = session_api_key_len - show;
                        if (start < 0) start = 0;
                        snprintf(masked, sizeof(masked), "***...%s", &session_api_key[start]);
                    }
                    DrawText(masked, screenWidth/2 - 340, screenHeight/2 - 20, 20, BLACK);

                    // capture character input
                    int key = GetCharPressed();
                    while (key > 0) {
                        // support backspace
                        if ((unsigned)key == 127 || key == 8) {
                            if (session_api_key_len > 0) { session_api_key[--session_api_key_len] = '\0'; }
                        } else if (key >= 32 && session_api_key_len < (int)sizeof(session_api_key)-1) {
                            session_api_key[session_api_key_len++] = (char)key; session_api_key[session_api_key_len] = '\0';
                        }
                        key = GetCharPressed();
                    }

                    if (IsKeyPressed(KEY_ENTER)) { entering = 0; }
                    if (IsKeyPressed(KEY_ESCAPE)) { session_api_key_len = 0; session_api_key[0] = '\0'; entering = 0; }

                    EndDrawing();
                    // small sleep to avoid busy loop
                    WaitTime(0.01f);
                }
            }

            // prefer session key if provided
            const char *use_key = (session_api_key_len>0) ? session_api_key : api_key;
            if (use_key != NULL && strlen(use_key) > 0 && api_url != NULL) {
                // Build a compact board summary for the prompt
                char prompt[2048];
                char board[1400]; board[0] = '\0';
                CardNode *c = cardList; int idx = 0;
                while (c != NULL) {
                    char line[128];
                    if (c->matched) snprintf(line, sizeof(line), "%d:MATCHED(%d); ", idx+1, c->id);
                    else if (c->revealed) snprintf(line, sizeof(line), "%d:REVEALED(%d); ", idx+1, c->id);
                    else snprintf(line, sizeof(line), "%d:UNKNOWN; ", idx+1);
                    strncat(board, line, sizeof(board)-strlen(board)-1);
                    c = c->next; idx++;
                }
                snprintf(prompt, sizeof(prompt),
                    "You are an assistant for a 4x4 memory game. The board state: %s\nI (the game) propose the player consider position %d (1-based). Please explain in one or two short sentences why choosing that card is a good move given the board state. Reply in plain text only.",
                    board, suggestionIndex+1);

                char *resp = call_gemini_api(use_key, api_url, prompt, 6);
                if (resp != NULL) {
                    // use remote explanation (trim to buffer)
                    strncpy(suggestionText, resp, sizeof(suggestionText)-1);
                    suggestionText[sizeof(suggestionText)-1] = '\0';
                    free(resp);
                }
            }

            if (suggestionIndex >= 0) suggestionTimer = 4.0f; else suggestionTimer = 2.0f;
            helpUsedThisRound = 1;
        }

        BeginDrawing();
        // Futuristic dark background
        ClearBackground(BLACK);
        // subtle vertical gradient
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, (Color){6,8,18,255}, (Color){0,0,0,255});
        // neon grid lines
        for (int g = 0; g < 12; g++) {
            int y = (int)(screenHeight * ((float)g / 12.0f));
            DrawLine(0, y, screenWidth, y, Fade((Color){12,30,50,255}, 0.03f));
        }
        // subtle vignette
        DrawRectangleLinesEx((Rectangle){40,40, screenWidth-80, screenHeight-80}, 4, Fade((Color){20,40,80,255}, 0.06f));

    // Header
    DrawText("ECHOES OF MEMORY", 36, 18, 32, (Color){80,220,255,255});
    DrawText("Clique nas cartas para virar. H = Ajuda, ESC = Sair, ENTER = Reiniciar.", 36, 58, 16, Fade((Color){180,220,255,200}, 0.85f));

        // Usa a nova função exibirTabuleiro
        exibirTabuleiro(cardList, cardBack, cardFronts, frontMissing, frontNames, 
                       startX, startY, cardW, cardH, pad);

        if (showingPreview) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));
            DrawText("Memorize as cartas!", screenWidth/2 - MeasureText("Memorize as cartas!", 40)/2, 100, 40, WHITE);
            DrawText(TextFormat("%.1f", previewTimer), screenWidth/2 - MeasureText("0.0", 60)/2, 160, 60, YELLOW);
        }

        // Draw suggestion overlay if active
        if (suggestionTimer > 0.0f) {
            suggestionTimer -= GetFrameTime();
            // translucent background
            DrawRectangle(100, screenHeight - 160, screenWidth - 200, 120, Fade(BLACK, 0.6f));
            DrawText(suggestionText, 120, screenHeight - 140, 20, RAYWHITE);
            // highlight suggested card if index valid
            if (suggestionIndex >= 0) {
                int row = suggestionIndex / 4;
                int col = suggestionIndex % 4;
                Rectangle dst;
                dst.x = startX + col * (cardW + pad);
                dst.y = startY + row * (cardH + pad);
                dst.width = cardW;
                dst.height = cardH;
                // neon highlight
                DrawRectangleLinesEx(dst, 6, (Color){110,255,200,255});
                DrawRectangleLinesEx(dst, 2, Fade((Color){110,255,200,255}, 0.6f));
            }
        }

        // Draw mistakes / lives HUD (top-right)
        {
            int hudX = screenWidth - 300;
            int hudY = 24;
            DrawText("Vidas", hudX, hudY, 18, (Color){180,240,255,220});
            for (int i = 0; i < maxMistakes; i++) {
                int cx = hudX + 80 + i * 30;
                int cy = hudY + 18;
                if (i < (maxMistakes - mistakes)) {
                    // alive
                    DrawCircle(cx, cy, 10, (Color){40,220,200,255});
                    DrawCircleLines(cx, cy, 12, (Color){20,120,120,180});
                } else {
                    // lost
                    DrawCircle(cx, cy, 10, Fade((Color){180,40,60,255}, 0.6f));
                    DrawCircleLines(cx, cy, 12, Fade((Color){120,20,30,255}, 0.6f));
                }
            }
            DrawText(TextFormat("Erros: %d/%d", mistakes, maxMistakes), hudX + 8, hudY + 40, 14, (Color){180,200,255,200});
        }

        // error flash overlay
        if (errorFlash > 0.0f) {
            errorFlash -= GetFrameTime();
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade((Color){255,40,40,120}, errorFlash*0.8f));
        }

        if (gameLost) {
            // show 'you lost' message for a short time then return to menu
            if (lostTimer > 0.0f) lostTimer -= GetFrameTime();
            DrawText("Voce perdeu a sua consciencia!", screenWidth/2 - MeasureText("Voce perdeu a sua consciencia!", 40)/2, 100, 40, RED);
            DrawText("Voltando ao jogo principal...", screenWidth/2 - MeasureText("Voltando ao jogo principal...", 20)/2, 150, 20, DARKGRAY);
            if (lostTimer <= 0.0f) {
                // cleanup and return 0 (failure)
                liberarMemoria(&cardList);
                free(lastSeenNodes);
                UnloadTexture(cardBack);
                for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
                return 0; // RETORNA 0: PERDEU/FALHA
            }
        } else if (gameWon) {
            DrawText("PARABENS! Voce restaurou suas memorias!", screenWidth/2 - MeasureText("PARABENS! Voce restaurou suas memorias!", 32)/2, 100, 32, GREEN);
            DrawText("A CORTEX foi derrotada! Pressione ENTER para retornar", screenWidth/2 - MeasureText("A CORTEX foi derrotada! Pressione ENTER para retornar", 20)/2, 140, 20, DARKBLUE);
            if (IsKeyPressed(KEY_ENTER)) {
                // cleanup and return 1 (success)
                liberarMemoria(&cardList);
                free(lastSeenNodes);
                UnloadTexture(cardBack);
                for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
                return 1; // RETORNA 1: VENCEU/SUCESSO
            }
        }
        
        // Se o jogador pressionar ESC durante o jogo ativo, ele cancela e volta
        if (IsKeyPressed(KEY_ESCAPE)) {
            // cleanup and return 0 (canceled)
            liberarMemoria(&cardList);
            free(lastSeenNodes);
            UnloadTexture(cardBack);
            for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
            return 0; // RETORNA 0: CANCELADO
        }

        EndDrawing();
    }

    // Se a janela fechar, retornar 0
    liberarMemoria(&cardList);
    free(lastSeenNodes);
    UnloadTexture(cardBack);
    for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
    return 0;
}