#include "conversor.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <limits>

namespace fs = std::filesystem;

static bool ler_inteiro_cabecalho(std::ifstream& in, int& valor) {
    in >> std::ws;
    while (in.peek() == '#') {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        in >> std::ws;
    }
    return static_cast<bool>(in >> valor);
}

bool ler_ppm(const std::string& caminho, Imagem& imagem) {
    std::ifstream in(caminho, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Erro ao abrir " << caminho << std::endl;
        return false;
    }

    std::string formato;
    in >> formato;
    if (formato != "P3" && formato != "P6") {
        std::cerr << "Formato PPM nao suportado: " << formato << std::endl;
        return false;
    }

    int maxval;
    if (!ler_inteiro_cabecalho(in, imagem.largura) ||
        !ler_inteiro_cabecalho(in, imagem.altura) ||
        !ler_inteiro_cabecalho(in, maxval) ||
        imagem.largura <= 0 || imagem.altura <= 0 || maxval <= 0 || maxval > 255) {
        std::cerr << "Cabecalho PPM invalido em " << caminho << std::endl;
        return false;
    }

    size_t total = static_cast<size_t>(imagem.largura) * imagem.altura * 3;
    imagem.pixels.resize(total);

    if (formato == "P3") {
        for (size_t i = 0; i < total; ++i) {
            int v;
            if (!(in >> v)) {
                std::cerr << "Dados PPM incompletos em " << caminho << std::endl;
                return false;
            }
            imagem.pixels[i] = static_cast<uint8_t>(v * 255 / maxval);
        }
    } else {
        in.get();
        in.read(reinterpret_cast<char*>(imagem.pixels.data()), total);
        if (static_cast<size_t>(in.gcount()) != total) {
            std::cerr << "Dados PPM incompletos em " << caminho << std::endl;
            return false;
        }
        if (maxval != 255) {
            for (auto& p : imagem.pixels) p = static_cast<uint8_t>(p * 255 / maxval);
        }
    }
    return true;
}

static uint32_t crc32(const uint8_t* dados, size_t n, uint32_t crc = 0) {
    static uint32_t tabela[256];
    static bool iniciada = false;
    if (!iniciada) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            tabela[i] = c;
        }
        iniciada = true;
    }
    crc = ~crc;
    for (size_t i = 0; i < n; ++i) crc = tabela[(crc ^ dados[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

static uint32_t adler32(const std::vector<uint8_t>& dados) {
    uint32_t a = 1, b = 0;
    for (uint8_t d : dados) {
        a = (a + d) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

static void escrever_u32(std::vector<uint8_t>& out, uint32_t v) {
    out.push_back(v >> 24);
    out.push_back(v >> 16);
    out.push_back(v >> 8);
    out.push_back(v);
}

static void escrever_chunk(std::ofstream& out, const char tipo[4], const std::vector<uint8_t>& dados) {
    std::vector<uint8_t> buf;
    escrever_u32(buf, static_cast<uint32_t>(dados.size()));
    buf.insert(buf.end(), tipo, tipo + 4);
    buf.insert(buf.end(), dados.begin(), dados.end());
    escrever_u32(buf, crc32(buf.data() + 4, buf.size() - 4));
    out.write(reinterpret_cast<const char*>(buf.data()), buf.size());
}

bool salvar_png(const Imagem& imagem, const std::string& caminho) {
    std::ofstream out(caminho, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Erro ao criar " << caminho << std::endl;
        return false;
    }

    const uint8_t assinatura[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    out.write(reinterpret_cast<const char*>(assinatura), 8);

    std::vector<uint8_t> ihdr;
    escrever_u32(ihdr, imagem.largura);
    escrever_u32(ihdr, imagem.altura);
    ihdr.insert(ihdr.end(), {8, 2, 0, 0, 0});
    escrever_chunk(out, "IHDR", ihdr);

    size_t bytes_linha = static_cast<size_t>(imagem.largura) * 3;
    std::vector<uint8_t> cru;
    cru.reserve((bytes_linha + 1) * imagem.altura);
    for (int y = 0; y < imagem.altura; ++y) {
        cru.push_back(0);
        auto inicio = imagem.pixels.begin() + y * bytes_linha;
        cru.insert(cru.end(), inicio, inicio + bytes_linha);
    }

    std::vector<uint8_t> idat = {0x78, 0x01};
    const size_t MAX_BLOCO = 65535;
    size_t pos = 0;
    do {
        size_t n = std::min(MAX_BLOCO, cru.size() - pos);
        bool ultimo = (pos + n == cru.size());
        idat.push_back(ultimo ? 1 : 0);
        idat.push_back(n & 0xFF);
        idat.push_back(n >> 8);
        idat.push_back(~n & 0xFF);
        idat.push_back((~n >> 8) & 0xFF);
        idat.insert(idat.end(), cru.begin() + pos, cru.begin() + pos + n);
        pos += n;
    } while (pos < cru.size());
    escrever_u32(idat, adler32(cru));
    escrever_chunk(out, "IDAT", idat);

    escrever_chunk(out, "IEND", {});
    return static_cast<bool>(out);
}

bool converter_ppm_para_png(const std::string& caminho_ppm, const std::string& dir_saida) {
    Imagem imagem;
    if (!ler_ppm(caminho_ppm, imagem)) return false;

    std::error_code ec;
    fs::create_directories(dir_saida, ec);
    if (ec) {
        std::cerr << "Erro ao criar diretorio " << dir_saida << ": " << ec.message() << std::endl;
        return false;
    }

    fs::path destino = fs::path(dir_saida) / fs::path(caminho_ppm).stem();
    destino += ".png";
    if (!salvar_png(imagem, destino.string())) return false;

    std::cout << caminho_ppm << " -> " << destino.string() << std::endl;
    return true;
}

int converter_todos_ppm(const std::string& dir_entrada, const std::string& dir_saida) {
    int convertidos = 0;
    std::error_code ec;
    for (const auto& entrada : fs::directory_iterator(dir_entrada, ec)) {
        if (entrada.is_regular_file() && entrada.path().extension() == ".ppm") {
            if (converter_ppm_para_png(entrada.path().string(), dir_saida)) convertidos++;
        }
    }
    if (ec) std::cerr << "Erro ao listar " << dir_entrada << ": " << ec.message() << std::endl;
    return convertidos;
}
