cmake_minimum_required(VERSION 3.10)
project(raster-2d-cutting CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(empacotador
    src/main.cpp
    src/grid.cpp
    src/molde.cpp
    src/empacotador.cpp
    src/conversor.cpp
)

target_include_directories(empacotador PRIVATE include)

# Converte todos os .ppm de um diretório (padrão: ".") para outputs/*.png
add_executable(ppm2png
    src/ppm2png.cpp
    src/conversor.cpp
)

target_include_directories(ppm2png PRIVATE include)
