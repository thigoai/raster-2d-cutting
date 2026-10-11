#include <iostream>
#include <vector>
#include <cmath>

class Grid {

    private:
        int largura;
        int comprimento;
        std::vector<int> dados;

    public:
        Grid(int l, int c): largura(l), comprimento(c), dados(l * c, 0){}
        int get_valor(int x, int y) const {
            return y * largura + x;
        }
        int get_largura() const { return largura; }
        int get_comprimento() const { return comprimento; }
        
        void set_valor(int x, int y, int valor){
            if (x >= 0 && x< largura && y >= 0 && y < comprimento){
                dados[get_valor(x, y)] = valor;
            }
        }
        // Algoritmo de Bresenham para traçar linhas 
        void draw_line(int x1, int y1, int x2, int y2){
            int dx = std::abs(x2 - x1);
            int dy = std::abs(y2 - y1);
            int sx = (x1 < x2)? 1 : -1;
            int sy = (y1 < y2)? 1 : -1;
            int err = dx - dy;

            while(true){
                set_valor(x1, y1, 1);

                if (x1 == x2 && y1 == y2) break;

                int e2 = 2 * err;
                if (e2 > -dy){
                    err -= dy;
                    x1 += sx;
                }
                if (e2 < dx){
                    err += dx;
                    y1 += sy;
                }
            }   
        }
};
