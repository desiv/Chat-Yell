//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#include "PacketWriter.h"


void PacketWriter::WriteByte(unsigned char a)
{
	offset++;
	Buffer[offset-1] = a;
}



void PacketWriter::WriteWord(unsigned short a)
{
	Reset();
	offset += 2;
	*(unsigned short*) (Buffer+(offset-2)) = a;
}



void PacketWriter::WriteString(unsigned char *text, int size)
{
	for(int x = 0; x < size; x++, offset++)
	{
		Buffer[offset] = text[x];
	}
}



void PacketWriter::Reset()
{
	offset = 0;
}


PacketWriter::PacketWriter()
{
	offset = 0;
}