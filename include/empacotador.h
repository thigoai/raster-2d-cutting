#ifndef EMPACOTADOR_HPP
#define EMPACOTADOR_HPP

#include <string>
#include "grid.h"

struct Componente {
    int id_molde;
    Grid grid;
    int area_retangulo;
};

bool colide_retangulo(const Grid& folha, const Grid& peca, int pos_x, int pos_y);
bool colide_poligono(const Grid& folha, const Grid& peca, int pos_x, int pos_y);
void fixar_peca(Grid& folha, const Grid& peca, int pos_x, int pos_y, int id_molde);
void alocar_componente(Grid& folha, const Componente& comp, int& comprimento_usado);
void exportar_ppm(const Grid& folha, int comprimento_usado, const std::string& nome_arquivo);

#endif // EMPACOTADOR_HPP