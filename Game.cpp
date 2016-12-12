/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <cstdio>
#include <string.h>
#include "Player.h"
#include "Game.h"

#define lines 6
#define columns 7
#define underline "\33[4m"
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define NoColor "\033[0m"
#define clearScreen() (cout << "\033[H\033[J")
#define gotoxy(x,y) (printf("\033[%d;%dH", (x), (y)))
using namespace std;

//constructor for Game object. setting game init data
Game::Game(char p1c, char p2c){
    /*creating game board with 7*6= 42 cells*/
    this->board=new char* [columns];
    for (int p = 0; p < columns; p++)
        this->board[p] = new char [lines];
    /* set all cells values to * */
    for (int p = 0; p < columns; p++)
        for(int j=0; j<lines;j++) 
            this->board[p][j]='*';
    /*setting starting player*/
    this->playerOneColor=p1c;
    this->playerTwoColor=p2c;
}
Game::~Game(){
    for (int p = 0; p < columns; p++)
        delete [] this->board[p];
    delete [] this->board;  
}
void Game::printBoard()
{
    char sideBar = '|';
    char lowBar = '_';
    int k = 0;
    int y_axis = 3;
    int x_axis = 3;
    cout << "Player 1 color: " << this->playerOneColor<< endl << "Player 2 color:" <<this->playerTwoColor<<endl;
    cout << endl << endl << endl;
    cout << "\t" << "╔";
    for (int k = 0; k < columns * 2-1; k++)
            if (k%2)
                    cout << "╦";
            else
                    cout << "   ";
    cout << "╗" << endl;
    for (int i = 0; i<lines*2-1; i++)
    {
        if (i%2)
        {
            cout << "\t" << "╠"; //<< underline;
            for (int j = 0; j < columns*2-1; j++)
            {
                    if (j%2)
                            cout << "╬";
                    else if (j != (columns*2 - 1))
                            cout << "═══";

            }
            cout << "╣" << endl;//"\33[0m" << "║" << endl;

        }
        else
        {
            cout << "\t" << "║" << " "; //<< underline;
            for (int j = 0; j < columns; j++)
            {
                    switch(this->board[j][i/2]){
                    case 'R':
                            cout << RED << "●" << NoColor;
                            break;
                    case 'B':
                            cout << BLUE << "●" << NoColor;
                            break;
                    default:
                            cout << this->board[j][i/2];
                            break;
                    }
                    //cout  << this->board[i/2][j];
                    if (j != (columns - 1))
                            cout << " ║ ";

            }
            cout << " ║" << endl;//"\33[0m" << "║" << endl;
        }
    }
    cout << "\t" << "╚";
    for (int k = 0; k < columns * 2-1; k++)
            if (k%2)
                    cout << "╩";
            else
                    cout << "═══";
    cout << "╝" << endl;
}
/*private method that checks if we have a 4 match
 option variable : 1-down, 2-left, 3-left and down (diagonal), 4-left and up (diagonal)
 returns true if we had a 4 match*/
 bool Game::checkFour(int option,int col,int row){
     char toChk=this->board[col][row];
     bool flag=false;
     int index,index2;
     switch (option){
         /*check 4 down*/
         case 1:
             for (index=row+1;index <= row+3;index ++){
                 if(this->board[col][index]!= toChk)
                     break;
             }
             if(index-1==row+3)
                 flag=true;
             break;
         /*check 4 left*/
         case 2:
             for(index=col+1;index <= col+3;index++)
                 if(this->board[index][row]!= toChk)
                     break;
             if(index-1==col+3)
                 flag=true;             
             break;
         /*check 4 diagonal (left and down)*/
         case 3:
             index=row+1;
             index2=col+1;
             while((index <= row+3) && (index2 <= col+3)){
                 if(this->board[index2][index]!=toChk)
                     break;
                 index++;
                 index2++;
             }
             if ((index==row+3)&&(index2 == col+3))
                 flag=true;
             break;
         /*check 4 diagonal (left and up)*/
         case 4:
             index=row-1;
             index2=col+1;
             while((index >= row-3) && (index2 <= col+3)){
                 if(this->board[index2][index]!=toChk)
                     break;
                 index--;
                 index2++;
             }
             if ((index+1==row-3) && (index2-1 == col+3))
                 flag=true;             
             break;
         default:
             break;
     }
     return flag;
}
/*runs all over the board and checks winning condition for each cell.
 if winning condition is true returning the winning player(8*n^2)*/
int Game::checkEndGame(){
    int flag=0;
    char check;
    for(int j=0;j<columns;j++){
        if(flag==1)
            break;
        for(int i=0;i<lines;i++){
            check=this->board[j][i];
            if(check =='*')
                continue;
            if(j <= 3){
                if(i<=2){
                    if(checkFour(1,j,i) || checkFour(2,j,i)|| checkFour(3,j,i)){
                        flag=1;
                        break;
                    }
                }
                else{
                    if(checkFour(4,j,i)||checkFour(2,j,i)){
                        flag=1;
                        break;
                    }
                }
                
            }
            else{
                if(i<=2){
                    if(checkFour(1,j,i)){
                        flag=1;
                        break;
                    }
                }                
            }
        }       
    }
    /*if we found winning and the char was player's 1 we return 1*/
    if((flag==1)&&(check==this->playerOneColor))
        return 1;
    /*if we found winning and the char was player's 2 we return 1*/
    else if((flag==1)&&(check==this->playerTwoColor))
        return 2;
    else
        return 0;
}
/*update board after insert or remove, note that addORremove variable is (0-6) range
 also note that the check if we can insert or remove is on client side..*/
void Game::updateBoard(int position, char addORremove,int playerPlaying)
{
    int lastBlank=0;
    /*searching for first position in column which the last blank*/
    while((this->board[position][lastBlank + 1]=='*') && (lastBlank < lines))
        lastBlank+=1;
    
    switch(addORremove){
        /*if command was add*/
        case 'A':
            /*if its player 1 turn -> inserting the color in the first free row in selected column*/
            if(playerPlaying == 1)
                this->board[position][lastBlank]=this->playerOneColor;
            /*same for player 2*/
            else if (playerPlaying == 2)
                this->board[position][lastBlank]=this->playerTwoColor;           
            cout<<"--player color added to column successfully"<<endl;           
            break;
        /*if command is remove*/
        case 'R':
            /*if we have more then 1 color at the column */
            if(lastBlank + 1 != lines){
                /*dropping all chars down*/
                for(int p = lastBlank + 1;p < lines; p++)
                    this->board[position][p+1]=this->board[position][p];
            }
            /*nulling the first position which hasn't been '*' */
            this->board[position][lastBlank+1]='*';
            cout<<"--player color removed from column successfully"<<endl;
            break;
        /*if command is not correct*/    
        default:
            cout<<"--Error while trying to insert to grid: no valid option selected (add or remove)."<<endl;
            break;
    }
}