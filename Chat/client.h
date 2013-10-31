//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#pragma once
#include <winsock2.h>

#include "mwindow.h"
#include "..\PacketReader.h"
#include "..\PacketWriter.h"

#include <windows.h>
#include <string>
#include <time.h>

#include <Richedit.h>

#include "..\common.h"

class Socket : protected WindowFunk
{
public:

	CHARFORMAT2 cf;

	PacketReader Reader;
	PacketWriter Writer;

	void Init(short int port);

	void SendMsg(unsigned char *box_text, int size_msg);
	void Recv();
	void ParseData(int bytes);

	bool IsConnected();

	void ShowMessage(char *text, int size, char *nick, int size_nick);

	void SetWindow(HWND hwnd);

	void Send(char *Buffer, int size);

	void OnNick();
	void OnGetNickList();

	Socket();
	~Socket();

	SOCKET s;

	fd_set master;

	unsigned char Buffer[1024];
	int bytes;
	bool connection;
	int cur_chatters;

	struct timeval tv;

protected:
};