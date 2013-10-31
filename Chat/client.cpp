#include "client.h"

void Socket::Init(short int port)
{
	WSADATA wsa;
	SOCKADDR_IN info = {0};
	int err = 0;
	char IP[] = "127.0.0.1\0";
	s = 0;

	char str[100] = {0};
	sprintf_s(str, "**Connecting to %s...", IP);
	SetText(str);
	
	FD_ZERO(&master); //zero fd struct

	err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if(err == NO_ERROR || err != 1)
	{
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		
		if(s != SOCKET_ERROR)
		{
			//server info
			info.sin_family = AF_INET;
			info.sin_addr.s_addr = inet_addr(IP);
			info.sin_port = htons(port);

			if(connect(s, (LPSOCKADDR)&info, sizeof(info)) != SOCKET_ERROR)
			{
				FD_SET(s, &master);
				connection = true;
				//success
			}
			else {SetText("*Failed to connect!");
				  WSACleanup();}
		}
		else {SetText("*Failed to set up sockets");
			  WSACleanup();}
	}
	else {SetText("*Failed to start wsa");
		  WSACleanup();}
}

void Socket::SendMsg(unsigned char *box_text, int size_msg)
{
	Writer.Reset();

	Writer.WriteWord(C_MESSAGE);	//opcode.text
	
	Writer.WriteByte(MyNick.nick_size); //nick size
	Writer.WriteString((unsigned char*)MyNick.nick, MyNick.nick_size); //write nick

	Writer.WriteByte(size_msg);	//size of message
	Writer.WriteString(box_text, size_msg);	//message

	send(s, (char*)Writer.Buffer, Writer.offset, 0);

}

void Socket::Recv()
{
	if(connection == true)
	{
		fd_set read_fds = master;
		if( select(s + 1, &read_fds, NULL, NULL, &tv) == -1)
		{
			int err = WSAGetLastError();
			if(err == 0){}
			else
			{
				MessageBox(0, "Function select() has failed!", "ERROR", MB_OK | MB_ICONERROR);
				this->~Socket();
				connection = false;
			}
			
		}

		if(FD_ISSET(s, &read_fds)) // we got something
		{
			if((bytes = recv(s, (char*)Buffer, 1024, 0)) > 0)
			{
				Reader.SetBuffer((unsigned char*)Buffer);
				ParseData(bytes); 
			}
			else 
			{
				SetText("**Connection lost...");
				FD_CLR(s, &read_fds);
				connection = false;
				this->~Socket();
			}
		}
	}
}

void Socket::ParseData(int bytes)
{
	Reader.Reset();
	

	while(Reader.GetOffset() < bytes)
	{
		unsigned short opcode = Reader.ReadWord();

		switch(opcode)
		{
		case C_MESSAGE: //text message
				{
					unsigned char *msg;
					unsigned char *nick;

					unsigned short size_nick = Reader.ReadByte();	//read nick
					nick = new unsigned char[size_nick];

					Reader.ReadString(nick, size_nick);	//read nick string

					unsigned short size_text = Reader.ReadByte();	//read text size
					msg = new unsigned char[size_text];

					Reader.ReadString(msg, size_text);	//read text

					ShowMessage((char*)msg, size_text, (char*)nick, size_nick);

					delete [] nick;
					delete [] msg;
					break;
				}

		case C_CONNECT:	//connected
			{
				unsigned char *nick;
				unsigned short size = Reader.ReadByte();
				
				nick = new unsigned char[size];
				
				Reader.ReadString(nick, size);

				MemberNotice((char*)nick, size, 0);	//notice everyone
				//AddNick((char*)nick, size);

				delete [] nick;
				break;
			}

		case C_DISCONNECT:	//disconnected
			{
				unsigned char *nick;
				unsigned short size = Reader.ReadByte();

				nick = new unsigned char[size];
				
				Reader.ReadString(nick, size);

				MemberNotice((char*)nick, size, 1);
				delete [] nick;
				break;
			}

		case S_SEND_NICK_LIST:	//got from server
			{
				for(int x = 0; x < 20; x++)	//delete old list
					ZeroMemory(&NickPar[x], sizeof(Nick));	

				for(int x = 0; x < 20; x++)	//delete nick list
					SendMessage(NickList, LB_DELETESTRING, 0, 0);


				int TotalMembers = Reader.ReadByte(); //get total members
				cur_chatters = TotalMembers; //used for deleting list

				for(int x = 0; x < TotalMembers; x++)
				{
					unsigned int size = Reader.ReadByte(); //get size
					unsigned char *nick;
					nick = new unsigned char[size];
					Reader.ReadString(nick, size); //get nick

					if(FindNick((char*)nick, size) > -1)
					{
						//nick found do nothing...
					}
					else
					{
						AddNick((char*)nick, size);
						UpdateWindow(NickList);
					}
				}
				break;
			}

		case S_WHO_ARE_YOU:
			{
				Writer.WriteWord(C_WHO_I_AM);	//
					Writer.WriteByte(MyNick.nick_size);	//size
					Writer.WriteString((unsigned char*)MyNick.nick, MyNick.nick_size); //nick
				Send((char*)Writer.Buffer, Writer.offset);
				break;
			}

		default:
			{
				SetText("Unknown opcode");
				Reader.SetOffset(1000);
				break;
			}
			
		}
	}	
}

