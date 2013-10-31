#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <windows.h>

#include "socket_server.h"

using namespace std;

#define CLIE 1
#define SERV 0

int main()
{
	sSocket Socket;	//initialization of class

	SOCKET sServer = {0};
	SOCKET sClient = {0};

	bool connection = true;
	Socket.SetSocket(sServer, SERV);

	if(Socket.Init(25015) == 1) connection = false;

	while(connection == true)
	{
		connection = Socket.MainFunction();
	}

	printf("Closing...\n");
	Socket.Close();
	system("PAUSE");
	return 0;
}