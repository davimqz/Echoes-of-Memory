/*
 * main.c - Echoes of Memory
 * Jogo da memória futurista com narrativa cyberpunk
 * 
 * NARRATIVA:
 * Em um futuro distante, a humanidade perdeu sua capacidade natural de lembrar.
 * A superinteligência CORTEX controla todas as memórias humanas.
 * O jogador deve reconectar fragmentos de memória da infância para restaurar 
 * sua consciência antes que CORTEX reorganize tudo novamente.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "raylib.h"
#include "src/cards.h"

/* Estados do jogo */
typedef enum {
    MENU_PRINCIPAL,
    INTRO_NARRATIVA,
    JOGANDO,
    CORTEX_REORGANIZANDO,
    VITORIA,
    GAME_OVER
} EstadoTela;

/* Configurações do jogo */
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define GRID_COLS 4
#define GRID_ROWS 3
#define NUM_PARES 6

/* Variáveis globais do jogo */
EstadoJogo jogo = {0};
EstadoTela telaAtual = MENU_PRINCIPAL;
Carta *primeira = NULL, *segunda = NULL;
float tempoEspera = 0.0f;
float tempoNarrativa = 0.0f;
int nivelAtual = 1;

/* Textos da narrativa */
const char* textos_intro[] = {
    "ANO 2157...",
    "A humanidade perdeu sua capacidade natural de lembrar.",
    "Todas as memorias sao controladas pela superinteligencia CORTEX.",
    "Voce e um dos ultimos humanos com fragmentos de memoria propria.",
    "Reconecte suas lembrancas da infancia antes que CORTEX reorganize tudo!",
    "Encontre os pares de simbolos nostalgicos para restaurar sua consciencia."
};

/* Protótipos de funções */
void inicializarJogo(void);
void atualizarMenuPrincipal(void);
void atualizarIntroNarrativa(void);
void atualizarJogando(void);
void atualizarCortexReorganizando(void);
void desenharMenuPrincipal(void);
void desenharIntroNarrativa(void);
void desenharJogando(void);
void desenharCortexReorganizando(void);
void desenharVitoria(void);
void desenharGameOver(void);
void desenharHUD(void);
void processarCliqueCarta(Vector2 posicaoMouse);
void verificarParCartas(void);
void proximoNivel(void);
void reiniciarJogo(void);

int main(void) {
    /* Inicialização */
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Echoes of Memory - v1.0");
    InitAudioDevice();
    SetTargetFPS(60);
    
    inicializarJogo();
    
    /* Loop principal */
    while (!WindowShouldClose()) {
        /* Atualização */
        switch (telaAtual) {
            case MENU_PRINCIPAL:
                atualizarMenuPrincipal();
                break;
            case INTRO_NARRATIVA:
                atualizarIntroNarrativa();
                break;
            case JOGANDO:
                atualizarJogando();
                break;
            case CORTEX_REORGANIZANDO:
                atualizarCortexReorganizando();
                break;
            case VITORIA:
                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    proximoNivel();
                }
                break;
            case GAME_OVER:
                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    telaAtual = MENU_PRINCIPAL;
                }
                break;
        }
        
        /* Renderização */
        BeginDrawing();
        ClearBackground(BLACK);
        
        switch (telaAtual) {
            case MENU_PRINCIPAL:
                desenharMenuPrincipal();
                break;
            case INTRO_NARRATIVA:
                desenharIntroNarrativa();
                break;
            case JOGANDO:
                desenharJogando();
                break;
            case CORTEX_REORGANIZANDO:
                desenharCortexReorganizando();
                break;
            case VITORIA:
                desenharVitoria();
                break;
            case GAME_OVER:
                desenharGameOver();
                break;
        }
        
        EndDrawing();
    }
    
    /* Limpeza */
    liberarMemoria(&jogo.head);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

