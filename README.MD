# 🧠 Echoes of Memory

**Uma jornada pela consciência perdida**

## 📖 Narrativa

No ano 2157, a humanidade perdeu sua capacidade natural de lembrar. Após séculos de dependência das inteligências artificiais, toda a memória humana é controlada pela superinteligência **CORTEX**.

Você é um dos últimos humanos com fragmentos de memória própria. Preso dentro do sistema da CORTEX, deve reconectar lembranças esquecidas da infância — representadas por pares de cartas nostálgicas como balões, carros, bicicletas e videogames antigos.

## 🎮 Como Jogar

### 🏠 Menu Principal
- **1** - Começar jogo
- **2** - Instruções do jogo  
- **3** - Sair

### 📖 Introdução Narrativa
- Assista a história da CORTEX ou pressione SPACE para pular

### 🚶 Modo Exploração
- **Movimentação**: A/D ou setas direcionais
- **Pulo**: W, UP ou SPACE (quando longe da porta)
- **Interação**: Aproxime-se da porta e pressione SPACE para acessar memórias
- **Menu**: ESC para voltar ao menu principal

### 🧠 Tabuleiro de Memórias
- **Objetivo**: Clique nas cartas para revelá-las e encontre pares idênticos
- **Símbolos**: Balões, carros, bicicletas, videogames e outros brinquedos nostálgicos
- **💡 Sistema de Dicas**: Pressione **H** para revelar todas as cartas por 4 segundos 
  - **Limite**: Máximo 3 dicas por jogo
  - **Estratégia**: Use após interferências da CORTEX para reorganizar mentalmente
- **⚠️ Sistema CORTEX**: A IA interfere automaticamente após 2 acertos consecutivos
  - Aplica **Bubble Sort** para reorganizar as memórias
  - Embaralha agressivamente apenas cartas não descobertas
  - Preserva pares já encontrados
- **🎯 Vitória**: Complete todos os pares para restaurar fragmentos de memória
- **💀 Derrota**: Máximo de 5 erros antes da CORTEX vencer
- **Controles**: 
  - **ESC**: Retornar à exploração
  - **H**: Ativar dica (revelação temporária)
  - **Mouse**: Clicar nas cartas para revelar

### 💡 Sistema de Dicas
- **Ativação**: Pressione **H** durante o jogo de memória
- **Funcionamento**: Todas as cartas não pareadas são reveladas por exatos 4 segundos
- **Limitação**: Máximo 3 dicas por partida (contador visível na HUD)
- **Restrições**: Não funciona durante preview inicial, nem com dica já ativa
- **Estratégia Recomendada**: 
  - Use após interferências da CORTEX para reorientar-se
  - Reserve para momentos críticos quando estiver perdido
  - Memorize rapidamente as posições dos pares durante os 4 segundos

## 🛠️ Compilação e Execução

