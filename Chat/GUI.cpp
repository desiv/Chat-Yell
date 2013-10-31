//Copyrighted 2010 under CC 3.0 by Marius Savickas
//http://creativecommons.org/licenses/by-nc/3.0/deed.en_US

#pragma once
#include "client.h"
#include "mwindow.h"


#include <windows.h>
#include <windowsx.h>

#include <Richedit.h>
#include "resource.h"

//Visuals////
#include <commctrl.h> //this is needed for visuals
#pragma comment(lib, "comctl32.lib")
//Visuals////

#include "..\common.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

HWND EditBox;
HWND Button;
HWND TextBox;
HWND NickBox;
HWND NickList;

WindowFunk parent; //initialization
Socket soc;	//init

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	InitCommonControls(); //for visual effects

#pragma region Parameters and Creation of window
	WNDCLASSEX wc;
	
	ZeroMemory(&wc , sizeof(WNDCLASSEX));//clear window class for use

	//parametters
	wc.cbSize			= sizeof(WNDCLASSEXW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);//set background
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= WndProc;
	wc.lpszClassName	= "mainWinName";
	wc.lpszMenuName		= "ID_MENU"; //menu name don't need it for now
	wc.style			= CS_HREDRAW | CS_VREDRAW; //redraw window both horizontal and vertical
	//parameters

	RegisterClassEx(&wc); //registering window

	//window creation somewhere in OS but we can't see it yet
	HWND hWnd = CreateWindowEx(0,//WS_EX_CONTROLPARENT, 
								"mainWinName", //out classname that we defined earlier
								"Yell",  //Title
								WS_OVERLAPPEDWINDOW, //this will add few command like minimaze, maximaze, resize frame
								CW_USEDEFAULT, CW_USEDEFAULT, // x, y coordinates
								600, 430,// width, height of window
								NULL, //not a parent window
								NULL, //we aren't using menus
								hInstance,
								NULL); //used with multiple windows
	
	ShowWindow(hWnd, iCmdShow); //showing window on screen
	UpdateWindow(hWnd);//updating it


#pragma region Loop/ checking For interactions with User

	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDW_ACCETABLE));

	while(true)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//GetMessage(&msg, NULL, 0, 0))
		{

			if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))//while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_KEYDOWN)
				{
					if(msg.wParam == VK_RETURN)
					{
						SendMessage(hWnd, 0x111, IDC_SEND, 0);	//send msg and...
						msg.message = 0;						//... set message to 0 so that we....
						msg.wParam = 0;							//... wouldn't get next line in text box.
						msg.lParam = 0;
					}
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		soc.Recv();

		if(msg.message == WM_QUIT)
		{
			soc.~Socket();
			break;
		}
		Sleep(1); //optimization
	}
	

#pragma endregion //Loop for checking user interactions
	
	return msg.wParam; // return msg.wParam form WinMain
}








	//WndProc responds to user interaction
