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

    // Inicia o pulo (Seu pulo original com KEY_SPACE)
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
    Texture2D doorClosed = LoadTexture("./assets/door/door_opened.png"); 
    Texture2D doorOpened = LoadTexture("./assets/door/door_closed.png");
    Texture2D spriteLeft = LoadTexture("./assets/characterLeft.png");
    Texture2D spriteRight = LoadTexture("./assets/characterRight.png");
    Texture2D spriteJumpLeft = LoadTexture("./assets/characterJumpLeft.png");
    Texture2D spriteJumpRight = LoadTexture("./assets/characterJumpRight.png");
    Texture2D spriteAgachandoLeft = LoadTexture("./assets/characterAgachandoLeft.png");
    Texture2D spriteAgachandoRight = LoadTexture("./assets/characterAgachandoRight.png");

     /*Usa GetScreenHeight() para a altura real*/
     float groundY = GetScreenHeight() - spriteLeft.height + 200; 
     float speed = 5.0f; // velocidade horizontal

    // Mude o valor do OFFSET para ajustar a distancia da colisão da borda para o centro 
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

    // ajustes da porta
    float doorYOffset = 470.0f; 
    float doorSpacing = doorClosed.width * -0.5f; 
    float totalDoorsWidth = (doorClosed.width * 3) + (doorSpacing * 0); 
    float startX = GetScreenWidth() / 2.45f - totalDoorsWidth / 2.45f; 

    Vector2 doorPositions[3];
    for (int i = 0; i < 3; i++) {
        doorPositions[i].x = startX + (i * (doorClosed.width + doorSpacing));
        doorPositions[i].y = groundY - doorClosed.height + doorYOffset; 
    }
    
    //Estado de cada porta (0 = fechada, 1 = aberta) e nível desbloqueado
    int doorStates[3] = {0, 0, 0}; 
    int levelUnlocked = 1; 
    
    //A primeira porta deve aparecer aberta por padrão, se for a fase inicial
    if (levelUnlocked >= 1) { 
        doorStates[0] = 1; // A primeira porta começa aberta
    }
    //  Fim ajustes de porta 


    // Variável de estado de jogo ---
    // 0 = Hub (este cenário)
    // 1 = No Jogo (Tela Preta)
    int gameState = 0; 


    /* ==========================
        MENU INICIAL (obrigatório)
       ========================== */
    int menuActive = 1;
    int difficulty = 2; 
    while (menuActive && !WindowShouldClose()) {
        // ... (Seu código de Menu não foi alterado) ...
        BeginDrawing();
        ClearBackground(BLACK);
        {
            Rectangle srcBg = { 0.0f, 0.0f, (float)menuBg.width, (float)menuBg.height };
            Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(menuBg, srcBg, dstBg, origin, 0.0f, WHITE);
        }
        int centerX = GetScreenWidth() / 2;
        int y = 200;
        DrawText("ECHOES OF MEMORY", centerX - MeasureText("ECHOES OF MEMORY", 60) / 2, y, 60, SKYBLUE);
        y += 120;
        DrawText("1 - Começar jogo", centerX - MeasureText("1 - Começar jogo", 30) / 2, y, 30, WHITE);
        y += 50;
        DrawText("2 - Escolher a dificuldade", centerX - MeasureText("2 - Escolher a dificuldade", 30) / 2, y, 30, LIGHTGRAY);
        y += 50;
        DrawText("3 - Instruções do jogo", centerX - MeasureText("3 - Instruções do jogo", 30) / 2, y, 30, LIGHTGRAY);
        y += 50;
        DrawText("4 - Sair", centerX - MeasureText("4 - Sair", 30) / 2, y, 30, LIGHTGRAY);
        y += 80;
        DrawText(TextFormat("Dificuldade atual: %s", difficulty==1?"Fácil":(difficulty==2?"Normal":"Difícil")), centerX - 150, y, 20, YELLOW);
        DrawText("Pressione o numero correspondente para escolher", centerX - MeasureText("Pressione o numero correspondente para escolher", 20) / 2, GetScreenHeight() - 80, 20, GRAY);
        EndDrawing();
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            menuActive = 0;
        } else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            int choosing = 1;
            while (choosing && !WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(BLACK);
                {
                    Rectangle srcBg = { 0.0f, 0.0f, (float)menuBg.width, (float)menuBg.height };
                    Rectangle dstBg = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
                    Vector2 origin = { 0.0f, 0.0f };
                    DrawTexturePro(menuBg, srcBg, dstBg, origin, 0.0f, WHITE);
                }
                DrawText("Escolha a dificuldade", GetScreenWidth()/2 - MeasureText("Escolha a dificuldade", 40)/2, 180, 40, SKYBLUE);
                DrawText("1 - Fácil", GetScreenWidth()/2 - MeasureText("1 - Fácil", 30)/2, 260, 30, LIGHTGRAY);
                DrawText("2 - Normal", GetScreenWidth()/2 - MeasureText("2 - Normal", 30)/2, 310, 30, LIGHTGRAY);
                DrawText("3 - Difícil", GetScreenWidth()/2 - MeasureText("3 - Difícil", 30)/2, 360, 30, LIGHTGRAY);
                DrawText("(Pressione qualquer tecla para confirmar a seleção)", GetScreenWidth()/2 - MeasureText("(Pressione qualquer tecla para confirmar a seleção)", 20)/2, GetScreenHeight() - 80, 20, GRAY);
                EndDrawing();
                if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) { difficulty = 1; }
                if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) { difficulty = 2; }
                if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) { difficulty = 3; }
                if (GetKeyPressed() != 0) choosing = 0;
            }
        } else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
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
                DrawText("Perto da porta, pressione SPACE para entrar no tabuleiro", 120, 340, 22, LIGHTGRAY);
                DrawText("Pressione qualquer tecla para voltar", 120, GetScreenHeight() - 80, 22, GRAY);
                EndDrawing();
                if (GetKeyPressed() != 0) showing = 0;
            }
        } else if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) {
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

            // ---- COLISÃO COM BORDA ----
            float currentPlayerWidth = 0;
            float currentPlayerHeight = 0; 
            
            // 1. Descobre a largura e altura do sprite atual
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

            // --- Lógica de Interação com a Porta 1 ---
            
            // --- CORREÇÃO 1: Removido o KEY_SPACE da interação ---
            // (IsKeyPressed(KEY_SPACE) foi removido)
            if (IsKeyPressed(KEY_E)) {
                
                // --- CORREÇÃO 2: Hitbox do jogador diminuída ---
                // A caixa de colisão agora é 1/3 da largura do sprite e centrada.
                float hitboxWidth = currentPlayerWidth / 3.0f; 
                float hitboxX = player.x + (currentPlayerWidth - hitboxWidth) / 2.0f; // Centraliza

                // Define a caixa de colisão do jogador (menor e centrada)
                Rectangle playerRect = { hitboxX, player.y, hitboxWidth, currentPlayerHeight };
                
                // Define a caixa de colisão da Porta 1
                Rectangle doorRect = { doorPositions[0].x, doorPositions[0].y, (float)doorClosed.width, (float)doorClosed.height };

                // Verifica se o jogador está colidindo com a porta 1 E se ela está aberta
                if (CheckCollisionRecs(playerRect, doorRect) && doorStates[0] == 1) {
                    gameState = 1; // Muda o estado do jogo para "Tela Preta"
                }
            }
            // --- Fim da Lógica de Interação ---


            // 2. Aplica a colisão usando a largura correta e o OFFSET
            if (player.x < COLLISION_OFFSET) {
                player.x = COLLISION_OFFSET; 
            }

            if (player.x + currentPlayerWidth > GetScreenWidth() - COLLISION_OFFSET) { 
                player.x = GetScreenWidth() - currentPlayerWidth - COLLISION_OFFSET; 
            }

            // ---- AGACHAR ----
            if (IsKeyDown(KEY_C))
                player.agachado = 1;
            else
                player.agachado = 0;

            // ---- PULO ----
            // (Não precisa do 'isInteracting' porque KEY_SPACE não está mais na lógica de interação)
            updateJump(&player, groundY);

        } else if (gameState == 1) { // --- Lógica da Tela Preta ---
            
            // Verifica se o jogador quer sair da tela preta
            if (IsKeyPressed(KEY_ESCAPE)) {
                gameState = 0; // Volta para o Hub
            }
        }


        // ---- DESENHO ----
        BeginDrawing();
        ClearBackground(BLACK);

        // --- Desenho baseado no Estado de Jogo ---
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
                if (doorStates[i] == 1) { // Porta aberta
                    DrawTexture(doorOpened, doorPositions[i].x, doorPositions[i].y, WHITE);
                } else { // Porta fechada
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

        } else if (gameState == 1) { // --- Desenha a Tela Preta ---
            
            // A tela já está limpa (ClearBackground(BLACK))
            int centerX = GetScreenWidth() / 2;
            int centerY = GetScreenHeight() / 2;
            
            DrawText("VOCÊ ENTROU NA FASE 1", centerX - MeasureText("VOCÊ ENTROU NA FASE 1", 40) / 2, centerY - 40, 40, WHITE);
            DrawText("Pressione ESC para voltar", centerX - MeasureText("Pressione ESC para voltar", 20) / 2, centerY + 20, 20, LIGHTGRAY);
        }

        EndDrawing();
    } // Fim do while principal

    // ---- LIMPEZA ----
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