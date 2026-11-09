#!/bin/bash

g++ -std=c++17 main.cpp game.cpp -o game \
    $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    ./game
else
    echo "Build failed!"
fi