
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "game.h"
#include "audio.h"

const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 900;

const sf::Color BG(4, 13, 30);
const sf::Color PANEL(8, 27, 52);
const sf::Color PANEL_LIGHT(14, 43, 75);
const sf::Color BLUE(0, 135, 255);
const sf::Color CYAN(75, 205, 255);
const sf::Color WHITE(235, 245, 255);
const sf::Color MUTED(145, 172, 200);
const sf::Color RED(235, 55, 70);
const sf::Color GREEN(55, 205, 135);
const sf::Color GOLD(255, 190, 55);
const sf::Color PURPLE(175, 105, 245);

enum class Screen {
    Menu,
    Difficulty,
    Playing,
    Pause,
    Won,
    Lost,
    Leaderboard,
    Settings,
    HowTo,
    Custom
};

enum class GameStatus {
    Active,
    Exploding,
    Revealing,
    Finished
};

sf::Font font;
Minesweeper game;
AudioManager audio;

Screen screen = Screen::Menu;
Screen previousScreen = Screen::Menu;
GameStatus gameStatus = GameStatus::Active;

sf::Clock gameClock;
sf::Clock animationClock;

bool firstMove = true;
bool soundEnabled = true;
bool musicEnabled = true;
bool animationEnabled = true;
bool timerEnabled = true;
bool darkTheme = true;

int themeColor = 0;
int difficulty = 0;

int customRows = 12;
int customCols = 12;
int customMines = 20;

int explodedRow = -1;
int explodedCol = -1;
int revealIndex = 0;

float savedTime = 0.f;

std::vector<std::string> leaderboard;

void drawText(
    sf::RenderWindow& window,
    const std::string& value,
    unsigned int size,
    float x,
    float y,
    sf::Color color = WHITE,
    bool bold = false
) {
    sf::Text t(font, value, size);
    t.setPosition({x, y});
    t.setFillColor(color);

    if (bold)
        t.setStyle(sf::Text::Bold);

    window.draw(t);
}

void drawCentered(
    sf::RenderWindow& window,
    const std::string& value,
    unsigned int size,
    float centerX,
    float y,
    sf::Color color = WHITE,
    bool bold = false
) {
    sf::Text t(font, value, size);
    t.setFillColor(color);

    if (bold)
        t.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = t.getLocalBounds();

    t.setPosition({
        centerX - bounds.size.x / 2.f,
        y
    });

    window.draw(t);
}

void drawPanel(
    sf::RenderWindow& window,
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Color fill = PANEL,
    sf::Color outline = sf::Color(42, 91, 139)
) {
    sf::RectangleShape panel(size);
    panel.setPosition(position);
    panel.setFillColor(fill);
    panel.setOutlineColor(outline);
    panel.setOutlineThickness(1.5f);

    window.draw(panel);
}

bool inside(
    sf::FloatRect rectangle,
    sf::Vector2f point
) {
    return rectangle.contains(point);
}

void drawButton(
    sf::RenderWindow& window,
    sf::FloatRect rectangle,
    const std::string& label,
    sf::Color fill = PANEL_LIGHT,
    sf::Color outline = sf::Color(48, 105, 158),
    unsigned int size = 21
) {
    sf::RectangleShape button(rectangle.size);

    button.setPosition(rectangle.position);
    button.setFillColor(fill);
    button.setOutlineColor(outline);
    button.setOutlineThickness(1.5f);

    window.draw(button);

    drawCentered(
        window,
        label,
        size,
        rectangle.position.x + rectangle.size.x / 2.f,
        rectangle.position.y + (rectangle.size.y - size) / 2.f - 3.f
    );
}

void drawBackground(sf::RenderWindow& window) {
    window.clear(
        darkTheme ? BG : sf::Color(218, 232, 245)
    );

    sf::CircleShape glow(190);
    glow.setPosition({-120, 100});
    glow.setFillColor(sf::Color(15, 70, 125, 22));
    window.draw(glow);

    glow.setPosition({950, 600});
    window.draw(glow);
}

