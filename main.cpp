#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <string>
#include "game.h"

const int WIDTH = 400;
const int HEIGHT = 500;
const int TILE_SIZE = 40;
const int BOARD_SIZE = 10;
const int BOARD_X = 0;
const int BOARD_Y = 60;

bool firstMove = true;
bool gameOver = false;
bool gameWon = false;

enum class GameState {
    Playing,
    Exploding,
    RevealingMines,
    Lost,
    Won
};

GameState gameState = GameState::Playing;

sf::Clock explosionClock;
sf::Clock revealClock;

int explosionRow = -1;
int explosionCol = -1;
int revealIndex = 0;

sf::Font font;

sf::Color numberColor(int value) {
    switch (value) {
        case 1: return sf::Color(60, 160, 255);
        case 2: return sf::Color(50, 200, 100);
        case 3: return sf::Color(255, 70, 70);
        case 4: return sf::Color(150, 90, 255);
        case 5: return sf::Color(255, 160, 50);
        case 6: return sf::Color(40, 210, 210);
        case 7: return sf::Color::White;
        case 8: return sf::Color(160, 160, 160);
        default: return sf::Color::White;
    }
}

void drawText(
    sf::RenderWindow& window,
    const std::string& value,
    unsigned int size,
    float x,
    float y,
    sf::Color color
) {
    sf::Text text(font, value, size);
    text.setFillColor(color);
    text.setPosition({x, y});
    window.draw(text);
}

void drawMine(
    sf::RenderWindow& window,
    float x,
    float y
) {
    sf::CircleShape mine(11);
    mine.setPosition({x + 9, y + 9});
    mine.setFillColor(sf::Color(235, 55, 65));
    mine.setOutlineThickness(2);
    mine.setOutlineColor(sf::Color(120, 20, 30));

    window.draw(mine);

    sf::CircleShape shine(3);
    shine.setPosition({x + 14, y + 12});
    shine.setFillColor(sf::Color(255, 190, 190));

    window.draw(shine);

    for (int i = 0; i < 8; i++) {
        float angle = i * 45.f * 3.14159f / 180.f;

        sf::RectangleShape spike({3, 9});
        spike.setOrigin({1.5f, 4.5f});
        spike.setPosition({
            x + 20 + 12 * std::cos(angle),
            y + 20 + 12 * std::sin(angle)
        });
        spike.setRotation(sf::degrees(i * 45.f));
        spike.setFillColor(sf::Color(235, 55, 65));

        window.draw(spike);
    }
}

void drawFlag(
    sf::RenderWindow& window,
    float x,
    float y
) {
    sf::RectangleShape pole({3, 25});
    pole.setPosition({x + 19, y + 9});
    pole.setFillColor(sf::Color::White);

    window.draw(pole);

    sf::ConvexShape flag;
    flag.setPointCount(3);
    flag.setPoint(0, {0, 0});
    flag.setPoint(1, {19, 6});
    flag.setPoint(2, {0, 13});
    flag.setPosition({x + 22, y + 9});
    flag.setFillColor(sf::Color(255, 65, 75));

    window.draw(flag);

    sf::RectangleShape base({15, 3});
    base.setPosition({x + 13, y + 34});
    base.setFillColor(sf::Color::White);

    window.draw(base);
}

void drawGrid(
    sf::RenderWindow& window,
    Minesweeper& game
) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {

            float x = BOARD_X + j * TILE_SIZE;
            float y = BOARD_Y + i * TILE_SIZE;

            bool revealed = game.isRevealed(i, j);
            bool flagged = game.isFlagged(i, j);

            sf::RectangleShape cell({
                TILE_SIZE - 2.f,
                TILE_SIZE - 2.f
            });

            cell.setPosition({x, y});

            if (revealed) {
                cell.setFillColor(sf::Color(15, 38, 65));
                cell.setOutlineColor(sf::Color(40, 80, 120));
            }
            else {
                cell.setFillColor(sf::Color(45, 85, 125));
                cell.setOutlineColor(sf::Color(75, 120, 160));
            }

            cell.setOutlineThickness(1);

            window.draw(cell);

            if (flagged && !revealed) {
                drawFlag(window, x, y);
            }

            if (revealed) {
                int value = game.getValue(i, j);

                if (value == -1) {
                    drawMine(window, x, y);
                }
                else if (value > 0) {
                    drawText(
                        window,
                        std::to_string(value),
                        22,
                        x + 13,
                        y + 7,
                        numberColor(value)
                    );
                }
            }
        }
    }
}

void drawExplosion(sf::RenderWindow& window) {
    float elapsed = explosionClock.getElapsedTime().asSeconds();

    float x = BOARD_X + explosionCol * TILE_SIZE;
    float y = BOARD_Y + explosionRow * TILE_SIZE;

    float flashAlpha = std::max(
        0.f,
        210.f - elapsed * 400.f
    );

    sf::RectangleShape flash({
        TILE_SIZE - 2.f,
        TILE_SIZE - 2.f
    });

    flash.setPosition({x, y});

    flash.setFillColor(sf::Color(
        255, 20, 30,
        static_cast<std::uint8_t>(flashAlpha)
    ));

    window.draw(flash);

    float radius = elapsed * 100.f;

    sf::CircleShape ring(radius);
    ring.setOrigin({radius, radius});

    ring.setPosition({
        x + TILE_SIZE / 2.f,
        y + TILE_SIZE / 2.f
    });

    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(4);

    ring.setOutlineColor(sf::Color(
        255,
        100,
        30,
        static_cast<std::uint8_t>(flashAlpha)
    ));

    window.draw(ring);

    drawText(
        window,
        "MINE DETONATED!",
        22,
        95,
        20,
        sf::Color(255, 75, 75)
    );
}

