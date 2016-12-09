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
#include "Player.h"

#define lines 6
#define coloumns 7
using namespace std;

/*struck for socket data*/
struct sockaddr_serverIn{
	short sin_family;
	u_short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};
struct user_data{

};
void nullTheArray(char * buffer)
{
	for (int i = 0; i<sizeof(buffer); i++)
		buffer[i] = '\0';
}

void printArray(char * array, int size)
{
	for (int i = 0; i < size; i++)
		cout << array[i];
	cout << endl;
}

void startConnection(int* sock, char* buffer, sockaddr_in* serv, sockaddr_in* client, user_data * u1, user_data* u2)
{

}

void initBoard(char* board)
{
	for (int p = 0; p < lines*coloumns; p++)
		board[p] = '*';
}

int main()
{
	/*variables*/
	Player u1, u2;
        string tmp;
	int sock;
	char buffer[1024] = { '\0' };
	int firstPlayer;
	unsigned int len;
	char board[lines*coloumns];
	initBoard(board);
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
	serv.sin_port = htons(65535);
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
		sprintf(buffer, "%s", "I");
		send(u1.getID(), buffer, sizeof(buffer), 0);
		nullTheArray(buffer);
		sprintf(buffer, "%s", "please insert color (R or B only):");
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
	//this command will be sent to user 1 only
	sprintf(buffer, "%s", "I");
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose 1 or 0");
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);

	//receive player one XOR bit
	buffer[1] = '\0';
	recv(u1.getID(), buffer, sizeof(buffer), 0);
	u1.setStartBit (buffer[0]);
	while (((u1.getStartBit () != '0') && (u1.getStartBit () != '1')) || buffer[1] != '\0')
	{
		sprintf(buffer, "%s", "I");
		send(u1.getID(), buffer, sizeof(buffer), 0);
		sprintf(buffer, "%s", "Choose a valid value (0 or 1)\n");
		send(u1.getID(), buffer, sizeof(buffer), 0);
		buffer[1] = '\0';
		recv(u1.getID(), buffer, sizeof(buffer), 0);
		u1.setStartBit(buffer[0]);
	}

	//receive player 2 XOR bit
	buffer[1] = '\0';
	recv(u2.getID(), buffer, sizeof(buffer), 0);
	u2.setStartBit (buffer[0]);
	while (((u2.getStartBit() != '0') && (u2.getStartBit() != '1')) || buffer[1] != '\0')
	{
		sprintf(buffer, "%s", "I");
		send(u2.getID(), buffer, sizeof(buffer), 0);
		sprintf(buffer, "%s", "Choose a valid value (0 or 1)\n");
		send(u2.getID(), buffer, sizeof(buffer), 0);
		buffer[1] = '\0';
		recv(u2.getID(), buffer, sizeof(buffer), 0);
		u2.setStartBit (buffer[0]);
	}
	//calculate
	u1.setStartBit (u1.getStartBit() - '0');
	u2.setStartBit (u2.getStartBit() - '0');
	firstPlayer = u1.getStartBit() ^ u2.getStartBit();


	//Start game screen
	sprintf(buffer, "%s", "U");
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	strcpy(buffer, board);
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);


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
        
	//while (checkIfWon()) - need to put inf loop until some1 wins..
	sprintf(buffer,"please select a column:\n");
	//maybe needs to be u2.. we need to check who wins first
	send(u1.getID(),buffer,sizeof(buffer), 0);
	nullTheArray(buffer);
	
	//send an end game message to both players
	sprintf(buffer, "%s", "E");
	send(u1.getID(), buffer, sizeof(buffer), 0);
	send(u2.getID(), buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	cout << endl << "% end of log %" << endl;

	int true1 = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true1, sizeof(int));
}
