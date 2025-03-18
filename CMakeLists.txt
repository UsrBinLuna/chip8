cmake_minimum_required(VERSION 3.30)
project(chip8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-g -O0 -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic -m64")
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -m64")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

add_executable(chip8
        include/tinyfiledialogs.c
        src/definitions.h
        src/chip8.cpp
        src/definitions.cpp
        src/opcodes.cpp
        src/gui.cpp
        src/gui.h
        src/sound.h
        src/font.cpp
)

add_link_options(-static -static-libgcc -static-libstdc++)

INCLUDE(FindPkgConfig)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# SDL2 stuff
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(ZLIB REQUIRED zlib)
pkg_check_modules(BZIP2 REQUIRED bzip2)
pkg_check_modules(GRAPHITE2 REQUIRED graphite2)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
link_directories (${SDL2_LIBRARY_DIRS} ${SDL2_TTF_LIBRARY_DIRS})
set(SDL2_LIBS -lSDL2main -lSDL2 -lSDL2_ttf -lmingw32)
target_link_libraries(chip8
        -Wl,-Bstatic
        -lstdc++
        -Wl,-Bdynamic
        -lmingw32
        -lSDL2main
        -Wl,-Bstatic
        -lSDL2
        -lSDL2_ttf
        -lfreetype
        -lharfbuzz
        -lgraphite2
        -lpng
        -lbrotlienc
        -lbrotlidec
        -lbrotlicommon
        -lz
        -lbz2
        -lpthread
        -Wl,-Bdynamic
        -lopengl32
        -lgdi32
        -lwinmm
        -limm32
        -lole32
        -loleaut32
        -lsetupapi
        -ldxguid
        -lversion
        -luuid
        -lrpcrt4
)

target_link_libraries(chip8 ${SDL2_TTF_LIBRARIES} SDL2main SDL2_ttf)

# Define SDL_MAIN_HANDLED to avoid SDL's redefinition of main
target_compile_definitions(chip8 PRIVATE SDL_MAIN_HANDLED)