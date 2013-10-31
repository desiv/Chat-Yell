//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US
#include "mwindow.h"

int WindowFunk::GetText(HWND textbox) //returns size
{
	char text[1024] = {0};
	int size;

	GetWindowText(textbox, text, 1024);
	size = GetWindowTextLength(textbox);

	for(int x = 0; x < size; x++)
	{
		textbox_text[x] = text[x];
	}

	SetWindowText(textbox, "");
	return size;
}

void WindowFunk::SetText(char text[], int showtime)	//flag: 1 - show time 0 - don't show
{
	cf.crTextColor = RGB(0, 150,0);	//color
	SendMessage(this->EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	//set this color

	if(showtime == 1)
	{
		Time_set();
	}

	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)text);	//message
	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)"\r\n"); //new line
	ScrollDown(this->EditBox);
	UpdateWindow(this->EditBox);
	
}

void WindowFunk::MemberNotice(char *nick, int size, int flag) //flag: 0 - connected, 1 - dc
{
	char t_nick[MAX_NICK];

	cf.crTextColor = RGB(80, 150, 0);
	SendMessage(EditBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf); //new line

	Time_set();	//time

	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)"*"); 	//just to know that it's not a text message from other client

	for(int x = 0; x < size; x++)	//nick
	{
		t_nick[x] = nick[x];
	}
	t_nick[size] = 0;

	SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)t_nick); //show nick

	if(flag == 0) //connected
	{
		SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)" wants to yell too!\r\n");
	}
	else if(flag == 1)
	{
		SendMessage(EditBox, EM_REPLACESEL, FALSE, (LPARAM)" has been disconnected!\r\n");
	}
	else 
	{
		//nick change
	}

	ScrollDown(this->EditBox);
	UpdateWindow(this->EditBox);
}

void WindowFunk::Time_set()
{
	time_t rtime;
	struct tm ptm;
	char c_time[13];
	char c_t_change[11] = {0};

	time(&rtime);
	localtime_s(&ptm, &rtime);
	
	strftime(c_time, 12, " [%H:%M:%S]", &ptm);
	for(int x = 0; x < 11; x++)
	{
		c_t_change[x] = c_time[x+1];
		if(c_time[x+1] == ' ')break;
	}


	SendMessage(EditBox, EM_REPLACESEL, false, (LPARAM)&c_t_change);
	SendMessage(EditBox, EM_REPLACESEL, false, (LPARAM)" ");
	UpdateWindow(EditBox);
}
void WindowFunk::OnLoadNick(char *nick)
{
	SetWindowText(this->NickBox, nick);
}

void WindowFunk::SetNick()
{
	unsigned char temp[20];

	int len = GetWindowText(this->NickBox, (char*)temp, 20);
	if(len <= 20 && len > 0)
	{
		ZeroMemory((char*)MyNick.nick, 20);
		MyNick.nick_size = 0;

		for(int x = 0; x < len; x++)
		{
			MyNick.nick[x] = temp[x];
			MyNick.nick_size++;
		}

		OnLoadNick((char*)MyNick.nick);
		UpdateWindow(NickBox);
	}
	else if(len == -1 && len == 0){}

	else SetText("!Get a better nick! OMG!", 0);

}

void WindowFunk::AddNick(char *nick, int size)
{	
	int isNick = FindNick(nick, size);
	if(isNick == -1)	//if there's no same nick then procede...
	{
		//find free spot:
		int free_array = 0;
		for(int i = 0; i < 20; i++)
		{
			if(NickPar[i].nick[0] == 0)
			{
				free_array = i;
				break;
			}
		}
		//add to the nick to the nick array:
		for(int x = 0; x < size; x++)
		{
			NickPar[free_array].nick[x] = nick[x];
		}
		NickPar[free_array].nick[size] = 0;
		NickPar[free_array].nick_size = size;

		//add nick to the nick list:
		SendMessage(NickList, LB_ADDSTRING, 0, (LPARAM)NickPar[free_array].nick);
		members++;
	}
}

int WindowFunk::FindNick(char *nick, int size)
{
	int found = 0;
	for(int x = 0; x < members; x++)	//array loop
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

void WindowFunk::ScrollDown(HWND edit)
{
	int curlines = SendMessage(edit, EM_GETLINECOUNT, 0, 0); //get how mane lines there is now
	if(curlines > lines) //if new line number exeeds old line number then...
	{
		int s = curlines - lines;
		SendMessage(edit, EM_LINESCROLL, 0, s);
		lines = curlines;
	}
}

WindowFunk::WindowFunk()
{
	//random nick
	char letters[] = {'a', 'b', 'c', 'g', 'h',
					  'e', 't', 'h', 'f', 'm',
					  'n', 'B', 'E', 'j', '0',
					  'o', 'F', 'Q', 'V', 'v',
					  'x', 'Z', 'D', 'U', 'i',
					  'y', 'O', '2', '1', '5',
					  '_', '5', '9', 'K', 'p',
					  't', 'j', 'g', 'C', 'Z',
					  'I', '1', '_', '7', '0',
					  'y', 'u', 'r', 'R', 'L'}; //50
	int random;
	srand((unsigned)time(0));
	char randNick[8] = {0};
	for(int i = 0; i < 7; i++)
	{
		random = rand()%49;
		randNick[i] = letters[random];
	}
	

	//by default nick will be
	ZeroMemory(&MyNick, sizeof(Nick));
	strcpy_s(MyNick.nick, randNick);
	MyNick.nick_size = 7;

	members = 0;
	i_have_list = false; 
	lines = 18;

	//text font
	memset(&cf, 0, sizeof(cf));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0,0,0);

	ZeroMemory(textbox_text, sizeof(textbox_text));
}

WindowFunk::~WindowFunk(){}
