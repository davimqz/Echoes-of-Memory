# 🧠 Echoes of Memory

**Uma jornada pela consciência perdida**

## 📖 Narrativa

No ano 2157, a humanidade perdeu sua capacidade natural de lembrar. Após séculos de dependência das inteligências artificiais, toda a memória humana é controlada pela superinteligência **CORTEX**.

Você é um dos últimos humanos com fragmentos de memória própria. Preso dentro do sistema da CORTEX, deve reconectar lembranças esquecidas da infância — representadas por pares de cartas nostálgicas como piões, pipas, tazos e videogames antigos.

Cada par encontrado restaura um fragmento de sua consciência, enquanto a IA tenta dificultar o processo reorganizando constantemente seu "campo mental".

## 🎮 Como Jogar

### 🏠 Menu Principal
- Pressione ENTER para iniciar a jornada
- ESC para sair

### 📖 Introdução Narrativa
- Assista a história da CORTEX ou pressione SPACE para pular

### 🚶 Modo Exploração (NOVO!)
- **Movimentação**: WASD ou setas direcionais
- **Pulo**: W, ↑ ou SPACE (quando longe da porta)
- **Agachar**: S, ↓ ou C  
- **Interação**: Aproxime-se da porta e pressione SPACE para acessar memórias
- **Menu**: ESC para voltar ao menu principal

### 🧠 Tabuleiro de Memórias
- Clique nas cartas para revelá-las
- Encontre pares de símbolos nostálgicos idênticos
- A CORTEX reorganizará as cartas após alguns acertos
- Complete todos os pares para restaurar fragmentos de memória
- **Voltar**: ESC para retornar à exploração
- **Reiniciar**: R para recomeçar o nível

## 🛠️ Compilação e Execução

### Compilar o projeto completo:
```bash
gcc -o main.exe main.c src/cards.c -I include -L lib -lraylib -lgdi32 -lwinmm
```

### Ou usar o Makefile:
```bash
make all
```

### Executar:
```bash
./main.exe
```

### Ou usar o Makefile:
```bash
make run
```

## 🏗️ Estruturas de Dados e Algoritmos

### Lista Encadeada (Estrutura Central)
```c
typedef struct Carta {
    int id;                    // Identificador único
    TipoSimbolo simbolo;       // Tipo nostálgico (pião, pipa, etc.)
    EstadoCarta estado;        // OCULTA, REVELADA, REMOVIDA
    float tempoRevelado;       // Controle de tempo
    struct Carta *next;        // Próximo fragmento
} Carta;
```

**Justificativa**: A lista encadeada permite navegação sequencial pelos fragmentos de memória, representando como as lembranças se conectam na mente humana.

### Bubble Sort (Algoritmo de Ordenação)
Implementado na função `ordenarCartas()`, o Bubble Sort reorganiza as cartas após cada acerto:

```c
void ordenarCartas(Carta **head) {
    // Troca conteúdos dos nós (não ponteiros) para simplificar
    // Critério: símbolo primeiro, depois ID
}
```

**Justificativa**: 
- Simples de visualizar e entender
- Permite efeitos visuais da reorganização da CORTEX
- Demonstra conceitos de ordenação de forma didática

## 🎯 Funções Principais

| Função | Descrição |
|--------|-----------|
| `inicializarCartas()` | Cria lista com pares embaralhados usando Fisher-Yates |
| `exibirTabuleiro()` | Renderiza cartas na tela com Raylib |
| `escolherCarta()` | Marca carta como revelada baseado na posição |
| `verificarPar()` | Compara símbolos de duas cartas |
| `ordenarCartas()` | Reorganiza usando Bubble Sort |
| `jogadaIA()` | IA CORTEX interfere no jogo (preparado para Gemini) |
| `liberarMemoria()` | Limpeza de memória alocada |

## 🤖 Inteligência Artificial

A **CORTEX** possui comportamento adaptativo:
- Detecta padrões nas jogadas do usuário
- Reorganiza cartas estrategicamente após acertos
- Sistema preparado para integração com **Gemini API**
- Interfere progressivamente conforme o jogo avança

## 🎨 Interface Gráfica

- **Engine**: Raylib (OpenGL)
- **Tema**: Cyberpunk futurista vs. nostalgia da infância
- **Assets**: Sprites animados do personagem, background detalhado
- **Símbolos**: Piões, pipas, tazos, videogames, bolas, bonecas, carrinhos, quebra-cabeças
- **Efeitos**: Gradientes, animações, distorções durante interferência da CORTEX
- **Mundo de Exploração**: Cenário 2D com movimentação livre e objetos interativos

## 📁 Estrutura do Projeto

```
Echoes-of-Memory/
├── main.c              # Loop principal, estados do jogo e exploração
├── src/
│   ├── cards.h          # Definições e protótipos do sistema de cartas
│   └── cards.c          # Implementação da lógica de memória
├── assets/              # Recursos visuais
│   ├── bg.png          # Background do mundo de exploração
│   ├── characterLeft.png         # Sprite do jogador (esquerda)
│   ├── characterRight.png        # Sprite do jogador (direita)
│   ├── characterJumpLeft.png     # Sprite de pulo (esquerda)
│   ├── characterJumpRight.png    # Sprite de pulo (direita)
│   ├── characterAgachandoLeft.png # Sprite agachado (esquerda)
│   ├── characterAgachandoRight.png # Sprite agachado (direita)
│   └── cards/           # Sprites das cartas (futuro)
├── include/             # Headers do Raylib
├── lib/                 # Bibliotecas do Raylib
├── Makefile            # Sistema de build
└── README.md           # Esta documentação
```

## 🎓 Conceitos Acadêmicos Demonstrados

- **Estruturas de Dados**: Lista encadeada, enums, structs
- **Algoritmos de Ordenação**: Bubble Sort com visualização
- **Gerenciamento de Memória**: Alocação dinâmica, liberação adequada
- **Programação de Jogos**: Game loops, estados, máquinas de estado
- **Sistema de Input**: Mouse, teclado, diferentes contextos de entrada
- **Interface Gráfica**: Renderização 2D, sprites animados, efeitos visuais
- **Física 2D**: Movimento, gravidade, colisões básicas
- **Arquitetura de Software**: Separação de responsabilidades, modularização

## 🔧 Dependências

- **Raylib 5.5+**: Engine gráfica
- **GCC/MinGW**: Compilador C
- **Windows**: Sistema operacional alvo

## 🚀 Futuras Implementações

- [ ] Integração completa com Gemini API
- [ ] Sprites customizados para cada símbolo nostálgico
- [ ] Sistema de som e música temática
- [ ] Múltiplos níveis de dificuldade
- [ ] Sistema de save/load
- [ ] Modo multiplayer vs CORTEX

---

*Desenvolvido como projeto acadêmico do Módulo 02 - Estruturas de Dados e Algoritmos*

**CORTEX v2157.11** - *"Suas memórias pertencem ao sistema"*
