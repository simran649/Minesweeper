#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "game.h"

bool firstMove = true;
bool gameOver = false;
bool gameWon = false;
void drawGrid(sf::RenderWindow& window, Minesweeper& game, bool gameOver, bool gameWon);

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
                if (event->is<sf::Event::MouseButtonPressed>() && !gameOver && !gameWon){
                    if(auto mouse = event->getIf<sf::Event::MouseButtonPressed>()){
                        
                        int x = mouse->position.x;        
                        int y = mouse->position.y;
                        
                        int row = y / 40;
                        int col = x / 40;

                        if(mouse->button == sf::Mouse::Button::Left){
                            game.handleClick(row, col, firstMove, gameOver);
                            if(!gameOver && game.checkwin()){
                                gameWon = true;
                            }
                        }
                        else if(mouse->button == sf::Mouse::Button::Right){
                            game.toggleFlag(row, col);
                        }
                    }
                }
        }

        window.clear(sf::Color::Blue);

        drawGrid(window, game, gameOver, gameWon);

        window.display();
    }
}