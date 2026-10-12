# 2D-irregular-cutting-solver

Implementação em **C++** de uma heurística construtiva para o problema de Corte de Estoque Bidimensional Irregular.

## Compilar e rodar

### Compilação

```bash
g++ -O3 -Iinclude src/main.cpp src/grid.cpp src/molde.cpp src/empacotador.cpp -o empacotador
./empacotador
```