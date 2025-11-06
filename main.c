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

/* Variáveis de resolução em tempo de execução (ajustadas em fullscreen) */
static int WIN_WIDTH = 0;
static int WIN_HEIGHT = 0;
/* View offsets para centralizar a "viewport" do jogo quando em fullscreen */
int viewOffsetX = 0;
int viewOffsetY = 0;
/* Ajuste fino: deslocamento dos pés do sprite (positivos movem o sprite para baixo) */
static int spriteFootOffset = 20;

/* Estados do jogo */
typedef enum {
    MENU_PRINCIPAL,
    INTRO_NARRATIVA,
    EXPLORACAO,          /* Novo estado: mundo livre */
    JOGANDO,             /* Tabuleiro de cartas */
    CORTEX_REORGANIZANDO,
    VITORIA,
    GAME_OVER
} EstadoTela;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define GRID_COLS 4
#define GRID_ROWS 3
#define NUM_PARES 6

/* Player para exploração */
typedef struct {
    float x, y;
    float velY;
    int facingRight;
    int agachado;
    int isJumping;
    int podeInteragir; /* Se está próximo da porta */
} Player;

/* Objeto interativo (porta) */
typedef struct {
    float x, y;
    float width, height;
    int ativo; /* Se pode ser usada */
} Porta;

/* Variáveis globais do jogo */
EstadoJogo jogo = {0};
EstadoTela telaAtual = MENU_PRINCIPAL;
Carta *primeira = NULL, *segunda = NULL;
float tempoEspera = 0.0f;
float tempoNarrativa = 0.0f;
int nivelAtual = 1;

/* Variáveis do mundo de exploração */
Player player = {0};
Porta porta = {0};
float groundY = 1.0f;

/* Texturas para exploração */
Texture2D bg;
Texture2D spriteLeft, spriteRight;
Texture2D spriteJumpLeft, spriteJumpRight;
Texture2D spriteAgachandoLeft, spriteAgachandoRight;

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
void inicializarExploracao(void);
void atualizarMenuPrincipal(void);
void atualizarIntroNarrativa(void);
void atualizarExploracao(void);
void atualizarJogando(void);
void atualizarCortexReorganizando(void);
void desenharMenuPrincipal(void);
void desenharIntroNarrativa(void);
void desenharExploracao(void);
void desenharJogando(void);
void desenharCortexReorganizando(void);
void desenharVitoria(void);
void desenharGameOver(void);
void desenharHUD(void);
void processarCliqueCarta(Vector2 posicaoMouse);
void verificarParCartas(void);
void proximoNivel(void);
void reiniciarJogo(void);
void updatePlayerMovement(void);
void updatePlayerJump(void);
void verificarInteracaoPorta(void);
void carregarTexturas(void);
void descarregarTexturas(void);

int main(void) {
     /* Inicialização (pedimos fullscreen na resolução virtual desejada - SCREEN_WIDTH x SCREEN_HEIGHT)
         Isso tenta abrir uma tela fullscreen com 1920x1080. Se o monitor não suportar exatamente
         essa resolução o backend pode ajustar ou retornar para modo janela. */
     SetConfigFlags(FLAG_FULLSCREEN_MODE);
     InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Echoes of Memory - v1.0");
    /* Atualiza variáveis runtime para uso em todo o código */
     WIN_WIDTH = GetScreenWidth();
     WIN_HEIGHT = GetScreenHeight();
     /* Calcula offsets para centralizar a área de jogo (virtual SCREEN_WIDTH x SCREEN_HEIGHT)
         dentro da tela real (WIN_WIDTH x WIN_HEIGHT). Se conseguimos abrir exatamente 1920x1080
         em fullscreen, os offsets serão 0. */
     viewOffsetX = (WIN_WIDTH - SCREEN_WIDTH) / 2;
     viewOffsetY = (WIN_HEIGHT - SCREEN_HEIGHT) / 2;
    InitAudioDevice();
    SetTargetFPS(60);
    
    /* Carrega texturas */
    carregarTexturas();
    
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
            case EXPLORACAO:
                atualizarExploracao();
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
            case EXPLORACAO:
                desenharExploracao();
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
    descarregarTexturas();
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

void inicializarExploracao(void) {
    /* Configura groundY para um cyhão mais baixo */
    /* Mantemos groundY relativo à resolução virtual (SCREEN_HEIGHT). Quando estiver
       em fullscreen desenharemos com um offset para centralizar o conteúdo. */
    groundY = SCREEN_HEIGHT - 120; /* Chão mais próximo da base da tela (virtual) */
    player.x = 100.0f;
    /* Alinha os pés do sprite ao chão usando a altura da textura do sprite padrão */
    if (spriteRight.height > 0) {
        player.y = groundY - (float)spriteRight.height + (float)spriteFootOffset;
    } else {
        player.y = groundY - 50 + (float)spriteFootOffset; /* fallback */
    }
    player.velY = 0.0f;
    player.facingRight = 1;
    player.agachado = 0;
    player.isJumping = 0;
    player.podeInteragir = 0;
    
    /* Configura porta - mesma base (pés) alinhada ao chão */
    porta.x = SCREEN_WIDTH - 200;
    porta.width = 80;
    porta.height = 100;
    porta.y = groundY - porta.height; /* Porta apoiada no chão */
    porta.ativo = 1;
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
        /* Vai para exploração em vez de direto ao jogo */
        inicializarExploracao();
        telaAtual = EXPLORACAO;
    }
}

