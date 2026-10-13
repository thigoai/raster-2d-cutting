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

Rode a partir da raiz do projeto (a instância é lida de `dataset/swim.txt`):

```bash
./build/empacotador
```

O programa imprime o comprimento usado e a eficiência do layout, e gera as imagens `outputs/layout_swim.ppm` e `outputs/layout_swim.png`.

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
