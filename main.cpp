#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>

void drawGrid(sf::RenderWindow& window);

int main() {
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(400, 400)), "Minesweeper");

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    auto mouse = event->getIf<sf::Event::MouseButtonPressed>();

                    int x = mouse->position.x;        
                    int y = mouse->position.y;
                    int row = y / 40;
                    int col = x / 40;
                    std::cout << "Clicked: " << row << ", " << col << std::endl;
                }
        }

        window.clear(sf::Color::Blue);

        drawGrid(window);

        window.display();
    }
}