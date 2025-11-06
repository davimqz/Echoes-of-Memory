#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

typedef struct {
    int cardType;
    int isFlipped;
    int isMatched;
    Rectangle rect;
} Card;

int main(void) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Jogo da Memoria - Simple");
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

    // Configura cartas (4x4 -> 16 cartas -> 8 pares)
    Card cards[16];
    int types[16];
    // Seleciona 8 pares a partir das imagens disponíveis sem repetir um mesmo par
    int requiredPairs = 8;
    // build list of available indices
    int available[7];
    for (int i = 0; i < frontCount; i++) available[i] = i;
    // shuffle available
    for (int i = frontCount - 1; i > 0; i--) { int j = rand() % (i + 1); int t = available[i]; available[i] = available[j]; available[j] = t; }
    int selectedCount = 0;
    // first take unique types
    for (int i = 0; i < frontCount && selectedCount < requiredPairs; i++) {
        types[selectedCount*2] = available[i];
        types[selectedCount*2 + 1] = available[i];
        selectedCount++;
    }
    // if still need more pairs (happens when frontCount < requiredPairs), fill by reusing available types
    while (selectedCount < requiredPairs) {
        int pick = rand() % frontCount;
        types[selectedCount*2] = available[pick];
        types[selectedCount*2 + 1] = available[pick];
        selectedCount++;
    }
    // agora embaralha as 16 posições (cada par ocupa duas posições)
    for (int i = 15; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = types[i]; types[i] = types[j]; types[j] = tmp;
    }

    float cardW = 120.0f;
    float cardH = 160.0f;
    float pad = 20.0f;
    float startX = (screenWidth - (4 * cardW + 3 * pad)) / 2.0f;
    float startY = (screenHeight - (4 * cardH + 3 * pad)) / 2.0f;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int idx = r * 4 + c;
            cards[idx].cardType = types[idx];
            cards[idx].isFlipped = 0;
            cards[idx].isMatched = 0;
            cards[idx].rect.x = startX + c * (cardW + pad);
            cards[idx].rect.y = startY + r * (cardH + pad);
            cards[idx].rect.width = cardW;
            cards[idx].rect.height = cardH;
        }
    }

    int first = -1, second = -1;
    float flipTimer = 0.0f;
    int matchedPairs = 0;
    int gameWon = 0;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (flipTimer > 0) {
            flipTimer -= GetFrameTime();
            if (flipTimer <= 0 && first != -1 && second != -1) {
                        if (cards[first].cardType != cards[second].cardType) {
                    cards[first].isFlipped = 0;
                    cards[second].isFlipped = 0;
                } else {
                    cards[first].isMatched = 1;
                    cards[second].isMatched = 1;
                    matchedPairs++;
                    if (matchedPairs >= 8) gameWon = 1;
                }
                first = -1;
                second = -1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && flipTimer <= 0) {
            for (int i = 0; i < 16; i++) {
                if (CheckCollisionPointRec(mouse, cards[i].rect)) {
                    if (!cards[i].isFlipped && !cards[i].isMatched) {
                        cards[i].isFlipped = 1;
                        if (first == -1) first = i;
                        else if (second == -1 && i != first) {
                            second = i;
                            // define tempo para ver as cartas
                            flipTimer = 0.9f;
                        }
                    }
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

    DrawText("Jogo da Memoria (4x4)", 20, 20, 24, DARKBLUE);
        DrawText("Clique nas cartas para virar. ESC = Sair, ENTER = Reiniciar.", 20, 50, 16, DARKGRAY);

        for (int i = 0; i < 16; i++) {
            Rectangle dst = cards[i].rect;
            if (cards[i].isMatched) {
                // carta encontrada - desenha a frente esmaecida (ou placeholder se faltar imagem)
                int t = cards[i].cardType;
                if (frontMissing[t]) {
                    DrawRectangleRec(dst, Fade(LIGHTGRAY, 0.6f));
                    DrawText(TextFormat("%s", frontNames[t]), (int)(dst.x + dst.width/2 - MeasureText(frontNames[t], 14)/2), (int)(dst.y + dst.height/2 - 7), 14, DARKBLUE);
                } else {
                    DrawTexturePro(cardFronts[t], (Rectangle){0,0, (float)cardFronts[t].width, (float)cardFronts[t].height}, dst, (Vector2){0,0}, 0.0f, Fade(WHITE, 0.6f));
                }
            } else if (cards[i].isFlipped) {
                int t = cards[i].cardType;
                if (frontMissing[t]) {
                    DrawRectangleRec(dst, LIGHTGRAY);
                    DrawText(TextFormat("%s", frontNames[t]), (int)(dst.x + dst.width/2 - MeasureText(frontNames[t], 14)/2), (int)(dst.y + dst.height/2 - 7), 14, DARKBLUE);
                } else {
                    DrawTexturePro(cardFronts[t], (Rectangle){0,0, (float)cardFronts[t].width, (float)cardFronts[t].height}, dst, (Vector2){0,0}, 0.0f, WHITE);
                }
            } else {
                DrawTexturePro(cardBack, (Rectangle){0,0, (float)cardBack.width, (float)cardBack.height}, dst, (Vector2){0,0}, 0.0f, WHITE);
            }
            DrawRectangleLinesEx(dst, 2, BLACK);
        }

        if (gameWon) {
            DrawText("PARABENS! Voce ganhou!", screenWidth/2 - MeasureText("PARABENS! Voce ganhou!", 32)/2, 100, 32, GREEN);
            DrawText("Pressione ENTER para reiniciar", screenWidth/2 - MeasureText("Pressione ENTER para reiniciar", 20)/2, 140, 20, DARKBLUE);
            if (IsKeyPressed(KEY_ENTER)) {
                // reinicia o jogo: escolhe 8 pares distintos quando possível e embaralha
                int requiredPairs = 8;
                int available[7]; for (int i = 0; i < frontCount; i++) available[i] = i;
                for (int i = frontCount - 1; i > 0; i--) { int j = rand() % (i + 1); int t = available[i]; available[i] = available[j]; available[j] = t; }
                int sel = 0;
                for (int i = 0; i < frontCount && sel < requiredPairs; i++) {
                    types[sel*2] = available[i]; types[sel*2+1] = available[i]; sel++;
                }
                while (sel < requiredPairs) { int pick = rand() % frontCount; types[sel*2] = available[pick]; types[sel*2+1] = available[pick]; sel++; }
                for (int i = 15; i > 0; i--) { int j = rand() % (i + 1); int t = types[i]; types[i] = types[j]; types[j] = t; }
                for (int i = 0; i < 16; i++) { cards[i].cardType = types[i]; cards[i].isFlipped = 0; cards[i].isMatched = 0; }
                first = second = -1; flipTimer = 0; matchedPairs = 0; gameWon = 0;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) break;

        EndDrawing();
    }

    // cleanup
    UnloadTexture(cardBack);
    for (int i = 0; i < 7; i++) UnloadTexture(cardFronts[i]);
    CloseWindow();
    return 0;
}