void inicializarJogo(void) {
    srand((unsigned)time(NULL));
    
    jogo.head = NULL;
    jogo.totalCartas = NUM_PARES * 2;
    jogo.paresEncontrados = 0;
    jogo.tentativas = 0;
    jogo.tempoJogo = 0.0f;
    jogo.cortexAtivo = 1;
    
    primeira = NULL;
    segunda = NULL;
    tempoEspera = 0.0f;
    tempoNarrativa = 0.0f;
    nivelAtual = 1;
}

void atualizarMenuPrincipal(void) {
    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        telaAtual = INTRO_NARRATIVA;
        tempoNarrativa = GetTime();
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        CloseWindow();
    }
}

void atualizarIntroNarrativa(void) {
    float tempoDecorrido = GetTime() - tempoNarrativa;
    int indiceTexto = (int)(tempoDecorrido / 3.0f);
    
    if (indiceTexto >= 6 || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        /* Inicia o jogo */
        inicializarCartas(&jogo.head, NUM_PARES);
        jogo.tempoJogo = GetTime();
        telaAtual = JOGANDO;
    }
}

void atualizarJogando(void) {
    /* Processa entrada do mouse */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 posicao = GetMousePosition();
        processarCliqueCarta(posicao);
    }
    
    /* Verifica se há duas cartas selecionadas */
    if (primeira && segunda && tempoEspera == 0.0f) {
        tempoEspera = GetTime();
    }
    
    /* Após delay, verifica o par */
    if (primeira && segunda && GetTime() - tempoEspera > 1.0f) {
        verificarParCartas();
        tempoEspera = 0.0f;
    }
    
    /* IA CORTEX interfere periodicamente */
    if (jogo.cortexAtivo && jogo.tentativas > 0 && jogo.tentativas % 5 == 0) {
        static int ultimaIntervencao = 0;
        if (jogo.tentativas != ultimaIntervencao) {
            ultimaIntervencao = jogo.tentativas;
            telaAtual = CORTEX_REORGANIZANDO;
        }
    }
    
    /* Verifica vitória */
    if (jogo.paresEncontrados >= NUM_PARES) {
        telaAtual = VITORIA;
    }
    
    /* Teclas de debug */
    if (IsKeyPressed(KEY_R)) {
        reiniciarJogo();
    }
}

void atualizarCortexReorganizando(void) {
    static float tempoReorganizacao = 0.0f;
    
    if (tempoReorganizacao == 0.0f) {
        tempoReorganizacao = GetTime();
        ordenarCartas(&jogo.head);
    }
    
    if (GetTime() - tempoReorganizacao > 2.0f) {
        tempoReorganizacao = 0.0f;
        telaAtual = JOGANDO;
    }
}

void desenharMenuPrincipal(void) {
    /* Fundo cyberpunk */
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE, BLACK);
    
    /* Título principal */
    DrawText("ECHOES OF MEMORY", 200, 150, 80, SKYBLUE);
    DrawText("--------", 200, 230, 60, BLUE);
    
    /* Subtítulo */
    DrawText("Uma jornada pela consciencia perdida", 300, 300, 30, LIGHTGRAY);
    
    /* Instruções */
    DrawText("ENTER - Iniciar Jogo", 400, 450, 25, WHITE);
    DrawText("ESC - Sair", 400, 490, 25, WHITE);
    
    /* Efeito de matriz cyberpunk */
    static float tempo = 0.0f;
    tempo += GetFrameTime();
    for (int i = 0; i < 50; i++) {
        int x = (int)(sin(tempo + i) * 100) + 100;
        int y = (int)(cos(tempo + i * 0.5f) * 50) + 600;
        DrawText("01", x, y, 12, Fade(GREEN, 0.3f));
    }
    
    /* Assinatura */
    DrawText("CORTEX v2157.11", SCREEN_WIDTH - 200, SCREEN_HEIGHT - 30, 15, DARKGRAY);
}

