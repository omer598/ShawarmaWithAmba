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
};
void nullTheArray(char * buffer)
{
    for(int i=0;i<sizeof(buffer);i++)
        buffer[i]='\0';
}

int main ()
{
    /*variables*/
    user_data u1,u2;
    int sock;
    unsigned int len;
    string tmp;
    char** board = new char*[lines];
	for (int p=0;p<lines;p++)
		board[p] = new char[coloumns];
    /*buffer for messages and data*/
    char buffer[1024]={'\0'};
    /*define socket data for both server and client*/
    struct sockaddr_in serv,client;
    /*setting listen socket.
     return -1 if failed, o.w returns socket number*/
    cout<<"% server log %"<<endl<<endl;
    if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        cout<<"--Error: Failed creating socket"<<endl;
        return 1;
    }
    /*sets server socket (to listen) data*/
    serv.sin_family = AF_INET;
    serv.sin_port = htons(65535);
    serv.sin_addr.s_addr = INADDR_ANY;
    len=sizeof(struct sockaddr_in);
    /*binding port to listen*/
    if((bind(sock,(struct sockaddr*)&serv,len))==-1)
    {
        cout<<"--Error: Failed to bind"<<endl;
        return 1;
    }
    /*set server to listen to 2 incoming connection*/
    listen (sock,2);
    /*accept first connection */
    cout<<"--waiting for player 1 to connect"<<endl;
    u1.client_id=accept(sock,(struct sockaddr *)&client ,&len);
    cout<<"--client connected from port: "<<ntohs(client.sin_port)<<" and ip: "<<inet_ntoa(client.sin_addr)<<endl;
    sprintf(buffer,"%s",inet_ntoa(client.sin_addr));
    send(u1.client_id,buffer,sizeof(buffer),0);
    nullTheArray(buffer);
    sprintf(buffer,"%s","please choose nickname:");
    cout<<"--nickname selection sent to user 1, waiting for nick input"<<endl;
    send(u1.client_id,buffer,sizeof(buffer),0);
    recv(u1.client_id,buffer,sizeof(buffer),0);
    u1.username.assign(buffer);
    cout<<"--player 1 nickname: "<<u1.username<<endl;
    cout<<"--waiting for player 2 to connect"<<endl;
    u2.client_id=accept(sock,(struct sockaddr *)&client ,&len);
    cout<<"--client connected from port: "<<ntohs(client.sin_port)<<" and ip: "<<inet_ntoa(client.sin_addr)<<endl;
    sprintf(buffer,"%s",inet_ntoa(client.sin_addr));
    send(u2.client_id,buffer,sizeof(buffer),0);
    nullTheArray(buffer);
    sprintf(buffer,"%s","please choose nickname:");
    cout<<"--nickname selection sent to user 2, waiting for nick input"<<endl;
    send(u2.client_id,buffer,sizeof(buffer),0);
    recv(u2.client_id,buffer,sizeof(buffer),0);
    u2.username.assign(buffer);
    cout<<"--player 2 nickname: "<<u2.username<<endl;
    cout<<"--waiting for player 1 to choose color"<<endl;

    //Player color pick and input control
    do{
    	nullTheArray(buffer);
    	//cout <<"while 1" << endl;
        //this command will be sent to user 1 only
        sprintf(buffer,"%s","I");
        send(u1.client_id,buffer,sizeof(buffer),0);
    	nullTheArray(buffer);
        sprintf(buffer,"%s","please insert color (R or B only):");
        send(u1.client_id,buffer,sizeof(buffer),0);
        recv(u1.client_id,buffer,sizeof(buffer),0);
        //command to u1 only end here
        u1.color = buffer[0];
        if(u1.color == 'R' || u1.color == 'r' )
        {
                u2.color='B';
                cout<<"--color selected successfully!"<<endl;
        }
        else if (u1.color == 'B' || u1.color == 'b' )
        {
                u2.color='R';
                cout<<"--color selected successfully!"<<endl;
        }
        else
        {
                u2.color='Z';
                cout<<"--wrong color selected, sending-again"<<endl;
        }
    } while (u2.color == 'Z');

    //Start game screen
    sprintf(buffer,"%s","2");
    send(u1.client_id,buffer,sizeof(buffer),0);
    send(u2.client_id,buffer,sizeof(buffer),0);
	nullTheArray(buffer);
	for (int i=0;i<lines;i++)
		for (int j=0;j>coloumns;j++)
			sprintf(buffer,"%c",board[i][j]);
    send(u1.client_id,buffer,sizeof(buffer),0);
    send(u2.client_id,buffer,sizeof(buffer),0);
	nullTheArray(buffer);
         
    
    //Print all players data -- server side
    cout << "--users data" << endl;
    cout << endl << "  user 1:" << endl << "  client id: " << u1.client_id << " nickname: " <<
            u1.username << " color: " << u1.color << endl << endl;
    cout << "  user 2:" << endl << "  client id: "<<u2.client_id<<" nickname: "<<
            u2.username<<" color: "<<u2.color<<endl;


    /*sends game data to both players*/
    buffer[0]='M';
    send(u1.client_id,buffer,sizeof(buffer),0);
    send(u2.client_id,buffer,sizeof(buffer),0);
    nullTheArray(buffer);
    sprintf(buffer,"you are playing against: %s ,your color is: %c\n" ,(char*)u2.username.c_str(),u1.color);
    send(u1.client_id,buffer,sizeof(buffer),0);
    nullTheArray(buffer);
    sprintf(buffer,"you are playing against: %s ,your color is: %c\n" ,(char*)u1.username.c_str(),u2.color);
    send(u2.client_id,buffer,sizeof(buffer),0);  
    nullTheArray(buffer);
          
    //send an end game message to both players
    sprintf(buffer,"%s","E");
    send(u1.client_id,buffer,sizeof(buffer),0);
    send(u2.client_id,buffer,sizeof(buffer),0); 
	nullTheArray(buffer);
    cout << endl << "% end of log %" << endl;

    int true1 = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true1,sizeof(int));
 }
