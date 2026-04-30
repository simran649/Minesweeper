#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "game.h"

void drawGrid(sf::RenderWindow& window, Minesweeper& game);

int main() {
    srand(time(0));

    Minesweeper game;
    game.initialization();
    game.placingmines();
    game.calculateno();

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(400, 400)), "Minesweeper");

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    if(auto mouse = event->getIf<sf::Event::MouseButtonPressed>()){
                        
                        int x = mouse->position.x;        
                        int y = mouse->position.y;
                        
                        int row = y / 40;
                        int col = x / 40;
                        
                        game.floodfill(row, col);
                        
                        std::cout << "Clicked: " << row << ", " << col << std::endl;
                    }
                }
        }

        window.clear(sf::Color::Blue);

        drawGrid(window, game);

        window.display();
    }
}