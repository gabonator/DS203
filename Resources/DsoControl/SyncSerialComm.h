// SyncSerialComm.h: interface for the CSyncSerialComm class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>

#if !defined(AFX_SYNCSERIALCOMM_H__D1CAB621_DF4B_4729_82AB_31D5B9EFE8A9__INCLUDED_)
#define AFX_SYNCSERIALCOMM_H__D1CAB621_DF4B_4729_82AB_31D5B9EFE8A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Name: CSyncSerialComm
// Version: 1.0
// Comment: This class is responsible for provide I/O operation with
// a serial port. It is implemented with Synchronous I/O viz. both
// input and output operations block. Both read and write operations
// are supported.
//////////////////////////////////////////////////////////////////////

class CSyncSerialComm
{
public:
	HRESULT Flush(DWORD dwFlag = PURGE_TXCLEAR | PURGE_RXCLEAR);
	HRESULT Write(const char *pszBuf, DWORD dwSize);
	HRESULT Read(char *ppszBuf, DWORD &dwSize);
	HRESULT ConfigPort(DWORD dwBaudRate = CBR_19200, DWORD dwTimeOutInSec = 5);
	HRESULT Close();	
	HRESULT Open();

	CSyncSerialComm(const char *pszPortName);
	virtual ~CSyncSerialComm();

private:
	char *m_pszPortName;
	HANDLE m_hSerialComm;
};

#endif // !defined(AFX_SYNCSERIALCOMM_H__D1CAB621_DF4B_4729_82AB_31D5B9EFE8A9__INCLUDED_)