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

using namespace std;
#define lines 6
#define coloumns 7

struct sockaddr_serverIn{
	short sin_family;
	u_short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};

void nullTheArray(char * buffer)
{
    for(int i=0;i<sizeof(buffer);i++)
        buffer[i]='\0';
}

void updateBoard(char* buffer,char** board)
{
	int k=0;
	for (int i=0;i<lines;i++)
	{
		for (int j=0;j<coloumns;j++)
		{
			board[i][j] = buffer[k];
			k++;
		}
	}

}

void printBoard(char** msg)
{
	char sideBar='|';
	char lowBar='_';
	int k=0;
	for(int i=0; i<lines; i++)
	{
		//cout<<sideBar;
			cout<<sideBar;
		for (int k = 0; k < coloumns*2; k++)
			cout<<lowBar;
		cout<<sideBar<<endl;

		cout<<sideBar;
		for (int j = 0; j < coloumns; j++)
		{
			cout<<msg[i][j];
			cout<<sideBar;
		}
		cout << endl;
	}
	cout << sideBar;
	for(int i=0;i<coloumns;i++)
		cout<<lowBar;
	cout<< sideBar <<endl;
}

int main()
{
	int sock,portNum;
	char buffer[1024]={'\0'};
	char** board;
	board = new char*[lines];
	for (int p=0;p<lines;p++)
		board[p] = new char[coloumns];
	portNum=11111;
	sock=socket(PF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		cout <<"Error establish socket" <<endl;
		return 0;
	}
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( 65535 );
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	//Receives first massage (ip back!)
	recv(sock,buffer,sizeof(buffer),0);
	cout<<buffer<<endl;
	recv(sock,buffer,sizeof(buffer),0);
	cout<<buffer<<endl;
        nullTheArray(buffer);
        cin>>buffer;
		send(sock,buffer,sizeof(buffer),0);
		//recv(sock,buffer,sizeof(buffer),0);
        while(buffer[0] != 'Z'){
    		recv(sock,buffer,sizeof(buffer),0);
			if(buffer[0] == '1') //Player Color pick
			{
				recv(sock,buffer,sizeof(buffer),0);
				cout<<buffer<<endl;
				nullTheArray(buffer);
				cin>>buffer;
				send(sock,buffer,sizeof(buffer),0);
				//recv(sock,buffer,sizeof(buffer),0);
			}
			if (buffer[0] == '2') //Update board and update printed board
			{
				recv(sock,buffer,sizeof(buffer),0);
				updateBoard(buffer, board);
				printBoard(board);
			}
        }
        recv(sock,buffer,sizeof(buffer),0);
        cout << "The game is over." << endl;
        close(sock);
        cout << "Socket closed!" << endl;
	return 0;
}

