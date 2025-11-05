#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct {
    float x;
    float y;
    float velY;
    int facingRight;
    int agachado;
    int isJumping;
} Player;

void updateJump(Player *player, float groundY) {
    const float gravity = 0.6f;
    const float jumpForce = -12.0f;

    // Inicia o pulo
    if (IsKeyPressed(KEY_SPACE) && !player->isJumping && !player->agachado) {
        player->isJumping = 1;
        player->velY = jumpForce;
    }

    // Atualiza posição no ar
    if (player->isJumping) {
        player->y += player->velY;
        player->velY += gravity;

        // Volta ao chão
        if (player->y >= groundY) {
            player->y = groundY;
            player->velY = 0;
            player->isJumping = 0;
        }
    }
}

int main() {
    const int screenWidth = 1368;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "Cenario com pulo e fundo rolando");

    Texture2D bg = LoadTexture("./assets/bg.png");
    Texture2D spriteLeft = LoadTexture("./assets/characterLeft.png");
    Texture2D spriteRight = LoadTexture("./assets/characterRight.png");
    Texture2D spriteJumpLeft = LoadTexture("./assets/characterJumpLeft.png");
    Texture2D spriteJumpRight = LoadTexture("./assets/characterJumpRight.png");
    Texture2D spriteAgachandoLeft = LoadTexture("./assets/characterAgachandoLeft.png");
    Texture2D spriteAgachandoRight = LoadTexture("./assets/characterAgachandoRight.png");

    float groundY = screenHeight - spriteLeft.height + 250;
    float speed = 5.0f; // velocidade horizontal

    // Corrigido: x, y, velY, facingRight, agachado, isJumping
    Player player = {
        screenWidth / 2.0f,  // Começa centralizado
        groundY,             // Posição no chão
        0.0f,                // Velocidade vertical
        1,                   // Virado pra direita
        0,                   // Não agachado
        0                    // Não pulando
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // ---- MOVIMENTO HORIZONTAL ----
        if (IsKeyDown(KEY_D)) {
            player.x += speed;
            player.facingRight = 1;
        }
        if (IsKeyDown(KEY_A)) {
            player.x -= speed;
            player.facingRight = 0;
        }

        // ---- AGACHAR ----
        if (IsKeyDown(KEY_C))
            player.agachado = 1;
        else
            player.agachado = 0;

        // ---- PULO ----
        updateJump(&player, groundY);

        // ---- DESENHO ----
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(bg, 0, 0, WHITE);

        // Escolhe sprite conforme estado
        if (player.isJumping) {
            if (player.facingRight)
                DrawTexture(spriteJumpRight, player.x, player.y, WHITE);
            else
                DrawTexture(spriteJumpLeft, player.x, player.y, WHITE);
        } else if (player.agachado) {
            if (player.facingRight)
                DrawTexture(spriteAgachandoRight, player.x, player.y, WHITE);
            else
                DrawTexture(spriteAgachandoLeft, player.x, player.y, WHITE);
        } else {
            if (player.facingRight)
                DrawTexture(spriteRight, player.x, player.y, WHITE);
            else
                DrawTexture(spriteLeft, player.x, player.y, WHITE);
        }

        EndDrawing();
    }

    // ---- LIMPEZA ----
    UnloadTexture(bg);
    UnloadTexture(spriteLeft);
    UnloadTexture(spriteRight);
    UnloadTexture(spriteJumpLeft);
    UnloadTexture(spriteJumpRight);
    UnloadTexture(spriteAgachandoLeft);
    UnloadTexture(spriteAgachandoRight);
    CloseWindow();

    return 0;
}
