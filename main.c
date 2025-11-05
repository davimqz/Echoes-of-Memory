#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct {
    float y;
    float velY;
    int facingRight;
    int agachado;
    int isJumping;
} Player;

void updateJump(Player *player, float groundY) {
    const float gravity = 0.6f;
    const float jumpForce = -12.0f;

    if (IsKeyPressed(KEY_SPACE) && !player->isJumping && !player->agachado) {
        player->isJumping = 1;
        player->velY = jumpForce;
    }

    if (player->isJumping) {
        player->y += player->velY;
        player->velY += gravity;

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

    Player player = {
        groundY,
        0.0f,
        1,
        0,
        0
    };

    float bgScroll = 0.0f;
    float bgSpeed = 6.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        if (IsKeyDown(KEY_D)) {
            bgScroll -= bgSpeed;
            player.facingRight = 1;
        }
        if (IsKeyDown(KEY_A)) {
            bgScroll += bgSpeed;
            player.facingRight = 0;
        }

        if (bgScroll <= -bg.width) bgScroll = 0;
        if (bgScroll >= bg.width) bgScroll = 0;

        if (IsKeyDown(KEY_C))
            player.agachado = 1;
        else
            player.agachado = 0;

        updateJump(&player, groundY);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(bg, bgScroll, 0, WHITE);
        DrawTexture(bg, bgScroll + bg.width, 0, WHITE);

        float playerX = screenWidth / 2.0f - spriteRight.width / 2.0f;

        if (player.isJumping) {
            if (player.facingRight)
                DrawTexture(spriteJumpRight, playerX, player.y, WHITE);
            else
                DrawTexture(spriteJumpLeft, playerX, player.y, WHITE);
        } else if (player.agachado) {
            if (player.facingRight)
                DrawTexture(spriteAgachandoRight, playerX, player.y, WHITE);
            else
                DrawTexture(spriteAgachandoLeft, playerX, player.y, WHITE);
        } else {
            if (player.facingRight)
                DrawTexture(spriteRight, playerX, player.y, WHITE);
            else
                DrawTexture(spriteLeft, playerX, player.y, WHITE);
        }

        EndDrawing();
    }

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
