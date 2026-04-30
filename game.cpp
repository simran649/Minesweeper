#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.h"
#include <SFML/Graphics.hpp>
using namespace std;

void Minesweeper::initialization(){

    //board initialization
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            board[i][j] = 0;
            reveal[i][j] = false;
            flag[i][j] = false;
        }
    }
}

void Minesweeper::placingmines(){
    int mines = 0;
    
    while(mines <30){

        int row = rand() % size;
        int col = rand() % size;

        if(board[row][col] != -1){
            board[row][col] = -1;
            mines++;
        }
    }
}

void Minesweeper::calculateno(){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(board[i][j] != -1){
                board[i][j] = 0;
            }
        }
    }
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(board[i][j]==-1){
                continue;
            }
            int count=0;
            for(int x=-1; x<=1; x++){
                for(int y=-1; y<=1; y++){
                    int ni = x + i;
                    int nj = y + j;

                    if(ni>=0 && ni<size && nj>=0 && nj<size){
                        if(board[ni][nj]==-1){
                            count++;
                        }
                    }
                }
            }
            board[i][j]=count;
        }
    }
}

void Minesweeper::floodfill(int i, int j){
    if(i<0 || i>=size || j<0 || j>=size){
        return;
    }

    if(reveal[i][j] == true){
        return;
    }

    if(board[i][j] == -1){
        return;
    }

    reveal[i][j] = true;
    if(board[i][j]!=0){
        return;
    }

    for(int x=-1; x<=1; x++){
        for(int y=-1; y<=1; y++){
            floodfill(i + x, j + y);
        }
    }
    
}

void Minesweeper::printboard(){
    cout<<"\n";
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){

            //if cell is flagged it will show F in place of the cell
            if(flag[i][j] == true){
                cout<<" F ";
            }

            //if cell is hidden it will show . here
            else if(reveal[i][j] == false){
                cout<<" . ";
            }

            //if there's a mine at that place it will show *
            else{
                if(board[i][j] == -1){
                    cout<<" * ";
                }
                else{
                    if(board[i][j] == 0){
                        cout<<"   ";
                    }
                    else{
                        cout<<" "<<board[i][j]<<" ";
                    }
                }
            }
        }
        cout<<endl;
    }
}

bool Minesweeper::checkwin(){

    int checkmines = 0;
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(reveal[i][j] == true && board[i][j] != -1){
                checkmines++;
            }
        }
    }
    int totalcells = size * size;
    int minecount = 30;
    int safe = totalcells - minecount;

    return (checkmines == safe);
}

void Minesweeper::play(){

    printboard();
    bool firstmove = true;

    while(true){
        int m, n;
        string ch;

        cout<<"Enter action (flag or click)..."<<endl;
        cin>>ch;

        cout<<"enter row and column: "<<endl;
        cin>>m>>n;
        m--;
        n--;

        //adding constraint so that row and column stay in the limits
        if(m<0 || m>=size || n<0 || n>=size){
            cout<<"Invalid input, try again"<<endl;
            continue;
        }

         //if the user wants to place a flag this will place the flag at the desired position and if the flag already exists it will remove it
        if(ch == "f" || ch == "F"){
            if(reveal[m][n] == true){
                cout<<"Cannot flag a revealed cell\n";
                continue;
            }
            if(flag[m][n] == true){
                flag[m][n] = false;
                cout << "Flag removed\n";
            }
            else{
                flag[m][n] = true;
                cout << "Flag placed\n";
            }

            printboard();
            continue;
        }

        // prevent clicking flagged cell
        if(flag[m][n] == true){
            cout << "Cell is flagged. Remove flag first.\n";
            continue;
        }

        //preventing selecting one cell multiple times
        if(reveal[m][n]==true){
            cout<<"cell already selected, try another cell...\n";
            continue;
        }

        //if at the selected cell there's a mine the program will end
        if(firstmove && board[m][n]==-1){
            board[m][n] = 0;

            while(true){
                int r = rand() % size;
                int c = rand() % size;

                if(board[r][c] != -1 && (r != m || c != n)){
                    board[r][c] = -1;
                    break;
                }
            }
            calculateno();
         
        }
        if(board[m][n] == -1){
                cout<<"\nGame Over..."<<endl;

                 //will reveal all mines
                for(int i=0; i<size; i++){
                    for(int j=0; j<size; j++){
                        if(board[i][j] == -1){
                            reveal[i][j] = true;
                        }
                    }
                }
                printboard(); 
                break;
            }

        //cell reveal logic
        if(board[m][n] == 0){
            floodfill(m, n);
        }
        else{
            reveal[m][n] = true;
        }

        //win check
        if(checkwin()){
            cout<<"You won..."<<endl;

            for(int i=0; i<size; i++){
                for(int j=0; j<size; j++){
                    if(board[i][j] == -1){
                        reveal[i][j] = true;
                    }
                }
            }
            printboard();
            break;
        }
        

        firstmove = false;

        printboard();
    }

}

bool Minesweeper::isRevealed(int i, int j){
    return reveal[i][j];
    }
    
int Minesweeper::getValue(int i, int j){ 
    return board[i][j];
    }

const int TILE_SIZE = 40;

void drawGrid(sf::RenderWindow& window, Minesweeper& game){
    sf::RectangleShape cell(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));

    cell.setOutlineThickness(1);
    cell.setOutlineColor(sf::Color::Black);

    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            
            cell.setPosition(sf::Vector2f(j * TILE_SIZE, i * TILE_SIZE));

            if (!game.isRevealed(i, j)) {
                cell.setFillColor(sf::Color(150, 150, 150));
            } 
            else {
                cell.setFillColor(sf::Color::White);
            }
            window.draw(cell);
            
        }
    }
}