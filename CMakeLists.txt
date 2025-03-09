cmake_minimum_required(VERSION 3.30)
project(chip8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(chip8
        src/definitions.h
        src/chip8.cpp
        src/definitions.cpp
        src/opcodes.cpp
        src/gui.cpp
        src/gui.h
)


INCLUDE(FindPkgConfig)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories (${SDL2_LIBRARY_DIRS})
target_link_libraries(chip8 ${SDL2_LIBRARIES} SDL2main)

# Define SDL_MAIN_HANDLED to avoid SDL's redefinition of main
target_compile_definitions(chip8 PRIVATE SDL_MAIN_HANDLED)