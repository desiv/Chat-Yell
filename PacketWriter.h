//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#pragma once
#include <winsock2.h>
#include <windows.h>

class PacketWriter
{
public:
	unsigned char Buffer[1024];
	int offset;

	PacketWriter();
	~PacketWriter(){};

	void WriteByte(unsigned char a);
	void WriteWord(unsigned short a);
	void WriteString(unsigned char *text, int size);

	void Reset();

	

protected:
	
};