#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.h"

using namespace std;

void Minesweeper::initialization() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            board[i][j] = 0;
            reveal[i][j] = false;
            flag[i][j] = false;
        }
    }
}

void Minesweeper::placingmines() {
    int mines = 0;

    while (mines < 30) {
        int row = rand() % size;
        int col = rand() % size;

        if (board[row][col] != -1) {
            board[row][col] = -1;
            mines++;
        }
    }
}

void Minesweeper::calculateno() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] != -1)
                board[i][j] = 0;
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == -1)
                continue;

            int count = 0;

            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    int ni = i + x;
                    int nj = j + y;

                    if (ni >= 0 && ni < size &&
                        nj >= 0 && nj < size &&
                        board[ni][nj] == -1) {
                        count++;
                    }
                }
            }

            board[i][j] = count;
        }
    }
}

void Minesweeper::floodfill(int i, int j) {
    if (i < 0 || i >= size || j < 0 || j >= size)
        return;

    if (reveal[i][j] || board[i][j] == -1 || flag[i][j])
        return;

    reveal[i][j] = true;

    if (board[i][j] != 0)
        return;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x != 0 || y != 0)
                floodfill(i + x, j + y);
        }
    }
}

void Minesweeper::printboard() {
    cout << "\n";

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (flag[i][j]) {
                cout << " F ";
            }
            else if (!reveal[i][j]) {
                cout << " . ";
            }
            else if (board[i][j] == -1) {
                cout << " * ";
            }
            else {
                cout << " " << board[i][j] << " ";
            }
        }

        cout << endl;
    }
}

bool Minesweeper::checkwin() {
    int safeCells = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (reveal[i][j] && board[i][j] != -1)
                safeCells++;
        }
    }

    return safeCells == 70;
}

void Minesweeper::play() {
    printboard();

    bool firstMove = true;

    while (true) {
        int row, col;
        string action;

        cout << "Enter action (click or flag): ";
        cin >> action;

        cout << "Enter row and column: ";
        cin >> row >> col;

        row--;
        col--;

        if (row < 0 || row >= size || col < 0 || col >= size)
            continue;

        if (action == "f" || action == "F") {
            toggleFlag(row, col);
            printboard();
            continue;
        }

        bool gameOver = false;

        handleClick(row, col, firstMove, gameOver);

        if (gameOver) {
            revealAll();
            cout << "Game Over\n";
            printboard();
            break;
        }

        if (checkwin()) {
            revealAll();
            cout << "You won!\n";
            printboard();
            break;
        }

        printboard();
    }
}

bool Minesweeper::isRevealed(int i, int j) {
    if (i < 0 || i >= size || j < 0 || j >= size)
        return false;

    return reveal[i][j];
}

int Minesweeper::getValue(int i, int j) {
    if (i < 0 || i >= size || j < 0 || j >= size)
        return 0;

    return board[i][j];
}

void Minesweeper::toggleFlag(int i, int j) {
    if (i < 0 || i >= size || j < 0 || j >= size)
        return;

    if (!reveal[i][j])
        flag[i][j] = !flag[i][j];
}

bool Minesweeper::isFlagged(int i, int j) {
    if (i < 0 || i >= size || j < 0 || j >= size)
        return false;

    return flag[i][j];
}

void Minesweeper::handleClick(
    int row,
    int col,
    bool& firstMove,
    bool& gameOver
) {
    if (row < 0 || row >= size || col < 0 || col >= size)
        return;

    if (flag[row][col] || reveal[row][col])
        return;

    if (firstMove && board[row][col] == -1) {
        board[row][col] = 0;

        while (true) {
            int r = rand() % size;
            int c = rand() % size;

            if (board[r][c] != -1 && (r != row || c != col)) {
                board[r][c] = -1;
                break;
            }
        }

        calculateno();
    }

    if (board[row][col] == -1) {
        gameOver = true;
        return;
    }

    if (board[row][col] == 0) {
        floodfill(row, col);
    }
    else {
        reveal[row][col] = true;
    }

    firstMove = false;
}

void Minesweeper::revealMineAt(int row, int col) {
    if (row < 0 || row >= size || col < 0 || col >= size)
        return;

    if (board[row][col] == -1)
        reveal[row][col] = true;
}

void Minesweeper::revealAll() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == -1)
                reveal[i][j] = true;
        }
    }
}