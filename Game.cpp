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
#define coloumns 7
using namespace std;

//constructor for Game object. setting game init data
Game::Game(int playerStarting,char p1c, char p2c){
    /*creating game board with 7*6= 42 cells*/
    this->board=new char[lines*coloumns];
    /* set all cells values to * */
    for (int p = 0; p < lines*coloumns; p++)
        board[p] = '*';
    /*setting starting player*/
    this->playerPlaying=playerStarting;
    this->playerOneColor=p1c;
    this->playerTwoColor=p2c;
}
Game::~Game(){
    delete [] this->board;
}
void Game::printBoard(){
    for(int i=0;i<lines;i++)
    {
        for(int j=0;j<coloumns;j++)
            cout<<this->board[j]<<" ";
        cout<<endl;
    }
    
}
bool Game::checkEndGame(){
    
}
/*update board after insert or remove, note that addORremove variable is (0-6) range
 also note that the check if we can insert or remove is on client side..*/
void Game::updateBoard(int position, char addORremove)
{
    int hlper = position;
    //search for first position which we have first color in column
    while ((this->board[position + 7] == '*') && ((position+7) <= hlper + coloumns * (lines-1)))
        position+=7;
    
    switch (addORremove){
        /*if command was insert data from top*/
        case 'A':
            /*if its player 1 turn -> inserting the color in the first free row in selected column*/
            if(playerPlaying == 1)
                this->board[position]=this->playerOneColor;
            /*same for player 2*/
            else if (playerPlaying == 2)
                this->board[position]=this->playerTwoColor;           
            cout<<"player color added to column successfully"<<endl;
            break;
        /*if command is remove*/
        case 'R':
            /*if we have more then 1 color at the column */
            if(position + 7 != (hlper + coloumns * (lines-1))){
                /*dropping all chars down*/
                for(int p = position +7;p < hlper + coloumns * (lines-1); p+=7)
                    this->board[p+7]=this->board[p];
            }
            /*nulling the first position which hasen't been * */
            this->board[position + 7]='*';
            cout<<"player color removed from column successfully"<<endl;
            break;
        /*if command is not correct*/    
        default:
            cout<<"--Error while trying to insert to grid: no valid option selected (add or remove)."<<endl;
    }
}