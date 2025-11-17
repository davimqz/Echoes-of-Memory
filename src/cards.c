#include "cards.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void sleep_ms(int ms) {
#ifdef _WIN32
    void __stdcall Sleep(unsigned long);
    Sleep((unsigned long)ms);
#else
    usleep(ms * 1000);
#endif
}

#include "raylib.h"

static Carta *criarCarta(int id, TipoSimbolo simbolo) {
    Carta *nova = (Carta *)malloc(sizeof(Carta));
    if (!nova) {
        return NULL;
    }
    
    nova->id = id;
    nova->simbolo = simbolo;
    nova->estado = OCULTA;
    nova->tempoRevelado = 0.0f;
    nova->next = NULL;
    
    return nova;
}

static void shuffleArray(TipoSimbolo *arr, int n) {
    if (n <= 1) {
        return;
    }
    
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        TipoSimbolo temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

static void trocarConteudoNos(Carta *a, Carta *b) {
    if (!a || !b) {
        return;
    }
    
    int tempId = a->id;
    TipoSimbolo tempSimbolo = a->simbolo;
    EstadoCarta tempEstado = a->estado;
    float tempTempo = a->tempoRevelado;
    
    a->id = b->id;
    a->simbolo = b->simbolo;
    a->estado = b->estado;
    a->tempoRevelado = b->tempoRevelado;
    
    b->id = tempId;
    b->simbolo = tempSimbolo;
    b->estado = tempEstado;
    b->tempoRevelado = tempTempo;
}

void inicializarCartas(Carta **head, int numPares) {
    if (!head || numPares <= 0) {
        return;
    }
    
    liberarMemoria(head);
    
    int totalCartas = numPares * 2;
    TipoSimbolo *simbolos = (TipoSimbolo *)malloc(sizeof(TipoSimbolo) * totalCartas);
    if (!simbolos) {
        return;
    }
    
    TipoSimbolo tipos[] = {
        SIMBOLO_PIAO, SIMBOLO_PIPA, SIMBOLO_TAZO, SIMBOLO_VIDEOGAME,
        SIMBOLO_BOLA, SIMBOLO_BONECA, SIMBOLO_CARRINHO, SIMBOLO_QUEBRACABECA
    };
    int numTipos = sizeof(tipos) / sizeof(tipos[0]);
    
    for (int i = 0; i < numPares; i++) {
        TipoSimbolo tipo = tipos[i % numTipos];
        simbolos[2 * i] = tipo;
        simbolos[2 * i + 1] = tipo;
    }
    
    srand((unsigned)time(NULL));
    shuffleArray(simbolos, totalCartas);
    
    Carta *atual = NULL;
    for (int i = 0; i < totalCartas; i++) {
        Carta *nova = criarCarta(i + 1, simbolos[i]);
        if (!nova) {
            free(simbolos);
            liberarMemoria(head);
            return;
        }
        
        if (!*head) {
            *head = nova;
            atual = nova;
        } else {
            atual->next = nova;
            atual = nova;
        }
    }
    
    free(simbolos);
}

void exibirTabuleiro(Carta *head, int cols, int rows) {
    if (!head) {
        return;
    }
    
    const int cardWidth = 80;
    const int cardHeight = 100;
    const int padding = 10;
    int startX = 50;
    int startY = 100;
    extern int viewOffsetX;
    extern int viewOffsetY;
    startX += viewOffsetX;
    startY += viewOffsetY;
    
    Carta *atual = head;
    int posicao = 0;
    
    while (atual && posicao < cols * rows) {
        int col = posicao % cols;
        int row = posicao / cols;
        
        int x = startX + col * (cardWidth + padding);
        int y = startY + row * (cardHeight + padding);
        
        Rectangle cardRect = { (float)x, (float)y, (float)cardWidth, (float)cardHeight };
        
        switch (atual->estado) {
            case OCULTA:
                DrawRectangleRec(cardRect, DARKBLUE);
                DrawRectangleLinesEx(cardRect, 2, SKYBLUE);
                DrawText("CORTEX", x + 10, y + 40, 10, LIGHTGRAY);
                break;
                
            case REVELADA:
                DrawRectangleRec(cardRect, LIGHTGRAY);
                DrawRectangleLinesEx(cardRect, 2, BLACK);
                
                char simboloTexto[20];
                Color cor = BLACK;
                switch (atual->simbolo) {
                    case SIMBOLO_PIAO:
                        strcpy(simboloTexto, "PIÃO");
                        cor = RED;
                        break;
                    case SIMBOLO_PIPA:
                        strcpy(simboloTexto, "PIPA");
                        cor = BLUE;
                        break;
                    case SIMBOLO_TAZO:
                        strcpy(simboloTexto, "TAZO");
                        cor = GOLD;
                        break;
                    case SIMBOLO_VIDEOGAME:
                        strcpy(simboloTexto, "GAME");
                        cor = PURPLE;
                        break;
                    case SIMBOLO_BOLA:
                        strcpy(simboloTexto, "BOLA");
                        cor = ORANGE;
                        break;
                    case SIMBOLO_BONECA:
                        strcpy(simboloTexto, "BONECA");
                        cor = PINK;
                        break;
                    case SIMBOLO_CARRINHO:
                        strcpy(simboloTexto, "CARRO");
                        cor = GREEN;
                        break;
                    case SIMBOLO_QUEBRACABECA:
                        strcpy(simboloTexto, "PUZZLE");
                        cor = BROWN;
                        break;
                }
                
                DrawText(simboloTexto, x + 5, y + 30, 12, cor);
                DrawText(TextFormat("%d", atual->id), x + 5, y + 75, 10, DARKGRAY);
                break;
                
            case REMOVIDA:
                DrawRectangleRec(cardRect, GREEN);
                DrawRectangleLinesEx(cardRect, 3, LIME);
                DrawText("RESTORED", x + 5, y + 40, 10, WHITE);
                break;
        }
        
        atual = atual->next;
        posicao++;
    }
}

int escolherCarta(Carta *head, int posicao, Carta **cartaSelecionada) {
    if (!head || !cartaSelecionada || posicao <= 0) {
        return 0;
    }
    
    Carta *carta = obterCartaPorPos(head, posicao);
    if (!carta) {
        return 0;
    }
    
    if (carta->estado != OCULTA) {
        return 0;
    }
    
    carta->estado = REVELADA;
    carta->tempoRevelado = GetTime();
    *cartaSelecionada = carta;
    
    return 1;
}

int verificarPar(Carta *carta1, Carta *carta2) {
    if (!carta1 || !carta2) {
        return 0;
    }
    
    return (carta1->simbolo == carta2->simbolo) && (carta1->id != carta2->id);
}

void ordenarCartas(Carta **head) {
    if (!head || !*head) {
        return;
    }
    
    printf("🧠 CORTEX está reorganizando os fragmentos de memória...\n");
    
    int trocou;
    do {
        trocou = 0;
        Carta *atual = *head;
        
        while (atual && atual->next) {
            /* Critério de ordenação: símbolo primeiro, depois ID */
            int deveTracar = 0;
            
            if (atual->simbolo > atual->next->simbolo) {
                deveTracar = 1;
            } else if (atual->simbolo == atual->next->simbolo && atual->id > atual->next->id) {
                deveTracar = 1;
            }
            
            if (deveTracar) {
                trocarConteudoNos(atual, atual->next);
                trocou = 1;
                sleep_ms(50);
            }
            
            atual = atual->next;
        }
    } while (trocou);
    
    printf("✅ Reorganização completa!\n");
}

void jogadaIA(EstadoJogo *estado, int *sugestaoPos1, int *sugestaoPos2) {
    if (!estado || !estado->head || !sugestaoPos1 || !sugestaoPos2) {
        return;
    }
    
    *sugestaoPos1 = -1;
    *sugestaoPos2 = -1;
    
    /* IA simples: procura por pares conhecidos */
    Carta *carta1 = estado->head;
    int pos1 = 1;
    
    while (carta1) {
        if (carta1->estado == OCULTA) {
            Carta *carta2 = carta1->next;
            int pos2 = pos1 + 1;
            
            while (carta2) {
                if (carta2->estado == OCULTA && carta1->simbolo == carta2->simbolo) {
                    *sugestaoPos1 = pos1;
                    *sugestaoPos2 = pos2;
                    printf("🤖 CORTEX detectou padrão nas posições %d e %d\n", pos1, pos2);
                    return;
                }
                carta2 = carta2->next;
                pos2++;
            }
        }
        carta1 = carta1->next;
        pos1++;
    }
    
    int totalCartas = contarCartasAtivas(estado->head);
    if (totalCartas >= 2) {
        *sugestaoPos1 = (rand() % totalCartas) + 1;
        do {
            *sugestaoPos2 = (rand() % totalCartas) + 1;
        } while (*sugestaoPos2 == *sugestaoPos1);
        
        printf("🎲 CORTEX está interferindo aleatoriamente...\n");
    }
}

void liberarMemoria(Carta **head) {
    if (!head || !*head) {
        return;
    }
    
    Carta *atual = *head;
    while (atual) {
        Carta *proximo = atual->next;
        free(atual);
        atual = proximo;
    }
    
    *head = NULL;
}

void posicaoParaGrid(int posicao, int cols, int *linha, int *coluna) {
    if (!linha || !coluna || posicao <= 0) {
        return;
    }
    
    int pos = posicao - 1;
    *linha = pos / cols;
    *coluna = pos % cols;
}

int gridParaPosicao(int linha, int coluna, int cols) {
    return (linha * cols + coluna) + 1;
}

Carta *obterCartaPorPos(Carta *head, int posicao) {
    if (!head || posicao <= 0) {
        return NULL;
    }
    
    Carta *atual = head;
    int pos = 1;
    
    while (atual && pos < posicao) {
        atual = atual->next;
        pos++;
    }
    
    return atual;
}

int contarCartasAtivas(Carta *head) {
    int count = 0;
    Carta *atual = head;
    
    while (atual) {
        if (atual->estado != REMOVIDA) {
            count++;
        }
        atual = atual->next;
    }
    
    return count;
}

void embaralharCartas(Carta **head) {
    if (!head || !*head) {
        return;
    }
    
    /* Conta cartas */
    int total = 0;
    Carta *atual = *head;
    while (atual) {
        total++;
        atual = atual->next;
    }
    
    if (total <= 1) {
        return;
    }
    
    Carta **array = (Carta **)malloc(sizeof(Carta *) * total);
    if (!array) {
        return;
    }
    
    atual = *head;
    for (int i = 0; i < total; i++) {
        array[i] = atual;
        atual = atual->next;
    }
    
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        trocarConteudoNos(array[i], array[j]);
    }
    
    free(array);
}

void imprimirEstadoCartas(Carta *head) {
    printf("\n=== ESTADO DOS FRAGMENTOS DE MEMÓRIA ===\n");
    
    Carta *atual = head;
    int pos = 1;
    
    while (atual) {
        printf("Pos %2d: ID=%d, Símbolo=%d, Estado=%d\n", 
               pos, atual->id, atual->simbolo, atual->estado);
        atual = atual->next;
        pos++;
    }
    
    printf("=========================================\n\n");
}

int calcularScore(EstadoJogo *estado) {
    if (!estado) {
        return 0;
    }
    
    int scoreBase = estado->paresEncontrados * 1000;
    int penalTempo = (int)(estado->tempoJogo * 10);
    int penalTentativas = estado->tentativas * 50;
    int bonusCortex = estado->cortexAtivo ? 500 : 0;
    
    int scoreFinal = scoreBase - penalTempo - penalTentativas + bonusCortex;
    return (scoreFinal > 0) ? scoreFinal : 0;
}