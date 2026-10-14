#ifndef CONVERSOR_HPP
#define CONVERSOR_HPP

#include <string>
#include <vector>
#include <cstdint>

struct Imagem {
    int largura = 0;
    int altura = 0;
    std::vector<uint8_t> pixels; // RGB, 3 bytes por pixel, linha a linha
};

bool ler_ppm(const std::string& caminho, Imagem& imagem);
bool salvar_png(const Imagem& imagem, const std::string& caminho);

// Converte um .ppm para .png com o mesmo nome dentro de dir_saida (criado se não existir)
bool converter_ppm_para_png(const std::string& caminho_ppm, const std::string& dir_saida = "outputs");

// Converte todos os .ppm de dir_entrada; retorna quantos foram convertidos
int converter_todos_ppm(const std::string& dir_entrada, const std::string& dir_saida = "outputs");

#endif // CONVERSOR_HPP
