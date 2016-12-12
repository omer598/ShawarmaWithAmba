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
    //this->playerPlaying=playerStarting;
    this->playerOneColor=p1c;
    this->playerTwoColor=p2c;
}
Game::~Game(){
    for (int p = 0; p < columns; p++)
        delete [] this->board[p];
    delete [] this->board;  
}
void Game::printBoard(){
    for(int i=0;i<lines;i++){
        for(int j=0;j<columns;j++)
            cout<<this->board[j][i]<<" ";
        cout<<endl;
    }
    
}
/*private method that checks if we have a 4 match
 option variable : 1-down, 2-left, 3-left and down (diagonal), 4-left and up (diagonal)
 returns true if we had a 4 match*/
 bool Game::checkFour(int option,int row,int col){
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
             if(index==row+3)
                 flag=true;
             break;
         /*check 4 left*/
         case 2:
             for(index=col+1;index <= col+3;index++)
                 if(this->board[index][row]!= toChk)
                     break;
             if(index==col+3)
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
             if ((index==row-3) && (index2 == col+3))
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
    for(int i=0;i<lines;i++){
        for(int j=0;j<columns;j++){
            check=this->board[j][i];
            if(j <= 3){
                if(i<=2){
                    if(checkFour(1,i,j) || checkFour(3,i,j)){
                        flag=1;
                        break;
                    }
                }
                else{
                    if(checkFour(4,i,j)){
                        flag=1;
                        break;
                    }
                }
                
            }
            else{
                if(i<=2){
                    if(checkFour(2,i,j)){
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