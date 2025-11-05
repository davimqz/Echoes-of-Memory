#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct {
    float x;
    float y;
    float velY;
    int facingRight;
    int isJumping;
} Player;

void moveCharacter(Player *player, float speed) {
    if (IsKeyDown(KEY_A)) {
        player->x -= speed;
        player->facingRight = 0;
    }
    if (IsKeyDown(KEY_D)) {
        player->x += speed;
        player->facingRight = 1;
    }

    // Limites horizontais
    if (player->x < 0) player->x = 0;
    if (player->x > GetScreenWidth() - 64) player->x = GetScreenWidth() - 64;
}

void updateJump(Player *player, float groundY) {
    const float gravity = 0.6f;
    const float jumpForce = -12.0f;

    // Iniciar pulo
    if (IsKeyPressed(KEY_SPACE) && !player->isJumping) {
        player->isJumping = 1;
        player->velY = jumpForce;
    }

    // Atualizar posição vertical se estiver pulando
    if (player->isJumping) {
        player->y += player->velY;
        player->velY += gravity;

        // Chegou ao chão
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

    InitWindow(screenWidth, screenHeight, "Cenario com pulo animado");

    // Sprites de movimento e pulo
    Texture2D spriteLeft = LoadTexture("./assets/characterLeft.png");
    Texture2D spriteRight = LoadTexture("./assets/characterRight.png");
    Texture2D spriteJumpLeft = LoadTexture("./assets/characterJumpLeft.png");
    Texture2D spriteJumpRight = LoadTexture("./assets/characterJumpRight.png");

    Texture2D bg = LoadTexture("./assets/cenario.png");

    float groundY = screenHeight - spriteLeft.height - 30;

    Player player = {
        screenWidth / 2.0f,
        groundY,
        0.0f,
        1,
        0
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        moveCharacter(&player, 8.0f);
        updateJump(&player, groundY);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(bg, 0, 0, WHITE);

        // Escolhe o sprite de acordo com o estado e direção
        if (player.isJumping) {
            if (player.facingRight)
                DrawTexture(spriteJumpRight, player.x, player.y, WHITE);
            else
                DrawTexture(spriteJumpLeft, player.x, player.y, WHITE);
        } else {
            if (player.facingRight)
                DrawTexture(spriteRight, player.x, player.y, WHITE);
            else
                DrawTexture(spriteLeft, player.x, player.y, WHITE);
        }

        DrawText("Use A/D para andar e ESPAÇO para pular", 30, 30, 25, BLACK);

        EndDrawing();
    }

    UnloadTexture(spriteLeft);
    UnloadTexture(spriteRight);
    UnloadTexture(spriteJumpLeft);
    UnloadTexture(spriteJumpRight);
    UnloadTexture(bg);
    CloseWindow();

    return 0;
}
