//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#pragma once
#include <winsock2.h>
#include <windows.h>

class PacketReader
{
public:
	PacketReader();
	~PacketReader(){};


	unsigned short ReadWord();
	unsigned short ReadByte();
	void ReadString(unsigned char *out, int size);

	void SetBuffer(unsigned char *buff);
	void Reset();
	int GetOffset();
	void SetOffset(int offset);

protected:
	int offset;
	unsigned char *buf;
};