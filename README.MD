# Echoes of Memory

## Nome do jogo
Echoes of Memory

## Introdução
"Echoes of Memory" é uma releitura futurista do clássico Jogo da Memória, criada no contexto da atividade prática do Módulo 02.

O projeto tem como público-alvo jovens e adultos interessados em tecnologia, ficção científica e jogos que misturam raciocínio lógico com elementos narrativos. Além disso, o jogo busca despertar a nostalgia das brincadeiras da infância enquanto introduz conceitos de estruturas de dados e algoritmos de ordenação de forma lúdica e aplicada.

### Narrativa
No futuro distante, os humanos perderam a capacidade natural de lembrar. Após séculos de dependência das IAs, a humanidade tornou-se incapaz de armazenar memórias por conta própria — a superinteligência CORTEX administra todas as lembranças humanas. 

O jogador é um dos poucos que ainda possui fragmentos de memória e, preso no sistema da CORTEX, deve reconectar lembranças representadas por pares de cartas (ícones da infância como piões, pipas, tazos e videogames). Cada par encontrado restaura um fragmento da consciência, enquanto a IA reorganiza o "campo mental" para dificultar a tarefa.

## Objetivo do jogo
Juntar todos os fragmentos de memória (achar todos os pares de cartas) para restaurar a consciência do jogador.

---

## Estruturas de dados (com justificativa)

### Estrutura do Centro - Lista Encadeada (Linked List)
- **Justificativa**: a lista encadeada permite navegação por posição e fácil reordenação (simulando o "campo mental" sendo reorganizado pela IA). Uma lista modela bem a ideia de um fluxo de memórias onde cada nó é uma carta/fragmento.
- **Uso**: cada nó representa uma carta com campos: id/identificador (tipo do símbolo), estado (oculta/revelada/marcada), e ponteiro para o próximo nó. A interface de exibição percorre a lista para desenhar o tabuleiro.

**Estrutura por nó:**
```c
struct CardNode {
    int id;              // identificador do símbolo (0..N-1)
    bool revealed;       // está temporariamente revelada
    bool matched;        // já foi combinada
    struct CardNode *next;
}
```

---

## Algoritmo de ordenação (com justificativa)

### Bubble Sort
- **Justificativa**: escolhido por ser simples de implementar e fácil de visualizar para fins didáticos. Permite demonstrar passo a passo as trocas entre nós adjacentes na lista encadeada, reforçando conceitos de complexidade e comportamento de algoritmos de ordenação.
- **Quando é aplicado**: após cada acerto (quando um par é encontrado), o Bubble Sort percorre a lista e realiza comparações/trocas baseadas em um critério (por exemplo, id da carta). Isso simula a "IA reorganizando memórias" em resposta ao sucesso do jogador.
- **Observação**: como Bubble Sort é O(n²), para pequenos tabuleiros (4x4 = 16 elementos) a performance é aceitável e mantém foco educativo.

**Pseudo-algoritmo:**
1. Repetir até nenhuma troca:
   2. Para cada par de nós adjacentes (A, B):
      - Se A->id > B->id, trocar A e B (ajustar ponteiros).
3. Final.

---

## Principais funções

### Funções principais do jogo
- `void inicializarCartas(int boardSize)` - Cria e preenche a lista encadeada com pares de cartas (embaralhados). Define estados iniciais (revealed=false, matched=false).

- `void exibirTabuleiro(struct CardNode *head)` - Percorre a lista e desenha as cartas na tela (posição calculada a partir do índice). Usa raylib para desenhar backs/foregrounds.

- `struct CardNode* escolherCarta(struct CardNode *head, int index)` - Retorna o ponteiro para a carta na posição informada (index baseado em 0..boardSize-1). Marca como temporariamente revelada.

- `int verificarPar(struct CardNode *a, struct CardNode *b)` - Compara os ids de duas cartas e retorna 1 se formam par, 0 caso contrário. Se formar par, marca ambos matched=true.

- `void ordenarCartas(struct CardNode **head)` - Implementa Bubble Sort na lista encadeada para reordenar as cartas conforme o id.

- `void jogadaIA(struct CardNode *head)` - Algoritmo simples que tenta prever a próxima jogada baseado no histórico de cartas reveladas. Implementação inicial como heurística simples; versão futura integra GEMINI.

- `void liberarMemoria(struct CardNode **head)` - Percorre e libera toda a memória alocada dinamicamente para a lista encadeada.

