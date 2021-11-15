cmake_minimum_required(VERSION 3.18)
project(Projet C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(Projet
        src/bmp.c
        src/bmp.h
        src/client.c
        src/client.h
        src/couleur.c
        src/couleur.h
        src/serveur.c
        src/serveur.h)