void drawHeading(
    sf::RenderWindow& window,
    const std::string& title,
    const std::string& subtitle = ""
) {
    drawCentered(
        window,
        title,
        36,
        WIDTH / 2.f,
        48,
        WHITE,
        true
    );

    if (!subtitle.empty()) {
        drawCentered(
            window,
            subtitle,
            17,
            WIDTH / 2.f,
            98,
            MUTED
        );
    }
}

std::string formatTime(float seconds) {
    int total = static_cast<int>(seconds);

    std::ostringstream output;

    output << std::setfill('0')
           << std::setw(2) << total / 60
           << ":"
           << std::setw(2) << total % 60;

    return output.str();
}

float currentTime() {
    if (screen == Screen::Playing)
        return savedTime + gameClock.getElapsedTime().asSeconds();

    return savedTime;
}

void loadLeaderboard() {
    leaderboard.clear();

    std::ifstream file("leaderboard.txt");
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty())
            leaderboard.push_back(line);
    }
}

void saveLeaderboard() {
    std::ofstream file("leaderboard.txt", std::ios::app);

    std::string mode =
        difficulty == 0 ? "Easy" :
        difficulty == 1 ? "Medium" :
        difficulty == 2 ? "Hard" : "Custom";

    file << mode << " | "
         << formatTime(currentTime())
         << '\n';

    loadLeaderboard();
}

void startGame(int rows, int cols, int mines, int mode) {
    game.configure(rows, cols, mines);
    game.placingmines();
    game.calculateno();

    firstMove = true;
    difficulty = mode;

    savedTime = 0.f;

    explodedRow = -1;
    explodedCol = -1;
    revealIndex = 0;

    gameStatus = GameStatus::Active;

    gameClock.restart();
    animationClock.restart();

    screen = Screen::Playing;
}

void drawMine(
    sf::RenderWindow& window,
    float x,
    float y,
    float size
) {
    float center = size / 2.f;
    float radius = size * 0.23f;

    sf::CircleShape body(radius);

    body.setPosition({
        x + center - radius,
        y + center - radius
    });

    body.setFillColor(RED);
    body.setOutlineColor(sf::Color(125, 24, 39));
    body.setOutlineThickness(2);

    window.draw(body);

    for (int i = 0; i < 8; i++) {
        float angle = i * 0.785398f;

        sf::RectangleShape spike({
            size * 0.07f,
            size * 0.25f
        });

        spike.setOrigin({
            size * 0.035f,
            size * 0.125f
        });

        spike.setPosition({
            x + center + std::cos(angle) * size * 0.28f,
            y + center + std::sin(angle) * size * 0.28f
        });

        spike.setRotation(sf::radians(angle));
        spike.setFillColor(RED);

        window.draw(spike);
    }
}

void drawFlag(
    sf::RenderWindow& window,
    float x,
    float y,
    float size
) {
    sf::RectangleShape pole({
        3.f,
        size * 0.62f
    });

    pole.setPosition({
        x + size * 0.45f,
        y + size * 0.2f
    });

    pole.setFillColor(WHITE);
    window.draw(pole);

    sf::ConvexShape flag;
    flag.setPointCount(3);

    flag.setPoint(0, {0, 0});
    flag.setPoint(1, {size * 0.4f, size * 0.12f});
    flag.setPoint(2, {0, size * 0.25f});

    flag.setPosition({
        x + size * 0.48f,
        y + size * 0.18f
    });

    flag.setFillColor(RED);
    window.draw(flag);
}

sf::Color getNumberColor(int number) {
    switch (number) {
        case 1: return sf::Color(65, 180, 255);
        case 2: return sf::Color(65, 220, 145);
        case 3: return sf::Color(255, 85, 90);
        case 4: return sf::Color(160, 125, 255);
        case 5: return sf::Color(255, 190, 65);
        case 6: return sf::Color(70, 215, 220);
        case 7: return WHITE;
        case 8: return MUTED;
        default: return WHITE;
    }
}

