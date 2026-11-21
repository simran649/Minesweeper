#!/bin/bash

g++ -std=c++17 main.cpp game.cpp audio.cpp -o game \
    $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system sfml-audio)

./game

if [ $? -eq 0 ]; then
    echo "Build successful!"
else
    echo "Build failed!"
fi