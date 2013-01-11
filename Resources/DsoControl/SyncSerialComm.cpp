// SyncSerialComm.cpp: implementation of the CSyncSerialComm class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SyncSerialComm.h"

#include <string.h>
#include <assert.h>

#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyncSerialComm::CSyncSerialComm(const char *pszPortName)
	: m_hSerialComm(INVALID_HANDLE_VALUE)
{
	assert(pszPortName);

	m_pszPortName = new char[strlen(pszPortName)+1];
	strcpy(m_pszPortName, pszPortName);
}

CSyncSerialComm::~CSyncSerialComm()
{
	if(m_pszPortName)
		delete[] m_pszPortName;

	Close();
}

//////////////////////////////////////////////////////////////////////
// Name: Open
// Version: 1.0
// Return: HRESULT
// Comment: This function is used open a connection with a serial port.
// Uses non-overlapped i/o, and allows for reading & writing to the 
// port.
//////////////////////////////////////////////////////////////////////

HRESULT CSyncSerialComm::Open()
{
	HRESULT hResult;

	m_hSerialComm = CreateFile(m_pszPortName, /* Port Name */ 
							   GENERIC_READ | GENERIC_WRITE, /* Desired Access */ 
							   0, /* Shared Mode */
							   NULL, /* Security */
							   OPEN_EXISTING, /* Creation Disposition */
							   0,
							   NULL); /* Non Overlapped */

	if(m_hSerialComm == INVALID_HANDLE_VALUE)
	{
		unsigned long error = ::GetLastError();
		hResult = E_FAIL;
	}

	else
		hResult = S_OK;

	return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name: Close
// Version: 1.0
// Return: HRESULT
// Comment: This function is used to close the serial port connection
// Note: This function is called with the destructor
//////////////////////////////////////////////////////////////////////

HRESULT CSyncSerialComm::Close()
{
	if(m_hSerialComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hSerialComm);
		m_hSerialComm = INVALID_HANDLE_VALUE;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Name: ConfigPort
// Version: 1.0
// Parameter: dwBaudRate - This must be set to the baud rate of the
// serial port connection otherwise invalid reads occur.
// dwTimeOutInSec - Specifies the timeout for read and write of the serial
// port connection in seconds
// Return: HRESULT
// Comment: This function is used configure the serial port connection.
//////////////////////////////////////////////////////////////////////

HRESULT CSyncSerialComm::ConfigPort(DWORD dwBaudRate, DWORD dwTimeOutInSec)
{
	if(!SetupComm(m_hSerialComm, 1024, 1024))
		return E_FAIL;

	DCB dcbConfig;

	if(GetCommState(m_hSerialComm, &dcbConfig)) /* Configuring Serial Port Settings */
	{
		dcbConfig.BaudRate = dwBaudRate;
		dcbConfig.ByteSize = 8;
		dcbConfig.Parity = NOPARITY;
		dcbConfig.StopBits = ONESTOPBIT;
		dcbConfig.fBinary = TRUE;
		dcbConfig.fParity = TRUE;
	}

	else
		return E_FAIL;

	if(!SetCommState(m_hSerialComm, &dcbConfig))
		return E_FAIL;

	COMMTIMEOUTS commTimeout;
				
	if(GetCommTimeouts(m_hSerialComm, &commTimeout)) /* Configuring Read & Write Time Outs */
	{
		commTimeout.ReadIntervalTimeout = 1000*dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutConstant = 1000*dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutMultiplier = 0;
		commTimeout.WriteTotalTimeoutConstant = 1000*dwTimeOutInSec;
		commTimeout.WriteTotalTimeoutMultiplier = 0;
	}

	else
		return E_FAIL;

	if(SetCommTimeouts(m_hSerialComm, &commTimeout))
		return S_OK;

	else
		return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
// Name: Read
// Version: 1.0
// Parameter: ppszBuf - The buffer that will have the value that was
// read in from the serial port.
// dwSize - The size of the buffer
// Return: HRESULT
// Comment: This function sets an event that will be signalled if the
// any byte is buffered internally. Once this occurs, the function keeps
// reading multiple a single byte at a time until there is no more furthur
// byte to read from the input stream
//////////////////////////////////////////////////////////////////////

HRESULT CSyncSerialComm::Read(char *ppszBuf, DWORD &dwSize)
{
	HRESULT hResult = S_OK;
	std::stringbuf sb;
	DWORD dwEventMask;

	if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) /* Setting Event Type */
		return E_FAIL;

	if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL)) /* Waiting For Event to Occur */
	{
		ReadFile(m_hSerialComm, ppszBuf, dwSize, &dwSize, NULL);

		/*
		char szBuf;
		DWORD dwIncommingReadSize;
		do
		{
			if(ReadFile(m_hSerialComm, &szBuf, 1, &dwIncommingReadSize, NULL) != 0)
			{
				if(dwIncommingReadSize > 0)
				{
					dwSize += dwIncommingReadSize;
					sb.sputn(&szBuf, dwIncommingReadSize);
				}
			}

			else
			{
				unsigned long error = ::GetLastError();
				hResult = E_FAIL;
				break;
			}

		} while(dwIncommingReadSize > 0);

		*ppszBuf = new char[dwSize];
		strcpy(*ppszBuf, (sb.str()).c_str());
		*/
		return hResult;
	}

	else
		return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
// Name: Write
// Version: 1.0
// Parameter: szBuf - The buffer holding the bytes to write to the serial
// port connection
// dwSize - The size of the buffer
// Return: HRESULT
// Comment: This function writes one byte at a time until all the bytes
// in the buffer is sent out
//////////////////////////////////////////////////////////////////////

HRESULT CSyncSerialComm::Write(const char *pszBuf, DWORD dwSize)
{
	HRESULT hResult = S_OK;

	assert(pszBuf);

	unsigned long dwNumberOfBytesSent = 0;

	while(dwNumberOfBytesSent < dwSize)
	{
		unsigned long dwNumberOfBytesWritten;

		if(WriteFile(m_hSerialComm, &pszBuf[dwNumberOfBytesSent], 1, &dwNumberOfBytesWritten, NULL) != 0)
		{
			if(dwNumberOfBytesWritten > 0)
				++dwNumberOfBytesSent;
			else
			{
				unsigned long error = ::GetLastError();
				hResult = E_FAIL;
				break;
			}
		}

		else
		{
			unsigned long error = ::GetLastError();
			hResult = E_FAIL;
			break;
		}
	}

	return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name: Flush
// Version: 1.0
// Parameter: dwFlag - The flag specifying if the input/output buffer
// to be flushed
// Return: HRESULT
// Comment: This function is flushes the specfied buffer
// Note: By default, both the input and output buffers are flushed
//////////////////////////////////////////////////////////////////////

HRESULT CSyncSerialComm::Flush(DWORD dwFlag)
{
	if(PurgeComm(m_hSerialComm, dwFlag))
		return S_OK;
	else
		return E_FAIL;
}