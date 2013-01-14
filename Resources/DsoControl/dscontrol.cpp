// dscontrol.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "dscontrol.h"
#include "SyncSerialComm.h"
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100
#define strPort "\\\\.\\COM11"

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

class CString
{
	char string[1024];
public:
	CString()
	{
		string[0] = 0;
	}
	CString(char* str)
	{
		strcpy(string, str);
	}
	char* GetBuffer()
	{
		return string;
	}
	int GetLength()
	{
		return strlen(string);
	}
	void Format(char* pszFormat, ...)
	{
		va_list argList;
		va_start(argList, pszFormat);
		vsprintf(string, pszFormat, argList);
		va_end(argList);
	}

};

CString m_strCommand = "";
LONG m_lLastMouseEvent = 0;
//HWND m_ghwndStatusBar = NULL;
//HWND m_hStatus = NULL;

class CComm : public CSyncSerialComm
{
public:
	CComm() : CSyncSerialComm( strPort )
	{
		HRESULT hOk = Open();
		_ASSERT( hOk == S_OK );
		hOk = ConfigPort( 115200UL, 1UL );
		_ASSERT( hOk == S_OK );
	}

	void Send(CString strMessage)
	{
		m_Comm.Flush(PURGE_RXCLEAR);
		HRESULT hOk = Write( strMessage.GetBuffer(), strMessage.GetLength() );
		_ASSERT( hOk == S_OK );
	}
} m_Comm;

class CFrameBuffer {
public:
	enum {
		Width = 400,
		Height = 240
	};

private:

	LPVOID m_pBuffer;
	HDC m_pDC;
	HANDLE m_Old;
	HBITMAP m_ourbitmap;

public:
	CFrameBuffer()
	{
		HDC hDC;
		BITMAPINFO bitmapinfo;
		hDC=CreateCompatibleDC(NULL);
		bitmapinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		bitmapinfo.bmiHeader.biWidth=Width;
		bitmapinfo.bmiHeader.biHeight=-Height; /* top-down */
		bitmapinfo.bmiHeader.biPlanes=1;
		bitmapinfo.bmiHeader.biBitCount=32;
		bitmapinfo.bmiHeader.biCompression=BI_RGB;
		bitmapinfo.bmiHeader.biSizeImage=0;
		bitmapinfo.bmiHeader.biClrUsed=256;
		bitmapinfo.bmiHeader.biClrImportant=256;
		m_pBuffer = NULL;
		m_ourbitmap=CreateDIBSection(hDC,&bitmapinfo,DIB_RGB_COLORS,&m_pBuffer,0,0);
		m_pDC=CreateCompatibleDC(NULL);
		m_Old=SelectObject(m_pDC,m_ourbitmap);
		DeleteDC(hDC);

		FillMemory(m_pBuffer, sizeof(DWORD)*Width*Height, 0xb0);
	}

	LPVOID GetBuffer()
	{
		return m_pBuffer;
	}

	void Blit(HDC hDC)
	{
		BitBlt(hDC,0,0,Width,Height,m_pDC,0,0,SRCCOPY);
	}

	~CFrameBuffer()
	{
		SelectObject(m_pDC,m_Old);
		DeleteDC(m_pDC);
		DeleteObject(m_ourbitmap);
	}
} m_Display;