float boardTileSize() {
    return std::min(
        48.f,
        std::min(
            820.f / game.getCols(),
            650.f / game.getRows()
        )
    );
}

sf::Vector2f boardPosition() {
    float tile = boardTileSize();

    float width = tile * game.getCols();
    float height = tile * game.getRows();

    return {
        (WIDTH - width) / 2.f,
        200.f + (650.f - height) / 2.f
    };
}

void drawBoard(sf::RenderWindow& window) {
    float tile = boardTileSize();
    sf::Vector2f position = boardPosition();

    for (int r = 0; r < game.getRows(); r++) {
        for (int c = 0; c < game.getCols(); c++) {
            float x = position.x + c * tile;
            float y = position.y + r * tile;

            bool revealed = game.isRevealed(r, c);
            bool flagged = game.isFlagged(r, c);

            sf::RectangleShape cell({
                tile - 3.f,
                tile - 3.f
            });

            cell.setPosition({x, y});

            if (revealed) {
                cell.setFillColor(sf::Color(8, 24, 44));
                cell.setOutlineColor(sf::Color(24, 54, 82));
            } else {
                cell.setFillColor(sf::Color(37, 79, 119));
                cell.setOutlineColor(sf::Color(75, 132, 178));
            }

            cell.setOutlineThickness(1.f);

            window.draw(cell);

            if (flagged && !revealed) {
                drawFlag(window, x, y, tile);
            }

            if (revealed) {
                int value = game.getValue(r, c);

                if (value == -1) {
                    drawMine(window, x, y, tile);
                } else if (value > 0) {
                    drawCentered(
                        window,
                        std::to_string(value),
                        static_cast<unsigned int>(tile * 0.55f),
                        x + tile / 2.f,
                        y + tile * 0.1f,
                        getNumberColor(value),
                        true
                    );
                }
            }
        }
    }
}

void drawMineCounter(sf::RenderWindow& window) {
    drawPanel(window, {360, 116}, {205, 62});
    drawPanel(window, {570, 116}, {205, 62});

    drawMine(window, 375, 127, 40);

    drawText(
        window,
        std::to_string(
            game.getMineCount() - game.getFlagCount()
        ),
        23,
        425,
        133,
        WHITE,
        true
    );

    drawFlag(window, 585, 125, 40);

    drawText(
        window,
        std::to_string(game.getFlagCount()),
        23,
        635,
        133,
        WHITE,
        true
    );
}

void drawMenu(sf::RenderWindow& window) {
    drawPanel(
        window,
        {110, 35},
        {980, 820},
        PANEL,
        sf::Color(38, 84, 133)
    );

    drawCentered(
        window,
        "MINESWEEPER",
        64,
        WIDTH / 2.f,
        135,
        WHITE,
        true
    );

    drawCentered(
        window,
        "THINK  •  REVEAL  •  WIN",
        18,
        WIDTH / 2.f,
        215,
        CYAN
    );

    drawMine(window, 545, 260, 110);

    const std::string labels[] = {
        "PLAY",
        "LEADERBOARD",
        "SETTINGS",
        "HOW TO PLAY",
        "EXIT"
    };

    for (int i = 0; i < 5; i++) {
        sf::FloatRect rect(
            {430.f, 405.f + i * 78.f},
            {340.f, 58.f}
        );

        drawButton(
            window,
            rect,
            labels[i],
            i == 0 ? BLUE : PANEL_LIGHT,
            i == 0 ? CYAN : sf::Color(48, 105, 158)
        );
    }
}

