#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include <string.h>
#include "memory.h"

typedef struct CardNode {
    int id;
    int revealed;
    int matched;
    struct CardNode *next;
} CardNode;

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

int list_length(CardNode *head) {
    int count = 0;
    CardNode *current = head;
    while (current != NULL) { count++; current = current->next; }
    return count;
}

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

void shuffle_array(int *array, int size) {
    srand((unsigned)time(NULL));
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i]; array[i] = array[j]; array[j] = temp;
    }
}

void inicializarCartas(CardNode **head, int boardRows, int boardCols, int requiredPairs) {
    if (*head != NULL) free_card_list(head);
    int totalCards = boardRows * boardCols;
    
    printf("DEBUG: Nível %dx%d = %d cartas, %d pares\n", boardRows, boardCols, totalCards, requiredPairs);

    int *cards = (int*)malloc(totalCards * sizeof(int));
    int cardIndex = 0;
    
    for (int pair = 0; pair < requiredPairs; pair++) {
        cards[cardIndex++] = pair;
        cards[cardIndex++] = pair;
        printf("DEBUG: Par %d - Tipo %d\n", pair + 1, pair);
    }
    
    shuffle_array(cards, totalCards);
    
    for (int i = 0; i < totalCards; i++) {
        append_card(head, cards[i]);
    }
    
    free(cards);
}

CardNode* escolherCarta(CardNode *head, int index) {
    CardNode *card = node_at(head, index);
    if (card == NULL) return NULL;
    if (card->matched) return NULL;
    card->revealed = 1;
    return card;
}

int verificarPar(CardNode *a, CardNode *b) {
    if (a == NULL || b == NULL) return 0;
    if (a == b) return 0;
    if (a->id == b->id) {
        a->matched = 1; b->matched = 1; a->revealed = 0; b->revealed = 0; return 1;
    } else {
        a->revealed = 0; b->revealed = 0; return 0;
    }
}

void swap_adjacent_nodes(CardNode **head, CardNode *prev, CardNode *a, CardNode *b) {
    if (a == NULL || b == NULL || a->next != b) return;
    if (prev == NULL) *head = b; else prev->next = b;
    a->next = b->next; b->next = a;
}

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

void liberarMemoria(CardNode **head) { free_card_list(head); }

