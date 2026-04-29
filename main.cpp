#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(600, 400)), "SFML Works!");

    while (window.isOpen()) {

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Blue);
        window.display();
    }
}