void drawDifficulty(sf::RenderWindow& window) {
    drawHeading(
        window,
        "SELECT DIFFICULTY",
        "Choose your challenge"
    );

    const std::string names[] = {
        "EASY",
        "MEDIUM",
        "HARD",
        "CUSTOM"
    };

    const std::string details[] = {
        "9 x 9       10 MINES",
        "16 x 16     40 MINES",
        "30 x 16     99 MINES",
        "Choose your own size and mine count"
    };

    sf::Color colors[] = {
        GREEN,
        GOLD,
        RED,
        PURPLE
    };

    for (int i = 0; i < 4; i++) {
        float y = 175.f + i * 145.f;

        drawPanel(
            window,
            {260, y},
            {680, 112},
            PANEL_LIGHT,
            colors[i]
        );

        drawText(
            window,
            names[i],
            28,
            300,
            y + 18,
            colors[i],
            true
        );

        drawText(
            window,
            details[i],
            18,
            300,
            y + 62,
            WHITE
        );

        drawText(
            window,
            ">",
            30,
            885,
            y + 35,
            WHITE,
            true
        );
    }

    drawButton(
        window,
        {{450, 790}, {300, 55}},
        "BACK"
    );
}

void drawPlaying(sf::RenderWindow& window) {
    drawText(
        window,
        "MINESWEEPER",
        27,
        75,
        54,
        CYAN,
        true
    );

    drawText(
        window,
        "TIME",
        14,
        830,
        48,
        MUTED
    );

    drawText(
        window,
        timerEnabled ? formatTime(currentTime()) : "--:--",
        26,
        830,
        67,
        WHITE,
        true
    );

    drawText(
        window,
        "II",
        29,
        1080,
        55,
        WHITE,
        true
    );

    drawMineCounter(window);
    drawBoard(window);

    drawButton(
        window,
        {{410, 838}, {180, 48}},
        "RESET"
    );

    drawButton(
        window,
        {{610, 838}, {180, 48}},
        "PAUSE"
    );
}

void drawPause(sf::RenderWindow& window) {
    drawPlaying(window);

    sf::RectangleShape overlay({WIDTH, HEIGHT});
    overlay.setFillColor(sf::Color(0, 0, 0, 195));

    window.draw(overlay);

    drawPanel(
        window,
        {390, 160},
        {420, 580},
        PANEL,
        sf::Color(52, 117, 176)
    );

    drawCentered(window, "II", 48, 600, 190, CYAN, true);
    drawCentered(window, "PAUSED", 34, 600, 260, WHITE, true);

    const std::string labels[] = {
        "RESUME",
        "RESTART",
        "SETTINGS",
        "MAIN MENU"
    };

    for (int i = 0; i < 4; i++) {
        drawButton(
            window,
            {{475, 340.f + i * 83.f}, {250, 60}},
            labels[i],
            i == 0 ? BLUE : PANEL_LIGHT
        );
    }
}

void drawEndScreen(sf::RenderWindow& window, bool won) {
    drawPlaying(window);

    sf::RectangleShape overlay({WIDTH, HEIGHT});
    overlay.setFillColor(sf::Color(0, 0, 0, 205));

    window.draw(overlay);

    drawPanel(
        window,
        {385, 150},
        {430, 625},
        PANEL,
        won ? GOLD : RED
    );

    drawCentered(
        window,
        won ? "YOU WIN!" : "BOOM!",
        52,
        600,
        190,
        won ? GOLD : RED,
        true
    );

    drawCentered(
        window,
        won ? "ALL SAFE TILES CLEARED" : "GAME OVER",
        24,
        600,
        265,
        WHITE,
        true
    );

    drawCentered(
        window,
        won ? "Congratulations!" : "You hit a mine!",
        18,
        600,
        310,
        MUTED
    );

    drawPanel(window, {445, 375}, {310, 115});

    drawText(window, "TIME", 20, 475, 390, MUTED);
    drawText(
        window,
        formatTime(currentTime()),
        24,
        650,
        385,
        CYAN,
        true
    );

    drawText(window, "MINES", 20, 475, 440, MUTED);
    drawText(
        window,
        std::to_string(game.getMineCount()),
        24,
        650,
        435,
        CYAN,
        true
    );

    drawButton(
        window,
        {{435, 525}, {330, 55}},
        won ? "PLAY AGAIN" : "TRY AGAIN",
        won ? BLUE : RED
    );

    drawButton(
        window,
        {{435, 595}, {330, 55}},
        "LEADERBOARD"
    );

    drawButton(
        window,
        {{435, 665}, {330, 55}},
        "MAIN MENU"
    );
}

