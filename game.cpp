#include <cstdlib>
#include "game.h"

Minesweeper::Minesweeper() {
    rows = 9;
    cols = 9;
    mineCount = 10;
    initialization();
}

void Minesweeper::configure(int newRows, int newCols, int newMines) {
    rows = newRows;
    cols = newCols;
    mineCount = newMines;
    initialization();
}

void Minesweeper::initialization() {
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            board[i][j] = 0;
            revealed[i][j] = false;
            flagged[i][j] = false;
        }
    }
}

void Minesweeper::placingmines() {
    int placed = 0;

    while (placed < mineCount) {
        int row = rand() % rows;
        int col = rand() % cols;

        if (board[row][col] != -1) {
            board[row][col] = -1;
            placed++;
        }
    }
}

void Minesweeper::calculateno() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j] == -1)
                continue;

            int count = 0;

            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    int r = i + x;
                    int c = j + y;

                    if (r >= 0 && r < rows &&
                        c >= 0 && c < cols &&
                        board[r][c] == -1) {
                        count++;
                    }
                }
            }

            board[i][j] = count;
        }
    }
}

void Minesweeper::floodfill(int row, int col) {
    if (row < 0 || row >= rows ||
        col < 0 || col >= cols ||
        revealed[row][col] ||
        flagged[row][col] ||
        board[row][col] == -1) {
        return;
    }

    revealed[row][col] = true;

    if (board[row][col] != 0)
        return;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x != 0 || y != 0)
                floodfill(row + x, col + y);
        }
    }
}

void Minesweeper::handleClick(
    int row,
    int col,
    bool& firstMove,
    bool& gameOver
) {
    if (row < 0 || row >= rows ||
        col < 0 || col >= cols ||
        revealed[row][col] ||
        flagged[row][col]) {
        return;
    }

    if (firstMove && board[row][col] == -1) {
        board[row][col] = 0;

        int newRow;
        int newCol;

        do {
            newRow = rand() % rows;
            newCol = rand() % cols;
        } while (
            board[newRow][newCol] == -1 ||
            (newRow == row && newCol == col)
        );

        board[newRow][newCol] = -1;
        calculateno();
    }

    if (board[row][col] == -1) {
        gameOver = true;
        return;
    }

    if (board[row][col] == 0)
        floodfill(row, col);
    else
        revealed[row][col] = true;

    firstMove = false;
}

void Minesweeper::toggleFlag(int row, int col) {
    if (row < 0 || row >= rows ||
        col < 0 || col >= cols ||
        revealed[row][col]) {
        return;
    }

    flagged[row][col] = !flagged[row][col];
}

void Minesweeper::revealMineAt(int row, int col) {
    if (row >= 0 && row < rows &&
        col >= 0 && col < cols &&
        board[row][col] == -1) {
        revealed[row][col] = true;
    }
}

void Minesweeper::revealAll() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j] == -1)
                revealed[i][j] = true;
        }
    }
}

bool Minesweeper::isRevealed(int row, int col) const {
    return row >= 0 && row < rows &&
           col >= 0 && col < cols &&
           revealed[row][col];
}

bool Minesweeper::isFlagged(int row, int col) const {
    return row >= 0 && row < rows &&
           col >= 0 && col < cols &&
           flagged[row][col];
}

bool Minesweeper::checkwin() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j] != -1 && !revealed[i][j])
                return false;
        }
    }

    return true;
}

int Minesweeper::getValue(int row, int col) const {
    if (row < 0 || row >= rows ||
        col < 0 || col >= cols) {
        return 0;
    }

    return board[row][col];
}

int Minesweeper::getRows() const {
    return rows;
}

int Minesweeper::getCols() const {
    return cols;
}

int Minesweeper::getMineCount() const {
    return mineCount;
}

int Minesweeper::getFlagCount() const {
    int count = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (flagged[i][j])
                count++;
        }
    }

    return count;
}