### Funções auxiliares
- `struct CardNode* node_at(struct CardNode *head, int index)` - Navega até o nó na posição especificada
- `void swap_adjacent_nodes(struct CardNode **head, struct CardNode *prev, struct CardNode *a, struct CardNode *b)` - Troca dois nós adjacentes na lista

---

## Interface gráfica

### Planejamento
- **Biblioteca**: raylib (já presente no workspace)
- **Layout**:
  - Resolução alvo: 1920x1080
  - Tabuleiro: grid 4x4 (padrão) com cartas desenhadas a partir do índice da lista encadeada
  - Menu inicial centralizado com opções: Jogar Agora, Escolher Dificuldade (Normal / Difícil), Sair
  - Animação: flip simples (troca de textura do back para o front); transição quando ordem muda (opcional)

### Arquivos de assets
- **Back**: `assets/cards/back/card.png`
- **Fronts**: `assets/cards/front/*.png`

---

## IA na dinâmica de jogo

### Implementação planejada
- **Simples**: "memória" baseada no histórico local — armazena índices de cartas já vistas e tenta combiná-las
- **Avançada**: integração com GEMINI para um agente que sugere jogadas ou aprende padrões

### Integração com GEMINI
- **Observação de privacidade**: integração com APIs externas exige manejo de chaves (não incluir chaves no repositório)
- **Configuração**: documentar como configurar GEMINI (variáveis de ambiente, rate-limiting)
- **Implementação**: stub inicial `jogadaIA()` com interface que permite trocar entre IA local e chamada à API GEMINI

---

## Casos de borda / Considerações

- **Arquivos de assets faltando**: fallback para nome ASCII e/ou desenhar placeholder
- **Performance**: Bubble Sort aceitável para ≤ 16 elementos
- **Concurrency**: single-threaded (UI + lógica), evitar chamadas de rede síncronas bloqueantes
- **API indisponível**: degradar para IA local quando GEMINI não estiver disponível

---

## Como compilar e executar

### Compilação (Windows + mingw/gcc)
```bash
# Compilar main.c (platformer original)
gcc -o main.exe main.c -I include -L lib -lraylib -lgdi32 -lwinmm

# Compilar memory.c (jogo da memória)
gcc -o memory.exe memory.c -I include -L lib -lraylib -lgdi32 -lwinmm

# Executar
./main.exe      # ou
./memory.exe
```

### Usando VS Code
Utilize a task de build já configurada no workspace:
- Pressione `Ctrl+Shift+P`
- Digite "Tasks: Run Task"
- Selecione "C/C++: gcc.exe build active file"

---

## Testes recomendados

### Happy path
- Iniciar jogo, encontrar dois pares distintos, verificar que `ordenarCartas()` é chamado
- Completar o jogo e observar tela de vitória

### Edge cases
- Remover um asset frontal e verificar placeholder visível
- Simular erro na API GEMINI e verificar fallback para IA local

### Unit tests (opcional)
- Funções de lista encadeada e bubble sort em pequenos harnesses C

---

## Arquivos do projeto

### Estrutura atual
```
├── main.c              # Jogo principal (platformer)
├── memory.c            # Jogo da memória (atual)
├── README.md           # Este documento
├── assets/
│   └── cards/
│       ├── back/
│       │   └── card.png
│       └── front/
│           ├── ball.png
│           ├── bike.png
│           ├── cookie.png
│           ├── dado.png
│           ├── palhaço.png
│           ├── pelucia.png
│           └── videoGame.png
├── include/
│   ├── raylib.h
│   ├── raymath.h
│   └── rlgl.h
└── lib/
```

### Arquivos planejados
- `include/cards_list.h` - Definição de struct e protótipos
- `src/cards_list.c` - Implementação de lista encadeada e sort
- `src/ai_gemini.c` - Stub/integração com GEMINI

---

## Status do desenvolvimento

### Concluído
- ✅ Protótipo básico do jogo da memória com raylib
- ✅ Menu centralizado e seleção de dificuldade
- ✅ Sistema de assets com fallback para arquivos faltando

### Em desenvolvimento
- 🔄 Implementação de lista encadeada
- 🔄 Algoritmo Bubble Sort
- 🔄 Integração com estruturas de dados

### Planejado
- ⏳ Integração com API GEMINI
- ⏳ Testes automatizados
- ⏳ Integração com menu principal (`main.c`)

---

## Contribuição

Este projeto faz parte de uma atividade acadêmica do Módulo 02. Para contribuir:

1. Clone o repositório
2. Crie uma branch para sua feature
3. Implemente as mudanças seguindo a estrutura documentada
4. Teste localmente
5. Submeta um pull request
---

## Licença

Projeto acadêmico - uso educacional.
