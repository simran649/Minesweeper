
#ifndef GAME_H
#define GAME_H

class Minesweeper {
private:
    static const int MAX_SIZE = 30;

    int board[MAX_SIZE][MAX_SIZE];
    bool revealed[MAX_SIZE][MAX_SIZE];
    bool flagged[MAX_SIZE][MAX_SIZE];

    int rows;
    int cols;
    int mineCount;

public:
    Minesweeper();

    void configure(int rows, int cols, int mines);
    void initialization();
    void placingmines();
    void calculateno();

    void floodfill(int row, int col);
    void handleClick(int row, int col, bool& firstMove, bool& gameOver);

    void toggleFlag(int row, int col);
    void revealMineAt(int row, int col);
    void revealAll();

    bool isRevealed(int row, int col) const;
    bool isFlagged(int row, int col) const;
    bool checkwin() const;

    int getValue(int row, int col) const;
    int getRows() const;
    int getCols() const;
    int getMineCount() const;
    int getFlagCount() const;
};

#endif