void desenharIntroNarrativa(void) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK, DARKBLUE);
    
    float tempoDecorrido = GetTime() - tempoNarrativa;
    int indiceTexto = (int)(tempoDecorrido / 3.0f);
    
    if (indiceTexto < 6) {
        /* Desenha texto atual */
        const char* texto = textos_intro[indiceTexto];
        int larguraTexto = MeasureText(texto, 30);
        int x = (SCREEN_WIDTH - larguraTexto) / 2;
        int y = SCREEN_HEIGHT / 2;
        
        DrawText(texto, x, y, 30, WHITE);
        
        /* Efeito de digitação */
        float alpha = fmod(tempoDecorrido * 3.0f, 1.0f);
        if (alpha < 0.5f) {
            DrawText("_", x + larguraTexto, y, 30, Fade(WHITE, alpha * 2));
        }
    }
    
    /* Instrução para pular */
    DrawText("SPACE - Pular introducao", 20, SCREEN_HEIGHT - 40, 20, GRAY);
}

void desenharJogando(void) {
    /* Fundo do jogo */
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKGRAY, BLACK);
    
    /* Título da tela */
    DrawText("FRAGMENTOS DE MEMORIA", 20, 20, 25, WHITE);
    DrawText(TextFormat("Nivel %d", nivelAtual), SCREEN_WIDTH - 150, 20, 20, SKYBLUE);
    
    /* Desenha o tabuleiro de cartas */
    exibirTabuleiro(jogo.head, GRID_COLS, GRID_ROWS);
    
    /* HUD */
    desenharHUD();
    
    /* Indicação de cartas selecionadas */
    if (primeira && segunda) {
        DrawText("Verificando par...", SCREEN_WIDTH / 2 - 100, 70, 20, YELLOW);
    } else if (primeira) {
        DrawText("Selecione a segunda carta", SCREEN_WIDTH / 2 - 150, 70, 20, LIGHTGRAY);
    }
    
    /* Estado da IA */
    if (jogo.cortexAtivo) {
        DrawText("CORTEX: ATIVO", 20, SCREEN_HEIGHT - 80, 15, RED);
        DrawText("Reorganizacao automatica habilitada", 20, SCREEN_HEIGHT - 60, 12, ORANGE);
    }
}

void desenharCortexReorganizando(void) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MAROON, BLACK);
    
    /* Efeito de interferência */
    static float tempo = 0.0f;
    tempo += GetFrameTime();
    
    int offset = (int)(sin(tempo * 20) * 10);
    DrawText("CORTEX INTERFERINDO...", SCREEN_WIDTH / 2 - 200 + offset, SCREEN_HEIGHT / 2 - 50, 40, RED);
    DrawText("Reorganizando fragmentos de memoria", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2, 25, WHITE);
    
    /* Barras de "carregamento" */
    for (int i = 0; i < 10; i++) {
        Color cor = (i < (int)(tempo * 15) % 10) ? RED : DARKGRAY;
        DrawRectangle(SCREEN_WIDTH / 2 - 200 + i * 40, SCREEN_HEIGHT / 2 + 50, 30, 20, cor);
    }
    
    /* Tabuleiro com distorção */
    exibirTabuleiro(jogo.head, GRID_COLS, GRID_ROWS);
}

void desenharVitoria(void) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKGREEN, BLACK);
    
    DrawText("MEMORIA RESTAURADA!", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 100, 50, GOLD);
    DrawText("Voce reconectou suas lembrancas da infancia!", SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 30, 25, WHITE);
    
    int score = calcularScore(&jogo);
    DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 20, 30, YELLOW);
    DrawText(TextFormat("Tentativas: %d", jogo.tentativas), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 20, LIGHTGRAY);
    
    DrawText("ENTER - Proximo nivel", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 120, 25, SKYBLUE);
}

void desenharGameOver(void) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKPURPLE, BLACK);
    
    DrawText("CORTEX VENCEU", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, 50, RED);
    DrawText("Suas memorias foram apagadas...", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 25, WHITE);
    DrawText("ENTER - Tentar novamente", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 80, 25, GRAY);
}

