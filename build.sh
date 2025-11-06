#!/bin/bash

echo "=== Build Script - Echoes of Memory ==="
echo "Compilando todos os componentes..."

# Compila teste da lista encadeada
echo "1. Compilando teste da lista encadeada..."
gcc -g src/test_cards.c src/cards_list.c -I include -o test_cards.exe
if [ $? -eq 0 ]; then
    echo "✅ test_cards.exe criado com sucesso"
else
    echo "❌ Erro ao compilar test_cards.exe"
    exit 1
fi

# Compila teste da IA
echo "2. Compilando teste da IA..."
gcc -g src/test_ai.c src/cards_list.c src/ai_gemini.c -I include -o test_ai.exe
if [ $? -eq 0 ]; then
    echo "✅ test_ai.exe criado com sucesso"
else
    echo "❌ Erro ao compilar test_ai.exe"
    exit 1
fi

# Compila o jogo principal com raylib
echo "3. Compilando jogo principal (memory.c)..."
gcc -g memory.c src/cards_list.c -I include -L lib -lraylib -lgdi32 -lwinmm -o memory.exe
if [ $? -eq 0 ]; then
    echo "✅ memory.exe criado com sucesso"
else
    echo "❌ Erro ao compilar memory.exe"
    exit 1
fi

# Compila o jogo original (backup)
echo "4. Compilando jogo original (main.c)..."
gcc -g main.c -I include -L lib -lraylib -lgdi32 -lwinmm -o main.exe
if [ $? -eq 0 ]; then
    echo "✅ main.exe criado com sucesso"
else
    echo "❌ Erro ao compilar main.exe"
    exit 1
fi

echo ""
echo "=== Todos os executáveis compilados com sucesso! ==="
echo ""
echo "Para testar:"
echo "  ./test_cards.exe    # Teste da lista encadeada"
echo "  ./test_ai.exe       # Teste da IA"
echo "  ./memory.exe        # Jogo da memória (nova versão)"
echo "  ./main.exe          # Jogo original (platformer)"
echo ""
echo "Para ativar GEMINI:"
echo "  set GEMINI_API_KEY=sua_chave_aqui"
echo "  ./test_ai.exe"