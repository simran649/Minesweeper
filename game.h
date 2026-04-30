#pragma once

class Minesweeper {
private:
    static const int size = 10;
    int board[size][size];
    bool reveal[size][size];
    bool flag[size][size];

public:
    void initialization();
    void placingmines();
    void calculateno();
    void play();
    void printboard();
    void floodfill(int i, int j);
    bool checkwin();

    bool isRevealed(int i, int j);
    int getValue(int i, int j);
};