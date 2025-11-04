#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_SPEED 300
#define BALL_SPEED 200

typedef struct {
    Vector2 position;
    Vector2 size;
    float speed;
} Paddle;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
} Ball;

int main() {
    // Inicialização
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong - Raylib");
    SetTargetFPS(60);
    
    // Variáveis do jogo
    Paddle leftPaddle = { {20, SCREEN_HEIGHT/2 - 50}, {20, 100}, PADDLE_SPEED };
    Paddle rightPaddle = { {SCREEN_WIDTH - 40, SCREEN_HEIGHT/2 - 50}, {20, 100}, PADDLE_SPEED };
    Ball ball = { {SCREEN_WIDTH/2, SCREEN_HEIGHT/2}, {BALL_SPEED, BALL_SPEED}, 10 };
    
    int leftScore = 0;
    int rightScore = 0;
    
    // Loop principal do jogo
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Controles dos jogadores
        // Jogador 1 (esquerda) - teclas W e S
        if (IsKeyDown(KEY_W) && leftPaddle.position.y > 0) {
            leftPaddle.position.y -= leftPaddle.speed * deltaTime;
        }
        if (IsKeyDown(KEY_S) && leftPaddle.position.y < SCREEN_HEIGHT - leftPaddle.size.y) {
            leftPaddle.position.y += leftPaddle.speed * deltaTime;
        }
        
        // Jogador 2 (direita) - setas UP e DOWN
        if (IsKeyDown(KEY_UP) && rightPaddle.position.y > 0) {
            rightPaddle.position.y -= rightPaddle.speed * deltaTime;
        }
        if (IsKeyDown(KEY_DOWN) && rightPaddle.position.y < SCREEN_HEIGHT - rightPaddle.size.y) {
            rightPaddle.position.y += rightPaddle.speed * deltaTime;
        }
        
        // Movimento da bola
        ball.position.x += ball.velocity.x * deltaTime;
        ball.position.y += ball.velocity.y * deltaTime;
        
        // Colisão da bola com as bordas superior e inferior
        if (ball.position.y <= ball.radius || ball.position.y >= SCREEN_HEIGHT - ball.radius) {
            ball.velocity.y *= -1;
        }
        
        // Colisão da bola com a raquete esquerda
        if (ball.position.x - ball.radius <= leftPaddle.position.x + leftPaddle.size.x &&
            ball.position.y >= leftPaddle.position.y &&
            ball.position.y <= leftPaddle.position.y + leftPaddle.size.y &&
            ball.velocity.x < 0) {
            ball.velocity.x *= -1;
        }
        
        // Colisão da bola com a raquete direita
        if (ball.position.x + ball.radius >= rightPaddle.position.x &&
            ball.position.y >= rightPaddle.position.y &&
            ball.position.y <= rightPaddle.position.y + rightPaddle.size.y &&
            ball.velocity.x > 0) {
            ball.velocity.x *= -1;
        }
        
        // Pontuação
        if (ball.position.x < 0) {
            rightScore++;
            ball.position = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
            ball.velocity.x = BALL_SPEED;
        }
        if (ball.position.x > SCREEN_WIDTH) {
            leftScore++;
            ball.position = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
            ball.velocity.x = -BALL_SPEED;
        }
        
        // Desenho
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Desenha as raquetes
        DrawRectangleV(leftPaddle.position, leftPaddle.size, WHITE);
        DrawRectangleV(rightPaddle.position, rightPaddle.size, WHITE);
        
        // Desenha a bola
        DrawCircleV(ball.position, ball.radius, WHITE);
        
        // Desenha a linha central
        for (int i = 0; i < SCREEN_HEIGHT; i += 20) {
            DrawRectangle(SCREEN_WIDTH/2 - 2, i, 4, 10, WHITE);
        }
        
        // Desenha a pontuação
        DrawText(TextFormat("%d", leftScore), SCREEN_WIDTH/4, 50, 60, WHITE);
        DrawText(TextFormat("%d", rightScore), 3*SCREEN_WIDTH/4, 50, 60, WHITE);
        
        // Instruções
        DrawText("Jogador 1: W/S", 10, SCREEN_HEIGHT - 40, 20, GRAY);
        DrawText("Jogador 2: UP/DOWN", SCREEN_WIDTH - 200, SCREEN_HEIGHT - 40, 20, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}