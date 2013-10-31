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