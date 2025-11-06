/* 
 * src/cards.h - Echoes of Memory
 * Interface para o sistema de cartas baseado em lista encadeada
 * Representa fragmentos de memória na narrativa futurista
 */

#ifndef CARDS_H
#define CARDS_H

#include <stddef.h>

/* Estados possíveis de uma carta/fragmento de memória */
typedef enum { 
    OCULTA = 0,    /* Fragmento ainda não revelado */
    REVELADA = 1,  /* Fragmento temporariamente visível */
    REMOVIDA = 2   /* Fragmento restaurado na consciência */
} EstadoCarta;

/* Tipos de símbolos nostálgicos */
typedef enum {
    SIMBOLO_PIAO = 1,
    SIMBOLO_PIPA = 2, 
    SIMBOLO_TAZO = 3,
    SIMBOLO_VIDEOGAME = 4,
    SIMBOLO_BOLA = 5,
    SIMBOLO_BONECA = 6,
    SIMBOLO_CARRINHO = 7,
    SIMBOLO_QUEBRACABECA = 8
} TipoSimbolo;

/* Nó da lista encadeada - cada fragmento de memória */
typedef struct Carta {
    int id;                    /* Identificador único */
    TipoSimbolo simbolo;       /* Tipo de símbolo nostálgico */
    EstadoCarta estado;        /* Estado atual do fragmento */
    float tempoRevelado;       /* Tempo desde que foi revelado */
    struct Carta *next;        /* Próximo fragmento na lista */
} Carta;

/* Estado do jogo */
typedef struct {
    Carta *head;               /* Primeiro fragmento da lista */
    int totalCartas;           /* Total de cartas no jogo */
    int paresEncontrados;      /* Pares já restaurados */
    int tentativas;            /* Número de tentativas */
    float tempoJogo;           /* Tempo total de jogo */
    int cortexAtivo;           /* Se a IA CORTEX está interferindo */
} EstadoJogo;

/* =================
 * FUNÇÕES PRINCIPAIS
 * =================*/

/* Inicializa o jogo com numPares de fragmentos embaralhados */
void inicializarCartas(Carta **head, int numPares);

/* Exibe o tabuleiro atual na tela usando Raylib */
void exibirTabuleiro(Carta *head, int cols, int rows);

/* Permite escolher uma carta por posição (1-based) */
int escolherCarta(Carta *head, int posicao, Carta **cartaSelecionada);

/* Verifica se duas cartas formam um par válido */
int verificarPar(Carta *carta1, Carta *carta2);

/* Reorganiza as cartas usando Bubble Sort após cada acerto */
void ordenarCartas(Carta **head);

/* IA CORTEX tenta interferir no jogo do jogador */
void jogadaIA(EstadoJogo *estado, int *sugestaoPos1, int *sugestaoPos2);

/* Libera toda memória alocada */
void liberarMemoria(Carta **head);

/* =================
 * FUNÇÕES AUXILIARES
 * =================*/

/* Converte posição linear para coordenadas de grid */
void posicaoParaGrid(int posicao, int cols, int *linha, int *coluna);

/* Converte coordenadas de grid para posição linear */
int gridParaPosicao(int linha, int coluna, int cols);

/* Obtém carta por posição na lista (1-based) */
Carta *obterCartaPorPos(Carta *head, int posicao);

/* Conta quantas cartas ainda estão ativas */
int contarCartasAtivas(Carta *head);

/* Embaralha as cartas usando Fisher-Yates */
void embaralharCartas(Carta **head);

/* Imprime estado atual das cartas (debug) */
void imprimirEstadoCartas(Carta *head);

/* Calcula score baseado em tempo e tentativas */
int calcularScore(EstadoJogo *estado);

#endif /* CARDS_H */