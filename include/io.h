#ifndef IO_HPP
#define IO_HPP

#include <string>
#include <fstream>
#include <map>
#include "grid.h"
#include "molde.h"

struct ParametrosInstancia {
    double escala = 0.0;
    double largura_folha_real = 0.0;
};

struct Config {
    std::string dataset = "./dataset";
    // nome da instância (ex.: "shirts" para dataset/shirts.txt) -> parâmetros
    std::map<std::string, ParametrosInstancia> instancias;
};

// Lê um arquivo de configuração (linhas com # são comentários) no formato:
//   dataset = <diretorio>
//   <instancia> = <escala> <largura_folha_real>
bool ler_config(const std::string& caminho, Config& config);

bool ler_molde(std::ifstream& arquivo, Molde& molde);

// Cria o diretório de destino se necessário; retorna false em caso de erro
bool exportar_ppm(const Grid& folha, int comprimento_usado, const std::string& nome_arquivo);

#endif // IO_HPP
