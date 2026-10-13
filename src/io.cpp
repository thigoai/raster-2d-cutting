#include "io.h"
#include <iostream>
#include <limits>
#include <filesystem>

static std::string aparar(const std::string& s) {
    const char* espacos = " \t\r\n";
    size_t inicio = s.find_first_not_of(espacos);
    if (inicio == std::string::npos) return "";
    size_t fim = s.find_last_not_of(espacos);
    return s.substr(inicio, fim - inicio + 1);
}

bool ler_config(const std::string& caminho, Config& config) {
    std::ifstream in(caminho);
    if (!in.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de configuracao: " << caminho << std::endl;
        return false;
    }

    bool tem_arquivo = false, tem_escala = false, tem_largura = false;
    std::string linha;
    int num_linha = 0;

    while (std::getline(in, linha)) {
        ++num_linha;
        linha = aparar(linha.substr(0, linha.find('#')));
        if (linha.empty()) continue;

        size_t igual = linha.find('=');
        if (igual == std::string::npos) {
            std::cerr << caminho << ":" << num_linha << ": linha invalida (esperado chave = valor)" << std::endl;
            return false;
        }

        std::string chave = aparar(linha.substr(0, igual));
        std::string valor = aparar(linha.substr(igual + 1));

        try {
            if (chave == "arquivo") {
                config.arquivo = valor;
                tem_arquivo = true;
            } else if (chave == "escala") {
                config.escala = std::stod(valor);
                tem_escala = true;
            } else if (chave == "largura_folha_real") {
                config.largura_folha_real = std::stod(valor);
                tem_largura = true;
            } else {
                std::cerr << caminho << ":" << num_linha << ": chave desconhecida '" << chave << "'" << std::endl;
                return false;
            }
        } catch (const std::exception&) {
            std::cerr << caminho << ":" << num_linha << ": valor invalido para '" << chave << "'" << std::endl;
            return false;
        }
    }

    if (!tem_arquivo || !tem_escala || !tem_largura) {
        std::cerr << "Configuracao incompleta em " << caminho
                  << " (necessario: arquivo, escala, largura_folha_real)" << std::endl;
        return false;
    }
    if (config.escala <= 0 || config.largura_folha_real <= 0) {
        std::cerr << "escala e largura_folha_real devem ser positivos" << std::endl;
        return false;
    }
    return true;
}

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

bool exportar_ppm(const Grid& folha, int comprimento_usado, const std::string& nome_arquivo) {
    std::filesystem::path dir = std::filesystem::path(nome_arquivo).parent_path();
    if (!dir.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            std::cerr << "Erro ao criar diretorio " << dir.string() << ": " << ec.message() << std::endl;
            return false;
        }
    }

    std::ofstream img(nome_arquivo);
    if (!img.is_open()) {
        std::cerr << "Erro ao criar " << nome_arquivo << std::endl;
        return false;
    }
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
    return static_cast<bool>(img);
}