void startExplosion(int row, int col) {
    explosionRow = row;
    explosionCol = col;

    gameState = GameState::Exploding;

    explosionClock.restart();
    revealClock.restart();

    revealIndex = 0;
}

void updateAnimation(Minesweeper& game) {
    if (gameState == GameState::Exploding) {

        if (explosionClock.getElapsedTime().asSeconds() >= 0.5f) {
            gameState = GameState::RevealingMines;
            revealClock.restart();
        }
    }

    else if (gameState == GameState::RevealingMines) {

        if (revealClock.getElapsedTime().asSeconds() >= 0.075f) {

            revealClock.restart();

            while (revealIndex < BOARD_SIZE * BOARD_SIZE) {

                int row = revealIndex / BOARD_SIZE;
                int col = revealIndex % BOARD_SIZE;

                revealIndex++;

                if (game.getValue(row, col) == -1) {
                    game.revealMineAt(row, col);
                    break;
                }
            }

            if (revealIndex >= BOARD_SIZE * BOARD_SIZE) {
                gameState = GameState::Lost;
                gameOver = true;
            }
        }
    }
}

void drawGameOver(sf::RenderWindow& window) {
    sf::RectangleShape overlay({
        static_cast<float>(WIDTH),
        static_cast<float>(HEIGHT)
    });

    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    window.draw(overlay);

    sf::RectangleShape panel({320, 180});
    panel.setPosition({40, 145});
    panel.setFillColor(sf::Color(10, 28, 52));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(sf::Color(235, 55, 65));

    window.draw(panel);

    drawText(
        window,
        "GAME OVER",
        36,
        100,
        175,
        sf::Color(255, 65, 75)
    );

    drawText(
        window,
        "You hit a mine!",
        18,
        125,
        235,
        sf::Color::White
    );

    drawText(
        window,
        "Press R to restart",
        16,
        130,
        280,
        sf::Color(100, 200, 255)
    );
}

void drawWin(sf::RenderWindow& window) {
    sf::RectangleShape overlay({
        static_cast<float>(WIDTH),
        static_cast<float>(HEIGHT)
    });

    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    window.draw(overlay);

    sf::RectangleShape panel({320, 180});
    panel.setPosition({40, 145});
    panel.setFillColor(sf::Color(10, 28, 52));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(sf::Color(50, 210, 130));

    window.draw(panel);

    drawText(
        window,
        "YOU WIN!",
        38,
        130,
        175,
        sf::Color(50, 220, 140)
    );

    drawText(
        window,
        "All safe tiles cleared!",
        16,
        105,
        235,
        sf::Color::White
    );

    drawText(
        window,
        "Press R to play again",
        16,
        105,
        280,
        sf::Color(100, 200, 255)
    );
}

void restartGame(Minesweeper& game) {
    game.initialization();
    game.placingmines();
    game.calculateno();

    firstMove = true;
    gameOver = false;
    gameWon = false;

    explosionRow = -1;
    explosionCol = -1;
    revealIndex = 0;

    gameState = GameState::Playing;
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!font.openFromFile("Roboto-Regular.ttf")) {
        std::cout << "Font not loaded\n";
        return 1;
    }

    Minesweeper game;

    restartGame(game);

    sf::RenderWindow window(
        sf::VideoMode({WIDTH, HEIGHT}),
        "Minesweeper"
    );

    window.setFramerateLimit(60);

    while (window.isOpen()) {

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto key = event->getIf<sf::Event::KeyPressed>()) {

                if (key->code == sf::Keyboard::Key::R) {
                    restartGame(game);
                }
            }

            if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()) {

                if (gameState != GameState::Playing)
                    continue;

                int x = mouse->position.x;
                int y = mouse->position.y;

                if (x < BOARD_X || x >= BOARD_X + BOARD_SIZE * TILE_SIZE)
                    continue;

                if (y < BOARD_Y || y >= BOARD_Y + BOARD_SIZE * TILE_SIZE)
                    continue;

                int row = (y - BOARD_Y) / TILE_SIZE;
                int col = (x - BOARD_X) / TILE_SIZE;

                if (mouse->button == sf::Mouse::Button::Left) {

                    bool clickedMine = false;

                    if (!game.isFlagged(row, col) &&
                        !game.isRevealed(row, col)) {

                        clickedMine = game.getValue(row, col) == -1 &&
                                      !firstMove;

                        game.handleClick(
                            row,
                            col,
                            firstMove,
                            gameOver
                        );

                        if (gameOver && clickedMine) {
                            startExplosion(row, col);
                        }
                        else if (game.checkwin()) {
                            gameWon = true;
                            gameState = GameState::Won;
                            game.revealAll();
                        }
                    }
                }

                else if (mouse->button == sf::Mouse::Button::Right) {
                    game.toggleFlag(row, col);
                }
            }
        }

        updateAnimation(game);

        window.clear(sf::Color(5, 15, 34));

        sf::RectangleShape header({
            static_cast<float>(WIDTH),
            55.f
        });

        header.setFillColor(sf::Color(10, 29, 55));
        window.draw(header);

        drawText(
            window,
            "MINESWEEPER",
            23,
            115,
            12,
            sf::Color(70, 195, 255)
        );

        drawGrid(window, game);

        if (gameState == GameState::Exploding) {
            drawExplosion(window);
        }

        if (gameState == GameState::RevealingMines) {
            drawText(
                window,
                "REVEALING MINES...",
                17,
                115,
                15,
                sf::Color(255, 190, 65)
            );
        }

        if (gameState == GameState::Lost) {
            drawGameOver(window);
        }

        if (gameState == GameState::Won) {
            drawWin(window);
        }

        window.display();
    }

    return 0;
}