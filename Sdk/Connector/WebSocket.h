#include <winsock.h>
#include <wininet.h>
#include <WinCrypt.h>
#include <string>
#include <list>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;
#pragma push_macro("printf")
#define printf Print

void Print(LPSTR pFormat, ...)
{
	static CHAR buff[1024];
	va_list argList;

	va_start(argList, pFormat);
	vsprintf(buff, pFormat, argList);
	va_end(argList);

	OutputDebugStringA("CWebSockets: ");
	OutputDebugStringA(buff);
}
 
class CHash
{
public:
	static LPSTR CreateHash(LPSTR tohash, ALG_ID alg)
	{
		HCRYPTPROV hProv;
		HCRYPTHASH hash;

		LPSTR Buf = GetBuffer();
		DWORD& BufSize = GetLength();
	 
		if ( !CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 
			CRYPT_VERIFYCONTEXT) )
		{
			printf("Call to CryptAcquireContext failed. Error: %d\n", 
				GetLastError());
			return NULL;
		}

		if ( !CryptCreateHash(hProv, alg, 0, 0, &hash) )
		{
			printf("Call to CryptCreateHash failed. Error: %d\n", 
				GetLastError());
			CryptReleaseContext(hProv, 0);
			return NULL;
		}

		if ( !CryptHashData(hash, (BYTE *)tohash, strlen(tohash), 0) )
		{
			printf("Call to CryptHashData failed. Error: %d\n",
				GetLastError());
			CryptDestroyHash(hash);
			CryptReleaseContext(hProv, 0);
			return NULL;
		}

		ZeroMemory(Buf, 1024);
		BufSize = 1024;

		if (!CryptGetHashParam(hash, HP_HASHVAL,
			(BYTE *) Buf, &BufSize, 0))
		{
			printf("Call to CryptGetHashParam failed. Error: %d\n",	
				GetLastError());
			CryptDestroyHash(hash);
			CryptReleaseContext(hProv, 0);
			return NULL;
		}

		CryptDestroyHash(hash);
		CryptReleaseContext(hProv, 0);
	 
		return Buf;
	}

	static LPSTR GetBuffer()
	{
		static char Buf[1024];
		return Buf;
	}
	
	static DWORD& GetLength()
	{
		static DWORD Len = 0;
		return Len;
	}


	static void base64_encode(unsigned char* bytes_to_encode, unsigned int in_len, char* out) 
	{
	  int i = 0;
	  int j = 0;
	  unsigned char char_array_3[3];
	  unsigned char char_array_4[4];
	  static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

	  while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
		  char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		  char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		  char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		  char_array_4[3] = char_array_3[2] & 0x3f;

		  for(i = 0; (i <4) ; i++)
			*out++ = base64_chars[char_array_4[i]];
		  i = 0;
		}
	  }

	  if (i)
	  {
		for(j = i; j < 3; j++)
		  char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
		  *out++ = base64_chars[char_array_4[j]];

		while((i++ < 3))
		  *out++ = '=';

	  }
	  *out = 0;
	}

};


class CWebSockets
{
public:
	enum {
		NUMCLIENTS = 3,
		BUFLEN = 1024
	};

private:
	struct SConnect
	{
		SOCKET socket;
		HANDLE handle;
		unsigned int id;
		int nWebSocket;
		CWebSockets* pThis;
		BOOL bReady;
	};

	typedef void (*Listener)(unsigned char*);

	HANDLE	m_ghMutex; 
	HANDLE	m_mainThread;
	BOOL	m_isMutexEnabled;	//FALSE to allow race conditions.
	SOCKET	m_listenfd;
	list<SConnect*> m_connectList;
	char*	m_szHost;
	int		m_nPort;
	Listener	m_Listener;

public:
	CWebSockets()
	{
		m_isMutexEnabled = FALSE;
		m_szHost = "127.0.0.1";
		m_nPort = 38950;
		m_Listener = NULL;
	}

	void Start()
	{
		printf("Start()\n");
		m_mainThread = (HANDLE)_beginthreadex(
				NULL, 
				0, 
				&MainCallback, 
				this,
				0, 
				NULL);
	}

	void Stop()
	{
		printf("Stop()\n");
		::SuspendThread(m_mainThread);
		closesocket(m_listenfd);
		m_listenfd = NULL;
		::ResumeThread(m_mainThread);

		Sleep(300);
		CloseHandle(m_mainThread);
		ServerDispose();
		ServerCleanup();
		printf("Done\n");
	}
	
