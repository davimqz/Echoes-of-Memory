

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
