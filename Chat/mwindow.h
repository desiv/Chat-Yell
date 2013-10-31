#pragma once
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <time.h>
#include <richedit.h>

#include <cstdlib>
#include <ctime>

#include "..\common.h"



class WindowFunk
{
public:

	CHARFORMAT2 cf;
	struct Nick NickPar[20];
	struct Nick MyNick;

	WindowFunk();
	~WindowFunk();

	void SetNick();
	void OnLoadNick(char *nick);

	int GetText(HWND textbox); //returns size
	unsigned char textbox_text[1024];
	
	void SetText(char text[], int showtime = 1);
	void MemberNotice(char *nick, int size, int flag);
	
	void Time_set();

	void ScrollDown(HWND edit);

	void AddNick(char *nick, int size); 
	int FindNick(char *nick, int size);	//returns array of that nick

	
	
	
protected:
	int lines;
	int members;
	bool i_have_list;
	

	HWND EditBox;
	HWND NickBox;
	HWND NickList;
};