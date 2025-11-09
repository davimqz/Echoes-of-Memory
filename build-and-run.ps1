# build-and-run.ps1
# Script para compilar e executar usando MSYS2 MinGW64 gcc
# Execute este script no PowerShell (ou no terminal MSYS2 MinGW64)

$gccPath = "C:\msys64\mingw64\bin\gcc.exe"
# possíveis nomes da DLL (algumas builds usam 'raylib.dll' ou 'libraylib.dll')
$possibleDlls = @("C:\msys64\mingw64\bin\libraylib.dll", "C:\msys64\mingw64\bin\raylib.dll")
$projectDir = Get-Location
$source = "main.c"
$exe = "main.exe"

Write-Host "Diretório: $projectDir"

if (-not (Test-Path $gccPath)) {
    Write-Error "gcc não encontrado em $gccPath. Abra o terminal MSYS2 MinGW64 ou instale MSYS2."
    exit 1
}

Write-Host "Usando compilador: $gccPath"

# Compilação
$compileCmd = "`"$gccPath`" $source -o $exe -IC:\msys64\mingw64\include -LC:\msys64\mingw64\lib -lraylib -lopengl32 -lgdi32 -lwinmm"
Write-Host "Compilando: $compileCmd"

# Executa o gcc diretamente e captura a saída (stdout+stderr) para exibir ao usuário
$argList = @($source, "-o", $exe, "-IC:\msys64\mingw64\include", "-LC:\msys64\mingw64\lib", "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm")
$procOutput = & $gccPath @argList 2>&1
$exit = $LASTEXITCODE

if ($procOutput) { Write-Host $procOutput }

if ($exit -ne 0) {
    Write-Error "Compilação falhou (exit code $exit). Veja a saída acima para detalhes."
    exit $exit
}

Write-Host "Compilação concluída: $exe"

# Copiar DLL se existir
## tenta localizar e copiar uma das DLLs possíveis
$dllFound = $null
foreach ($dll in $possibleDlls) {
    if (Test-Path $dll) { $dllFound = $dll; break }
}

if ($dllFound) {
    $dllName = [System.IO.Path]::GetFileName($dllFound)
    Write-Host "Copiando DLL encontrada: $dllName -> $projectDir"
    Copy-Item $dllFound -Destination (Join-Path $projectDir $dllName) -Force
} else {
    Write-Host "Nenhuma DLL da Raylib encontrada em MSYS2. Vou ajustar PATH temporariamente para procurar em tempo de execução..."
    $env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
}

# Executar
Write-Host "Executando .\$exe"
& .\$exe

exit 0