void atualizarExploracao(void) {
    /* Atualiza movimento do player */
    updatePlayerMovement();
    updatePlayerJump();
    verificarInteracaoPorta();
    
    /* Teclas de debug */
    if (IsKeyPressed(KEY_ESCAPE)) {
        telaAtual = MENU_PRINCIPAL;
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
    
    /* Teclas de controle */
    if (IsKeyPressed(KEY_R)) {
        reiniciarJogo();
    }
    
    /* ESC volta para exploração */
    if (IsKeyPressed(KEY_ESCAPE)) {
        telaAtual = EXPLORACAO;
        printf("🚪 Voltando à exploração...\n");
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
    DrawRectangleGradientV(0, 0, WIN_WIDTH, WIN_HEIGHT, DARKBLUE, BLACK);
    
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
    DrawText("CORTEX v2157.11", WIN_WIDTH - 200, WIN_HEIGHT - 30, 15, DARKGRAY);
}

void desenharIntroNarrativa(void) {
    DrawRectangleGradientV(0, 0, WIN_WIDTH, WIN_HEIGHT, BLACK, DARKBLUE);
    
    float tempoDecorrido = GetTime() - tempoNarrativa;
    int indiceTexto = (int)(tempoDecorrido / 3.0f);
    
    if (indiceTexto < 6) {
        /* Desenha texto atual */
        const char* texto = textos_intro[indiceTexto];
        int larguraTexto = MeasureText(texto, 30);
    int x = (WIN_WIDTH - larguraTexto) / 2;
    int y = WIN_HEIGHT / 2;
        
        DrawText(texto, x, y, 30, WHITE);
        
        /* Efeito de digitação */
        float alpha = fmod(tempoDecorrido * 3.0f, 1.0f);
        if (alpha < 0.5f) {
            DrawText("_", x + larguraTexto, y, 30, Fade(WHITE, alpha * 2));
        }
    }
    
    /* Instrução para pular */
    DrawText("SPACE - Pular introducao", 20, WIN_HEIGHT - 40, 20, GRAY);
}

void desenharJogando(void) {
    /* Fundo do jogo */
    DrawRectangleGradientV(0, 0, WIN_WIDTH, WIN_HEIGHT, DARKGRAY, BLACK);
    
    /* Título da tela */
    DrawText("FRAGMENTOS DE MEMORIA", 20, 20, 25, WHITE);
    DrawText(TextFormat("Nivel %d", nivelAtual), WIN_WIDTH - 150, 20, 20, SKYBLUE);
    
    /* Desenha o tabuleiro de cartas */
    exibirTabuleiro(jogo.head, GRID_COLS, GRID_ROWS);
    
    /* HUD */
    desenharHUD();
    
    /* Indicação de cartas selecionadas */
    if (primeira && segunda) {
        DrawText("Verificando par...", WIN_WIDTH / 2 - 100, 70, 20, YELLOW);
    } else if (primeira) {
        DrawText("Selecione a segunda carta", WIN_WIDTH / 2 - 150, 70, 20, LIGHTGRAY);
    }
    
    /* Estado da IA */
    if (jogo.cortexAtivo) {
        DrawText("CORTEX: ATIVO", 20, WIN_HEIGHT - 80, 15, RED);
        DrawText("Reorganizacao automatica habilitada", 20, WIN_HEIGHT - 60, 12, ORANGE);
    }
    
    /* Controles */
    DrawText("ESC - Voltar à exploração", WIN_WIDTH - 220, WIN_HEIGHT - 30, 14, GRAY);
}

void desenharCortexReorganizando(void) {
    DrawRectangleGradientV(0, 0, WIN_WIDTH, WIN_HEIGHT, MAROON, BLACK);
    
    /* Efeito de interferência */
    static float tempo = 0.0f;
    tempo += GetFrameTime();
    
    int offset = (int)(sin(tempo * 20) * 10);
    DrawText("CORTEX INTERFERINDO...", WIN_WIDTH / 2 - 200 + offset, WIN_HEIGHT / 2 - 50, 40, RED);
    DrawText("Reorganizando fragmentos de memoria", WIN_WIDTH / 2 - 180, WIN_HEIGHT / 2, 25, WHITE);
    
    /* Barras de "carregamento" */
    for (int i = 0; i < 10; i++) {
        Color cor = (i < (int)(tempo * 15) % 10) ? RED : DARKGRAY;
    DrawRectangle(WIN_WIDTH / 2 - 200 + i * 40, WIN_HEIGHT / 2 + 50, 30, 20, cor);
    }
    
    /* Tabuleiro com distorção */
    exibirTabuleiro(jogo.head, GRID_COLS, GRID_ROWS);
}

void desenharVitoria(void) {
    DrawRectangleGradientV(0, 0, WIN_WIDTH, WIN_HEIGHT, DARKGREEN, BLACK);
    
    DrawText("MEMORIA RESTAURADA!", WIN_WIDTH / 2 - 250, WIN_HEIGHT / 2 - 100, 50, GOLD);
    DrawText("Voce reconectou suas lembrancas da infancia!", WIN_WIDTH / 2 - 300, WIN_HEIGHT / 2 - 30, 25, WHITE);
    
    int score = calcularScore(&jogo);
    DrawText(TextFormat("Score: %d", score), WIN_WIDTH / 2 - 80, WIN_HEIGHT / 2 + 20, 30, YELLOW);
    DrawText(TextFormat("Tentativas: %d", jogo.tentativas), WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 + 60, 20, LIGHTGRAY);
    
    DrawText("ENTER - Proximo nivel", WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 + 120, 25, SKYBLUE);
}

void desenharGameOver(void) {
    DrawRectangleGradientV(0, 0, WIN_WIDTH, WIN_HEIGHT, DARKPURPLE, BLACK);
    
    DrawText("CORTEX VENCEU", WIN_WIDTH / 2 - 200, WIN_HEIGHT / 2 - 50, 50, RED);
    DrawText("Suas memorias foram apagadas...", WIN_WIDTH / 2 - 200, WIN_HEIGHT / 2, 25, WHITE);
    DrawText("ENTER - Tentar novamente", WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 + 80, 25, GRAY);
}

void desenharHUD(void) {
    /* Painel de informações */
    DrawRectangle(20, WIN_HEIGHT - 150, 300, 120, Fade(BLACK, 0.7f));
    DrawRectangleLines(20, WIN_HEIGHT - 150, 300, 120, SKYBLUE);
    
    DrawText("STATUS DA MISSAO", 30, WIN_HEIGHT - 140, 16, WHITE);
    DrawText(TextFormat("Pares encontrados: %d/%d", jogo.paresEncontrados, NUM_PARES), 30, WIN_HEIGHT - 115, 14, LIGHTGRAY);
    DrawText(TextFormat("Tentativas: %d", jogo.tentativas), 30, WIN_HEIGHT - 95, 14, LIGHTGRAY);
    
    float tempoDecorrido = GetTime() - jogo.tempoJogo;
    int minutos = (int)tempoDecorrido / 60;
    int segundos = (int)tempoDecorrido % 60;
    DrawText(TextFormat("Tempo: %02d:%02d", minutos, segundos), 30, WIN_HEIGHT - 75, 14, LIGHTGRAY);
    
    int cartas_ativas = contarCartasAtivas(jogo.head);
    DrawText(TextFormat("Fragmentos ativos: %d", cartas_ativas), 30, WIN_HEIGHT - 55, 14, LIGHTGRAY);
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
    
    /* Volta para exploração em vez de próximo nível direto */
    liberarMemoria(&jogo.head);
    
    printf("🆙 Memórias restauradas! Voltando à exploração...\n");
    telaAtual = EXPLORACAO;
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

/* ================================
 * FUNÇÕES DO SISTEMA DE EXPLORAÇÃO
 * ================================ */

void updatePlayerMovement(void) {
    const float speed = 5.0f;
    
    /* Movimento horizontal - usando WASD e setas */
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        player.x += speed;
        player.facingRight = 1;
        /* Permite movimento até próximo da borda direita */
        if (player.x > SCREEN_WIDTH - 60) {
            player.x = SCREEN_WIDTH - 60;
        }
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        player.x -= speed;
        player.facingRight = 0;
        if (player.x < 0) {
            player.x = 0;
        }
    }
    
    /* Agachar - usando S ou C */
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_C)) {
        player.agachado = 1;
    } else {
        player.agachado = 0;
    }
}

