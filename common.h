#pragma once

#define MAX_TEXT 1024
#define MAX_NICK 20

//opcodes:
#define C_MESSAGE			0x2F2F
#define C_CONNECT			0xB245
#define C_DISCONNECT		0xB255
#define S_SEND_NICK_LIST	0xA11A
#define C_REQ_NICK_LIST		0xA12A

#define S_WHO_ARE_YOU		0xC10A
#define C_WHO_I_AM			0xC11A

#define S_NICK_EXISTS		0x1234
//END//

//Menu:
#define ID_HELP_ABOUT 101
//END//

enum 
{
	IDC_BUTTON3, //unused

	IDC_CONN, //show window
	IDC_SEND, //hide window //change win name

	IDC_TEXT,	//output window
	IDC_TEXT2,	//input window

	IDC_NICK,
	IDC_SET_NICK,
	IDC_NICK_LIST,
	IDC_GET_NICK_LIST, //get nick list


	IDW_ACCETABLE,
};

struct Nick
{
	int id;
	char nick[20];
	int nick_size;
};