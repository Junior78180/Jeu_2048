cmake_minimum_required(VERSION 3.20)
project(Jeu2048 C)

set(CMAKE_C_STANDARD 11)

# Indiquer où se trouvent SDL2
include_directories(${CMAKE_SOURCE_DIR}/SDL2/x86_64-w64-mingw32/include/SDL2)
link_directories(${CMAKE_SOURCE_DIR}/SDL2/x86_64-w64-mingw32/lib)

add_executable(Jeu2048 main.c)

# Linker SDL2
target_link_libraries(Jeu2048 SDL2)
