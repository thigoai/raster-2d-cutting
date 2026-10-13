#include "empacotador.h"
#include <algorithm>

// primeiro estagio: verificar retangulo envolvente da peça
bool colide_retangulo(const Grid& folha, const Grid& peca, int pos_x, int pos_y) {
    if (pos_x < 0 || pos_y < 0 ||
        pos_x + peca.get_largura() > folha.get_largura() ||
        pos_y + peca.get_comprimento() > folha.get_comprimento()) {
        return true;
    }

    for (int i = 0; i < peca.get_comprimento(); ++i) {
        for (int j = 0; j < peca.get_largura(); ++j) {
            if (folha.get_valor(pos_x + j, pos_y + i)) return true;
        }
    }
    return false;
}

// verifica se colide com outro retangulo
bool colide_poligono(const Grid& folha, const Grid& peca, int pos_x, int pos_y) {
    if (pos_x < 0 || pos_y < 0 ||
        pos_x + peca.get_largura() > folha.get_largura() ||
        pos_y + peca.get_comprimento() > folha.get_comprimento()) {
        return true;
    }

    for (int i = 0; i < peca.get_comprimento(); ++i) {
        for (int j = 0; j < peca.get_largura(); ++j) {
            if (peca.get_valor(j, i) != 0 && folha.get_valor(pos_x + j, pos_y + i)) return true;
        }
    }
    return false;
}

void fixar_peca(Grid& folha, const Grid& peca, int pos_x, int pos_y, int id_molde) {
    for (int i = 0; i < peca.get_comprimento(); ++i) {
        for (int j = 0; j < peca.get_largura(); ++j) {
            if (peca.get_valor(j, i) != 0) {
                folha.set_valor(pos_x + j, pos_y + i, id_molde + 1);
            }
        }
    }
}

void alocar_componente(Grid& folha, const Componente& comp, int& comprimento_usado) {
    int pos_x = folha.get_largura() - comp.grid.get_largura();
    int pos_y = comprimento_usado;

    // ESTÁGIO 1: Retângulo envolvente
    bool moveu = true;
    while (moveu) {
        moveu = false;
        while (pos_y > 0 && !colide_retangulo(folha, comp.grid, pos_x, pos_y - 1)) {
            pos_y--;
            moveu = true;
        }
        while (pos_x > 0 && !colide_retangulo(folha, comp.grid, pos_x - 1, pos_y)) {
            pos_x--;
            moveu = true;
        }
    }

    // ESTÁGIO 2: Forma poligonal real
    moveu = true;
    while (moveu) {
        moveu = false;
        while (pos_y > 0 && !colide_poligono(folha, comp.grid, pos_x, pos_y - 1)) {
            pos_y--;
            moveu = true;
        }
        while (pos_x > 0 && !colide_poligono(folha, comp.grid, pos_x - 1, pos_y)) {
            pos_x--;
            moveu = true;
        }
    }

    fixar_peca(folha, comp.grid, pos_x, pos_y, comp.id_molde);
    comprimento_usado = std::max(comprimento_usado, pos_y + comp.grid.get_comprimento());
}
