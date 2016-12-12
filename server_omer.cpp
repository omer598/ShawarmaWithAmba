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
	int client_id;
	string username;
	char color;
	char start_bit;
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
	user_data u1, u2;
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
	u1.client_id = accept(sock, (struct sockaddr *)&client, &len);
	cout << "--client connected from port: " << ntohs(client.sin_port) << " and ip: " << inet_ntoa(client.sin_addr) << endl;
	sprintf(buffer, "%s", inet_ntoa(client.sin_addr));
	send(u1.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose nickname:");
	cout << "--nickname selection sent to user 1, waiting for nick input" << endl;
	send(u1.client_id, buffer, sizeof(buffer), 0);
	recv(u1.client_id, buffer, sizeof(buffer), 0);
	u1.username.assign(buffer);
	cout << "--player 1 nickname: " << u1.username << endl;

	/*accept 2nd connection */
	cout << "--waiting for player 2 to connect" << endl;
	u2.client_id = accept(sock, (struct sockaddr *)&client, &len);
	cout << "--client connected from port: " << ntohs(client.sin_port) << " and ip: " << inet_ntoa(client.sin_addr) << endl;
	sprintf(buffer, "%s", inet_ntoa(client.sin_addr));
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose nickname:");
	cout << "--nickname selection sent to user 2, waiting for nick input" << endl;
	send(u2.client_id, buffer, sizeof(buffer), 0);
	recv(u2.client_id, buffer, sizeof(buffer), 0);
	u2.username.assign(buffer);
	cout << "--player 2 nickname: " << u2.username << endl;
	cout << "--waiting for player 1 to choose color" << endl;

	//Player color pick and input control
	do{
		nullTheArray(buffer);
		//this command will be sent to user 1 only
		sprintf(buffer, "%s", "I");
		send(u1.client_id, buffer, sizeof(buffer), 0);
		nullTheArray(buffer);
		sprintf(buffer, "%s", "please insert color (R or B only):");
		send(u1.client_id, buffer, sizeof(buffer), 0);
		recv(u1.client_id, buffer, sizeof(buffer), 0);
		//command to u1 only end here
		u1.color = buffer[0];
		if (u1.color == 'R' || u1.color == 'r')
		{
			u1.color = 'R';
			u2.color = 'B';
			cout << "--Color selected successfully!" << endl;
		}
		else if (u1.color == 'B' || u1.color == 'b')
		{
			u1.color = 'B';
			u2.color = 'R';
			cout << "--Color selected successfully!" << endl;
		}
		else
		{
			u2.color = 'Z';
			cout << "--Wrong color selected, sending-again" << endl;
		}
	} while (u2.color == 'Z');

	//Player choose a bit and server XORs the bits and chooses who starts the game
	nullTheArray(buffer);
	//this command will be sent to user 1 only
	sprintf(buffer, "%s", "I");
	send(u1.client_id, buffer, sizeof(buffer), 0);
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "%s", "please choose 1 or 0\n");
	send(u1.client_id, buffer, sizeof(buffer), 0);
	send(u2.client_id, buffer, sizeof(buffer), 0);

	//receive player one XOR bit
	buffer[1] = '\0';
	recv(u1.client_id, buffer, sizeof(buffer), 0);
	u1.start_bit = buffer[0];
	while (((u1.start_bit != '0') && (u1.start_bit != '1')) || buffer[1] != '\0')
	{
		sprintf(buffer, "%s", "I");
		send(u1.client_id, buffer, sizeof(buffer), 0);
		sprintf(buffer, "%s", "Choose a valid value (0 or 1)\n");
		send(u1.client_id, buffer, sizeof(buffer), 0);
		buffer[1] = '\0';
		recv(u1.client_id, buffer, sizeof(buffer), 0);
		u1.start_bit = buffer[0];
	}

	//receive player 2 XOR bit
	buffer[1] = '\0';
	recv(u2.client_id, buffer, sizeof(buffer), 0);
	u2.start_bit = buffer[0];
	while (((u2.start_bit != '0') && (u2.start_bit != '1')) || buffer[1] != '\0')
	{
		sprintf(buffer, "%s", "I");
		send(u2.client_id, buffer, sizeof(buffer), 0);
		sprintf(buffer, "%s", "Choose a valid value (0 or 1)\n");
		send(u2.client_id, buffer, sizeof(buffer), 0);
		buffer[1] = '\0';
		recv(u2.client_id, buffer, sizeof(buffer), 0);
		u2.start_bit = buffer[0];
	}
	//calculate who starts the game
	u1.start_bit = u1.start_bit - '0';
	u2.start_bit = u2.start_bit - '0';
	firstPlayer = u1.start_bit ^ u2.start_bit;


	//Start game

	//send updated board
	sprintf(buffer, "%s", "U");
	send(u1.client_id, buffer, sizeof(buffer), 0);
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	strcpy(buffer, board);
	send(u1.client_id, buffer, sizeof(buffer), 0);
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);


	//Print all players data -- server side
	cout << "--users data" << endl;
	cout << endl << "  user 1:" << endl << "  client id: " << u1.client_id << " nickname: " <<
		u1.username << " color: " << u1.color << endl << endl;
	cout << "  user 2:" << endl << "  client id: " << u2.client_id << " nickname: " <<
		u2.username << " color: " << u2.color << endl;
	cout << "--Player " << 1+firstPlayer << " starts" << endl;

	/*sends game data to both players*/
	buffer[0] = 'M';
	send(u1.client_id, buffer, sizeof(buffer), 0);
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "You are playing against player %s, your color is: %c\nPlayer %d starts\n", (char*)u2.username.c_str(), u1.color, (1 + firstPlayer));
	send(u1.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	sprintf(buffer, "You are playing against player %s, your color is: %c\nPlayer %d starts\n", (char*)u1.username.c_str(), u2.color, (1 + firstPlayer));
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);

	//send an end game message to both players
	sprintf(buffer, "%s", "E");
	send(u1.client_id, buffer, sizeof(buffer), 0);
	send(u2.client_id, buffer, sizeof(buffer), 0);
	nullTheArray(buffer);
	cout << endl << "% end of log %" << endl;

	int true1 = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true1, sizeof(int));
}
