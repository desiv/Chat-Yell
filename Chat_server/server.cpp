//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#include "socket_server.h"

void sSocket::ParseData(int bytes)
{
	Reader.Reset();

	while(Reader.GetOffset() < bytes)
	{
		int opcode = Reader.ReadWord();

		switch(opcode)
		{
		case C_MESSAGE:
				SendToAll(bytes);
				break;

		//case C_REQ_NICK_LIST:
		//		Writer.WriteWord(S_SEND_NICK_LIST);
		//			Writer.WriteByte(TotalMembers); //how many members
		//		for(int x = 0; x < 20; x++)
		//		{
		//			if(NickPar[x].nick_size > 0)
		//			{
		//				Writer.WriteByte(NickPar[x].nick_size);	//nick size
		//				Writer.WriteString((unsigned char*)NickPar[x].nick, NickPar[x].nick_size);	//nick
		//			}
		//		}
		//		Send((char*)Writer.Buffer, Writer.offset);
		//		break;

		case C_WHO_I_AM:	//client told us who he is:
			unsigned int size = Reader.ReadByte(); //size
			unsigned char *nick;
			nick = new unsigned char[size];
			Reader.ReadString(nick, size);	//get nick

			//add to the list
			if(FindNick((char*)nick, size) == -1) //there's no nick
				AddNick((char*)nick, size);

			else	//send that nick already exists
			{
				Writer.WriteWord(S_NICK_EXISTS);
				Send((char*)Writer.Buffer, Writer.offset);
				//now client should close the connection...
				break;
			}

			//notice all
			SendToAllNotice((char*)nick, size, 0);
			SendNickList();
			break;
			}
		}
}



int sSocket::FindNick(char *nick, int size)
{
	int found = 0;
	for(int x = 0; x < TotalMembers; x++)	//array loop
	{
		if(size == NickPar[x].nick_size)
		{
			found = 0;
			for(int y = 0; y < 20; y++)		//char loop
			{
				if(y == size || y == NickPar[x].nick_size)
					break;

				if(NickPar[x].nick[y] != nick[y])
				{
					found = -1;	//not found
					break;
				}	
			}
			if(found != -1)	//nick found
			{
				return x;
			}
		}
	}
	return -1;
}

int sSocket::FindNick(int id)
{
	for(int x = 0; x < TotalMembers; x++)//find nick by id
	{
		if(NickPar[x].id == id)	//nick found
			return x;
	}

	return -1; //not found
}

void sSocket::AddNick(char *nick, int size)
{
	//find free spot
	int free = -1;
	for(int x = 0; x < 20; x++)
	{
		if(NickPar[x].nick[x] == 0)
		{
			free = x;
			break;
		}
	}

	if(free != -1)
	{
		//nick
		for(int x = 0; x < size; x++)
		{
			NickPar[free].nick[x] = nick[x];
		}
		NickPar[free].nick_size = size;
		NickPar[free].id = i; //for banning and else
		TotalMembers++;
	}
	
}
void sSocket::SendToAllNotice(char *nick, int size, int flag)
{
	if(flag == 0)
		Writer.WriteWord(C_CONNECT);
	else
		Writer.WriteWord(C_DISCONNECT);

		Writer.WriteByte(size);
		Writer.WriteString((unsigned char*)nick, size);
	
		//send to all
	for(int x = 0; x <= max_con; x++)
		{
			if(FD_ISSET(x, &master))// blah... i don't want to send to alll connections...
			{
				if(x != this->s)	//we don't want to send to ourselfs ;o
				{
					if(send(x, (char*)Writer.Buffer, Writer.offset, 0) == SOCKET_ERROR)
					{
						printf("*Failed to send\n");
						closesocket(x);
						FD_CLR(x, &master);
					}
				}
			}
		}
}

void sSocket::SendNickList(int client)
{
	Writer.WriteWord(S_SEND_NICK_LIST);
		Writer.WriteByte(TotalMembers); //how many members

	for(int x = 0; x < 20; x++)
	{
		if(NickPar[x].nick_size > 0)
		{
			Writer.WriteByte(NickPar[x].nick_size);	//nick size
			Writer.WriteString((unsigned char*)NickPar[x].nick, NickPar[x].nick_size);	//nick
		}
	}

	send(client, (char*)Writer.Buffer, Writer.offset, 0);
}

void sSocket::SendNickList()
{
	Writer.WriteWord(S_SEND_NICK_LIST);
		Writer.WriteByte(TotalMembers); //how many members

	for(int x = 0; x < 20; x++)
	{
		if(NickPar[x].nick_size > 0)
		{
			Writer.WriteByte(NickPar[x].nick_size);	//nick size
			Writer.WriteString((unsigned char*)NickPar[x].nick, NickPar[x].nick_size);	//nick
		}
	}

	for(int x = 0; x <= max_con; x++)
		{
			if(FD_ISSET(x, &master))// blah... i don't want to send to alll connections...
			{
				if(x != this->s)	//we don't want to send to ourselfs ;o
				{
					send(x, (char*)Writer.Buffer, Writer.offset, 0);
				}
			}
		}
}