void drawLeaderboard(sf::RenderWindow& window) {
    drawHeading(
        window,
        "LEADERBOARD",
        "Your completed games"
    );

    drawPanel(
        window,
        {270, 175},
        {660, 560}
    );

    drawText(window, "#", 18, 300, 195, CYAN, true);
    drawText(window, "DIFFICULTY", 18, 390, 195, CYAN, true);
    drawText(window, "TIME", 18, 740, 195, CYAN, true);

    loadLeaderboard();

    for (int i = 0; i < 10; i++) {
        float y = 245.f + i * 45.f;

        if (i < static_cast<int>(leaderboard.size())) {
            drawPanel(
                window,
                {295, y},
                {610, 38},
                PANEL_LIGHT,
                sf::Color(28, 68, 106)
            );

            drawText(
                window,
                std::to_string(i + 1),
                17,
                310,
                y + 7,
                WHITE
            );

            drawText(
                window,
                leaderboard[i],
                17,
                390,
                y + 7,
                WHITE
            );
        }
    }

    if (leaderboard.empty()) {
        drawCentered(
            window,
            "NO COMPLETED GAMES YET",
            20,
            600,
            430,
            MUTED
        );
    }

    drawButton(
        window,
        {{450, 790}, {300, 55}},
        "BACK"
    );
}

void drawToggle(
    sf::RenderWindow& window,
    float x,
    float y,
    bool enabled
) {
    sf::RectangleShape track({65, 30});

    track.setPosition({x, y});
    track.setFillColor(
        enabled ? BLUE : sf::Color(65, 77, 95)
    );

    window.draw(track);

    sf::CircleShape knob(11);

    knob.setPosition({
        enabled ? x + 39.f : x + 5.f,
        y + 4.f
    });

    knob.setFillColor(WHITE);

    window.draw(knob);
}

void drawSettings(sf::RenderWindow& window) {
    drawHeading(
        window,
        "SETTINGS",
        "Customize your experience"
    );

    drawPanel(window, {285, 165}, {630, 540});

    drawText(window, "AUDIO", 16, 315, 180, CYAN, true);

    const std::string labels[] = {
        "Sound",
        "Music",
        "Animations",
        "Timer",
        "Dark Theme"
    };

    bool values[] = {
        soundEnabled,
        musicEnabled,
        animationEnabled,
        timerEnabled,
        darkTheme
    };

    for (int i = 0; i < 5; i++) {
        float y = 230.f + i * 82.f;

        drawText(
            window,
            labels[i],
            21,
            340,
            y,
            WHITE
        );

        drawToggle(
            window,
            770,
            y - 2,
            values[i]
        );
    }

    drawText(
        window,
        "THEME COLOR",
        16,
        315,
        650,
        CYAN,
        true
    );

    sf::Color colors[] = {
        BLUE,
        GREEN,
        PURPLE,
        GOLD,
        RED
    };

    for (int i = 0; i < 5; i++) {
        sf::CircleShape swatch(16);

        swatch.setPosition({
            500.f + i * 55.f,
            645
        });

        swatch.setFillColor(colors[i]);

        if (themeColor == i) {
            swatch.setOutlineColor(WHITE);
            swatch.setOutlineThickness(3);
        }

        window.draw(swatch);
    }

    drawButton(
        window,
        {{450, 790}, {300, 55}},
        "BACK"
    );
}

