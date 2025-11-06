#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "include/cards_list.h"

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
    inicializarCartas(&cardList, 4); // 4x4 board

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
    int difficulty = 0; // 0 = Normal, 1 = Difícil

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
                inicializarCartas(&cardList, 4); // cria nova lista 4x4
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
            int gapOpt = 30;
            int totalH = titleFont + gapTitle + optFont + gapOpt + optFont;
            int y0 = screenHeight/2 - totalH/2;
            int y = y0;
            DrawText("Escolha a dificuldade", cx - MeasureText("Escolha a dificuldade", titleFont)/2, y, titleFont, DARKBLUE);
            y += titleFont + gapTitle;
            DrawText("1 - Normal", cx - MeasureText("1 - Normal", optFont)/2, y, optFont, BLACK);
            y += optFont + gapOpt;
            DrawText("2 - Difícil", cx - MeasureText("2 - Difícil", optFont)/2, y, optFont, BLACK);
            DrawText("Pressione 1 ou 2 para escolher, ESC para voltar", cx - MeasureText("Pressione 1 ou 2 para escolher, ESC para voltar", 20)/2, screenHeight - 40, 20, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) { difficulty = 0; state = STATE_MENU; }
            if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) { difficulty = 1; state = STATE_MENU; }
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
                        if (first == NULL) {
                            first = selectedCard;
                            firstIndex = index;
                        } else if (second == NULL && selectedCard != first) {
                            second = selectedCard;
                            secondIndex = index;
                            // define tempo para ver as cartas dependendo da dificuldade
                            flipTimer = (difficulty == 0) ? 0.9f : 0.6f;
                        }
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
                inicializarCartas(&cardList, 4);
                first = second = NULL; firstIndex = secondIndex = -1; 
                flipTimer = 0; matchedPairs = 0; gameWon = 0;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) break;

        EndDrawing();
    }

    // cleanup
    liberarMemoria(&cardList);
    UnloadTexture(cardBack);
    for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
    CloseWindow();
    return 0;
}