	void SetListener( Listener l )
	{
		m_Listener = l;
	}

	void Send( char* pstr )
	{
		list<SConnect*>::iterator i;
		for( i = m_connectList.begin(); i != m_connectList.end(); i++ )
		{
			SConnect* pConnect = (*i);
			if ( pConnect->socket && pConnect->bReady )
				Send( pstr, pConnect->socket, pConnect->nWebSocket );
		}
	}

private:
	static unsigned __stdcall MainCallback( void* pArguments ) 
	{
		CWebSockets* pThis = (CWebSockets*)pArguments;
		pThis->Main();
		return 0;
	}

	BOOL Main()
	{
		if ( !InitResources() )
			return FALSE;
		if ( !ServerStartup() )
			return FALSE;
	
		while (m_listenfd) 
		{
			if ( m_connectList.size() >= NUMCLIENTS )
			{
				printf("No more clients allowed.\n");
				for (int i=0; i<200 && m_listenfd; i++)
					Sleep(100);
				continue;
			}

			printf("WebSocket: Waiting for client ... (listenfd=%d)\n", m_listenfd);
			SOCKET connectfd = AcceptClient();
			printf("WebSocket: AcceptClient returned %d (listenfd=%d)\n", connectfd, m_listenfd);
			if ( !m_listenfd )
				break;

			if ( connectfd == INVALID_SOCKET )
			{
				printf("Restarting server\n");
				Sleep( 1000 );
				if ( !Restart() ) 
				{
					printf("Failed to restart, disabling server...\n");
				}
				continue;
			}
			SConnect* pConnect = new SConnect();
			m_connectList.push_back(pConnect);
			pConnect->pThis = this;
			pConnect->socket = connectfd;
			pConnect->nWebSocket = 0;
			pConnect->bReady = FALSE;

			pConnect->handle = (HANDLE)_beginthreadex(
				NULL, 
				0, 
				&OnConnectCallback, 
				pConnect, // &m_connectDescriptors[m_nClients], //connectfd
				CREATE_SUSPENDED, 
				&pConnect->id);

			::ResumeThread(pConnect->handle);
		}
		printf("Closing Main thread.\n");
		ServerDispose();
		return TRUE;
	}

private:
	BOOL Restart()
	{
		closesocket(m_listenfd);
		return ServerStartup();
	}