void drawHowTo(sf::RenderWindow& window) {
    drawHeading(
        window,
        "HOW TO PLAY",
        "Learn the basics of Minesweeper"
    );

    const std::string titles[] = {
        "1. Click to Reveal",
        "2. Read the Numbers",
        "3. Place a Flag",
        "4. Clear All Safe Tiles",
        "5. Avoid Mines"
    };

    const std::string descriptions[] = {
        "Left-click a tile to reveal what is underneath.",
        "Numbers show how many mines are nearby.",
        "Right-click to mark a tile with a flag.",
        "Reveal every safe tile to win the game.",
        "Do not click on a mine!"
    };

    for (int i = 0; i < 5; i++) {
        float y = 160.f + i * 112.f;

        drawPanel(
            window,
            {240, y},
            {720, 90},
            PANEL_LIGHT
        );

        sf::CircleShape icon(25);

        icon.setPosition({265, y + 20});
        icon.setFillColor(BLUE);

        window.draw(icon);

        drawCentered(
            window,
            std::to_string(i + 1),
            21,
            290,
            y + 31,
            WHITE,
            true
        );

        drawText(
            window,
            titles[i],
            20,
            335,
            y + 14,
            WHITE,
            true
        );

        drawText(
            window,
            descriptions[i],
            16,
            335,
            y + 50,
            MUTED
        );
    }

    drawButton(
        window,
        {{450, 790}, {300, 55}},
        "BACK"
    );
}

void drawCustom(sf::RenderWindow& window) {
    drawHeading(
        window,
        "CUSTOM GAME",
        "Configure your own board"
    );

    const std::string labels[] = {
        "ROWS",
        "COLUMNS",
        "MINES"
    };

    int values[] = {
        customRows,
        customCols,
        customMines
    };

    for (int i = 0; i < 3; i++) {
        float y = 255.f + i * 135.f;

        drawText(
            window,
            labels[i],
            22,
            380,
            y + 12,
            CYAN,
            true
        );

        drawText(
            window,
            std::to_string(values[i]),
            30,
            700,
            y + 5,
            WHITE,
            true
        );

        drawButton(
            window,
            {{600, y}, {60, 55}},
            "-"
        );

        drawButton(
            window,
            {{780, y}, {60, 55}},
            "+"
        );
    }

    drawText(
        window,
        "Maximum board size: 30 x 30",
        17,
        440,
        670,
        MUTED
    );

    drawButton(
        window,
        {{430, 720}, {340, 55}},
        "START GAME",
        BLUE
    );

    drawButton(
        window,
        {{450, 790}, {300, 55}},
        "BACK"
    );
}

void startLossAnimation() {
    savedTime += gameClock.getElapsedTime().asSeconds();

    if (animationEnabled) {
        gameStatus = GameStatus::Revealing;
        revealIndex = 0;
        animationClock.restart();
    } else {
        game.revealAll();
        gameStatus = GameStatus::Finished;
        screen = Screen::Lost;
    }
}

void updateLossAnimation() {
    if (gameStatus != GameStatus::Revealing)
        return;

    float interval = animationEnabled ? 0.20f : 0.f;
    if (animationClock.getElapsedTime().asSeconds() < interval)
        return;

    animationClock.restart();

    int total = game.getRows() * game.getCols();

    int revealedCount = 0;

    while (revealIndex < total) {
        int row = revealIndex / game.getCols();
        int col = revealIndex % game.getCols();

        revealIndex++;

        if (game.getValue(row, col) == -1) {
            game.revealMineAt(row, col);
            revealedCount = 1;
            break;
        }
    }

    if (revealIndex >= total && revealedCount == 0) {
        game.revealAll();
        gameStatus = GameStatus::Finished;
        screen = Screen::Lost;
    }
}

void pauseGame() {
    if (screen != Screen::Playing)
        return;

    savedTime += gameClock.getElapsedTime().asSeconds();

    screen = Screen::Pause;
}

void resumeGame() {
    gameClock.restart();
    screen = Screen::Playing;
}

