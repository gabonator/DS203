// nocache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <crtdbg.h>
#include <string.h>
#include "WebSocket.h"

#define _ASSERT_VALID(e) { if ( !(e) ) _ASSERT(0); }
const CHAR sdkFile[] = "G:\\SDK.BIN";

bool bRunning = true;

CWebSockets m_ws;
volatile int nRequest = 0;
CHAR sendmsg[1024] = {0};

// reading {{{
HANDLE _rdh = NULL;
BYTE _rdb[512];
int _rdi = 0;
void _Set( HANDLE h)
{
	_rdh = h;
}
void _Push( BYTE* buf, int nSize )
{
	_ASSERT( nSize == sizeof(_rdb) );
	memcpy( _rdb, buf, 512 );
	_rdi = 0;
}
int _Get()
{
	if ( _rdi < sizeof(_rdb) )
	{
		int nAux = _rdb[_rdi++];
		return nAux;
	}
	_ASSERT( _rdh );
	DWORD dwRead;
	ReadFile(_rdh, _rdb, sizeof(_rdb), &dwRead, NULL );
	_ASSERT( dwRead == sizeof(_rdb) );
	_rdi = 0;
	return _Get();
}
// }}} reading

bool hasZero( char* p, int n )
{
	for (int i=0; i<n; i++)
		if ( p[
			i] == 0 )
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
	if ( dwRead == 0 )
	{
		int nErr = GetLastError();
		printf("Read error %d ", nErr);
	}
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

	_Set( hFile );
	_Push( aSector, 512 );

	/*
	strToken[0] = _Get();
	strToken[1] = _Get();
	strToken[2] = _Get();
	strToken[3] = _Get();
	*/

	CArray<CString> arrAns;
	do 
	{
		CHAR strToken[4]  = { _Get(), _Get(), _Get(), _Get() };
		if ( memcmp( strToken, "ANS=", 4 ) == 0 )
		{
			CString strAns = "\"";
			_ASSERT_VALID( _Get() == '(' );
			CHAR type[4] = {_Get(), _Get(), _Get(), _Get()};

			if ( memcmp( type, "text", 4 ) == 0 )
			{
				CHAR ch = 0; 
				do {
					ch = _Get();
				} while ( ch != ')' );
				_ASSERT_VALID( _Get() == ' ' );

				do {
					ch = _Get();
					if (ch)
						strAns += ch;
				} while ( ch != 0 );
				strAns += "\"";
				printf( "%s", strAns.GetBuffer() );
			} else
			if ( memcmp( type, "bina", 4 ) == 0 )
			{
				_ASSERT_VALID( _Get() == 'r' );
				_ASSERT_VALID( _Get() == 'y' );
				_ASSERT_VALID( _Get() == ',' );
				_ASSERT_VALID( _Get() == ' ' );
				_ASSERT_VALID( _Get() == 'l' );
				_ASSERT_VALID( _Get() == 'e' );
				_ASSERT_VALID( _Get() == 'n' );
				_ASSERT_VALID( _Get() == '=' );
				CString strLen;
				CHAR ch = 0;
				do {
					ch = _Get();
					strLen += ch;
				} while ( ch && ch != ')' );
				int nLen = atoi( strLen.GetBuffer() );

				strAns = "[";
				for ( int i = 0; i < nLen; i++ )
				{
					CString strSingle;
					if ( i>0)
						strAns += ",";
					unsigned char uch = _Get();
					strSingle.Format("0x%02x", uch);
					strAns += strSingle;
				}
				strAns += "]";
			}
			arrAns.Add( strAns );
		} else
		if ( memcmp( strToken, "END\x1b", 4 ) == 0 )
		{
			break;
		} else
		{
			_ASSERT(0);
			break;
		}
	} while (1);

	/*
	if ( arrAns.GetSize() == 1 )
	{
		m_ws.Send( arrAns[0].GetBuffer() );
	} else*/
	{
		CString strAns = "[";
		for (int i=0; i<arrAns.GetSize(); i++)
		{
			if ( i > 0 )
				strAns+=", ";
			strAns += arrAns[i];
		}
		strAns += "]";
		m_ws.Send( strAns.GetBuffer() );
	}
	/*
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

	}*/
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
	if ( sendmsg[0] != 0 )
	{
		printf("Wait, previous buffer not sent yet!");
		return;
	}
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
