#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Ponto{
    double x, y;
};

struct Molde{
    int id;
    int quant;
    std::vector<Ponto> vertices;
    double largura, comprimento;
};

bool ler_molde(std::ifstream& arquivo, Molde& molde){
    std::string m;

    if (!(arquivo >> m)) return false;

    arquivo >> molde.id;

    arquivo >> m >> molde.quant;

    int num_vertices;
    arquivo >> m >> m >> m >> num_vertices;

    arquivo >> m >> m;

    double min_x, max_x, min_y, max_y = 0;

    for (int i = 0; i < num_vertices; ++i){
        Ponto p;
        arquivo >> p.x >> p.y;
        molde.vertices.push_back(p);

        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
    }

    // Calcula a dimensão do molde 
    molde.largura = max_x - min_x;
    molde.comprimento = max_y - min_y;

    return true;
}


int main() {
    std::ifstream arquivo("./dataset/swim.txt");
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro." << std::endl;
        return 1;
    }

    Molde molde_atual;
    
    while (ler_molde(arquivo, molde_atual)) {
        std::cout << "Molde " << molde_atual.id << " {" << std::endl;
        std::cout << "- Quantidade: " << molde_atual.quant << std::endl;
        std::cout << "- Largura: " << molde_atual.largura << std::endl;
        std::cout << "- Comprimento: " << molde_atual.comprimento << 
                    "\n}\n" << std::endl;
        
        molde_atual.vertices.clear(); 
    }

    arquivo.close();
    return 0;
}