void updatePlayerJump(void) {
    const float gravity = 0.6f;
    const float jumpForce = -12.0f;
    /* Calcula altura de aterrissagem baseada na altura do sprite padrão */
    float spriteH = (spriteRight.height > 0) ? (float)spriteRight.height : 50.0f;
    float playerGroundY = groundY - spriteH + (float)spriteFootOffset; /* Mesma altura usada na inicialização */
    
    /* Inicia o pulo - W, UP ou SPACE (quando não próximo da porta) */
    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || 
         (IsKeyPressed(KEY_SPACE) && !player.podeInteragir)) && 
        !player.isJumping && !player.agachado) {
        player.isJumping = 1;
        player.velY = jumpForce;
    }
    
    /* Atualiza posição no ar */
    if (player.isJumping) {
        player.y += player.velY;
        player.velY += gravity;
        
        /* Volta ao chão na altura correta */
        if (player.y >= playerGroundY) {
            player.y = playerGroundY;
            player.velY = 0;
            player.isJumping = 0;
        }
    }
}

void verificarInteracaoPorta(void) {
    /* Verifica distância do player à porta - área mais generosa */
    float distX = fabs(player.x - porta.x);
    float distY = fabs(player.y - porta.y);
    
    /* Área de interação maior e mais permissiva */
    if (distX < 150 && distY < 80 && porta.ativo) {
        player.podeInteragir = 1;
        
        /* Interação com SPACE */
        if (IsKeyPressed(KEY_SPACE)) {
            /* Inicializa jogo de cartas e vai para tabuleiro */
            inicializarCartas(&jogo.head, NUM_PARES);
            jogo.tempoJogo = GetTime();
            telaAtual = JOGANDO;
            printf("🚪 Entrando no tabuleiro de memórias...\n");
        }
    } else {
        player.podeInteragir = 0;
    }
}

