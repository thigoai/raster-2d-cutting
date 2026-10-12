#include "empacotador.h"
#include <fstream>
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

void exportar_ppm(const Grid& folha, int comprimento_usado, const std::string& nome_arquivo) {
    std::ofstream img(nome_arquivo);
    int larg = folha.get_largura();

    // Cabeçalho PPM (P3 = RGB em texto)
    // Largura da imagem = comprimento_usado (eixo horizontal)
    // Altura da imagem = largura da folha (eixo vertical)
    img << "P3\n" << comprimento_usado << " " << larg << "\n255\n";

    for (int x = larg - 1; x >= 0; --x) { // De cima para baixo
        for (int y = 0; y < comprimento_usado; ++y) { // Da esquerda para a direita
            int val = folha.get_valor(x, y);
            if (val == 0) {
                img << "20 20 20 "; // Fundo escuro para o tecido vazio
            } else {
                // Gera uma cor diferente baseada no ID do molde
                int r = (val * 73) % 200 + 55;
                int g = (val * 151) % 200 + 55;
                int b = (val * 211) % 200 + 55;
                img << r << " " << g << " " << b << " ";
            }
        }
        img << "\n";
    }
    img.close();
}