void handleBoardClick(sf::Vector2f mousePosition, bool rightClick) {
    if (screen != Screen::Playing ||
        gameStatus != GameStatus::Active) {
        return;
    }

    float tile = boardTileSize();
    sf::Vector2f position = boardPosition();

    float x = mousePosition.x - position.x;
    float y = mousePosition.y - position.y;

    if (x < 0 || y < 0)
        return;

    int col = static_cast<int>(x / tile);
    int row = static_cast<int>(y / tile);

    if (row < 0 || row >= game.getRows() ||
        col < 0 || col >= game.getCols()) {
        return;
    }

    if (rightClick) {
        game.toggleFlag(row, col);
        if (soundEnabled)
            audio.play("button_click");
        return;
    }

    bool wasRevealed = game.isRevealed(row, col);
    bool lost = false;

    game.handleClick(
        row,
        col,
        firstMove,
        lost
    );

    if (!wasRevealed && game.isRevealed(row, col) && soundEnabled)
        audio.play("tile_reveal");

    if (lost) {
        startLossAnimation();
        return;
    }

    if (game.checkwin()) {
        savedTime += gameClock.getElapsedTime().asSeconds();

        game.revealAll();

        gameStatus = GameStatus::Finished;
        screen = Screen::Won;

        saveLeaderboard();
    }
}

void handleClick(sf::Vector2f p, bool rightClick) {
    if (rightClick) {
        handleBoardClick(p, true);
        return;
    }

    if (screen == Screen::Playing) {
        if (gameStatus != GameStatus::Active)
            return;

        sf::FloatRect resetRect({410, 838}, {180, 48});
        sf::FloatRect pauseRect({610, 838}, {180, 48});
        sf::FloatRect settingsRect({1030, 40}, {120, 70});

        if (inside(resetRect, p)) {
            startGame(
                game.getRows(),
                game.getCols(),
                game.getMineCount(),
                difficulty
            );
        } else if (inside(pauseRect, p) ||
                   inside(settingsRect, p)) {
            pauseGame();
        } else {
            handleBoardClick(p, false);
        }

        return;
    }

    if (screen == Screen::Menu) {
        if (inside({{430, 405}, {340, 58}}, p)) {
            screen = Screen::Difficulty;
        } else if (inside({{430, 483}, {340, 58}}, p)) {
            screen = Screen::Leaderboard;
        } else if (inside({{430, 561}, {340, 58}}, p)) {
            previousScreen = Screen::Menu;
            screen = Screen::Settings;
        } else if (inside({{430, 639}, {340, 58}}, p)) {
            screen = Screen::HowTo;
        } else if (inside({{430, 717}, {340, 58}}, p)) {
            std::exit(0);
        }

        return;
    }

    if (screen == Screen::Difficulty) {
        if (inside({{260, 175}, {680, 112}}, p)) {
            startGame(9, 9, 10, 0);
        } else if (inside({{260, 320}, {680, 112}}, p)) {
            startGame(16, 16, 40, 1);
        } else if (inside({{260, 465}, {680, 112}}, p)) {
            startGame(16, 30, 99, 2);
        } else if (inside({{260, 610}, {680, 112}}, p)) {
            screen = Screen::Custom;
        } else if (inside({{450, 790}, {300, 55}}, p)) {
            screen = Screen::Menu;
        }

        return;
    }

    if (screen == Screen::Pause) {
        if (inside({{475, 340}, {250, 60}}, p)) {
            resumeGame();
        } else if (inside({{475, 423}, {250, 60}}, p)) {
            startGame(
                game.getRows(),
                game.getCols(),
                game.getMineCount(),
                difficulty
            );
        } else if (inside({{475, 506}, {250, 60}}, p)) {
            previousScreen = Screen::Pause;
            screen = Screen::Settings;
        } else if (inside({{475, 589}, {250, 60}}, p)) {
            screen = Screen::Menu;
        }

        return;
    }

    if (screen == Screen::Won || screen == Screen::Lost) {
        if (inside({{435, 525}, {330, 55}}, p)) {
            startGame(
                game.getRows(),
                game.getCols(),
                game.getMineCount(),
                difficulty
            );
        } else if (inside({{435, 595}, {330, 55}}, p)) {
            screen = Screen::Leaderboard;
        } else if (inside({{435, 665}, {330, 55}}, p)) {
            screen = Screen::Menu;
        }

        return;
    }

    if (screen == Screen::Leaderboard ||
        screen == Screen::HowTo) {
        if (inside({{450, 790}, {300, 55}}, p))
            screen = Screen::Menu;

        return;
    }

    if (screen == Screen::Settings) {
        for (int i = 0; i < 5; i++) {
            float y = 230.f + i * 82.f;

            if (inside({{300, y - 10}, {600, 65}}, p)) {
                if (i == 0) {
                    soundEnabled = !soundEnabled;
                    audio.setMuted(!soundEnabled);
                }
                if (i == 1) musicEnabled = !musicEnabled;
                if (i == 2) animationEnabled = !animationEnabled;
                if (i == 3) timerEnabled = !timerEnabled;
                if (i == 4) darkTheme = !darkTheme;

                return;
            }
        }

        if (p.y >= 635 && p.y <= 690) {
            themeColor = std::clamp(
                static_cast<int>((p.x - 480) / 55),
                0,
                4
            );

            return;
        }

        if (inside({{450, 790}, {300, 55}}, p))
            screen = previousScreen;

        return;
    }

    if (screen == Screen::Custom) {
        for (int i = 0; i < 3; i++) {
            float y = 255.f + i * 135.f;

            if (inside({{600, y}, {60, 55}}, p)) {
                if (i == 0)
                    customRows = std::max(5, customRows - 1);

                if (i == 1)
                    customCols = std::max(5, customCols - 1);

                if (i == 2)
                    customMines = std::max(1, customMines - 1);

                return;
            }

            if (inside({{780, y}, {60, 55}}, p)) {
                if (i == 0)
                    customRows = std::min(30, customRows + 1);

                if (i == 1)
                    customCols = std::min(30, customCols + 1);

                if (i == 2)
                    customMines = std::min(
                        customRows * customCols - 1,
                        customMines + 1
                    );

                return;
            }
        }

        if (inside({{430, 720}, {340, 55}}, p)) {
            customMines = std::clamp(
                customMines,
                1,
                customRows * customCols - 1
            );

            startGame(
                customRows,
                customCols,
                customMines,
                3
            );

            return;
        }

        if (inside({{450, 790}, {300, 55}}, p))
            screen = Screen::Difficulty;
    }
}