void desenharExploracao(void) {
     /* Fundo do mundo de exploração: desenha o bg escalado para preencher a
         área virtual SCREEN_WIDTH x SCREEN_HEIGHT e aplica viewOffset para
         centralizar quando em fullscreen. Usamos DrawTexturePro para escalar. */
     Rectangle srcBg = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
     Rectangle dstBg = { (float)viewOffsetX, (float)viewOffsetY, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
     Vector2 origin = { 0.0f, 0.0f };
     DrawTexturePro(bg, srcBg, dstBg, origin, 0.0f, WHITE);
    
    /* Porta (objeto interativo) */
    Rectangle portaRect = { porta.x + viewOffsetX, porta.y + viewOffsetY, porta.width, porta.height };
    Color corPorta = player.podeInteragir ? GOLD : BROWN;
    DrawRectangleRec(portaRect, corPorta);
    DrawRectangleLines((int)porta.x + viewOffsetX, (int)porta.y + viewOffsetY, (int)porta.width, (int)porta.height, BLACK);
    
    /* Maçaneta */
    DrawCircle((int)porta.x + viewOffsetX + 65, (int)porta.y + viewOffsetY + 50, 5, YELLOW);
    
    /* Desenha o player usando sprites */
    if (player.isJumping) {
        if (player.facingRight) {
            DrawTexture(spriteJumpRight, (int)player.x + viewOffsetX, (int)player.y + viewOffsetY, WHITE);
        } else {
            DrawTexture(spriteJumpLeft, (int)player.x + viewOffsetX, (int)player.y + viewOffsetY, WHITE);
        }
    } else if (player.agachado) {
        if (player.facingRight) {
            DrawTexture(spriteAgachandoRight, (int)player.x + viewOffsetX, (int)player.y + viewOffsetY, WHITE);
        } else {
            DrawTexture(spriteAgachandoLeft, (int)player.x + viewOffsetX, (int)player.y + viewOffsetY, WHITE);
        }
    } else {
        if (player.facingRight) {
            DrawTexture(spriteRight, (int)player.x + viewOffsetX, (int)player.y + viewOffsetY, WHITE);
        } else {
            DrawTexture(spriteLeft, (int)player.x + viewOffsetX, (int)player.y + viewOffsetY, WHITE);
        }
    }
    
    /* UI do mundo de exploração */
    DrawRectangle(0, 0, WIN_WIDTH, 100, Fade(BLACK, 0.7f));
    DrawText("ECHOES OF MEMORY - EXPLORAÇÃO", 20, 20, 20, WHITE);
    DrawText("Use WASD ou setas para mover", 20, 45, 16, LIGHTGRAY);
    DrawText("SPACE perto da porta para acessar memórias", 20, 65, 16, LIGHTGRAY);
    
    if (player.podeInteragir) {
        DrawText("PRESSIONE SPACE PARA ENTRAR", (int)porta.x + viewOffsetX - 50, (int)porta.y + viewOffsetY - 30, 16, YELLOW);
        /* Efeito pulsante */
        static float tempo = 0.0f;
        tempo += GetFrameTime();
        float alpha = (sin(tempo * 8) + 1) / 2;
        DrawRectangleLines((int)porta.x + viewOffsetX - 5, (int)porta.y + viewOffsetY - 5, (int)porta.width + 10, (int)porta.height + 10, 
                          Fade(YELLOW, alpha));
    }
    
    DrawText("ESC - Voltar ao menu", WIN_WIDTH - 200, WIN_HEIGHT - 30, 14, GRAY);
    
    /* Informações de debug */
    DrawText(TextFormat("Player: (%.0f, %.0f) [draw at %.0f, %.0f]", player.x, player.y, player.x + viewOffsetX, player.y + viewOffsetY), 20, 90, 14, WHITE);
    DrawText(TextFormat("Porta: (%.0f, %.0f) [draw at %.0f, %.0f]", porta.x, porta.y, porta.x + viewOffsetX, porta.y + viewOffsetY), 20, 110, 14, WHITE);
    float distX = fabs(player.x - porta.x);
    float distY = fabs(player.y - porta.y);
    DrawText(TextFormat("Distância: X=%.0f Y=%.0f", distX, distY), 20, 130, 14, WHITE);
    DrawText(TextFormat("Pode interagir: %s", player.podeInteragir ? "SIM" : "NÃO"), 20, 150, 14, 
             player.podeInteragir ? GREEN : RED);
}

/* ====================================
 * FUNÇÕES DE GERENCIAMENTO DE TEXTURAS
 * ==================================== */

void carregarTexturas(void) {
    bg = LoadTexture("./assets/bg.png");
    spriteLeft = LoadTexture("./assets/characterLeft.png");
    spriteRight = LoadTexture("./assets/characterRight.png");
    spriteJumpLeft = LoadTexture("./assets/characterJumpLeft.png");
    spriteJumpRight = LoadTexture("./assets/characterJumpRight.png");
    spriteAgachandoLeft = LoadTexture("./assets/characterAgachandoLeft.png");
    spriteAgachandoRight = LoadTexture("./assets/characterAgachandoRight.png");
    
    printf("✅ Texturas carregadas com sucesso!\n");
}

void descarregarTexturas(void) {
    UnloadTexture(bg);
    UnloadTexture(spriteLeft);
    UnloadTexture(spriteRight);
    UnloadTexture(spriteJumpLeft);
    UnloadTexture(spriteJumpRight);
    UnloadTexture(spriteAgachandoLeft);
    UnloadTexture(spriteAgachandoRight);
    
    printf("🗑️ Texturas liberadas da memória!\n");
}