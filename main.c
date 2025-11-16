#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "memory.h" // <<< ADICIONADO: Necessário para declarar RunMemoryGame

typedef struct {
    float x;
    float y;
    float velY;
    int facingRight;
    int agachado;
    int isJumping;
} Player;

// --- Função de Pulo (Intocada) ---
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
    Texture2D menuBg = LoadTexture("./assets/menu_bg.png");
    
    // --- Texturas de Portas (Intocadas) ---
    Texture2D doorClosed = LoadTexture("./assets/door/door_opened.png"); 
    Texture2D doorOpened = LoadTexture("./assets/door/door_closed.png");

    Texture2D spriteLeft = LoadTexture("./assets/characterLeft.png");
    Texture2D spriteRight = LoadTexture("./assets/characterRight.png");
    Texture2D spriteJumpLeft = LoadTexture("./assets/characterJumpLeft.png");
    Texture2D spriteJumpRight = LoadTexture("./assets/characterJumpRight.png");
    Texture2D spriteAgachandoLeft = LoadTexture("./assets/characterAgachandoLeft.png");
    Texture2D spriteAgachandoRight = LoadTexture("./assets/characterAgachandoRight.png");

     /*Usa GetScreenHeight() para a altura real (Seu valor original) */
     float groundY = GetScreenHeight() - spriteLeft.height + 200; 
     float speed = 5.0f; // velocidade horizontal

    // Mude o valor do OFFSET para ajustar a distancia da colisão da borda para o centro (Seu valor original)
    const float COLLISION_OFFSET = -330.0f; 

    
    Player player = {
        GetScreenWidth() / 2.0f,    // Usa GetScreenWidth() para centralizar
        groundY,                    // Posição no chão
        0.0f,                       // Velocidade vertical
        1,                          // Virado pra direita
        0,                          // Não agachado
        0                           // Não pulando
    };

    SetTargetFPS(60);

    // --- POSICIONAMENTO DAS PORTAS (Intocado) ---
    float doorYOffset = 430.0f; 
    float doorSpacing = doorClosed.width * 0.5f;
    float doorWidth = (float)doorClosed.width; // Largura real da porta recortada
    float doorHeight = (float)doorClosed.height; // Altura real

    // Lógica correta para centralizar as 3 portas na tela
    float totalDoorsWidth = (doorWidth * 3) + (doorSpacing * 2); // 3 portas, 2 espaços
    float startX = (GetScreenWidth() / 2.0f) - (totalDoorsWidth / 2.0f); 

    Vector2 doorPositions[3];
    for (int i = 0; i < 3; i++) {
        doorPositions[i].x = startX + (i * (doorWidth + doorSpacing));
        // A posição Y é: O chão (groundY) - a altura da porta + o seu offset de ajuste fino
        doorPositions[i].y = groundY - doorHeight + doorYOffset; 
    }
    
    
    //Estado de cada porta (0 = fechada, 1 = aberta) e nível desbloqueado
    int doorStates[3] = {0, 0, 0}; 
    int levelUnlocked = 1;
    int fragmentsCollected = 0; // Contador de fragmentos de memória coletados 
    
    //A primeira porta deve aparecer aberta por padrão, se for a fase inicial
    if (levelUnlocked >= 1) { 
        doorStates[0] = 1; // A primeira porta começa aberta
    }
    
    // Variável de estado de jogo
    int gameState = 0; // 0 = Hub

    /* ==========================
        MENU INICIAL (Intocado)
       ========================== */
    int menuActive = 1;
    int difficulty = 2; // Fixado em Normal
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
        
        // Calcula altura total do menu para centralizar verticalmente
        int totalMenuHeight = 60 + 120 + 30 + 80 + 30 + 50 + 30; // título + espaços + opções
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
            menuActive = 0;
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
                DrawText("Instruções", GetScreenWidth()/2 - MeasureText("Instruções", 40)/2, 140, 40, SKYBLUE);
                DrawText("Use A/D ou setas para mover", 120, 220, 24, LIGHTGRAY);
                DrawText("W, UP ou SPACE para pular", 120, 260, 24, LIGHTGRAY);
                DrawText("C ou S para agachar", 120, 300, 24, LIGHTGRAY);
                DrawText("Perto da porta, pressione E para entrar no tabuleiro", 120, 340, 22, LIGHTGRAY); // Texto alterado para KEY_E
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
    // --- Fim do Menu ---


    while (!WindowShouldClose()) {
        
        // --- Lógica de Estado de Jogo ---
        if (gameState == 0) { // Estamos no Hub (Cenário)
        
            // ---- MOVIMENTO HORIZONTAL ----
            if (IsKeyDown(KEY_D)) {
                player.x += speed;
                player.facingRight = 1;
            }
            if (IsKeyDown(KEY_A)) {
                player.x -= speed;
                player.facingRight = 0;
            }

            // ---- Lógica de colisão, pulo, agachar (Intocada) ----
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
            } else if (player.agachado) {
                if (player.facingRight) {
                    currentPlayerWidth = spriteAgachandoRight.width;
                    currentPlayerHeight = spriteAgachandoRight.height;
                } else {
                    currentPlayerWidth = spriteAgachandoLeft.width;
                    currentPlayerHeight = spriteAgachandoLeft.height;
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
            
            // Lógica de Interação com as Portas
            if (IsKeyPressed(KEY_E)) {
                
                float hitboxWidth = currentPlayerWidth / 3.0f;
                float hitboxX = player.x + (currentPlayerWidth - hitboxWidth) / 2.0f;
                Rectangle playerRect = { hitboxX, player.y, hitboxWidth, currentPlayerHeight };
                
                float doorHitboxWidth = doorWidth * 0.5f;
                
                // Verifica interação com Porta 1
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
                        int gameResult = RunMemoryGame(difficulty - 1); 

                        if (gameResult == 1) {
                            doorStates[0] = 0;
                            fragmentsCollected++;
                            
                            // Mostra mensagem de fragmento coletado
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
                            printf("Porta 1 resolvida! Próxima porta desbloqueada.\n");
                        } else {
                            printf("Jogo da Memória Encerrado. Voltando ao Hub.\n");
                        }
                    }
                }
                
                // Verifica interação com Porta 2
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
                        int gameResult = RunMemoryGame(difficulty - 1); 

                        if (gameResult == 1) {
                            doorStates[1] = 0;
                            fragmentsCollected++;
                            
                            // Mostra mensagem de fragmento coletado
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
                            printf("Porta 2 resolvida! Próxima porta desbloqueada.\n");
                        } else {
                            printf("Jogo da Memória Encerrado. Voltando ao Hub.\n");
                        }
                    }
                }
                
                // Verifica interação com Porta 3
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
                        int gameResult = RunMemoryGame(difficulty - 1); 

                        if (gameResult == 1) {
                            doorStates[2] = 0;
                            fragmentsCollected++;
                            
                            // Mostra mensagem de fragmento final coletado
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
                            
                            // Popup final especial quando completar todos os fragmentos
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
                                        // Aqui você pode adicionar a lógica para mostrar a memória final
                                        printf("Final do jogo! Memória mais marcante sendo revelada...\n");
                                    }
                                }
                            }
                            
                            printf("Porta 3 resolvida! Jogo completado!\n");
                        } else {
                            printf("Jogo da Memória Encerrado. Voltando ao Hub.\n");
                        }
                    }
                }
            }
            // Fim da Lógica de Interação

            // Aplica a colisão
            if (player.x < COLLISION_OFFSET) {
                player.x = COLLISION_OFFSET; 
            }

            if (player.x + currentPlayerWidth > GetScreenWidth() - COLLISION_OFFSET) { 
                player.x = GetScreenWidth() - currentPlayerWidth - COLLISION_OFFSET; 
            }

            // AGACHAR
            if (IsKeyDown(KEY_C))
                player.agachado = 1;
            else
                player.agachado = 0;

            // PULO
            updateJump(&player, groundY);

        } 
        // <<< O BLOCO 'else if (gameState == 1)' FOI REMOVIDO >>>

        // ---- DESENHO ----
        BeginDrawing();
        ClearBackground(BLACK);

        // --- Desenho baseado no Estado de Jogo ---
        // O minigame desenha a si mesmo. Este bloco só precisa do gameState == 0
        if (gameState == 0) { // Desenha o Hub
        
            /* Desenha o background do JOGO */
            {
                Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
                Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }; 
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
            }

            //desenha as portas
            for (int i = 0; i < 3; i++) {
                if (doorStates[i] == 1) { // Porta aberta (disponível)
                    DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                } else { // Porta fechada (trancada ou completada)
                    DrawTexture(doorClosed, doorPositions[i].x, doorPositions[i].y, WHITE);
                }
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
        } 
        // <<< O BLOCO 'else if (gameState == 1)' FOI REMOVIDO DAQUI TAMBÉM >>>

        EndDrawing();
    } // Fim do while principal

    // ---- LIMPEZA (Intocada) ----
    UnloadTexture(bg);
    UnloadTexture(menuBg);
    UnloadTexture(spriteLeft);
    UnloadTexture(spriteRight);
    UnloadTexture(spriteJumpLeft);
    UnloadTexture(spriteJumpRight);
    UnloadTexture(spriteAgachandoLeft);
    UnloadTexture(spriteAgachandoRight);
    UnloadTexture(doorClosed);
    UnloadTexture(doorOpened);
    CloseWindow();

    return 0;
}