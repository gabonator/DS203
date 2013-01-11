// ds203ui.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ds203ui.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

#include <crtdbg.h>
#include <Source/HwLayer/Win32/device.h>

CDevice *CDevice::m_pInstance = NULL;
CDevice g_dev;
CApplication g_app;

HWND g_hwnd = NULL;
BOOL g_running = FALSE;

DWORD WINAPI ThreadProcDraw(HANDLE handle) 
{
	while ( !g_hwnd )
		Sleep( 10 );

	ShowWindow( g_hwnd, SW_SHOW );
	HDC hdc = GetDC( g_hwnd );

	while (g_running) 
	{
		g_dev.Blit( hdc );
		Sleep( 10 );
	}
	return 0;
}

DWORD WINAPI ThreadProcApp(HANDLE handle) 
{
	while ( !g_hwnd )
		Sleep( 10 );

	g_app.Create();
	while (g_running) 
	{
		g_dev();
		g_app();	// contains sleep(1);
	}
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DS203UI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DS203UI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_DS203UI);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	int nWidth = 400 + GetSystemMetrics(SM_CXFRAME)*2;
	int nHeight = 240 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME)*2;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, nWidth, nHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	g_hwnd = hWnd;
	
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
#include <stdio.h>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static HANDLE hDrawThread = NULL;
	static HANDLE hAppThread = NULL;

	switch (message)
	{
	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		g_dev.OnKeyDown( (int)wParam );
		break;

	case WM_KEYUP:
		g_dev.OnKeyUp( (int)wParam );
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		g_dev.Blit( hdc );
		EndPaint(hWnd, &ps);
		break;

	case WM_CREATE:
	{
		g_running = TRUE;
		hDrawThread = CreateThread( NULL, NULL, &ThreadProcDraw, NULL, NULL, NULL );
		hAppThread = CreateThread( NULL, NULL, &ThreadProcApp, NULL, NULL, NULL );
		break;
	}

	case WM_CLOSE:
		g_running = FALSE;
		Sleep(10);
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		TerminateThread( hDrawThread, 1 );
		TerminateThread( hAppThread, 1 );
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		g_dev.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_LBUTTONDOWN:
		g_dev.OnMouseDown();
		break;

	case WM_LBUTTONUP:
		g_dev.OnMouseUp();
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
