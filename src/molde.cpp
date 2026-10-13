#include "molde.h"
#include <string>
#include <limits>
#include <cmath>

bool ler_molde(std::ifstream& arquivo, Molde& molde) {
    std::string m;
    if (!(arquivo >> m)) return false;

    std::string aux_id, aux_quant, aux_num_vertices;

    arquivo >> aux_id;
    arquivo >> m >> aux_quant;

    arquivo >> m >> m >> m >> aux_num_vertices;
    arquivo >> m >> m;

    int num_vertices = stoi(aux_num_vertices);
    molde.id = stoi(aux_id);
    molde.quant = stoi(aux_quant);

    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();

    molde.vertices.clear();

    for (int i = 0; i < num_vertices; ++i) {
        Ponto p;
        std::string aux_px, aux_py;
        
        arquivo >> aux_px >> aux_py;

        p.x = stoi(aux_px);
        p.y = stoi(aux_py);
        
        molde.vertices.push_back(p);

        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
    }

    molde.largura = max_x - min_x;
    molde.comprimento = max_y - min_y;
    molde.min_x = min_x;
    molde.min_y = min_y;

    return true;
}

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