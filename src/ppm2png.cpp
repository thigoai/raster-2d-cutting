#include <iostream>
#include <string>

#include "conversor.h"

int main(int argc, char* argv[]) {
    std::string dir_entrada = argc > 1 ? argv[1] : ".";
    std::string dir_saida = argc > 2 ? argv[2] : "outputs";

    int convertidos = converter_todos_ppm(dir_entrada, dir_saida);
    std::cout << convertidos << " arquivo(s) convertido(s)." << std::endl;
    return convertidos > 0 ? 0 : 1;
}
