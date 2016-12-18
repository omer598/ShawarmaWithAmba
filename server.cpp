//============================================================================
// Name        : server.cpp
// Author      : Omri Israeli @ Omer Eini
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : server side for 4 in a row
//============================================================================
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include "Player.h"
#include "Game.h"


#define lines 6
#define coloumns 7
#define bufferSize 1024
#define clearScreen() (cout << "\033[H\033[J")
using namespace std;

/*struck for socket data*/
struct sockaddr_serverIn{
	short sin_family;
	u_short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};

void nullTheArray(char * buffer)
{
	for (int i = 0; i<bufferSize; i++)
		buffer[i] = '\0';
}

void printArray(char * array, int size)
{
	for (int i = 0; i < size; i++)
		cout << array[i];
	cout << endl;
}
/*put first char to Msg*/
void getMsgReady(char c,char * buffer)
{
    string s; 
    s.assign(buffer);
    s.insert(0,&c);
    s.copy(buffer,s.size(),0);
}
/*updates board (to print) and send to each user*/
void updateBoards(Game * g, char * buffer, Player * u1, Player * u2)
{
	//nullTheArray(buffer);
	g->returnBoard(buffer);
	g->printBoard();
        getMsgReady('U',buffer);
        //cout <<"the buffer"<<endl<<buffer << endl;
	send(u1->getID(), buffer, bufferSize, 0);
        //cout <<"the buffer after 1"<<endl<<buffer << endl;
	send(u2->getID(), buffer, bufferSize, 0);
        //cout <<"the buffer after 2"<<endl<<buffer << endl;
	nullTheArray(buffer);
}

