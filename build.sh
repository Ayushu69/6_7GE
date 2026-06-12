#!/usr/bin/env bash

mkdir -p bin

g++ src/main.cpp \
src/core/Game.cpp \
src/entities/Player.cpp \
src/physics/Collision.cpp \
src/core/TextureManager.cpp \
src/core/Tilemap.cpp \
src/core/Animator.cpp \
-o bin/six_sevenGE.exe \
$(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer

echo "Build done ✅"