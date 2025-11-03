#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct {
    float x;
    float y;
    int facingRight; 
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

    if (player->x < 0) player->x = 0;
    if (player->x > GetScreenWidth() - 64) player->x = GetScreenWidth() - 64; 
}

int main() {
    const int screenWidth = 1368;   
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "Cenario com Boneco");

    Texture2D spriteLeft = LoadTexture("./assets/characterLeft.png");
    Texture2D spriteRight = LoadTexture("./assets/characterRight.png");

    Player player = { 
        screenWidth / 2.0f, screenHeight - spriteLeft.height - 30, 1 
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        moveCharacter(&player, 8.0f);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (player.facingRight)
            DrawTexture(spriteRight, player.x, player.y, WHITE);
        else
            DrawTexture(spriteLeft, player.x, player.y, WHITE);

        DrawText("Use A e D para andar", 30, 30, 25, BLACK);

        EndDrawing();
    }

    UnloadTexture(spriteLeft);
    UnloadTexture(spriteRight);
    CloseWindow();

    return 0;
}
