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
     const int screenWidth = 1920;
     const int screenHeight = 1080;

     /* Solicita fullscreen antes de criar a janela e garante que o jogo seja
         iniciado em fullscreen na resolução desejada. */
     SetConfigFlags(FLAG_FULLSCREEN_MODE);
     InitWindow(screenWidth, screenHeight, "Cenario com pulo e fundo rolando");
     /* Força o toggle para fullscreen (alguns backends precisam do toggle) */
     ToggleFullscreen();

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
        /* Desenha o background escalado para preencher 100% da janela/monitor
           mesmo em fullscreen. Usa DrawTexturePro com src/dst rectangles. */
        {
            Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
            Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
        }

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
