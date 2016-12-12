/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Game.h
 * Author: omri
 *
 * Created on December 9, 2016, 2:37 AM
 */

#ifndef GAME_H
#define GAME_H

class Game
{
    private:
        char ** board;
        //int playerPlaying;
        char playerOneColor;
        char playerTwoColor;
        bool checkFour(int,int,int);
    public:
        //char ** board;
        Game(char,char);
        ~Game();
        int checkEndGame();
        void updateBoard(int,char,int);
        void printBoard();
        void returnBoard(char * buff);
        char ** getBoard();
};

#endif /* GAME_H */

