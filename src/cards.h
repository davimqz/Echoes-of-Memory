#ifndef CARDS_H
#define CARDS_H

#include <stddef.h>

typedef enum { 
    OCULTA = 0,
    REVELADA = 1,
    REMOVIDA = 2
} EstadoCarta;

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

typedef struct Carta {
    int id;
    TipoSimbolo simbolo;
    EstadoCarta estado;
    float tempoRevelado;
    struct Carta *next;
} Carta;

typedef struct {
    Carta *head;
    int totalCartas;
    int paresEncontrados;
    int tentativas;
    float tempoJogo;
    int cortexAtivo;
} EstadoJogo;

/* =================
 * FUNÇÕES PRINCIPAIS
 * =================*/

/* Inicializa o jogo com numPares de fragmentos embaralhados */
void inicializarCartas(Carta **head, int numPares);

/* Exibe o tabuleiro atual na tela usando Raylib */
void exibirTabuleiro(Carta *head, int cols, int rows);

int escolherCarta(Carta *head, int posicao, Carta **cartaSelecionada);

int verificarPar(Carta *carta1, Carta *carta2);

void ordenarCartas(Carta **head);

void jogadaIA(EstadoJogo *estado, int *sugestaoPos1, int *sugestaoPos2);

void liberarMemoria(Carta **head);

void posicaoParaGrid(int posicao, int cols, int *linha, int *coluna);

int gridParaPosicao(int linha, int coluna, int cols);

Carta *obterCartaPorPos(Carta *head, int posicao);

int contarCartasAtivas(Carta *head);

void embaralharCartas(Carta **head);

void imprimirEstadoCartas(Carta *head);

int calcularScore(EstadoJogo *estado);

#endif