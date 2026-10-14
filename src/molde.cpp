#include "molde.h"
#include <cmath>

bool contem_ponto(double px, double py, const std::vector<Ponto>& vertices) {
    bool dentro = false;
    const int n = vertices.size();

    for (int i = 0, j = n - 1; i < n; j = i++) {
        const double xi = vertices[i].x, yi = vertices[i].y;
        const double xj = vertices[j].x, yj = vertices[j].y;

        bool intersecta = ((yi > py) != (yj > py)) &&
                          (px < (xj - xi) * (py - yi) / (yj - yi) + xi);
        if (intersecta) dentro = !dentro;
    }
    return dentro;
}

Grid rasterizar_contorno(const Molde& molde, double escala) {
    int largura_grid = std::ceil(molde.largura * escala) + 1;
    int comprimento_grid = std::ceil(molde.comprimento * escala) + 1;

    Grid grid_molde(largura_grid, comprimento_grid);
    const size_t n = molde.vertices.size();

    for (size_t i = 0; i < n; ++i) {
        Ponto p1 = molde.vertices[i];
        Ponto p2 = molde.vertices[(i + 1) % n];

        int x1 = std::round((p1.x - molde.min_x) * escala);
        int y1 = std::round((p1.y - molde.min_y) * escala);
        int x2 = std::round((p2.x - molde.min_x) * escala);
        int y2 = std::round((p2.y - molde.min_y) * escala);

        grid_molde.draw_line(x1, y1, x2, y2);
    }

    return grid_molde;
}

void preencher_poligono(Grid& grid_molde, const Molde& molde, double escala) {
    int largura_grid = grid_molde.get_largura();
    int comprimento_grid = grid_molde.get_comprimento();

    for (int y_grid = 0; y_grid < comprimento_grid; ++y_grid) {
        for (int x_grid = 0; x_grid < largura_grid; ++x_grid) {
            // Verifica se o píxel ainda está vazio
            if (grid_molde.get_valor(x_grid, y_grid) == 0) {
                // Converte coordenada discreta do Grid para coordenada contínua do Molde
                double real_x = (x_grid / escala) + molde.min_x;
                double real_y = (y_grid / escala) + molde.min_y;

                if (contem_ponto(real_x, real_y, molde.vertices)) {
                    grid_molde.set_valor(x_grid, y_grid, 1);
                }
            }
        }
    }
}

Grid rasterizar_molde(const Molde& molde, double escala) {
    Grid grid_molde = rasterizar_contorno(molde, escala);
    preencher_poligono(grid_molde, molde, escala);
    return grid_molde;
}