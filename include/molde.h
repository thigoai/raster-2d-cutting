#ifndef MOLDE_HPP
#define MOLDE_HPP

#include <vector>
#include <fstream>
#include "grid.h"

struct Ponto {
    double x, y;
};

struct Molde {
    int id;
    int quant;
    std::vector<Ponto> vertices;
    double largura, comprimento;
    double min_x, min_y;
};

bool ler_molde(std::ifstream& arquivo, Molde& molde);
bool contem_ponto(double px, double py, const std::vector<Ponto>& vertices);
Grid rasterizar_contorno(const Molde& molde, double escala);
void preencher_poligono(Grid& grid_molde, const Molde& molde, double escala);
Grid rasterizar_molde(const Molde& molde, double escala = 1.0);

#endif // MOLDE_HPP