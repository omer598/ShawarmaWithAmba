//============================================================================
// Name        : client.cpp
// Author      : Omri Israeli @ Omer Eini
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : client side for 4 in a row
//============================================================================

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

using namespace std;
#define lines 6
#define coloumns 7
#define underline "\33[4m"
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define NoColor "\033[0m"
#define clearScreen() (cout << "\033[H\033[J")
#define gotoxy(x,y) (printf("\033[%d;%dH", (x), (y)))

struct sockaddr_serverIn{
	short sin_family;
	u_short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};

void nullTheArray(char * buffer)
{
	for (int i = 0; i<sizeof(buffer); i++)
		buffer[i] = '\0';
}

void updateBoard(char* buffer, char** board)
{
	int k = 1;
	for (int i = 0; i< lines; i++)
	{
		for (int j = 0; j< coloumns; j++)
		{
			board[i][j] = buffer[k];
			k++;
		}
	}
}

void printBoard(char** msg)
{
	char sideBar = '|';
	char lowBar = '_';
	int k = 0;
	clearScreen();
	int y_axis = 3;
	int x_axis = 3;
	//gotoxy(x_axis,y_axis);
	cout << "Player 1 color: " << endl << "Player 2 color:" << endl;
	cout << endl << endl << endl;
	cout << "\t" << "╔";
	for (int k = 0; k < coloumns * 2-1; k++)
		if (k%2)
			cout << "╦";
		else
			cout << "   ";
	cout << "╗" << endl;
	//gotoxy(x_axis,++y_axis);

	for (int i = 0; i<lines*2-1; i++)
	{
		if (i%2)
		{
			cout << "\t" << "╠"; //<< underline;
			for (int j = 0; j < coloumns*2-1; j++)
			{
				if (j%2)
					cout << "╬";
				else if (j != (coloumns*2 - 1))
					cout << "═══";

			}
			cout << "╣" << endl;//"\33[0m" << "║" << endl;

		}
		else
		{
			cout << "\t" << "║" << " "; //<< underline;
			for (int j = 0; j < coloumns; j++)
			{
				switch(msg[i/2][j]){
				case 'R':
					cout << RED << "●" << NoColor;
					break;
				case 'B':
					cout << BLUE << "●" << NoColor;
					break;
				default:
					cout << msg[i/2][j];
					break;
				}
				//cout  << msg[i/2][j];
				if (j != (coloumns - 1))
					cout << " ║ ";

			}
			cout << " ║" << endl;//"\33[0m" << "║" << endl;
		}
		//gotoxy(x_axis,++y_axis);
	}

	cout << "\t" << "╚";
	for (int k = 0; k < coloumns * 2-1; k++)
		if (k%2)
			cout << "╩";
		else
			cout << "═══";
	cout << "╝" << endl;

}

int printMenu() {
	int num;
	cout << "i.\tmake a move (only once per turn)." << endl;
	cout << "ii.\twrite a message to the other player." << endl;
	cout << "iii.\tend the turn (only after making a move!!)." << endl;
	cin >> num;
	return num;
}
/*clears first char from msg
void getClearMsg(char * buffer){
    string s;
    s.assign(buffer);
    s.erase(0,1);
    s.copy(buffer,s.size(),0);
    buffer[s.size()]='\0';
}*/

int main(int argc,char * argv [])
{
	clearScreen();
	int sock, portNum;
	string s;
	char buffer[1024] = { '\0' };
	char** board;
	board = new char*[lines];
	for (int p = 0; p<lines; p++)
		board[p] = new char[coloumns];
	sock = socket(PF_INET, SOCK_STREAM, 0);
	int port = atoi(argv[2]);
	if (sock<0)
	{
		cout << "Error establish socket" << endl;
		return 0;
	}
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	//Receives first massage (ip back!)
	recv(sock, buffer, sizeof(buffer), 0);
	cout << buffer << endl;
	recv(sock, buffer, sizeof(buffer), 0);
	cout << buffer << endl;
	nullTheArray(buffer);
	cin >> buffer;
	send(sock, buffer, sizeof(buffer), 0);
        buffer[0]='A';
        /*E- end game command (if server found a winner)*/
	while (buffer[0] != 'E')
	{
		recv(sock, buffer, sizeof(buffer), 0);
		if (buffer[0] == 'I') /*I- command that gets input from client*/
		{
                        //getClearMsg(buffer);
			cout << buffer+1 << endl;
			nullTheArray(buffer);
			cin >> buffer;
			send(sock, buffer, sizeof(buffer), 0);
		}
		else if (buffer[0] == 'U') /*U- update board user side and print it*/
		{
                        //getClearMsg(buffer);
                        cout << buffer+1 << endl;
			updateBoard(buffer+1, board);
			printBoard(board);
		}
		else if (buffer[0] == 'M') /*sys massage- prints game info*/
		{
			/*receives game info*/
			recv(sock, buffer, sizeof(buffer), 0);
			cout << "Game info: " << endl << buffer << endl;
		}
                /*T - game play, commands from server in game*/
		else if (buffer[0] == 'T')
		{
			string s;
                        /*if we got second T we had error. printing the error*/
			if (buffer[1]=='T')
			{
				//getClearMsg(buffer);
				//getClearMsg(buffer);
				cout << buffer+2 << endl;
			} else {
				switch (printMenu()){
					case 1: /*move sending, receiving command (column, A/R) and sending to server in
											 order to check if valid*/
						cout << "Please enter a move: column,'A'/'R'\n";
						getline(cin,s);
						getline(cin,s);
						s.insert(0, "N");
						nullTheArray(buffer);
						s.copy(buffer,s.size(),0);
						send(sock, buffer, sizeof(buffer), 0);
						break;
					case 2: /*sending of chat. getting the massage and setting first char to be C (symbol of massage sending)*/
						getline(cin,s);
						getline(cin,s);
						s.insert(0, "C");
						nullTheArray(buffer);
						s.copy(buffer,s.size(),0);
						send(sock, buffer, sizeof(buffer), 0);
						break;
					case 3: /*End of turn, sets first cell to F (symbol of end of turn)*/
						buffer[0] = 'F';
						send(sock, buffer, sizeof(buffer), 0);
						break;
					default:
						break;
				}
			}
		}
                /*C is chat massage from other user. prints it*/
		else if (buffer[0] == 'C')
		{
			s.assign(buffer);
			s.erase(0,1);
			cout << s << endl;
			nullTheArray(buffer);
		}

	}

	//recv(sock,buffer,sizeof(buffer),0);
        /*delete dynamic allocations!!!*/
	cout <<buffer+1<<"The game is over." << endl;
	close(sock);
	cout << "Socket closed!" << endl;
        for (int p = 0; p<lines; p++)
            delete [] board[p];
        delete[] board;
	return 0;
}