void Socket::ShowMessage(char *text, int size_text, char *nick, int size_nick)
{
	char t_text[MAX_TEXT];
	char t_nick[MAX_NICK];
	bool my_nick = true;

	cf.crTextColor = RGB(0, 0,0);	//color
	SendMessage(EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	//set
	//show time
	Time_set();

	//nick
	for(int x = 0; x < size_nick; x++)	//copy nick
	{
		t_nick[x] = nick[x];	//copy nick

		if(t_nick[x] != MyNick.nick[x])
		{my_nick = false;}
	}
	t_nick[size_nick] = 0;	//set to 0

	if(my_nick)
	{
		cf.crTextColor = RGB(150,50,50);	//set to my nick color
		SendMessage(EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	//set

		SendMessage(EditBox, EM_REPLACESEL, false, (LPARAM)t_nick);//nick

		cf.crTextColor = RGB(0,0,0);
		SendMessage(EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	//set
	}
	else
	{
		cf.crTextColor = RGB(90,90,90);	//set to my nick color
		SendMessage(EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	//set

		SendMessage(EditBox, EM_REPLACESEL, false, (LPARAM)t_nick);//nick

		cf.crTextColor = RGB(0,0,0);	//set to my nick color
		SendMessage(EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	//set
	}
	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)": "); //we need to separate nick from text

	//message
	for(int x = 0; x < size_text; x++)
	{
		t_text[x] = text[x];
	}
	t_text[size_text] = 0;

	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)t_text);
	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	ScrollDown(this->EditBox);
	UpdateWindow(EditBox);
}

bool Socket::IsConnected()
{
	if(connection == true)
	{
		return true;
	}
	else return false;
}

void Socket::SetWindow(HWND hwnd)
{
	this->EditBox = GetDlgItem(hwnd, IDC_TEXT);
	this->NickBox = GetDlgItem(hwnd, IDC_NICK);
	this->NickList = GetDlgItem(hwnd, IDC_NICK_LIST);
	
	SetText("Yell v0.61 BETA by Marius\r\n", 0);
	SetText("Last update: 2010-03-24\r\n*Nicks Added\r\n", 0);
	
	OnLoadNick((char*)MyNick.nick);

	UpdateWindow(NickBox);
	UpdateWindow(EditBox);
	UpdateWindow(NickList);
}

void Socket::Send(char *Buffer, int size)
{
	send(s, Buffer, size, 0);
	Writer.Reset();
}

void Socket::OnNick()
{
	SetNick();
}

void Socket::OnGetNickList()
{
	//if(i_have_list == false)
	//{
		Writer.WriteWord(C_REQ_NICK_LIST);
		Send((char*)Writer.Buffer, Writer.offset);
	//}
}

Socket::Socket()
{
	connection = false;
	
	//text font
	memset(&cf, 0, sizeof(cf));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0,0,0);

	cur_chatters = 0;
	


	tv.tv_sec = 0;
	tv.tv_usec = 10;
}
Socket::~Socket()
{
	for(int x = 0; x < 20; x++)
		SendMessage(NickList, LB_DELETESTRING, 0, 0);

	closesocket(s);
	WSACleanup();
}
