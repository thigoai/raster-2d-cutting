# raster-2d-cutting

Heurística construtiva em **C++** para o problema de Corte de Estoque Bidimensional Irregular.
As peças são rasterizadas em uma grade e alocadas na folha em ordem decrescente de área do retângulo envolvente.

## Requisitos

- CMake 3.10+
- Compilador com suporte a C++17

## Compilar

```bash
cmake -S . -B build
cmake --build build
```

## Executar

Rode a partir da raiz do projeto. Todas as instâncias (`.txt`) de `dataset/` são executadas, cada uma com a escala e a largura de folha definidas em `config/config.txt`:

```
dataset = ./dataset
shirts   = 10  40     # <instancia> = <escala> <largura_folha_real>
trousers = 10  79
swim     = 0.1 5752
```

```bash
./build/empacotador [caminho_config]
```

O programa imprime uma tabela markdown (também salva em `outputs/resultados.md`) com total de peças alocadas, comprimento na grelha, comprimento real (C_L), eficiência do layout e tempo de execução de cada instância, e gera as imagens `outputs/layout_<instancia>.png`.

## Estrutura

```
.
├── CMakeLists.txt
├── dataset            # Instâncias
│   ├── shirts.txt
│   ├── swim.txt
│   └── trousers.txt
├── include 
│   ├── empacotador.h
│   ├── grid.h
│   └── molde.h
├── README.md
└── src
    ├── empacotador.cpp
    ├── grid.cpp
    ├── main.cpp         
    └── molde.cpp

```