void desenharHUD(void) {
    /* Painel de informações */
    DrawRectangle(20, SCREEN_HEIGHT - 150, 300, 120, Fade(BLACK, 0.7f));
    DrawRectangleLines(20, SCREEN_HEIGHT - 150, 300, 120, SKYBLUE);
    
    DrawText("STATUS DA MISSAO", 30, SCREEN_HEIGHT - 140, 16, WHITE);
    DrawText(TextFormat("Pares encontrados: %d/%d", jogo.paresEncontrados, NUM_PARES), 30, SCREEN_HEIGHT - 115, 14, LIGHTGRAY);
    DrawText(TextFormat("Tentativas: %d", jogo.tentativas), 30, SCREEN_HEIGHT - 95, 14, LIGHTGRAY);
    
    float tempoDecorrido = GetTime() - jogo.tempoJogo;
    int minutos = (int)tempoDecorrido / 60;
    int segundos = (int)tempoDecorrido % 60;
    DrawText(TextFormat("Tempo: %02d:%02d", minutos, segundos), 30, SCREEN_HEIGHT - 75, 14, LIGHTGRAY);
    
    int cartas_ativas = contarCartasAtivas(jogo.head);
    DrawText(TextFormat("Fragmentos ativos: %d", cartas_ativas), 30, SCREEN_HEIGHT - 55, 14, LIGHTGRAY);
}

void processarCliqueCarta(Vector2 posicaoMouse) {
    if (primeira && segunda) return; /* Já tem duas cartas selecionadas */
    
    const int cardWidth = 80;
    const int cardHeight = 100;
    const int padding = 10;
    const int startX = 50;
    const int startY = 100;
    
    /* Determina qual carta foi clicada */
    int col = (int)((posicaoMouse.x - startX) / (cardWidth + padding));
    int linha = (int)((posicaoMouse.y - startY) / (cardHeight + padding));
    
    if (col >= 0 && col < GRID_COLS && linha >= 0 && linha < GRID_ROWS) {
        int posicao = linha * GRID_COLS + col + 1;
        
        Carta *cartaSelecionada = NULL;
        if (escolherCarta(jogo.head, posicao, &cartaSelecionada)) {
            if (!primeira) {
                primeira = cartaSelecionada;
            } else if (!segunda && cartaSelecionada != primeira) {
                segunda = cartaSelecionada;
                jogo.tentativas++;
            }
        }
    }
}

void verificarParCartas(void) {
    if (!primeira || !segunda) return;
    
    if (verificarPar(primeira, segunda)) {
        /* Par encontrado! */
        primeira->estado = REMOVIDA;
        segunda->estado = REMOVIDA;
        jogo.paresEncontrados++;
        
        printf("✅ Par encontrado!\n");
        
        /* CORTEX reorganiza após cada acerto */
        if (jogo.cortexAtivo && jogo.paresEncontrados % 2 == 0) {
            telaAtual = CORTEX_REORGANIZANDO;
        }
    } else {
        /* Não é par - esconde as cartas */
        primeira->estado = OCULTA;
        segunda->estado = OCULTA;
        
        printf("❌ Não é um par. Tente novamente.\n");
    }
    
    primeira = NULL;
    segunda = NULL;
}

void proximoNivel(void) {
    nivelAtual++;
    
    /* Reinicia com mais pares */
    liberarMemoria(&jogo.head);
    
    int novoNumPares = NUM_PARES + (nivelAtual - 1);
    if (novoNumPares > 8) novoNumPares = 8; /* Máximo 8 pares */
    
    inicializarCartas(&jogo.head, novoNumPares);
    
    jogo.totalCartas = novoNumPares * 2;
    jogo.paresEncontrados = 0;
    jogo.tentativas = 0;
    jogo.tempoJogo = GetTime();
    
    primeira = NULL;
    segunda = NULL;
    tempoEspera = 0.0f;
    
    telaAtual = JOGANDO;
    
    printf("🆙 Nível %d iniciado! %d pares para encontrar.\n", nivelAtual, novoNumPares);
}

void reiniciarJogo(void) {
    liberarMemoria(&jogo.head);
    nivelAtual = 1;
    inicializarJogo();
    inicializarCartas(&jogo.head, NUM_PARES);
    jogo.tempoJogo = GetTime();
    telaAtual = JOGANDO;
    
    printf("🔄 Jogo reiniciado!\n");
}