#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
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

int main(void) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Echoes of Memory - Lista Encadeada");
    SetTargetFPS(60);

    // Carrega texturas
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

    typedef enum { STATE_MENU=0, STATE_DIFF=1, STATE_PLAY=2 } AppState;
    AppState state = STATE_MENU;
    // difficulty: 0 = Fácil, 1 = Médio, 2 = Difícil
    int difficulty = 1; // default Médio

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        // Menu state
        if (state == STATE_MENU) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            int cx = screenWidth/2;
            // layout fonts and gaps
            int titleFont = 48;
            int opt1Font = 32;
            int opt2Font = 28;
            int diffFont = 24;
            int opt4Font = 28;
            int gapTitle = 40;
            int gap1 = 30;
            int gap2 = 20;
            int gap3 = 40;
            int totalHeight = titleFont + gapTitle + opt1Font + gap1 + opt2Font + gap2 + diffFont + gap3 + opt4Font;
            int startY = screenHeight/2 - totalHeight/2;
            int y = startY;
            DrawText("ECHOES OF MEMORY", cx - MeasureText("ECHOES OF MEMORY", titleFont)/2, y, titleFont, DARKBLUE);
            y += titleFont + gapTitle;
            DrawText("1 - Jogar Agora", cx - MeasureText("1 - Jogar Agora", opt1Font)/2, y, opt1Font, BLACK);
            y += opt1Font + gap1;
            DrawText("2 - Escolher dificuldade", cx - MeasureText("2 - Escolher dificuldade", opt2Font)/2, y, opt2Font, BLACK);
            y += opt2Font + gap2;
            DrawText(TextFormat("Dificuldade atual: %s", difficulty==0?"Normal":"Difícil"), cx - MeasureText("Dificuldade atual: Normal", diffFont)/2, y, diffFont, DARKGRAY);
            y += diffFont + gap3;
            DrawText("4 - Sair", cx - MeasureText("4 - Sair", opt4Font)/2, y, opt4Font, BLACK);
            // footer centered
            DrawText("(Use 1/2/4 para escolher)", cx - MeasureText("(Use 1/2/4 para escolher)", 20)/2, screenHeight - 40, 20, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
                // start game: reset board (reshuffle) and go to play state
                liberarMemoria(&cardList); // libera lista anterior
                inicializarCartas(&cardList, 4, frontCount); // cria nova lista 4x4
                first = second = NULL; firstIndex = secondIndex = -1; 
                flipTimer = 0; matchedPairs = 0; gameWon = 0;
                state = STATE_PLAY;
            }
            if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
                state = STATE_DIFF;
            }
            if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) {
                break;
            }

            WaitTime(0.01f);
            continue;
        }

        if (state == STATE_DIFF) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            int cx = screenWidth/2;
            int titleFont = 36;
            int optFont = 28;
            int gapTitle = 40;
            int gapOpt = 24;
            int totalH = titleFont + gapTitle + optFont + gapOpt + optFont + gapOpt + optFont;
            int y0 = screenHeight/2 - totalH/2;
            int y = y0;
            DrawText("Escolha a dificuldade", cx - MeasureText("Escolha a dificuldade", titleFont)/2, y, titleFont, DARKBLUE);
            y += titleFont + gapTitle;
            DrawText("1 - Fácil", cx - MeasureText("1 - Fácil", optFont)/2, y, optFont, BLACK);
            y += optFont + gapOpt;
            DrawText("2 - Médio", cx - MeasureText("2 - Médio", optFont)/2, y, optFont, BLACK);
            y += optFont + gapOpt;
            DrawText("3 - Difícil", cx - MeasureText("3 - Difícil", optFont)/2, y, optFont, BLACK);
            DrawText("Pressione 1, 2 ou 3 para escolher, ESC para voltar", cx - MeasureText("Pressione 1, 2 ou 3 para escolher, ESC para voltar", 20)/2, screenHeight - 40, 20, DARKGRAY);
            EndDrawing();
            if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) { difficulty = 0; state = STATE_MENU; }
            if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) { difficulty = 1; state = STATE_MENU; }
            if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) { difficulty = 2; state = STATE_MENU; }
            if (IsKeyPressed(KEY_ESCAPE)) state = STATE_MENU;
            WaitTime(0.01f);
            continue;
        }

        if (flipTimer > 0) {
            flipTimer -= GetFrameTime();
            if (flipTimer <= 0 && first != NULL && second != NULL) {
                int isMatch = verificarPar(first, second);
                if (isMatch) {
                    matchedPairs++;
                    if (matchedPairs >= 8) gameWon = 1;
                    // Aplica bubble sort após encontrar um par
                    printf("Par encontrado! Aplicando reordenação CORTEX...\n");
                    ordenarCartas(&cardList);
                        // Clear remembered pointer for this type
                        if (first != NULL) lastSeenNodes[first->id] = NULL;
                }
                first = second = NULL;
                firstIndex = secondIndex = -1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && flipTimer <= 0) {
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
                                // set a short timer to process match automatically
                                flipTimer = 0.25f;
                            }
                        } else if (second == NULL && selectedCard != first) {
                            second = selectedCard;
                            secondIndex = index;
                            // define tempo para ver as cartas dependendo da dificuldade
                            if (difficulty == 0) flipTimer = 1.2f; // easy: more time
                            else if (difficulty == 1) flipTimer = 0.9f; // medium
                            else flipTimer = 0.6f; // hard
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

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Echoes of Memory (Lista Encadeada + Bubble Sort)", 20, 20, 24, DARKBLUE);
        DrawText("Clique nas cartas para virar. ESC = Sair, ENTER = Reiniciar.", 20, 50, 16, DARKGRAY);

        // Usa a nova função exibirTabuleiro
        exibirTabuleiro(cardList, cardBack, cardFronts, frontMissing, frontNames, 
                       startX, startY, cardW, cardH, pad);

        if (gameWon) {
            DrawText("PARABENS! Voce restaurou suas memorias!", screenWidth/2 - MeasureText("PARABENS! Voce restaurou suas memorias!", 32)/2, 100, 32, GREEN);
            DrawText("A CORTEX foi derrotada! Pressione ENTER para reiniciar", screenWidth/2 - MeasureText("A CORTEX foi derrotada! Pressione ENTER para reiniciar", 20)/2, 140, 20, DARKBLUE);
            if (IsKeyPressed(KEY_ENTER)) {
                // reinicia o jogo: cria nova lista embaralhada
                liberarMemoria(&cardList);
                inicializarCartas(&cardList, 4, frontCount);
                first = second = NULL; firstIndex = secondIndex = -1; 
                flipTimer = 0; matchedPairs = 0; gameWon = 0;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) break;

        EndDrawing();
    }

    // cleanup
    liberarMemoria(&cardList);
    free(lastSeenNodes);
    UnloadTexture(cardBack);
    for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
    CloseWindow();
    return 0;
}
