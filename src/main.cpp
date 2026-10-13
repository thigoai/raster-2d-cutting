#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <filesystem>

#include "grid.h"
#include "molde.h"
#include "empacotador.h"
#include "conversor.h"
#include "io.h"

int main(int argc, char* argv[]) {
    const std::string caminho_config = (argc > 1) ? argv[1] : "./config/config.txt";

    Config config;
    if (!ler_config(caminho_config, config)) return 1;

    std::ifstream arquivo(config.arquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro: " << config.arquivo << std::endl;
        return 1;
    }

    const double ESCALA = config.escala;
    const double LARGURA_FOLHA_REAL = config.largura_folha_real;

    std::vector<Componente> lista_pecas;
    int soma_comprimentos_grid = 0;
    int area_total_ocupada_pelas_pecas = 0;

    Molde molde_atual;
    while (ler_molde(arquivo, molde_atual)) {
        Grid g = rasterizar_molde(molde_atual, ESCALA);

        int pixeis_peca = 0;
        for (int y = 0; y < g.get_comprimento(); ++y) {
            for (int x = 0; x < g.get_largura(); ++x) {
                if (g.get_valor(x, y) != 0) pixeis_peca++;
            }
        }

        for (int q = 0; q < molde_atual.quant; ++q) {
            Componente p{molde_atual.id, g, g.get_largura() * g.get_comprimento()};
            lista_pecas.push_back(p);
            soma_comprimentos_grid += g.get_comprimento();
            area_total_ocupada_pelas_pecas += pixeis_peca;
        }
    }
    arquivo.close();

    // 1. Ordenação decrescente pela área do retângulo envolvente
    std::sort(lista_pecas.begin(), lista_pecas.end(), [](const Componente& a, const Componente& b) {
        return a.area_retangulo > b.area_retangulo;
    });

    // 2. Cria a folha de tecido
    int largura_folha_grid = std::ceil(LARGURA_FOLHA_REAL * ESCALA);
    Grid folha(largura_folha_grid, soma_comprimentos_grid + 10);

    // 3. Empacotamento em duas etapas
    int comprimento_final_grid = 0;
    for (const auto& peca : lista_pecas) {
        alocar_componente(folha, peca, comprimento_final_grid);
    }

    // 4. Resultados
    double comprimento_real_usado = comprimento_final_grid / ESCALA;
    double eficiencia = static_cast<double>(area_total_ocupada_pelas_pecas) /
                        (largura_folha_grid * comprimento_final_grid) * 100.0;

    std::cout << "=== RESULTADO DO EMPACOTAMENTO ===" << std::endl;
    std::cout << "Total de pecas alocadas : " << lista_pecas.size() << std::endl;
    std::cout << "Comprimento na grelha   : " << comprimento_final_grid << " celulas" << std::endl;
    std::cout << "Comprimento real (C_L)  : " << comprimento_real_usado << std::endl;
    std::cout << "Eficiencia do layout    : " << eficiencia << "%" << std::endl;

    // Nome da saída derivado da instância (ex.: dataset/swim.txt -> outputs/layout_swim.ppm)
    const std::string dir_saida = "outputs";
    const std::string nome_ppm = dir_saida + "/layout_" + std::filesystem::path(config.arquivo).stem().string() + ".ppm";
    if (!exportar_ppm(folha, comprimento_final_grid, nome_ppm)) return 1;
    converter_ppm_para_png(nome_ppm, dir_saida);
    return 0;
}