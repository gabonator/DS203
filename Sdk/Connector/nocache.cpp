// nocache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <crtdbg.h>
#include <string.h>
#include "WebSocket.h"

const CHAR sdkFile[] = "G:\\SDK.BIN";

bool bRunning = true;

CWebSockets m_ws;
volatile int nRequest = 0;
CHAR sendmsg[1024] = {0};

bool hasZero( char* p, int n )
{
	for (int i=0; i<n; i++)
		if ( p[i] == 0 )
			return true;
	return false;
}

BOOL Do(HANDLE hFile)
{
	static BYTE aLastRead[512] = {0};
	static BYTE aLastWrite[512] = {0};
	static int nBusy = 0;

	BYTE aSector[512];

	SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

	if (sendmsg[0])
	{
		strcpy( (char*)aSector, "REQ=" );
		strcpy( (char*)aSector+4, sendmsg );
		printf("'%s'", aSector);
		DWORD dwWritten = 0;
		WriteFile(hFile, aSector, sizeof(aSector), &dwWritten, NULL );
		_ASSERT( dwWritten == sizeof(aSector) );
		memcpy(aLastRead, aSector, sizeof(aSector));
		sendmsg[0] = 0;
		return TRUE; 
	}

	DWORD dwRead = 0;
	ReadFile(hFile, aSector, sizeof(aSector), &dwRead, NULL );
	_ASSERT( dwRead > 0 );

	if ( memcmp(aLastRead, aSector, sizeof(aSector)) == 0 )
	{
		printf(".");
		return TRUE;
	}
	if ( memcmp("ANS=", aSector, 4) != 0 )
	{
		printf(":");
		Sleep(100);
		if ( ++nBusy > 4 )
		{
			printf("busy");
			Sleep(2000);
		}
		return TRUE;
	}
	memcpy(aLastRead, aSector, sizeof(aSector));
	nBusy = 0;

	if ( memcmp( aSector, "ANS=", 4 ) == 0 )
	{
		if ( memcmp( aSector+5, "text", 4) == 0 )
		{
			_ASSERT( hasZero( (char*)aSector, sizeof(aSector ) ) );
			printf("%s\n", (char*)aSector);

			CString strMsg;
			_ASSERT( strstr( (char*)aSector, ")" ) );
			strMsg.Format("\"%s\"", strstr( (char*)aSector, ")" )+2 );
			m_ws.Send( strMsg.GetBuffer() );
		} else
		if ( memcmp( aSector+5, "binary", 6) == 0 )
		{
			char* pLen = strstr( (char*)aSector, "len=" );
			_ASSERT( pLen );
			pLen += 4;
			int nLen = atoi( pLen );
			_ASSERT( nLen > 0 );

			int nOffset = (DWORD)strstr( (char*)aSector, ")" )-(DWORD)aSector+1;
			_ASSERT( nOffset > 0 );
			printf("stream %d bytes", nLen);
			CString strOut = "[";

			for ( int i=0; i<nLen; i++)
			{
				unsigned char ch = aSector[nOffset++];

				CString strSingle;
				if ( i>0)
					strOut += ",";
				strSingle.Format("0x%02x", ch);
				strOut += strSingle;

				if (nOffset >= 512)
				{
					nOffset = 0;
					DWORD dwRead;
					ReadFile(hFile, aSector, sizeof(aSector), &dwRead, NULL );
					_ASSERT( dwRead > 0 );
				}
			}
			strOut += "]";
			m_ws.Send( strOut.GetBuffer() );
		} else
			printf("Unknown data\n");

	}
	return TRUE;
}
/*
	strcpy((char*)aSector, "REQ=");
	if ( *fgets((char*)aSector+4, sizeof(aSector)-4, stdin) == 0 )
		return FALSE;

	if ( aSector[strlen((char*)aSector)-1] == 10 )
		aSector[strlen((char*)aSector)-1] = 0;
		
	if ( aSector[4] == 0 )
		return FALSE;

	SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

	DWORD dwWritten = 0;
	WriteFile(hFile, aSector, sizeof(aSector), &dwWritten, NULL );
	_ASSERT( dwWritten == sizeof(aSector) );
	memcpy(aLastRead, aSector, sizeof(aSector));

}*/

BOOL CtrlHandler( DWORD fdwCtrlType )
{
	switch( fdwCtrlType ) {
	case CTRL_C_EVENT:
		bRunning = FALSE;
		printf( "Ctrl-C event\n\n" );
		return TRUE;
	default:
		return FALSE;
	}
}

void listener( UCHAR* pstr )
{
	_ASSERT( sendmsg[0] == 0 );
	strcpy( (char*)sendmsg+1, (char*)pstr+1 );
	sendmsg[0] = pstr[0];
}

int main(int argc, CHAR* argv[])
{
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
	m_ws.SetListener( listener );

	DeleteFile( sdkFile );

	HANDLE hInit = CreateFile(
		sdkFile,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_FLAG_NO_BUFFERING,
		NULL);
	
	_ASSERT( hInit != INVALID_HANDLE_VALUE );

	//DWORD dwSize = GetFileSize( hInit, NULL );
	//if ( dwSize != 32768 )
	{
		char buf[32768] = {0};
		DWORD dwInitWritten;
		WriteFile( hInit, buf, 32768, &dwInitWritten, NULL);
		_ASSERT( dwInitWritten == 32768);
	}
	CloseHandle( hInit );

	HANDLE hFile = CreateFile(
		sdkFile,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);

	m_ws.Start();
	while (bRunning && Do(hFile) )
	{
		Sleep(100);
	}
	m_ws.Stop();

	CloseHandle(hFile);
	return 0;
}
