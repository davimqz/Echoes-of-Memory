#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>
#include "memory.h"

typedef struct {
    float x;
    float y;
    float velY;
    int facingRight;
    int isJumping;
} Player;

void updateJump(Player *player, float groundY) {
    const float gravity = 0.6f;
    const float jumpForce = -12.0f;

    if (IsKeyPressed(KEY_SPACE) && !player->isJumping) {
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
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(screenWidth, screenHeight, "Cenario com pulo e fundo rolando");
    ToggleFullscreen();

    Texture2D bg = LoadTexture("./assets/bg.png");
    Texture2D menuBg = LoadTexture("./assets/menu_bg.png");
    Texture2D memoriaTexture = LoadTexture("./assets/memoria.png");
    
    Texture2D doorClosed = LoadTexture("./assets/door/door_opened.png"); 
    Texture2D doorOpened = LoadTexture("./assets/door/door_closed.png");

    Texture2D spriteLeft = LoadTexture("./assets/characterLeft.png");
    Texture2D spriteRight = LoadTexture("./assets/characterRight.png");
    Texture2D spriteJumpLeft = LoadTexture("./assets/characterJumpLeft.png");
    Texture2D spriteJumpRight = LoadTexture("./assets/characterJumpRight.png");

     float groundY = GetScreenHeight() - spriteLeft.height + 200; 
     float speed = 5.0f;

    const float COLLISION_OFFSET = -330.0f; 

    Player player = {
        GetScreenWidth() / 2.0f,
        groundY,
        0.0f,
        1,
        0
    };

    SetTargetFPS(60);

    float doorYOffset = 430.0f; 
    float doorSpacing = doorClosed.width * 0.5f;
    float doorWidth = (float)doorClosed.width;
    float doorHeight = (float)doorClosed.height;

    float totalDoorsWidth = (doorWidth * 3) + (doorSpacing * 2);
    float startX = (GetScreenWidth() / 2.0f) - (totalDoorsWidth / 2.0f); 

    Vector2 doorPositions[3];
    for (int i = 0; i < 3; i++) {
        doorPositions[i].x = startX + (i * (doorWidth + doorSpacing));
        doorPositions[i].y = groundY - doorHeight + doorYOffset; 
    }
    
    int doorStates[3] = {0, 0, 0}; 
    int levelUnlocked = 1;
    int fragmentsCollected = 0;
    
    if (levelUnlocked >= 1) {
        doorStates[0] = 1;
    }
    
    int gameState = 0;

    int menuActive = 1;
    int difficulty = 2;
    while (menuActive && !WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground(BLACK);
        {
            Rectangle srcBg = { 0.0f, 0.0f, (float)menuBg.width, (float)menuBg.height };
            Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(menuBg, srcBg, dstBg, origin, 0.0f, WHITE);
        }
        int centerX = GetScreenWidth() / 2;
        int centerY = GetScreenHeight() / 2;
        
        int totalMenuHeight = 60 + 120 + 30 + 80 + 30 + 50 + 30;
        int startY = centerY - totalMenuHeight / 2;
        
        int y = startY;
        DrawText("ECHOES OF MEMORY", centerX - MeasureText("ECHOES OF MEMORY", 60) / 2, y, 60, SKYBLUE);
        y += 120;
        DrawText("1 - Começar jogo", centerX - MeasureText("1 - Começar jogo", 30) / 2, y, 30, WHITE);
        y += 80;
        DrawText("2 - Instruções do jogo", centerX - MeasureText("2 - Instruções do jogo", 30) / 2, y, 30, LIGHTGRAY);
        y += 50;
        DrawText("3 - Sair", centerX - MeasureText("3 - Sair", 30) / 2, y, 30, LIGHTGRAY);
        DrawText("Pressione o numero correspondente para escolher", centerX - MeasureText("Pressione o numero correspondente para escolher", 20) / 2, GetScreenHeight() - 80, 20, GRAY);
        EndDrawing();
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            int storyActive = 1;
            float storyTimer = 0.0f;
            int currentLine = 0;
            
            const char* storyLines[] = {
                "No ano 2157, a humanidade perdeu sua capacidade natural de lembrar.",
                "",
                "",
                "Após séculos de dependência das inteligências artificiais,",
                "toda a memória humana é controlada pela superinteligência CORTEX.",
                "",
                "",
                "Você é um dos últimos humanos com fragmentos de memória própria.",
                "",
                "",
                "Preso dentro do sistema da CORTEX, deve reconectar lembranças",
                "esquecidas da infância — representadas por pares de cartas",
                "nostálgicas como piões, pipas, tazos e videogames antigos.",
                "",
                "Pressione ENTER para começar sua jornada..."
            };
            const int totalLines = 15;
            const float lineDelay = 1.5f;
            
            while (storyActive && !WindowShouldClose()) {
                storyTimer += GetFrameTime();
                
                if (storyTimer >= lineDelay && currentLine < totalLines) {
                    currentLine++;
                    storyTimer = 0.0f;
                }
                
                BeginDrawing();
                ClearBackground(BLACK);
                {
                    Rectangle srcBg = { 0.0f, 0.0f, (float)menuBg.width, (float)menuBg.height };
                    Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
                    Vector2 origin = { 0.0f, 0.0f };
                    DrawTexturePro(menuBg, srcBg, dstBg, origin, 0.0f, Fade(WHITE, 0.3f));
                }
                
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
                
                int centerX = GetScreenWidth() / 2;
                int startY = 280;
                
                DrawText("ECHOES OF MEMORY", centerX - MeasureText("ECHOES OF MEMORY", 60) / 2, 100, 60, (Color){80,220,255,255});
                DrawText("____________________", centerX - MeasureText("____________________", 40) / 2, 170, 40, (Color){80,220,255,100});
                
                for (int i = 0; i <= currentLine && i < totalLines; i++) {
                    if (strlen(storyLines[i]) > 0) {
                        float alpha = 1.0f;
                        if (i == currentLine) {
                            alpha = storyTimer / 0.5f;
                            if (alpha > 1.0f) alpha = 1.0f;
                        }
                        
                        Color textColor = Fade(RAYWHITE, alpha);
                        int fontSize = 24;
                        int textWidth = MeasureText(storyLines[i], fontSize);
                        DrawText(storyLines[i], centerX - textWidth / 2, startY + i * 30, fontSize, textColor);
                    }
                }
                
                if (currentLine >= totalLines - 1) {
                    float blinkAlpha = (sin(GetTime() * 3.0f) + 1.0f) / 2.0f;
                    DrawText("Pressione ENTER para continuar...", centerX - MeasureText("Pressione ENTER para continuar...", 20) / 2, 
                             GetScreenHeight() - 80, 20, Fade(YELLOW, blinkAlpha));
                             
                    if (IsKeyPressed(KEY_ENTER)) {
                        storyActive = 0;
                        menuActive = 0;
                    }
                } else {
                    DrawText("Pressione SPACE para pular...", centerX - MeasureText("Pressione SPACE para pular...", 16) / 2, 
                             GetScreenHeight() - 50, 16, Fade(LIGHTGRAY, 0.7f));
                             
                    if (IsKeyPressed(KEY_SPACE)) {
                        storyActive = 0;
                        menuActive = 0;
                    }
                }
                
                EndDrawing();
            }
        } else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            int showing = 1;
            while (showing && !WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(BLACK);
                {
                    Rectangle srcBg = { 0.0f, 0.0f, (float)menuBg.width, (float)menuBg.height };
                    Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
                    Vector2 origin = { 0.0f, 0.0f };
                    DrawTexturePro(menuBg, srcBg, dstBg, origin, 0.0f, WHITE);
                }
                DrawText("Instrucoes", GetScreenWidth()/2 - MeasureText("Instrucoes", 40)/2, 140, 40, SKYBLUE);
                DrawText("EXPLORACAO:", 120, 200, 28, YELLOW);
                DrawText("- Use A/D ou setas para mover", 120, 240, 24, LIGHTGRAY);
                DrawText("- W, UP ou SPACE para pular", 120, 280, 24, LIGHTGRAY);
                DrawText("- Perto da porta, pressione SPACE para entrar", 120, 320, 22, LIGHTGRAY);
                
                DrawText("JOGO DA MEMORIA:", 120, 380, 28, YELLOW);
                DrawText("- Clique nas cartas para revela-las", 120, 420, 24, LIGHTGRAY);
                DrawText("- Encontre pares de simbolos identicos", 120, 460, 24, LIGHTGRAY);
                DrawText("- Pressione H para DICAS (maximo 3 por jogo)", 120, 500, 24, SKYBLUE);
                DrawText("- ESC para voltar a exploracao", 120, 540, 24, LIGHTGRAY);
                
                DrawText("CUIDADO: A CORTEX interferira quando acertar muito!", 120, 600, 22, RED);
                DrawText("Pressione qualquer tecla para voltar", 120, GetScreenHeight() - 80, 22, GRAY);
                EndDrawing();
                if (GetKeyPressed() != 0) showing = 0;
            }
        } else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
            CloseWindow();
            return 0;
        }
        if (!menuActive) break;
        WaitTime(0.01f);
    }

    while (!WindowShouldClose()) {
        
        if (gameState == -1) {
            static int showingDescription = 0;
            
            BeginDrawing();
            ClearBackground(BLACK);
            
            if (!showingDescription) {
                int centerX = GetScreenWidth() / 2;
                int centerY = GetScreenHeight() / 2;
                
                DrawText("SUA MEMORIA MAIS MARCANTE", centerX - MeasureText("SUA MEMORIA MAIS MARCANTE", 40) / 2, 50, 40, (Color){100, 255, 200, 255});
                
                if (memoriaTexture.id != 0) {
                    Rectangle srcMemoria = { 0.0f, 0.0f, (float)memoriaTexture.width, (float)memoriaTexture.height };
                    
                    float scale = 1.0f;
                    if (memoriaTexture.width > 400 || memoriaTexture.height > 300) {
                        float scaleX = 400.0f / memoriaTexture.width;
                        float scaleY = 300.0f / memoriaTexture.height;
                        scale = (scaleX < scaleY) ? scaleX : scaleY;
                    }
                    
                    float newWidth = memoriaTexture.width * scale;
                    float newHeight = memoriaTexture.height * scale;
                    
                    Rectangle dstMemoria = { 
                        centerX - newWidth / 2, 
                        centerY - newHeight / 2, 
                        newWidth, 
                        newHeight 
                    };
                    Vector2 origin = { 0.0f, 0.0f };
                    
                    DrawTexturePro(memoriaTexture, srcMemoria, dstMemoria, origin, 0.0f, WHITE);
                }
                
                DrawText("Pressione qualquer tecla para ver a descrição...", centerX - MeasureText("Pressione qualquer tecla para ver a descrição...", 20) / 2, GetScreenHeight() - 100, 20, YELLOW);
                
                if (GetKeyPressed() != 0) {
                    showingDescription = 1;
                }
            } else {
                int centerX = GetScreenWidth() / 2;
                
                DrawText("LEMBRANCA RECUPERADA", centerX - MeasureText("LEMBRANCA RECUPERADA", 40) / 2, 100, 40, (Color){255, 215, 0, 255});
                
                DrawText("Você se lembra agora... Era verão, e você estava no quintal", centerX - MeasureText("Você se lembra agora... Era verão, e você estava no quintal", 24) / 2, 200, 24, WHITE);
                DrawText("da sua avó brincando com seus brinquedos favoritos.", centerX - MeasureText("da sua avó brincando com seus brinquedos favoritos.", 24) / 2, 240, 24, WHITE);
                DrawText("", centerX, 280, 24, WHITE);
                DrawText("O som das cigarras, o cheiro de terra molhada após a chuva,", centerX - MeasureText("O som das cigarras, o cheiro de terra molhada após a chuva,", 24) / 2, 320, 24, LIGHTGRAY);
                DrawText("e a sensação de que o tempo poderia parar para sempre.", centerX - MeasureText("e a sensação de que o tempo poderia parar para sempre.", 24) / 2, 360, 24, LIGHTGRAY);
                DrawText("", centerX, 400, 24, LIGHTGRAY);
                DrawText("Era a memória mais pura da sua infância.", centerX - MeasureText("Era a memória mais pura da sua infância.", 28) / 2, 440, 28, (Color){100, 255, 200, 255});
                DrawText("", centerX, 480, 24, WHITE);
                DrawText("Agora você se lembrou de quem realmente é.", centerX - MeasureText("Agora você se lembrou de quem realmente é.", 24) / 2, 520, 24, WHITE);
                
                DrawText("Pressione qualquer tecla para voltar ao menu principal...", centerX - MeasureText("Pressione qualquer tecla para voltar ao menu principal...", 20) / 2, GetScreenHeight() - 80, 20, YELLOW);
                
                if (GetKeyPressed() != 0) {
                    CloseWindow();
                    return 0;
                }
            }
            
            EndDrawing();
            
        } else if (gameState == 0) {
        
            if (IsKeyDown(KEY_D)) {
                player.x += speed;
                player.facingRight = 1;
            }
            if (IsKeyDown(KEY_A)) {
                player.x -= speed;
                player.facingRight = 0;
            }

            float currentPlayerWidth = 0;
            float currentPlayerHeight = 0; 
            
            if (player.isJumping) {
                if (player.facingRight) {
                    currentPlayerWidth = spriteJumpRight.width;
                    currentPlayerHeight = spriteJumpRight.height;
                } else {
                    currentPlayerWidth = spriteJumpLeft.width;
                    currentPlayerHeight = spriteJumpLeft.height;
                }
            } else {
                if (player.facingRight) {
                    currentPlayerWidth = spriteRight.width;
                    currentPlayerHeight = spriteRight.height;
                } else {
                    currentPlayerWidth = spriteLeft.width;
                    currentPlayerHeight = spriteLeft.height;
                }
            }
            
            if (IsKeyPressed(KEY_E)) {
                
                float hitboxWidth = currentPlayerWidth / 3.0f;
                float hitboxX = player.x + (currentPlayerWidth - hitboxWidth) / 2.0f;
                Rectangle playerRect = { hitboxX, player.y, hitboxWidth, currentPlayerHeight };
                
                float doorHitboxWidth = doorWidth * 0.5f;
                
                float doorHitboxX1 = doorPositions[0].x + (doorWidth - doorHitboxWidth) / 2.0f;
                Rectangle doorRect1 = { doorHitboxX1, doorPositions[0].y, doorHitboxWidth, doorHeight };

                if (CheckCollisionRecs(playerRect, doorRect1) && doorStates[0] == 1) {
                    int confirmed = 0;
                    int choosing = 1;
                    while (choosing && !WindowShouldClose()) {
                        BeginDrawing();
                        ClearBackground(BLACK);
                        
                        {
                            Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                            Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                            Vector2 origin = { 0.0f, 0.0f };
                            DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                        }

                        for (int i = 0; i < 3; i++) {
                            if (doorStates[i] == 1) {
                                DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                            } else {
                                DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                            }
                        }

                        if (player.facingRight)
                            DrawTexture(spriteRight, player.x, player.y, WHITE);
                        else
                            DrawTexture(spriteLeft, player.x, player.y, WHITE);

                        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
                        
                        int popupW = 600;
                        int popupH = 200;
                        int popupX = GetScreenWidth()/2 - popupW/2;
                        int popupY = GetScreenHeight()/2 - popupH/2;
                        
                        DrawRectangle(popupX, popupY, popupW, popupH, WHITE);
                        DrawRectangleLines(popupX, popupY, popupW, popupH, BLACK);
                        
                        DrawText("Voce interagiu com o primeiro nivel.", popupX + 20, popupY + 30, 24, BLACK);
                        DrawText("Voce quer realmente comecar?", popupX + 20, popupY + 60, 24, BLACK);
                        
                        DrawText("S - Sim", popupX + 100, popupY + 120, 20, DARKGREEN);
                        DrawText("N - Nao", popupX + 300, popupY + 120, 20, MAROON);
                        
                        EndDrawing();
                        
                        if (IsKeyPressed(KEY_S)) {
                            confirmed = 1;
                            choosing = 0;
                        }
                        if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                            confirmed = 0;
                            choosing = 0;
                        }
                    }
                    
                    if (confirmed) {
                        int gameResult = RunMemoryGame(0);

                        if (gameResult == 1) {
                            doorStates[0] = 0;
                            fragmentsCollected++;
                            
                            int showingFragment = 1;
                            while (showingFragment && !WindowShouldClose()) {
                                BeginDrawing();
                                ClearBackground(BLACK);
                                
                                {
                                    Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                                    Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                                    Vector2 origin = { 0.0f, 0.0f };
                                    DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                                }

                                for (int i = 0; i < 3; i++) {
                                    if (doorStates[i] == 1) {
                                        DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    } else {
                                        DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    }
                                }

                                if (player.facingRight)
                                    DrawTexture(spriteRight, player.x, player.y, WHITE);
                                else
                                    DrawTexture(spriteLeft, player.x, player.y, WHITE);

                                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                                
                                int popupW = 700;
                                int popupH = 250;
                                int popupX = GetScreenWidth()/2 - popupW/2;
                                int popupY = GetScreenHeight()/2 - popupH/2;
                                
                                DrawRectangle(popupX, popupY, popupW, popupH, (Color){20, 30, 50, 255});
                                DrawRectangleLines(popupX, popupY, popupW, popupH, (Color){80, 220, 255, 255});
                                
                                DrawText("FRAGMENTO DE MEMORIA COLETADO!", popupX + 20, popupY + 30, 28, (Color){80, 220, 255, 255});
                                DrawText("Nivel 1 - 1/3 Fragmentos Coletados", popupX + 20, popupY + 80, 24, WHITE);
                                DrawText("Voce restaurou parte de suas memorias...", popupX + 20, popupY + 120, 20, LIGHTGRAY);
                                
                                DrawText("Pressione ENTER para continuar", popupX + 20, popupY + 180, 18, YELLOW);
                                
                                EndDrawing();
                                
                                if (IsKeyPressed(KEY_ENTER)) {
                                    showingFragment = 0;
                                }
                            }
                            
                            if (levelUnlocked < 2) levelUnlocked = 2;
                            if (levelUnlocked >= 2) doorStates[1] = 1;
                        }
                    }
                }
                
                float doorHitboxX2 = doorPositions[1].x + (doorWidth - doorHitboxWidth) / 2.0f;
                Rectangle doorRect2 = { doorHitboxX2, doorPositions[1].y, doorHitboxWidth, doorHeight };

                if (CheckCollisionRecs(playerRect, doorRect2) && doorStates[1] == 1) {
                    int confirmed = 0;
                    int choosing = 1;
                    while (choosing && !WindowShouldClose()) {
                        BeginDrawing();
                        ClearBackground(BLACK);
                        
                        {
                            Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                            Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                            Vector2 origin = { 0.0f, 0.0f };
                            DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                        }

                        for (int i = 0; i < 3; i++) {
                            if (doorStates[i] == 1) {
                                DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                            } else {
                                DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                            }
                        }

                        if (player.facingRight)
                            DrawTexture(spriteRight, player.x, player.y, WHITE);
                        else
                            DrawTexture(spriteLeft, player.x, player.y, WHITE);

                        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
                        
                        int popupW = 600;
                        int popupH = 200;
                        int popupX = GetScreenWidth()/2 - popupW/2;
                        int popupY = GetScreenHeight()/2 - popupH/2;
                        
                        DrawRectangle(popupX, popupY, popupW, popupH, WHITE);
                        DrawRectangleLines(popupX, popupY, popupW, popupH, BLACK);
                        
                        DrawText("Voce interagiu com o segundo nivel.", popupX + 20, popupY + 30, 24, BLACK);
                        DrawText("Voce quer realmente comecar?", popupX + 20, popupY + 60, 24, BLACK);
                        
                        DrawText("S - Sim", popupX + 100, popupY + 120, 20, DARKGREEN);
                        DrawText("N - Nao", popupX + 300, popupY + 120, 20, MAROON);
                        
                        EndDrawing();
                        
                        if (IsKeyPressed(KEY_S)) {
                            confirmed = 1;
                            choosing = 0;
                        }
                        if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                            confirmed = 0;
                            choosing = 0;
                        }
                    }
                    
                    if (confirmed) {
                        int gameResult = RunMemoryGame(1);

                        if (gameResult == 1) {
                            doorStates[1] = 0;
                            fragmentsCollected++;
                            
                            int showingFragment = 1;
                            while (showingFragment && !WindowShouldClose()) {
                                BeginDrawing();
                                ClearBackground(BLACK);
                                
                                {
                                    Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                                    Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                                    Vector2 origin = { 0.0f, 0.0f };
                                    DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                                }

                                for (int i = 0; i < 3; i++) {
                                    if (doorStates[i] == 1) {
                                        DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    } else {
                                        DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    }
                                }

                                if (player.facingRight)
                                    DrawTexture(spriteRight, player.x, player.y, WHITE);
                                else
                                    DrawTexture(spriteLeft, player.x, player.y, WHITE);

                                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                                
                                int popupW = 700;
                                int popupH = 250;
                                int popupX = GetScreenWidth()/2 - popupW/2;
                                int popupY = GetScreenHeight()/2 - popupH/2;
                                
                                DrawRectangle(popupX, popupY, popupW, popupH, (Color){20, 30, 50, 255});
                                DrawRectangleLines(popupX, popupY, popupW, popupH, (Color){80, 220, 255, 255});
                                
                                DrawText("FRAGMENTO DE MEMORIA COLETADO!", popupX + 20, popupY + 30, 28, (Color){80, 220, 255, 255});
                                DrawText("Nivel 2 - 2/3 Fragmentos Coletados", popupX + 20, popupY + 80, 24, WHITE);
                                DrawText("Mais memorias estao voltando...", popupX + 20, popupY + 120, 20, LIGHTGRAY);
                                
                                DrawText("Pressione ENTER para continuar", popupX + 20, popupY + 180, 18, YELLOW);
                                
                                EndDrawing();
                                
                                if (IsKeyPressed(KEY_ENTER)) {
                                    showingFragment = 0;
                                }
                            }
                            
                            if (levelUnlocked < 3) levelUnlocked = 3;
                            if (levelUnlocked >= 3) doorStates[2] = 1;
                        }
                    }
                }
                
                float doorHitboxX3 = doorPositions[2].x + (doorWidth - doorHitboxWidth) / 2.0f;
                Rectangle doorRect3 = { doorHitboxX3, doorPositions[2].y, doorHitboxWidth, doorHeight };

                if (CheckCollisionRecs(playerRect, doorRect3) && doorStates[2] == 1) {
                    int confirmed = 0;
                    int choosing = 1;
                    while (choosing && !WindowShouldClose()) {
                        BeginDrawing();
                        ClearBackground(BLACK);
                        
                        {
                            Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                            Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                            Vector2 origin = { 0.0f, 0.0f };
                            DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                        }

                        for (int i = 0; i < 3; i++) {
                            if (doorStates[i] == 1) {
                                DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                            } else {
                                DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                            }
                        }

                        if (player.facingRight)
                            DrawTexture(spriteRight, player.x, player.y, WHITE);
                        else
                            DrawTexture(spriteLeft, player.x, player.y, WHITE);

                        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
                        
                        int popupW = 600;
                        int popupH = 200;
                        int popupX = GetScreenWidth()/2 - popupW/2;
                        int popupY = GetScreenHeight()/2 - popupH/2;
                        
                        DrawRectangle(popupX, popupY, popupW, popupH, WHITE);
                        DrawRectangleLines(popupX, popupY, popupW, popupH, BLACK);
                        
                        DrawText("Voce interagiu com o terceiro nivel.", popupX + 20, popupY + 30, 24, BLACK);
                        DrawText("Voce quer realmente comecar?", popupX + 20, popupY + 60, 24, BLACK);
                        
                        DrawText("S - Sim", popupX + 100, popupY + 120, 20, DARKGREEN);
                        DrawText("N - Nao", popupX + 300, popupY + 120, 20, MAROON);
                        
                        EndDrawing();
                        
                        if (IsKeyPressed(KEY_S)) {
                            confirmed = 1;
                            choosing = 0;
                        }
                        if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                            confirmed = 0;
                            choosing = 0;
                        }
                    }
                    
                    if (confirmed) {
                        int gameResult = RunMemoryGame(2);

                        if (gameResult == 1) {
                            doorStates[2] = 0;
                            fragmentsCollected++;
                            
                            int showingFragment = 1;
                            while (showingFragment && !WindowShouldClose()) {
                                BeginDrawing();
                                ClearBackground(BLACK);
                                
                                {
                                    Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                                    Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                                    Vector2 origin = { 0.0f, 0.0f };
                                    DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                                }

                                for (int i = 0; i < 3; i++) {
                                    if (doorStates[i] == 1) {
                                        DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    } else {
                                        DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    }
                                }

                                if (player.facingRight)
                                    DrawTexture(spriteRight, player.x, player.y, WHITE);
                                else
                                    DrawTexture(spriteLeft, player.x, player.y, WHITE);

                                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                                
                                int popupW = 700;
                                int popupH = 250;
                                int popupX = GetScreenWidth()/2 - popupW/2;
                                int popupY = GetScreenHeight()/2 - popupH/2;
                                
                                DrawRectangle(popupX, popupY, popupW, popupH, (Color){20, 30, 50, 255});
                                DrawRectangleLines(popupX, popupY, popupW, popupH, (Color){80, 220, 255, 255});
                                
                                DrawText("FRAGMENTO DE MEMORIA COLETADO!", popupX + 20, popupY + 30, 28, (Color){80, 220, 255, 255});
                                DrawText("Nivel 3 - 3/3 Fragmentos Coletados", popupX + 20, popupY + 80, 24, WHITE);
                                DrawText("Todas as suas memorias foram restauradas!", popupX + 20, popupY + 120, 20, LIGHTGRAY);
                                
                                DrawText("Pressione ENTER para continuar", popupX + 20, popupY + 180, 18, YELLOW);
                                
                                EndDrawing();
                                
                                if (IsKeyPressed(KEY_ENTER)) {
                                    showingFragment = 0;
                                }
                            }
                            
                            if (fragmentsCollected >= 3) {
                                int showingFinalPopup = 1;
                                while (showingFinalPopup && !WindowShouldClose()) {
                                    BeginDrawing();
                                    ClearBackground(BLACK);
                                    
                                    {
                                        Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                                        Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                                        Vector2 origin = { 0.0f, 0.0f };
                                        DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
                                    }

                                    for (int i = 0; i < 3; i++) {
                                        DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                                    }

                                    if (player.facingRight)
                                        DrawTexture(spriteRight, player.x, player.y, WHITE);
                                    else
                                        DrawTexture(spriteLeft, player.x, player.y, WHITE);

                                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.9f));
                                    
                                    int popupW = 800;
                                    int popupH = 350;
                                    int popupX = GetScreenWidth()/2 - popupW/2;
                                    int popupY = GetScreenHeight()/2 - popupH/2;
                                    
                                    DrawRectangle(popupX, popupY, popupW, popupH, (Color){10, 20, 40, 255});
                                    DrawRectangleLines(popupX, popupY, popupW, popupH, (Color){100, 255, 200, 255});
                                    DrawRectangleLines(popupX + 5, popupY + 5, popupW - 10, popupH - 10, (Color){80, 200, 160, 255});
                                    
                                    DrawText("PARABENS!", popupX + popupW/2 - MeasureText("PARABENS!", 40)/2, popupY + 30, 40, (Color){100, 255, 200, 255});
                                    DrawText("Voce coletou todos os fragmentos de memoria!", popupX + 30, popupY + 90, 24, WHITE);
                                    DrawText("3/3 Fragmentos Coletados", popupX + 30, popupY + 130, 28, (Color){255, 215, 0, 255});
                                    
                                    DrawText("Voce gostaria de lembrar da sua memoria", popupX + 30, popupY + 180, 22, LIGHTGRAY);
                                    DrawText("mais marcante da infancia?", popupX + 30, popupY + 210, 22, LIGHTGRAY);
                                    
                                    DrawText("ENTER - Sim, quero lembrar", popupX + 50, popupY + 270, 20, (Color){100, 255, 200, 255});
                                    
                                    EndDrawing();
                                    
                                    if (IsKeyPressed(KEY_ENTER)) {
                                        showingFinalPopup = 0;
                                        gameState = -1;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (player.x < COLLISION_OFFSET) {
                player.x = COLLISION_OFFSET; 
            }

            if (player.x + currentPlayerWidth > GetScreenWidth() - COLLISION_OFFSET) { 
                player.x = GetScreenWidth() - currentPlayerWidth - COLLISION_OFFSET; 
            }

            updateJump(&player, groundY);

        } 

        BeginDrawing();
        ClearBackground(BLACK);

        if (gameState == 0) {
        
            {
                Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
            }

            for (int i = 0; i < 3; i++) {
                if (doorStates[i] == 1) {
                    DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                } else {
                    DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                }
            }

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
        } 

        EndDrawing();
    }

    UnloadTexture(bg);
    UnloadTexture(menuBg);
    UnloadTexture(spriteLeft);
    UnloadTexture(spriteRight);
    UnloadTexture(spriteJumpLeft);
    UnloadTexture(spriteJumpRight);
    UnloadTexture(menuBg);
    UnloadTexture(memoriaTexture);
    UnloadTexture(memoriaTexture);
    UnloadTexture(doorOpened);
    CloseWindow();

    return 0;
}