void exibirTabuleiro(CardNode *head, Texture2D cardBack, Texture2D cardFronts[], 
                     int frontMissing[], const char *frontNames[], 
                     float startX, float startY, float cardW, float cardH, float pad, int boardCols) {
    CardNode *current = head;
    int index = 0;
    
    while (current != NULL) {
        int row = index / boardCols;
        int col = index % boardCols;
        
        Rectangle dst;
        dst.x = startX + col * (cardW + pad);
        dst.y = startY + row * (cardH + pad);
        dst.width = cardW;
        dst.height = cardH;
        
        if (current->matched) {
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

int RunMemoryGame(int initialDifficulty) {
    printf("DEBUG: RunMemoryGame chamado com initialDifficulty = %d\n", initialDifficulty);
    
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    Texture2D cardBack = LoadTexture("./assets/cards/back/card.png");
    Texture2D cardFronts[10];
    const char *frontFiles[10][2] = {
        {"./assets/cards/front/balao.png", NULL},
        {"./assets/cards/front/carro.png", NULL},
        {"./assets/cards/front/pirulio.png", NULL},
        {"./assets/cards/front/ball.png", NULL},
        {"./assets/cards/front/bike.png", NULL},
        {"./assets/cards/front/cookie.png", NULL},
        {"./assets/cards/front/dado.png", NULL},
        {"./assets/cards/front/palha\xE7o.png", "./assets/cards/front/palhaco.png"},
        {"./assets/cards/front/pelucia.png", NULL},
        {"./assets/cards/front/videoGame.png", NULL}
    };
    const char *frontNames[10] = {"balao","carro","pirulio","ball","bike","cookie","dado","palhaco","pelucia","videoGame"};
    int frontCount = 10;
    int frontMissing[10] = {0};
    
    int boardRows, boardCols, requiredPairs;
    printf("DEBUG: initialDifficulty recebido = %d\n", initialDifficulty);
    
    switch(initialDifficulty) {
        case 0:
            boardRows = 4; boardCols = 3; requiredPairs = 6;
            printf("DEBUG: Configurando Nível 1 - 4x3\n");
            break;
        case 1:
            boardRows = 4; boardCols = 4; requiredPairs = 8;
            printf("DEBUG: Configurando Nível 2 - 4x4\n");
            break;
        case 2:
            boardRows = 5; boardCols = 4; requiredPairs = 10;
            printf("DEBUG: Configurando Nível 3 - 5x4\n");
            break;
        default:
            boardRows = 4; boardCols = 4; requiredPairs = 8;
            printf("DEBUG: Configurando Nível padrão - 4x4\n");
            break;
    }

    for (int i = 0; i < frontCount; i++) {
        const char *f0 = frontFiles[i][0];
        cardFronts[i] = LoadTexture(f0);
        if (cardFronts[i].width == 0 || cardFronts[i].height == 0) {
            const char *fb = frontFiles[i][1];
            if (fb != NULL) {
                UnloadTexture(cardFronts[i]);
                cardFronts[i] = LoadTexture(fb);
            }
        }
        if (cardFronts[i].width == 0 || cardFronts[i].height == 0) frontMissing[i] = 1;
    }

    CardNode *cardList = NULL;
    inicializarCartas(&cardList, boardRows, boardCols, requiredPairs);

    const int maxMistakes = 5;
    int mistakes = 0;
    int gameLost = 0;
    float errorFlash = 0.0f;
    float lostTimer = 0.0f;
    const float lostShowTime = 2.0f;

    float cardW = 120.0f;
    float cardH = 160.0f;
    float pad = 20.0f;
    float startX = (screenWidth - (boardCols * cardW + (boardCols-1) * pad)) / 2.0f;
    float startY = (screenHeight - (boardRows * cardH + (boardRows-1) * pad)) / 2.0f;

    CardNode *first = NULL, *second = NULL;
    int firstIndex = -1, secondIndex = -1;
    float flipTimer = 0.0f;
    int matchedPairs = 0;
    int gameWon = 0;

    float previewTimer = 5.0f;
    int showingPreview = 1;

    int difficulty = initialDifficulty;

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

        if (flipTimer > 0) {
            printf("DEBUG: flipTimer atual = %.2f segundos\n", flipTimer);
            flipTimer -= GetFrameTime();
            if (flipTimer <= 0 && first != NULL && second != NULL) {
                printf("DEBUG: Timer expirou! Verificando par...\n");
                int isMatch = verificarPar(first, second);
                if (isMatch) {
                    matchedPairs++;
                    if (matchedPairs >= requiredPairs) gameWon = 1;
                    printf("Par encontrado!\n");
                } else {
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
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && flipTimer <= 0 && !gameLost && !gameWon && !showingPreview) {
            CardNode *current = cardList;
            int index = 0;
            
            while (current != NULL) {
                int row = index / boardCols;
                int col = index % boardCols;
                Rectangle cardRect;
                cardRect.x = startX + col * (cardW + pad);
                cardRect.y = startY + row * (cardH + pad);
                cardRect.width = cardW;
                cardRect.height = cardH;
                
                if (CheckCollisionPointRec(mouse, cardRect)) {
                    CardNode *selectedCard = escolherCarta(cardList, index);
                    if (selectedCard != NULL) {
                        if (first == NULL) {
                            first = selectedCard;
                            firstIndex = index;
                        } else if (second == NULL && selectedCard != first) {
                            second = selectedCard;
                            secondIndex = index;
                            flipTimer = 1.0f;
                            printf("DEBUG: flipTimer definido para 1.0f\n");
                        }
                    }
                    break;
                }
                current = current->next;
                index++;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, (Color){6,8,18,255}, (Color){0,0,0,255});
        for (int g = 0; g < 12; g++) {
            int y = (int)(screenHeight * ((float)g / 12.0f));
            DrawLine(0, y, screenWidth, y, Fade((Color){12,30,50,255}, 0.03f));
        }
        DrawRectangleLinesEx((Rectangle){40,40, screenWidth-80, screenHeight-80}, 4, Fade((Color){20,40,80,255}, 0.06f));

        DrawText("ECHOES OF MEMORY", 36, 18, 32, (Color){80,220,255,255});
        DrawText("Clique nas cartas para virar. ESC = Sair, ENTER = Reiniciar.", 36, 58, 16, Fade((Color){180,220,255,200}, 0.85f));

        exibirTabuleiro(cardList, cardBack, cardFronts, frontMissing, frontNames, 
                       startX, startY, cardW, cardH, pad, boardCols);

        if (showingPreview) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));
            DrawText("Memorize as cartas!", screenWidth/2 - MeasureText("Memorize as cartas!", 40)/2, 100, 40, WHITE);
            DrawText(TextFormat("%.1f", previewTimer), screenWidth/2 - MeasureText("0.0", 60)/2, 160, 60, YELLOW);
        }

        {
            int hudX = screenWidth - 300;
            int hudY = 24;
            DrawText("Vidas", hudX, hudY, 18, (Color){180,240,255,220});
            for (int i = 0; i < maxMistakes; i++) {
                int cx = hudX + 80 + i * 30;
                int cy = hudY + 18;
                if (i < (maxMistakes - mistakes)) {
                    DrawCircle(cx, cy, 10, (Color){40,220,200,255});
                    DrawCircleLines(cx, cy, 12, (Color){20,120,120,180});
                } else {
                    DrawCircle(cx, cy, 10, Fade((Color){180,40,60,255}, 0.6f));
                    DrawCircleLines(cx, cy, 12, Fade((Color){120,20,30,255}, 0.6f));
                }
            }
            DrawText(TextFormat("Erros: %d/%d", mistakes, maxMistakes), hudX + 8, hudY + 40, 14, (Color){180,200,255,200});
        }

        if (errorFlash > 0.0f) {
            errorFlash -= GetFrameTime();
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade((Color){255,40,40,120}, errorFlash*0.8f));
        }

        if (gameLost) {
            if (lostTimer > 0.0f) lostTimer -= GetFrameTime();
            DrawText("Voce perdeu a sua consciencia!", screenWidth/2 - MeasureText("Voce perdeu a sua consciencia!", 40)/2, 100, 40, RED);
            DrawText("Voltando ao jogo principal...", screenWidth/2 - MeasureText("Voltando ao jogo principal...", 20)/2, 150, 20, DARKGRAY);
            if (lostTimer <= 0.0f) {
                liberarMemoria(&cardList);
                UnloadTexture(cardBack);
                for (int i = 0; i < frontCount; i++) UnloadTexture(cardFronts[i]);
                return 0;
            }
        } else if (gameWon) {
            DrawText("PARABENS! Voce restaurou suas memorias!", screenWidth/2 - MeasureText("PARABENS! Voce restaurou suas memorias!", 32)/2, 100, 32, GREEN);
            DrawText("A CORTEX foi derrotada! Pressione ENTER para retornar", screenWidth/2 - MeasureText("A CORTEX foi derrotada! Pressione ENTER para retornar", 20)/2, 140, 20, DARKBLUE);
            if (IsKeyPressed(KEY_ENTER)) {
                liberarMemoria(&cardList);
                UnloadTexture(cardBack);
                for (int i = 0; i < frontCount; i++) UnloadTexture(cardFronts[i]);
                return 1;
            }
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            liberarMemoria(&cardList);
            UnloadTexture(cardBack);
            for (int i = 0; i < frontCount; i++) UnloadTexture(cardFronts[i]);
            return 0;
        }

        EndDrawing();
    }

    liberarMemoria(&cardList);
    UnloadTexture(cardBack);
    for (int i = 0; i < frontCount; i++) UnloadTexture(cardFronts[i]);
    return 0;
}