//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include <windows.h>

#include "..\PacketWriter.h"
#include "..\PacketReader.h"
#include "..\common.h"

using namespace std;

class sSocket
{
public:
	sSocket();
	~sSocket();
	SOCKADDR_IN* InfoServer;

	PacketWriter Writer;
	PacketReader Reader;
	struct Nick NickPar[20];
	
	int TotalMembers;
	int nAccept;
	int i; //looping through all connections

	void SetSocket(SOCKET sock, int flag); //useless atm

	int Init(unsigned short port);

	//client
	int Accept();
	void RecvData();
	void SendData(SOCKET cl, int size);
	void NewConnection();
	void CloseConnection();
	void SendToAll(int bytes);
	void SendToAllNotice(char *nick, int size, int flag);
	void Send(char *Buffer, int size);

	void SendNickList(int client);	//send to specific client
	void SendNickList();	//send to all clients
	// client

	void Close();	//used for server

	void print(int size);

	int FindNick(int id); //returns array number
	int FindNick(char *nick, int size); //returns array of that nick
	void AddNick(char *nick, int size);

	void ParseData(int bytes);

	bool MainFunction();

private:

	SOCKET s;
	SOCKET c;

	fd_set master;
	fd_set fdTemp;
	int max_con; //max connections

	struct timeval time;

	char Buffer[1024];
	char Sbuffer[1024];
	

};