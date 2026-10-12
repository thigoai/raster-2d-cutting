#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

class Grid {
private:
    int largura;
    int comprimento;
    std::vector<int> dados;

public:
    Grid(int l, int c);

    int get_indice(int x, int y) const;
    int get_valor(int x, int y) const;
    void set_valor(int x, int y, int valor);

    int get_largura() const;
    int get_comprimento() const;

    void draw_line(int x1, int y1, int x2, int y2);
};

#endif // GRID_HPP