	BOOL ServerStartup() 
	{
		m_listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	
		struct sockaddr_in server = { 0 };
		server.sin_family = AF_INET;	
		server.sin_addr.s_addr = inet_addr(m_szHost);
		server.sin_port = htons(m_nPort);
		
		// Bind the socket.
		if (bind( m_listenfd, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR) 	
		{
			printf("Error on server bind.\n");
			printf("WSAGetLastError = %d\n", WSAGetLastError() );
			return FALSE;
		}		
		if (listen(m_listenfd, SOMAXCONN ) == SOCKET_ERROR)
		{
			printf("Error listening on socket.\n");
			return FALSE;
		}		
		printf("Server is listening on socket... %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));	
		return TRUE;
	}

	BOOL InitResources() 
	{
		m_ghMutex = CreateMutex( 
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

		if (m_ghMutex == NULL) 
		{
			printf("CreateMutex error: %x\n", GetLastError());
			return FALSE;
		}

		WSADATA wsaData;	
		if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)// Initialize Winsock
		{
			printf("Error at WSAStartup()\n");
			return FALSE;
		}

		return TRUE;		
	}

	void ServerCleanup()
	{
		if ( m_ghMutex )
		{
			CloseHandle(m_ghMutex);
			m_ghMutex = NULL;
		}
		WSACleanup();
	}

	void ServerDispose() 
	{
		list<SConnect*>::iterator i;
		for( i = m_connectList.begin(); i != m_connectList.end(); i++ )
		{
			SConnect* pConnect = (*i);
			if ( pConnect->socket )
			{
				if (pConnect->handle) 
					::SuspendThread(pConnect->handle);

				closesocket( pConnect->socket );
				pConnect->socket = NULL;

				if (pConnect->handle) 
					::ResumeThread(pConnect->handle);
			}
		}

		Sleep(100);
		
		for( i = m_connectList.begin(); i != m_connectList.end(); i++ )
		{
			SConnect* pConnect = (*i);
			if ( pConnect->handle )
			{
				CloseHandle( pConnect->handle );
				delete pConnect;
			}
			m_connectList.remove( pConnect );
			delete pConnect;
		}
		//m_connectList.clear();

		if ( m_listenfd )
			closesocket(m_listenfd);
	}

	SOCKET AcceptClient() 
	{
		struct sockaddr clientinfo = { 0 };
		SOCKET connectfd = accept(m_listenfd, &clientinfo, NULL);		
		if ( connectfd == INVALID_SOCKET )
		{
			printf("Invalid socket, WSAGetLastError = %d\n", WSAGetLastError() );
			return INVALID_SOCKET;
		}
		struct sockaddr_in* ipv4info = (struct sockaddr_in*)&clientinfo;
		printf("Client connected on socket %x. Address %s:%d\n", connectfd, 
		inet_ntoa(ipv4info->sin_addr), ntohs(ipv4info->sin_port));		
		return connectfd;
	}

	// Comm
	int Send(char* src, SOCKET s, int nWebSocket)
	{	
		_ASSERT( s );
		if ( nWebSocket == 13 || nWebSocket == 8 )
		{
			unsigned char* frame = NULL;
			unsigned char* frameMsg = NULL;

			int nMsgLen = strlen(src);
			int nFrameLen = nMsgLen + 2;

			if ( nMsgLen <= 125 )
			{
				frame = new unsigned char[nFrameLen];
				frame[0] = 0x81;
				frame[1] = nMsgLen;
				frameMsg = frame+2;
			} else if ( nMsgLen <= 65000 )
			{
				nFrameLen += 2;
				frame = new unsigned char[nFrameLen];
				frame[0] = 0x81;
				frame[1] = 126;
				frame[2] = nMsgLen >> 8;
				frame[3] = nMsgLen & 0xff;
				frameMsg = frame + 4;
			} else
			{
				nFrameLen += 8;
				frame = new unsigned char[nFrameLen];
				frame[0] = 0x81;
				frame[1] = 127;

				frame[2] = 0;
				frame[3] = 0;
				frame[4] = 0;
				frame[5] = 0;
				frame[6] = (nMsgLen >> 24) & 0xff;
				frame[7] = (nMsgLen >> 16) & 0xff;
				frame[8] = (nMsgLen >>  8) & 0xff;
				frame[9] = (nMsgLen >>  0) & 0xff;

				frameMsg = frame + 10;
			}
			memcpy(frameMsg, src, nMsgLen);

			int nCount = send(s, (char*)frame, nFrameLen, 0);
			delete frame;
			return nCount;
		}

		_ASSERT( nWebSocket == 0 );
		return send(s, src, strlen(src), 0);
	}

	int Recv(char* buffer, int buflen, SOCKET s, int nWebSocket)
	{	
		if ( nWebSocket == 13 || nWebSocket == 8 )
		{
			unsigned char msg[BUFLEN];
			unsigned char *pmsg = msg;
			int nRecv = recv(s, (char*)msg, BUFLEN, 0);
			if ( nRecv <= 0 )
				return -1;
			if ( nRecv <= 2 )
			{
				_ASSERT(0);
				return 0;
			}	
			
			if ( *pmsg == 0x88 )
			{
				// closing frame
				return -1;
			}

			if ( *pmsg != 0x81 ) 
			{
				// buffer mismatch
				printf("Buffer mismatch, ignored %d bytes\n", nRecv);
				return 0;
			}

			_ASSERT(*pmsg == 0x81); // final text frame
			pmsg++;
			BOOL bMask = *pmsg & 0x80;
			DWORD nLen = *pmsg & 0x7f;
			pmsg++;

			if ( nLen == 126 )
			{
				unsigned char bHi = *pmsg++;
				unsigned char bLow = *pmsg++;
				nLen = (bHi<<8) | bLow;
			} else if ( nLen == 127 )
			{
				unsigned char bA = *pmsg++;
				unsigned char bB = *pmsg++;
				unsigned char bC = *pmsg++;
				unsigned char bD = *pmsg++;
				nLen = bA;
				nLen <<= 8;
				nLen |= bB;
				nLen <<= 8;
				nLen |= bC;
				nLen <<= 8;
				nLen |= bD;
			}

			unsigned char aMask[4] = {0};
			
			if ( bMask )
			{
				aMask[0] = *pmsg++;
				aMask[1] = *pmsg++;
				aMask[2] = *pmsg++;
				aMask[3] = *pmsg++;
			}

			_ASSERT( (int)(pmsg-msg) <= (int)nRecv-(int)nLen );
			if ( pmsg-msg != nRecv-nLen )
			{
				int nExtra = nRecv-nLen+(msg-pmsg);
				printf("Ignored %d bytes from buffer\n", nExtra);
			}

			_ASSERT( (int)nLen < (int)buflen );
			int i, n = min(buflen-1, (int)nLen);
			for (i=0; i<n; i++)
				buffer[i] = pmsg[i] ^ aMask[i&3];
			buffer[i] = 0;
			return i;

		}
		_ASSERT( nWebSocket == 0 );
		return recv(s, buffer, buflen, 0);
	}

	static unsigned __stdcall OnConnectCallback( void* pArguments ) 
	{
		SConnect *pConnect = (SConnect *)pArguments;
		CWebSockets* pThis = pConnect->pThis;
		SOCKET connectfd = pConnect->socket;
		
		pThis->OnConnect( pConnect );

		closesocket( pConnect->socket );
		pConnect->socket = NULL;
		CloseHandle( pConnect->handle );
		pConnect->handle = NULL;
		pThis->m_connectList.remove( pConnect );

		delete pConnect;
		return 0;
	}

	BOOL OnConnect( SConnect *pConnect ) 
	{
		char shared_buffer[ BUFLEN ];

		pConnect->nWebSocket = Handshake( pConnect->socket );
		
		if ( pConnect->nWebSocket )
		{
			sprintf(shared_buffer, "client socket descriptor is %d, version %d.", 
				pConnect->socket, pConnect->nWebSocket);
			if ( Send(shared_buffer, pConnect->socket, pConnect->nWebSocket) < 0 )
			{
				printf("WSAGetLastError: %d\n", WSAGetLastError());
				return TRUE;
			}
		}

		pConnect->bReady = TRUE;

		int received;
		do {
			AcquireMutex();		
			received = Recv(shared_buffer, BUFLEN, pConnect->socket, pConnect->nWebSocket);
			if ( received > 0 )
			{
				shared_buffer[received] = 0;
				//printf("server thread %x\tshared buffer: \"%s\"\n", GetCurrentThreadId(), shared_buffer);
				if ( m_Listener )
					m_Listener( (unsigned char*)shared_buffer );
				//Send(shared_buffer, pConnect->socket, pConnect->nWebSocket);
			}
			ReleaseMutex();
		} while (received > -1 && pConnect->socket);	//continue until client SOCKET closes.

		pConnect->bReady = FALSE;
		printf("Client socket connection closed (%d). Exiting server thread %x...\n", WSAGetLastError(), (DWORD)pConnect->handle);
		return TRUE;
	}

	void AcquireMutex() 
	{
		if (!m_isMutexEnabled)	
			return;
		if ( WaitForSingleObject(m_ghMutex, INFINITE) != WAIT_OBJECT_0) { 
			printf("Error on WaitForSingleObject (thread %x)\n", GetCurrentThreadId()); 
		}
	}

	void ReleaseMutex() 
	{
		if (!m_isMutexEnabled)
			return;

		if (!::ReleaseMutex(m_ghMutex)) 
		{ 
			printf("Error releasing Mutex on thread %x.\n", GetCurrentThreadId()); 
		}
	}

	static int Handshake( SOCKET connectfd )
	{
		const int nBufSize = 1024;
		char requestbuf[nBufSize];
		int received = recv(connectfd, requestbuf, nBufSize-1, 0);
		requestbuf[received] = 0;

		const char* pfindver = "Sec-WebSocket-Version:";
		char* pver = strstr( requestbuf, pfindver );
		if ( !pver )
			return 0;
		pver += strlen( pfindver );
		while ( *pver == ' ' )
			pver++;

		int nVersion = atoi(pver);

		const char* pfindkey = "Sec-WebSocket-Key:";
		char* pkey = strstr( requestbuf, pfindkey );
		if ( !pkey )
			return FALSE;
		pkey += strlen( pfindkey  );
		while ( *pkey == ' ' )
			pkey++;
		char *pend = strstr(pkey, "\r\n");
		if ( !pend )
			return FALSE;
		*pend = 0;

		char strHash[256];
		strcpy(strHash, pkey);
		strcat(strHash, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		CHash::CreateHash(strHash, CALG_SHA1);
		CHash::base64_encode((unsigned char*)CHash::GetBuffer(), CHash::GetLength(), strHash );
		
		char strReply[256];
		sprintf(strReply,
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n\r\n", strHash );

		if ( send(connectfd, strReply, strlen(strReply), 0) <= 0 )
			return 0;

		return nVersion;
	}
};

#pragma pop_macro("printf")