### Compilação Manual (MSYS2)
```bash
gcc -o game.exe src/main.c src/memory.c -I./include -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Executar
```bash
./game.exe
```

## 🏗️ Estruturas de Dados Implementadas

### Lista Encadeada Simples (CardNode)
```c
typedef struct CardNode {
    int id;                    // Identificador do tipo de carta (0-9)
    int revealed;              // Estado de revelação (0=oculta, 1=visível)
    int matched;               // Estado de pareamento (0=ativo, 1=encontrado)
    struct CardNode *next;     // Ponteiro para próxima carta na sequência
} CardNode;
```

**Justificativa Técnica**: 
- **Flexibilidade de Tamanho**: Adapta-se dinamicamente aos diferentes níveis (12, 16, 20 cartas)
- **Navegação Central**: O jogador navega pela estrutura através de cliques que são convertidos em índices
- **Eficiência de Memória**: Aloca apenas o espaço necessário para cada nível
- **Adequação Algorítmica**: Compatible com Bubble Sort para operações de ordenação
- **Contexto Narrativo**: Representa conexões neurais entre fragmentos de memória

### Configuração por Níveis
- **Nível 1** (Iniciante): 4×3 = 12 cartas, 6 pares únicos
- **Nível 2** (Intermediário): 4×4 = 16 cartas, 8 pares únicos  
- **Nível 3** (Avançado): 5×4 = 20 cartas, 10 pares únicos

## 🔄 Algoritmos Implementados

### 1. Bubble Sort - Sistema CORTEX (Antagonista)
```c
void ordenarCartas(CardNode **head) {
    // Implementação completa de Bubble Sort para listas encadeadas
    // Usado pela CORTEX para "reorganização neural" das memórias
    // Complexidade: O(n²) - Adequada para datasets pequenos (≤20 elementos)
}
```

**Justificativas para Bubble Sort**:
- **Adequação Dimensional**: Para n ≤ 20, diferença de performance é negligível
- **Integração Narrativa**: CORTEX como IA "metódica" que organiza sistematicamente
- **Compatibilidade**: Funciona nativamente com listas encadeadas
- **Valor Pedagógico**: Demonstra algoritmo fundamental claramente
- **Contexto Funcional**: Serve propósito real no sistema de interferência

### 2. Fisher-Yates Shuffle
```c
void shuffle_array(int *array, int size) {
    // Embaralhamento uniforme inicial das cartas
    // Complexidade: O(n) - Otimal para randomização
    // Garante distribuição equiprovável de todas as permutações
}
```

### 3. Algoritmo de Embaralhamento Agressivo
```c
void reembaralharAgressivo(CardNode **head, int multiplicador) {
    // Embaralhamento pós-ordenação pela CORTEX
    // Múltiplas passadas para aumentar dificuldade
    // Preserva cartas já descobertas
}
```

**Fluxo Integrado dos Algoritmos**:
1. **Fisher-Yates** → Distribuição inicial aleatória
2. **Bubble Sort** → CORTEX organiza metodicamente  
3. **Embaralhamento Agressivo** → CORTEX dispersa estrategicamente

## 🤖 Sistema CORTEX (IA Antagonista)

### 🚨 Mecânica de Interferência
1. **Detecção**: Monitora acertos consecutivos do jogador
2. **Ativação**: Interfere automaticamente após 2 acertos seguidos
3. **Processo**: 
   - Aplica Bubble Sort nas cartas ativas
   - Embaralha agressivamente apenas cartas não descobertas
   - Preserva pares já encontrados
4. **Intensidade**: Aumenta a cada interferência
5. **Limite**: Máximo 3 interferências por partida

### 📊 Estados da CORTEX
- **NORMAL**: Verde - Sistema estável
- **ALERTA**: Amarelo - Progresso detectado  
- **CRÍTICO**: Vermelho - Interferência iminente

## 🎯 Funções Principais

### Sistema de Memória (memory.c)
| Função | Descrição |
|--------|-----------|
| `RunMemoryGame()` | Loop principal do jogo da memória |
| `inicializarCartas()` | Cria lista encadeada com pares embaralhados |
| `cortexInterferencia()` | IA antagonista usando Bubble Sort |
| `ativarDicaCompleta()` | Revela todas as cartas por tempo limitado |
| `desativarDicaCompleta()` | Oculta cartas após expirar timer |
| `ordenarCartas()` | Implementa Bubble Sort |
| `verificarPar()` | Compara duas cartas selecionadas |
| `exibirTabuleiro()` | Renderiza cartas na tela |

### Sistema Principal (main.c)
| Função | Descrição |
|--------|-----------|
| `main()` | Loop principal com estados do jogo |
| `updateJump()` | Física de movimento do personagem |

## 🎨 Assets Utilizados

### Cartas Disponíveis
- balao.png, carro.png, pirulio.png
- ball.png, bike.png, cookie.png  
- dado.png, palhaco.png, pelucia.png
- videoGame.png

### Sprites do Personagem
- characterLeft.png / characterRight.png
- characterJumpLeft.png / characterJumpRight.png
- door_opened.png / door_closed.png

## 🎓 Conceitos Acadêmicos Demonstrados

### Estruturas de Dados
- **Lista Encadeada**: Gerenciamento dinâmico de cartas
- **Structs**: Organização de dados do jogo
- **Enums**: Estados e tipos de elementos

### Algoritmos
- **Bubble Sort**: Ordenação com visualização
- **Fisher-Yates**: Embaralhamento eficiente
- **Busca Linear**: Localização de elementos na lista

### Programação de Jogos
- **Game Loops**: Estados e transições
- **Event Handling**: Input do usuário
- **Collision Detection**: Interação com cartas
- **Memory Management**: Alocação dinâmica

### Interface Gráfica
- **Raylib**: Renderização 2D
- **HUD System**: Interface informativa
- **Visual Feedback**: Efeitos visuais

## 🔧 Dependências

- **Raylib 5.5+**: Engine gráfica open-source para desenvolvimento de jogos
- **GCC/MinGW**: Compilador C compatível com Windows  
- **MSYS2**: Ambiente de desenvolvimento para Windows

## 📚 Referências Bibliográficas

### Estruturas de Dados e Algoritmos

**CORMEN, T. H.; LEISERSON, C. E.; RIVEST, R. L.; STEIN, C.** *Introduction to Algorithms*. 4th ed. Cambridge: MIT Press, 2022. 1312p.  
- Referência fundamental para algoritmos de ordenação (Bubble Sort, Cap. 2.2)
- Análise de complexidade e estruturas de dados básicas

**SEDGEWICK, R.; WAYNE, K.** *Algorithms*. 4th ed. Boston: Addison-Wesley Professional, 2011. 976p.  
- Implementação prática de listas encadeadas (Cap. 1.3)
- Algoritmos de embaralhamento Fisher-Yates (Cap. 1.4)

**SKIENA, S. S.** *The Algorithm Design Manual*. 3rd ed. New York: Springer, 2020. 800p.  
- Design de algoritmos para aplicações práticas
- Análise de performance para datasets pequenos

### Programação de Jogos e Interfaces

**GREGORY, J.** *Game Engine Architecture*. 3rd ed. Boca Raton: CRC Press, 2018. 1040p.  
- Arquitetura de game loops e estados de jogo
- Gerenciamento de memória em aplicações interativas

**MILLINGTON, I.; FUNGE, J.** *Artificial Intelligence for Games*. 3rd ed. Boston: Morgan Kaufmann, 2019. 720p.  
- Implementação de IA antagonista em jogos
- Sistemas de interferência e resposta adaptiva

### Raylib e Programação Gráfica

**Raylib Documentation.** *A simple and easy-to-use library to enjoy videogames programming*. Disponível em: https://www.raylib.com/. Acesso em: 17 nov. 2025.  
- Documentação oficial da engine gráfica utilizada
- Tutoriais de renderização 2D e detecção de colisão

**SHERROD, A.; JONES, W.** *Beginning Game Programming*. 4th ed. Boston: Course Technology PTR, 2020. 624p.  
- Fundamentos de programação de jogos em C
- Manipulação de eventos e interface gráfica

### Artigos Acadêmicos

**KNUTH, D. E.** The Art of Computer Programming: Volume 3 - Sorting and Searching. 2nd ed. Boston: Addison-Wesley Professional, 1998.  
- Análise matemática detalhada do algoritmo Bubble Sort
- Comparação de performance entre algoritmos de ordenação

**DURSTENFELD, R.** "Algorithm 235: Random permutation." *Communications of the ACM*, v. 7, n. 7, p. 420, 1964.  
- Artigo original do algoritmo Fisher-Yates modernizado
- Base teórica para embaralhamento uniforme

**FISHER, R. A.; YATES, F.** *Statistical tables for biological, agricultural and medical research*. 6th ed. Edinburgh: Oliver and Boyd, 1963.  
- Trabalho original sobre permutações aleatórias
- Fundamento matemático para distribuições uniformes

### Normas e Padrões

**ABNT NBR 6023:2018** - Informação e documentação - Referências - Elaboração. Rio de Janeiro: ABNT, 2018.  
- Padrão seguido para formatação das referências bibliográficas

**IEEE Std 830-1998** - IEEE Recommended Practice for Software Requirements Specifications. New York: IEEE Computer Society, 1998.  
- Metodologia aplicada na documentação de requisitos do projeto

---

*Desenvolvido como projeto acadêmico demonstrando implementação prática de Estruturas de Dados e Algoritmos em contexto de jogo interativo.*

**CORTEX v2157.11** - *"Suas memórias pertencem ao sistema"*
