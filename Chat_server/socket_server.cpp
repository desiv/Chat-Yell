#include "socket_server.h"

int sSocket::Init(unsigned short port)
{
	WSADATA wsa;
	InfoServer = new SOCKADDR_IN;
	ZeroMemory(InfoServer, sizeof(*InfoServer));
	int error = 0;

	FD_ZERO(&master);

	//wsa data
	error = WSAStartup(MAKEWORD(2, 2), &wsa);
	if(error != NO_ERROR || error == 1)
	{
		printf("Error at WSAStartup() Error: %d\n", error);
		WSACleanup();
		return 1;
	}

	//server info
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == SOCKET_ERROR || s == 1)
	{
		printf("Error at defining socket info Error: %d\n", s);
		WSACleanup();
		return 1;
	}

	//server information
	InfoServer->sin_family = AF_INET;
	InfoServer->sin_addr.s_addr = htonl(INADDR_ANY);	//address
	InfoServer->sin_port = htons(port);
	

	//starting server and binding to 12442 port
	if(bind(s, (sockaddr*)InfoServer, sizeof(*InfoServer)) == SOCKET_ERROR)
	{
		printf("Failed to bind the server\n");
		WSACleanup();
		return 1;
	}


	//set how many client we want to accept
	while(listen(s, 10) == SOCKET_ERROR)
	{
		Sleep(10);
	}

	printf("Server is running...\n");
	
	FD_SET(s, &master);
	max_con = s; //atm the only connection is this 

	delete [] InfoServer;
	return 0;
}

void sSocket::RecvData()
{
	int bytes = 0;

	bytes = recv(i, (char*)Buffer, 1023, 0);
	
	if(bytes <= 0)	//error
	{
		if(bytes == 0)
		{
			CloseConnection();
		}
		else
		{
			//printf("*Failed to recv data!\n");
			CloseConnection();
		}
	}
	else	//data
	{
		Reader.SetBuffer((unsigned char*)Buffer);
		ParseData(bytes);
	}
	
}

void sSocket::SendData(SOCKET cl, int size)
{
	if(send(cl, (char*)Sbuffer, size, 0) == SOCKET_ERROR)
	{
		printf("*Failed to send\n");
		closesocket(cl);
		FD_CLR(cl, &master);
	}
	//else printf("Data Sent\n\n");
	
}

void sSocket::SetSocket(SOCKET sock, int flag) //1 for client 0 - server
{
	if(flag == 0)
	{
		this->s = sock;
	}
	else 
		this->c = sock;
}

int sSocket::Accept()
{
	return accept(s, 0, 0);
}

void sSocket::NewConnection()
{
	//handle new connections
	nAccept = Accept();

	if(nAccept == -1)
	{
		printf("*Failed to accept new client* Error: %d", WSAGetLastError());
	}
	else
	{
		FD_SET(nAccept, &master); //add new connection
		if(nAccept > max_con)
		{
			max_con = nAccept;	//set max connection to loop through
		}
		
		//tell me about ur self:
		Writer.WriteWord(S_WHO_ARE_YOU);
		send(nAccept, (char*)Writer.Buffer, Writer.offset, 0);

		printf("**A new connection has been opened**\n");
	}
}

void sSocket::SendToAll(int bytes)
{
	for(int s = 0; s < bytes; s++)
		{
			Sbuffer[s] = this->Buffer[s]; //data copy
		}


		for(int x = 0; x <= max_con; x++)
		{
			if(FD_ISSET(x, &master))// blah... i don't want to send to alll connections...
			{
				if(x != this->s)	//we don't want to send to ourselfs ;o
				{
					SendData(x, bytes);
				}
			}
		}
}
void sSocket::Send(char *Buffer, int size)
{
	for(int s = 0; s < size; s++)
	{
		Sbuffer[s] = this->Buffer[s]; //data copy
	}


	for(int x = 0; x <= max_con; x++)
	{
		if(FD_ISSET(x, &master))// blah... i don't want to send to alll connections...
		{
			if(x != this->s)	//we don't want to send to ourselfs ;o
			{
				SendData(x, size);
			}
		}
	}
}

void sSocket::CloseConnection()
{
	printf("**Connection with socket %d lost**\n", i);

	closesocket(i);
	FD_CLR(i, &master);

	int arr = FindNick(i);
	TotalMembers--;
	
	SendToAllNotice((char*)NickPar[arr].nick, NickPar[arr].nick_size, 1);	//notice all
	ZeroMemory(&NickPar[arr], sizeof(Nick));	//clear spot
	SendNickList();
}



//////// Main Funk //////////////
bool sSocket::MainFunction()
{
	fdTemp = master;

	if(select(max_con+1, &fdTemp, 0, 0, &time) < 0)
	{
		printf("Smth wrong with select!\nError: %d", WSAGetLastError());
		return false;
	}
	
	for(i = 0; i <= max_con; i++)
	{
		if(FD_ISSET(i, &fdTemp))
		{
			if(i == s) //new connection
			{
				NewConnection();
			}
			else //data
			{
				RecvData();
			}
		}
	}
	return true;
}
/////// End of Main Funk /////////

void sSocket::print(int size)
{
	char *text = new char[size];
	for(int x = 0; x < size; x++)
	{
		text[x] = Buffer[x];
	}
	
	for(int x = 0; x < size; x++)
	{
		cout << text[x];
	}
	cout << "\n\n";
	delete [] text;
}
void sSocket::Close()
{
	closesocket(s);
	closesocket(c);
	WSACleanup();
}


sSocket::sSocket()
{
	for(int x = 0; x < 20; x++)
	{
		ZeroMemory(&NickPar[x], sizeof(Nick));
	}

	TotalMembers = 0;
	
	time.tv_sec = 0;
	time.tv_usec = 10;
}

sSocket::~sSocket()
{
}