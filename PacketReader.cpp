#include "PacketReader.h"


unsigned short PacketReader::ReadByte()
{
	offset++;
	return buf[(offset-1)];
}



unsigned short PacketReader::ReadWord()
{
	offset += 2;
	return *(unsigned short*) (buf+(offset - 2));
}



void PacketReader::ReadString(unsigned char *out, int size)
{
	for(int x = 0; x < size; x++, offset++)
	{
		out[x] = buf[offset];
	}
}



void PacketReader::Reset()
{
	offset = 0;
}


void PacketReader::SetBuffer(unsigned char *buff)
{
	this->buf = buff;
}

int PacketReader::GetOffset()
{
	return offset;
}

void PacketReader::SetOffset(int offset)
{
	this->offset = offset;
}

PacketReader::PacketReader()
{
	offset = 0;
}