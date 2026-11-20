#!/bin/bash

g++ -std=c++20 main.cpp game.cpp audio.cpp -o game \
    $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system sfml-audio)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    ./game
else
    echo "Build failed!"
fi