int main() {
    audio.loadSound("button_hover", "assets/audio/sfx/button-hover.wav");
    audio.loadSound("button_click", "assets/audio/sfx/button-click.wav");
    std::srand(
        static_cast<unsigned int>(std::time(nullptr))
    );

    if (!font.openFromFile("Roboto-Regular.ttf"))
        return 1;

    loadLeaderboard();

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
                if (key->code == sf::Keyboard::Key::Escape) {
                    if (screen == Screen::Playing)
                        pauseGame();
                    else if (screen == Screen::Pause)
                        resumeGame();
                    else if (screen != Screen::Menu)
                        screen = Screen::Menu;
                }
            }

            if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f p(
                    static_cast<float>(mouse->position.x),
                    static_cast<float>(mouse->position.y)
                );

                if (mouse->button == sf::Mouse::Button::Left && soundEnabled)
                    audio.play("button_click");

                handleClick(
                    p,
                    mouse->button == sf::Mouse::Button::Right
                );
            }
        }

        updateLossAnimation();

        drawBackground(window);

        switch (screen) {
            case Screen::Menu:
                drawMenu(window);
                break;

            case Screen::Difficulty:
                drawDifficulty(window);
                break;

            case Screen::Playing:
                drawPlaying(window);
                break;

            case Screen::Pause:
                drawPause(window);
                break;

            case Screen::Won:
                drawEndScreen(window, true);
                break;

            case Screen::Lost:
                drawEndScreen(window, false);
                break;

            case Screen::Leaderboard:
                drawLeaderboard(window);
                break;

            case Screen::Settings:
                drawSettings(window);
                break;

            case Screen::HowTo:
                drawHowTo(window);
                break;

            case Screen::Custom:
                drawCustom(window);
                break;
        }

        window.display();
    }

    return 0;
}