int main(int argc, char * argv[])
{
	clearScreen();
	/*variables*/
        string tmp;
        Player u1, u2;
        char buffer[1024] = { '\0' };
        char board[lines*coloumns];
	int sock;
	int firstPlayer,playerPlayed=0;
	unsigned int len;
	int endGameFlag = 0;
	int playerTurn;
	int port = atoi(argv[1]);
	string s;
	/*buffer for messages and data*/
	/*define socket data for both server and client*/
	struct sockaddr_in serv, client;
	/*setting listen socket.
	return -1 if failed, o.w returns socket number*/
	cout << "% server log %" << endl << endl;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cout << "--Error: Failed creating socket" << endl;
		return 1;
	}
	/*sets server socket (to listen) data*/
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;
	len = sizeof(struct sockaddr_in);
	/*binding port to listen*/
	if ((bind(sock, (struct sockaddr*)&serv, len)) == -1)
	{
		cout << "--Error: Failed to bind" << endl;
		return 1;
	}
	/*set server to listen to 2 incoming connection*/
	listen(sock, 2);


	/*accept first connection */
	cout << "--waiting for player 1 to connect" << endl;
	u1.setID (accept(sock, (struct sockaddr *)&client, &len));
	cout << "--client connected from port: " << ntohs(client.sin_port) << " and ip: " << inet_ntoa(client.sin_addr) << endl;
	sprintf(buffer, "%s", inet_ntoa(client.sin_addr));
	send(u1.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose nickname:");
	cout << "--nickname selection sent to user 1, waiting for nick input" << endl;
	send(u1.getID(), buffer, sizeof(buffer), 0);
	recv(u1.getID(), buffer, sizeof(buffer), 0);
        tmp.assign(buffer);
	u1.setUN(tmp);
	cout << "--player 1 nickname: " << u1.getUN() << endl;

	/*accept 2nd connection */
	cout << "--waiting for player 2 to connect" << endl;
	u2.setID(accept(sock, (struct sockaddr *)&client, &len));
	cout << "--client connected from port: " << ntohs(client.sin_port) << " and ip: " << inet_ntoa(client.sin_addr) << endl;
	sprintf(buffer, "%s", inet_ntoa(client.sin_addr));
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose nickname:");
	cout << "--nickname selection sent to user 2, waiting for nick input" << endl;
	send(u2.getID(), buffer, sizeof(buffer), 0);
	recv(u2.getID(), buffer, sizeof(buffer), 0);
        tmp.assign(buffer);
	u2.setUN(tmp);
	cout << "--player 2 nickname: " << u2.getUN() << endl;
	cout << "--waiting for player 1 to choose color" << endl;

	//Player color pick and input control
	do{
		nullTheArray(buffer);
		//this command will be sent to user 1 only
		sprintf(buffer, "%s", "please insert color (R or B only):");
                getMsgReady('I',buffer);
		send(u1.getID(), buffer, sizeof(buffer), 0);
		recv(u1.getID(), buffer, sizeof(buffer), 0);
		//command to u1 only end here
		u1.setColor( buffer[0]);
		if (u1.getColor() == 'R' || u1.getColor() == 'r')
		{
			u1.setColor ('R');
			u2.setColor ('B');
			cout << "--Color selected successfully!" << endl;
		}
		else if (u1.getColor() == 'B' || u1.getColor() == 'b')
		{
			u1.setColor ('B');
			u2.setColor ('R');
			cout << "--Color selected successfully!" << endl;
		}
		else
		{
			u2.setColor ('Z');
			cout << "--Wrong color selected, sending-again" << endl;
		}
	} while (u2.getColor() == 'Z');

	//Player choose a bit and server XORs the bits and chooses who starts the game
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose 1 or 0");
        getMsgReady('I',buffer);
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);

	//receive player one XOR bit
	//buffer[1] = '\0';
	recv(u1.getID(), buffer, sizeof(buffer), 0);
	u1.setStartBit (buffer[0]);
	while (((u1.getStartBit () != '0') && (u1.getStartBit () != '1')) || buffer[1] != '\0')
	{       
		sprintf(buffer, "%s", "Choose a valid value (0 or 1)\n");
                getMsgReady('I',buffer);
		send(u1.getID(), buffer, sizeof(buffer), 0);
		//buffer[1] = '\0';
		recv(u1.getID(), buffer, sizeof(buffer), 0);
		u1.setStartBit(buffer[0]);
	}

	//receive player 2 XOR bit
	//buffer[1] = '\0';
	recv(u2.getID(), buffer, sizeof(buffer), 0);
	u2.setStartBit (buffer[0]);
	while (((u2.getStartBit() != '0') && (u2.getStartBit() != '1')) || buffer[1] != '\0')
	{
                
		sprintf(buffer, "%s", "Choose a valid value (0 or 1)\n");
                getMsgReady('I',buffer);
		send(u2.getID(), buffer, sizeof(buffer), 0);
		//buffer[1] = '\0';
		recv(u2.getID(), buffer, sizeof(buffer), 0);
		u2.setStartBit (buffer[0]);
	}
	//calculate
	u1.setStartBit (u1.getStartBit() - '0');
	u2.setStartBit (u2.getStartBit() - '0');
	firstPlayer = u1.getStartBit() ^ u2.getStartBit();


	Game g1(u1.getColor(), u2.getColor());
	//Start game screen
	updateBoards(&g1,buffer,&u1,&u2); /////////PO!!!!!!!!/////////////
	//Print all players data -- server side
	cout << "--users data" << endl;
	cout << endl << "  user 1:" << endl << "  client id: " << u1.getID() << " nickname: " <<
		u1.getUN() << " color: " << u1.getColor() << endl << endl;
	cout << "  user 2:" << endl << "  client id: " << u2.getID() << " nickname: " <<
		u2.getUN() << " color: " << u2.getColor() << endl;
	cout << "--Player " << 1+firstPlayer << " starts"  << endl;

	/*sends game data to both players*/
	buffer[0] = 'M';
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "You are playing against: %s ,your color is: %c\nPlayer %d starts\nGood Luck", (char*)u2.getUN().c_str(), u1.getColor(), (1 + firstPlayer));
	send(u1.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "You are playing against: %s ,your color is: %c\nPlayer %d starts\nGood Luck", (char*)u1.getUN().c_str(), u2.getColor(), (1 + firstPlayer));
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);

	endGameFlag = 0;
	playerTurn = firstPlayer;
	while(!endGameFlag){
		if (playerTurn) { //2nd player turn
			sprintf(buffer, "%s", "T");
			send(u2.getID(), buffer, sizeof(buffer), 0);
			nullTheArray(buffer);
			recv(u2.getID(), buffer, sizeof(buffer), 0);
			//cout << buffer[0] << ' ' <<buffer[1]-'0' << ' ' << buffer[3] << ' ' << playerTurn+1 << endl;
			switch (buffer[0]){
				case 'N': //receive a move
                                        /* checks if we got correct cell INPUT*/
                                        if(buffer[1] < '0' || buffer[1] >'6'){
                                            nullTheArray(buffer);
                                            sprintf(buffer,"TT\nplease insert a valid cell number (0-6)\n");
                                            send(u2.getID(), buffer, sizeof(buffer), 0);                                            
                                        }
                                        /*checks if move inserted correctly*/
                                        else if(buffer[3] != 'A' && buffer[3] != 'R'){
                                            nullTheArray(buffer);
                                            sprintf(buffer,"TT\nplease insert a valid move (A/R)\n");
                                            send(u2.getID(), buffer, sizeof(buffer), 0);                                               
                                        }
                                        /*checks if input if column cells are valid to insert or remove. if not - sends error*/
                                        else if((buffer[3] == 'A' && g1.getBoard()[buffer[1]-'0'][0] != '*')||(buffer[3]=='R' && g1.getBoard()[buffer[1]-'0'][5] != u2.getColor()))
					{
						nullTheArray(buffer);
						sprintf(buffer,"TT\ncannot perform move to cell. select another\n");
						send(u2.getID(), buffer, sizeof(buffer), 0);
					}
                                        /*checks if player already played. if yes - sends error*/
					else if (playerPlayed) {
						cout << "Player " << 1+playerTurn << "played" << endl;
						nullTheArray(buffer);
						sprintf(buffer,"TTyou are now allowed to play again. select another option");
						send(u2.getID(), buffer, sizeof(buffer), 0);
						updateBoards(&g1,buffer,&u1,&u2);
					}
                                        /*if all doing player selected move*/
					else {
						g1.updateBoard(buffer[1]-'0',buffer[3],playerTurn+1);
						updateBoards(&g1,buffer,&u1,&u2);
						playerPlayed=1;
					}
					break;
				case 'C': //receive a chat massage
					s = string(buffer).erase(0,1);
					s.insert(0,"--Chat from Player 2: ");
					cout << s << endl;
					s.insert(0,"C");
					nullTheArray(buffer);
					s.copy(buffer,s.size(),0);
					cout << "B: " << buffer <<endl;
					send(u1.getID(), buffer, sizeof(buffer), 0);
					//send(u2.getID(), buffer, sizeof(buffer), 0);
					break;
				case 'F': //End of turn
				   if(!playerPlayed){
						cout << "--info: Player didn't make any move" << endl;
						nullTheArray(buffer);
						sprintf(buffer,"TTplease make a move before ending your turn!");
						send(u2.getID(), buffer, sizeof(buffer), 0);
						//updateBoards(&g1,buffer,&u1,&u2);
					}
					else{
						cout << "--info: Player 2 has made a move and moving to player 1" << endl;
						playerPlayed = 0;
						playerTurn = 0;
					}
					break;
				default:
					break;
			}
                    if (g1.checkEndGame(u1.getColor()) && g1.checkEndGame(u2.getColor()))
                    {
                        endGameFlag = 1;
                        sprintf(buffer,"E%s Won!!\n",u2.getUN());  
                    }
                    else if (g1.checkEndGame(u2.getColor()))
                    {
                        endGameFlag = 1;
                        sprintf(buffer,"E%s Won!!\n",u2.getUN()); 
                    }
                    else if (g1.checkEndGame(u1.getColor()))
                    {
                        endGameFlag = 1;
                        sprintf(buffer,"E%s Won!!\n",u1.getUN()); 
                    }
                    else
                        endGameFlag = 0;
		}
		else { //1st player turn
                        cout<<"now player 1"<<endl;
			nullTheArray(buffer);
			sprintf(buffer, "%s", "T");
			send(u1.getID(), buffer, sizeof(buffer), 0);
			nullTheArray(buffer);
			recv(u1.getID(), buffer, sizeof(buffer), 0);
			cout << buffer[0] << ' ' <<buffer[1]-'0' << ' ' << buffer[3] << ' ' << playerTurn+1 << endl;
			switch (buffer[0]){
				case 'N': //receive a move
                                        /* checks if we got correct cell INPUT*/
                                        if(buffer[1] < '0' || buffer[1] >'6'){
                                            nullTheArray(buffer);
                                            sprintf(buffer,"TT\nplease insert a valid cell number (0-6)\n");
                                            send(u1.getID(), buffer, sizeof(buffer), 0);                                            
                                        }
                                        /*checks if move inserted correctly*/
                                        else if(buffer[3] != 'A' && buffer[3] != 'R'){
                                            nullTheArray(buffer);
                                            sprintf(buffer,"TT\nplease insert a valid move (A/R)\n");
                                            send(u1.getID(), buffer, sizeof(buffer), 0);                                               
                                        }
                                        /*checks if input if column cells are valid to insert or remove. if not - sends error*/
                                        else if((buffer[3] == 'A' && g1.getBoard()[buffer[1]-'0'][0] != '*')||(buffer[3]=='R' && g1.getBoard()[buffer[1]-'0'][5] != u1.getColor())){
						nullTheArray(buffer);
						sprintf(buffer,"TT\ninvalid cell, select another!\n");
						send(u1.getID(), buffer, sizeof(buffer), 0);
					}
					else if (playerPlayed)
					{
						cout << "--info: Player " << 1+playerTurn << "played" << endl;
					nullTheArray(buffer);
					sprintf(buffer,"TT\nyou are not allowed to play again.\nselect another option!\n");
					send(u1.getID(), buffer, sizeof(buffer), 0);
					updateBoards(&g1,buffer,&u1,&u2);
					}
					else{
						g1.updateBoard(buffer[1]-'0',buffer[3],playerTurn+1);
						updateBoards(&g1,buffer,&u1,&u2);
                                                playerPlayed=1;
					}
					break;
				case 'C': //receive a chat massage
					s = string(buffer).erase(0,1);
					s.insert(0,"--Chat from Player 1: ");
					cout << s << endl;
					s.insert(0,"C");
					nullTheArray(buffer);
					s.copy(buffer,s.size(),0);
					cout << "B: " << buffer <<endl;
					//send(u1.getID(), buffer, sizeof(buffer), 0);
					send(u2.getID(), buffer, sizeof(buffer), 0);
					break;
				case 'F': //End of turn
					/*checks if player mad a move before ending turn. if not sending error*/
					if(!playerPlayed){
						cout << "--info: player didn't make any move" << endl;
						nullTheArray(buffer);
						sprintf(buffer,"TT\nyou are not allowed to play again.\nselect another option!\n");
						send(u1.getID(), buffer, sizeof(buffer), 0);
						//updateBoards(&g1,buffer,&u1,&u2);
					}
					/*if player played his turn setting flag to next player*/
					else{
						cout << "--info: Player 1 has made a move and moving to player 2" << endl;
						nullTheArray(buffer);
						playerPlayed = 0;
						playerTurn = 1;
					}
					break;
				default:
					break;
			}

		}
                if (g1.checkEndGame(u1.getColor()) && g1.checkEndGame(u2.getColor()))
                {
                    endGameFlag = 1;
                    sprintf(buffer,"E%s Won!!\n",u1.getUN());   
                }
                else if (g1.checkEndGame(u1.getColor()))
                {
                    endGameFlag = 1;
                    sprintf(buffer,"E%s Won!!\n",u1.getUN()); 
                }
                else if (g1.checkEndGame(u2.getColor()))
                {
                    endGameFlag = 1;
                    sprintf(buffer,"E%s Won!!\n",u2.getUN()); 
                }
                else
                    endGameFlag = 0;
	}
        
	//while (checkIfWon()) - need to put inf loop until some1 wins..
	//sprintf(buffer,"please select a column:\n");
	//maybe needs to be u2.. we need to check who wins first
	//send(u1.getID(),buffer,sizeof(buffer), 0);
	//nullTheArray(buffer);
	
	//send an end game message to both players
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	cout << endl << "% end of log %" << endl;

	int true1 = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true1, sizeof(int));
}