LRESULT CALLBACK WndProc(HWND hWnd, // "handle" to the window that this message is for
						 UINT msg, // Type of message (e.g WM_PAINT)
						 WPARAM wparam, //information about actual message
						 LPARAM lparam) // MORE info about message
{
	switch(msg)
	{
	case WM_CREATE:
		{
			//Button 1: Connect
			Button = CreateWindow("Button", //class name or smth
					              "Connect", // button name
							      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Style
							      10, 100, // x, y position
							      100, 30, // width, height
							      hWnd, // parent window
							     (HMENU)IDC_CONN, //child menu ID
								 (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
								  NULL); //pointer not needed
			//Button 2: send
			Button = CreateWindow("Button", //class name or smth
								  "Send", // button name
							      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Style
						          460, 315, // x, y position
							      100, 45, // width, height
							      hWnd, // parent window
							     (HMENU)IDC_SEND, //child menu ID
							     (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
							      NULL); //pointer not needed

			//Button 3: set nick
			Button = CreateWindow("Button", //class name or smth
								  "Change nick", // button name
							      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Style
						          10, 10, // x, y position
							      100, 30, // width, height
							      hWnd, // parent window
							     (HMENU)IDC_SET_NICK, //child menu ID
							     (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
							      NULL); //pointer not needed

			//Button 4: get nick list
			Button = CreateWindow("Button", //class name or smth
								  "Refresh list", // button name
							      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Style
						          10, 160, // x, y position
							      100, 30, // width, height
							      hWnd, // parent window
							     (HMENU)IDC_GET_NICK_LIST, //child menu ID
							     (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
							      NULL); //pointer not needed
		
			LoadLibrary("Msftedit.dll");
			//Output window
			EditBox = CreateWindow("RICHEDIT50W", //remembah! write it right if you don't wanna get headache. it suppose to be correct word (e.g Edit, Button, etc)
								   "", // don't need name. it's a Text Box!
								   ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY,//styles
								   150, 5, //x, y
								   320, 300, //width, height
								   hWnd, //parent window
								  (HMENU)IDC_TEXT, // id 
								  (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), //well... i don't know this
								   NULL); // don't have pointer

			//text box
			TextBox = CreateWindow("EDIT", //remembah! write it right if you don't wanna get headache. it suppose to be correct word (e.g Edit, Button, etc)
								   "", // don't need name. it's a Text Box!
								   WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, //styles
								   150, 315, //x, y
								   305, 45, //width, height
								   hWnd, //parent window
								  (HMENU)IDC_TEXT2, // id
								  (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), //well... i don't know this
								   NULL); // don't have pointer

			//nick
			NickBox = CreateWindow("EDIT", //remembah! write it right if you don't wanna get headache. it suppose to be correct word (e.g Edit, Button, etc)
								   "", // don't need name. it's a Text Box!
								   WS_CHILD | WS_VISIBLE | WS_BORDER,//styles
								   10, 60, //x, y
								   100, 20, //width, height
								   hWnd, //parent window
								  (HMENU)IDC_NICK, // id 
								  (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), //well... i don't know this
								   NULL); // don't have pointer
			//nick list
			NickList = CreateWindow("ListBox", //remembah! write it right if you don't wanna get headache. it suppose to be correct word (e.g Edit, Button, etc)
								   "", // don't need name. it's a Text Box!
								   WS_CHILD | WS_VISIBLE | WS_BORDER,//styles
								   480, 5, //x, y
								   100, 300, //width, height
								   hWnd, //parent window
								  (HMENU)IDC_NICK_LIST, // id 
								  (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), //well... i don't know this
								   NULL); // don't have pointer

			soc.SetWindow(hWnd); //sets editbox, nickbox handle to winfunk class
			SendMessage(EditBox, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
			UpdateWindow(EditBox);
			



		}break;

	case WM_PAINT:
			HDC hdc;
			PAINTSTRUCT ps;

			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		return 0;
		break;
	case WM_COMMAND:
		{

			switch(wparam)
			{

			//buton press, connect
			case IDC_CONN:
				{
					if(!soc.IsConnected())
					{
						soc.Init(25015);
					}
					else MessageBox(hWnd, "Already online!", "ERROR", MB_OK | MB_ICONERROR);
				}break;
			
			//send
			case IDC_SEND:
				{
					
					if(soc.IsConnected())
					{
						int size = parent.GetText(TextBox);
						if(size > 0)
						{
							soc.SendMsg((unsigned char*)parent.textbox_text, size);	//GetText() returns size
						}
					}
					else 
					{
						SendMessage(hWnd, 0x111, IDC_CONN, 0);
						//MessageBox(hWnd, "You have to be online before you can send messages!", "ERROR", MB_OK | MB_ICONERROR);
						//SetWindowText(TextBox, "");
					}
				}break;

			case IDC_SET_NICK:
				{
					soc.OnNick();
					break;
				}
			//case IDC_GET_NICK_LIST:
			//	{
			//		soc.OnGetNickList();
			//		break;
			//	}

			case ID_HELP_ABOUT:
				MessageBox(hWnd, "Marius (c) 2010", "About", MB_OK);
				break;

			}break;
		}break;

	case WM_SETFOCUS:
		SetFocus(TextBox);
		return 0;
		break;

	case WM_QUIT:
		//soc.~Socket();
		PostQuitMessage(0);
		return 0;
		break;

	case WM_DESTROY:
		//soc.~Socket();
		PostQuitMessage(0);
		return 0;
		break;
	}
	return DefWindowProc(hWnd, msg, wparam, lparam);
}
