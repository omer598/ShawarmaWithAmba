/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   player.h
 * Author: omri
 *
 * Created on December 8, 2016, 5:34 AM
 */

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <string.h>

using namespace std;

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    private:
        int client_id;
	string username;
	char color;
	char start_bit;
    public:
        void setID(int id){this->client_id=id;};
        void setUN(string userN){this->username=userN;};
        void setColor(char c) {this->color=c;};
        void setStartBit(char bit){this->start_bit=bit;};
        int getID(){return this->client_id;};
        string getUN(){return this->username;};
        char getColor (){return this->color;};
        char getStartBit (){return this->start_bit;};
}



#endif /* PLAYER_H */

