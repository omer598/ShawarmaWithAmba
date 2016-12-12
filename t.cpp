/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   t.cpp
 * Author: omri
 *
 * Created on December 12, 2016, 3:15 PM
 */


#include <stdio.h>
#include <iostream>
#include "Game.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    Game g1('R','B');
    g1.printBoard();
    g1.updateBoard(0,'A',1);
    g1.printBoard();
    g1.updateBoard(0,'A',1);
    g1.printBoard();
    g1.updateBoard(0,'A',1);
    g1.printBoard();
    g1.updateBoard(0,'A',1); 
    g1.printBoard();
    if(g1.checkEndGame()==1)
        cout<< "player 1"<< endl;
    else if(g1.checkEndGame()==2)
        cout<< "Player 2"<< endl;
    else
        cout<< "none"<< endl;
    return 0;
}

