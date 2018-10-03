/*
2014/07/09 oka

論理回路シミュレーター

*/

#define _CRT_SECURE_NO_WARNINGS

//ヘッダ
#include"common.h"

#include<Winuser.h>

extern LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

HINSTANCE hInst;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	const char ClassName[]="win32app";
	const DWORD Style=WS_OVERLAPPEDWINDOW;
	WNDCLASSEX wcx;
	MSG Msg;
	HWND hWnd;
	HACCEL hAccel;

	hInst=hInstance;

	wcx.cbSize			=sizeof(WNDCLASSEX);
	wcx.style			=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wcx.lpfnWndProc		=WindowProc;
	wcx.cbClsExtra		=0;
	wcx.cbWndExtra		=0;
	wcx.hInstance		=hInstance;
	wcx.hIcon			=LoadIcon(hInstance,MAKEINTRESOURCE((DWORD)IDI_APPLICATION));
	wcx.hCursor			=LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground	=(HBRUSH)GetStockObject(NULL_BRUSH);
	wcx.lpszMenuName	="MAINMENU";
	wcx.lpszClassName	=ClassName;
	wcx.hIconSm			=LoadIcon(hInstance,MAKEINTRESOURCE((DWORD)IDI_APPLICATION));
	if(!RegisterClassEx(&wcx))return 1;

	hWnd=CreateWindow(ClassName,"無題",Style,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL);
	if(!hWnd)return 1;

	hAccel=LoadAccelerators(hInstance,"ACCEL");

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	for(;;)
	{
		int i;
		i=GetMessage(&Msg,hWnd,0,0);
		if(i==0||i==-1) break;
		if(!TranslateAccelerator(hWnd,hAccel,&Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return (int)Msg.wParam;
}