DWORD FROM_565_TO_RGB(unsigned short clr565)
{
	unsigned char b = clr565 & 0x1F;
	clr565 >>= 5;
	unsigned char g = clr565 & 0x3F;
	clr565 >>= 6;
	unsigned char r = (unsigned char)clr565;
	r = r*255/0x1f;
	g = g*255/0x3f;
	b = b*255/0x1f;
	return RGB(r, g, b);
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
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DSCONTROL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DSCONTROL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DSCONTROL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DSCONTROL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	int nWidth = 400 + GetSystemMetrics(SM_CXFRAME)*2;
	int nHeight = 240 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME)*2 + GetSystemMetrics(SM_CYMENU);

	//DWORD dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	//int nWidth = 400 + GetSystemMetrics(SM_CXFRAME)*2;
	//int nHeight = 240 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME)*2+10 + GetSystemMetrics(SM_CYMENU);

	hWnd = CreateWindow(szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, 0, /*CW_USEDEFAULT, 0,*/ nWidth, nHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	/*
	m_ghwndStatusBar = CreateWindowEx(0, // extended not required
	STATUSCLASSNAME, // status bar class name, equivalent to
	// "msctls_statusbar32"
	"", //caption not required
	WS_CHILD | WS_VISIBLE,
	-100, // x 
	-100, // y
	10, // width
	10, // height
	hWnd,
	NULL,
	(HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE),
	NULL);
	*/
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// connect, transfer bitmap

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_TRANSFER:
			{
				// does not work reliably
				m_strCommand = "\nLCD.GetBitmap(0,0,399,239);\n";
				m_Comm.Flush();
				m_Comm.Send(m_strCommand);
				char* buf = new char[200000];
				char* check = "QAN=(binary, len=192000)";
				DWORD dwRead = strlen(check);
				m_Comm.Read(buf, dwRead);
				_ASSERT( memcmp(check, buf, strlen(check)) == 0 );

				DWORD dwTarget = 400*240*2;
				DWORD dwSum = 0;
				while ( dwSum < dwTarget )
				{
					dwRead = dwTarget - dwSum;
					m_Comm.Read(buf, dwRead);
					dwSum += dwRead;
				}

				unsigned short* pSource = (unsigned short*)buf;

				DWORD* pBuffer = (DWORD*)m_Display.GetBuffer();
				for (int i=0; i<400*240; i++)
					*pBuffer++ = FROM_565_TO_RGB(*pSource++);

				delete buf;

			};
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		m_Display.Blit(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		if ( GetTickCount() - m_lLastMouseEvent > 30 ) // do not overload the uart
		{
			CString strEvent;
			strEvent.Format("MAIN.Mouse(%d,%d,%d);\n", LOWORD(lParam), HIWORD(lParam), 0);
			m_Comm.Send(strEvent);
			m_lLastMouseEvent = GetTickCount();
		}
		break;

	case WM_LBUTTONDOWN:
		{
			CString strEvent;
			strEvent.Format("MAIN.Mouse(%d,%d,%d);\n", LOWORD(lParam), HIWORD(lParam), 1);
			m_Comm.Send(strEvent);
			m_lLastMouseEvent = GetTickCount();
		}
		break;

	case WM_LBUTTONUP:
		{
			CString strEvent;
			strEvent.Format("MAIN.Mouse(%d,%d,%d);\n", LOWORD(lParam), HIWORD(lParam), 0);
			m_Comm.Send(strEvent);
			m_lLastMouseEvent = GetTickCount();
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT: m_Comm.Send("WND.Message(WND::WmKey, KEY::Left);\n"); break;
		case VK_RIGHT: m_Comm.Send("WND.Message(WND::WmKey, KEY::Right);\n"); break;
		case VK_UP: m_Comm.Send("WND.Message(WND::WmKey, KEY::Up);\n"); break;
		case VK_DOWN: m_Comm.Send("WND.Message(WND::WmKey, KEY::Down);\n"); break;
		case VK_RETURN: m_Comm.Send("WND.Message(WND::WmKey, KEY::Enter);\n"); break;
		case VK_BACK: m_Comm.Send("WND.Message(WND::WmKey, KEY::Escape);\n"); break;

		case VK_F1: m_Comm.Send("WND.Message(WND::WmKey, KEY::F1);\n"); break;
		case VK_F2: m_Comm.Send("WND.Message(WND::WmKey, KEY::F2);\n"); break;
		case VK_F3: m_Comm.Send("WND.Message(WND::WmKey, KEY::F3);\n"); break;
		case VK_F4: m_Comm.Send("WND.Message(WND::WmKey, KEY::F4);\n"); break;
		}
/*
	case WM_CREATE:
		{
			InitCommonControls();
			m_hStatus=CreateStatusWindow(WS_CHILD | WS_VISIBLE, (LPCTSTR)NULL, hWnd, 200);
			SendMessage(m_hStatus, SB_SIMPLE, (WPARAM)(BOOL)TRUE, (LPARAM)0);
			SetWindowText( m_hStatus, "Pokus...");
			RECT rectStatus;
			GetClientRect( m_hStatus, &rectStatus);

			RECT rect;
			rect.left = 100;
			rect.top = 100;
			rect.right = rect.left + 400;
			rect.bottom = rect.top + 240 + (rectStatus.bottom - rectStatus.top);
			AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, TRUE );

			SetWindowPos( hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE );
			InvalidateRect( hWnd, NULL, FALSE );
			break;
		}
*/
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
