#include <Source/Core/Sdk.h>

class CSdkStreamProvider : public CSdkEval
{
public:
	CHAR m_strExpression[FILEINFO::SectorSize];
	CHAR m_strSimpleAns[64];
	PSTR m_pszExpression;
	int m_nSimpleLen;
	int m_nStreamLen;
	int m_nSimplePos;
	bool m_bTerminator;
	CEval::CStream* m_pStream;

  void Evaluate( PSTR pszExpression )
	{
		// copy buffer and set pointer to start of that string
		strcpy( m_strExpression, pszExpression );
		m_pszExpression = m_strExpression; // points to the first letter
		m_nSimplePos = 0;
		m_nStreamLen = -1;
		m_nSimpleLen = -1;
		m_pStream = NULL;
		m_bTerminator = FALSE;
	}

	int GetChar()
	{	
		if ( m_nSimpleLen > 0 )
		{
			int ch = m_strSimpleAns[ m_nSimplePos++ ];
			if ( m_nSimplePos >= m_nSimpleLen )
				m_nSimpleLen = -1;
			return ch;
		}

		if ( m_nStreamLen > 0 )
		{
			int ch = m_pStream->Get();
			if ( --m_nStreamLen <= 0 )
				m_nStreamLen = -1;
			return ch;
		}

		if ( m_nSimpleLen == -1 && m_nStreamLen == -1 )
		{
			if ( m_pszExpression == NULL || *m_pszExpression == 0 )
			{/*
				if ( !m_bTerminator )
				{
					m_bTerminator = TRUE;
					strcpy( m_strSimpleAns, "END\x1b" );
					m_nSimpleLen = 4;
					m_nSimplePos = 0;
					return GetChar();
				}*/
				// at end of evaluated expression
				return -1;
			} else
			{
				// Evaluate expression
				CSdkEval::CEvalOperand opResult = Eval(m_pszExpression);

				if ( opResult.m_eType == CEval::CEvalOperand::eoVariable )
				{
					opResult = opResult.m_Data.m_pVariable->Get();
				}

				switch ( opResult.m_eType )               
				{
					case CEval::CEvalOperand::eoError: 
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, msg) Error"); 
						BIOS::DBG::Print("Error Evaluating '%s'", m_pszExpression);
						MainWnd.m_wndMessage.Show(&MainWnd, "SDK Warning", "Invalid request", RGB565(FF0000));

						m_nSimpleLen = strlen(m_strSimpleAns) + 1; // including terminating zero
						m_nSimplePos = 0;
					break;

					case CEval::CEvalOperand::eoFloat: 
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, float->int) %d", opResult.GetInteger()); 
						m_nSimpleLen = strlen(m_strSimpleAns) + 1;
						m_nSimplePos = 0;
					break;

					case CEval::CEvalOperand::eoInteger: 	
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, si32) %d", opResult.m_Data.m_iData);	 
						m_nSimpleLen = strlen(m_strSimpleAns) + 1;
						m_nSimplePos = 0;
					break;

					case CEval::CEvalOperand::eoString: 
						opResult.m_Data.m_pString[ opResult.m_Data.m_pData32[1] ] = 0;
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, string) %s", opResult.m_Data.m_pString); 
						m_nSimpleLen = strlen(m_strSimpleAns) + 1;
						m_nSimplePos = 0;
					break;

					case CEval::CEvalOperand::eoCString: 
						//opResult.m_Data.m_pString[ opResult.m_Data.m_pData32[1] ] = 0;
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, string) ", opResult.m_Data.m_pcString); 
						memcpy( m_strSimpleAns+strlen(m_strSimpleAns), opResult.m_Data.m_pcString, opResult.m_Data.m_pData32[1]+1 );
					
						m_nSimpleLen = strlen(m_strSimpleAns) + 1;
						m_nSimplePos = 0;
					break;

					case CEval::CEvalOperand::eoNone: 
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, msg) none");
						m_nSimpleLen = strlen(m_strSimpleAns) + 1;
						m_nSimplePos = 0;
					break;

					default:
						_ASSERT( 0 );
						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(text, msg) unknown");
						m_nSimpleLen = strlen(m_strSimpleAns) + 1;
						m_nSimplePos = 0;
					break;

					case CEval::CEvalOperand::eoStream:  
						m_pStream = opResult.m_Data.m_pStream;
						m_nStreamLen = m_pStream->GetSize();

						BIOS::DBG::sprintf(m_strSimpleAns, "ANS=(binary, len=%d)", m_nStreamLen);
						m_nSimpleLen = strlen(m_strSimpleAns);
						m_nSimplePos = 0;
					break;				
				}

				// tokenize the expression with ; as expression delimiter, jump to next expression
//				m_pszExpression = strstr( m_pszExpression, ";" );
//				if ( m_pszExpression )
//					m_pszExpression++;
				m_pszExpression = m_pEndPtr;

				return GetChar();	// call reursively 
			}
		}
		_ASSERT( 0 );
		return -1;
	}
};

void CMainWnd::SdkUartProc()
{
	static char buffer[128];
	static int npos = 0;
	static CSdkStreamProvider SdkStream;

	int ch;
	while ( (ch = BIOS::SERIAL::Getch()) >= 0 )
	{
		if ( Settings.Runtime.m_bUartEcho )
			BIOS::SERIAL::Putch(ch);

		if ( ch == 0x0d || ch == 0x0a )
		{
			if (npos > 0)
			{
				buffer[npos] = 0;
				// eval
				SdkStream.Evaluate( buffer );
				while ( ( ch = SdkStream.GetChar() ) != -1 )
					BIOS::SERIAL::Putch( ch );
			}
			npos = 0;
			continue;
		}
		buffer[npos++] = ch;
		if ( npos >= (int)COUNT(buffer) -1 )
		{
			npos = 0;
			m_wndMessage.Show(this, "SDK Warning", "Unrecognized command", RGB565(FF0000));
		}
	}
}

void CMainWnd::SdkDiskProc()
{
#if 0
	static FILEINFO fbase;
	static bool bInit = TRUE;
	FILEINFO f;
	static CSdkStreamProvider SdkStream;

	char* buf = (char*)BIOS::DSK::GetSharedBuffer();

	if ( bInit )
	{
		if ( BIOS::DSK::Open(&f, "SDK     BIN", BIOS::DSK::IoRead) )
		{
			fbase = f;
			m_wndMessage.Show(this, "SDK Information", "SDK File Found!", RGB565(00FF00));

			memset(buf, 0, FILEINFO::SectorSize);       	
			strcpy(buf, "ANS=(text, msg) DSO SDK Version 1.0 by Valky.eu 2012. Ready... Type 'REQ=your expression'");
			strcpy(buf+strlen(buf)+1, "END\x1b");
			_ASSERT_VALID( BIOS::DSK::Write(&f, (ui8*)buf ) );
			bInit = FALSE;
		} else
			return;
	}	

	buf[0] = 0;
	f = fbase;
	_ASSERT_VALID( BIOS::DSK::Read(&f, (ui8*)buf) );
		
	if ( memcmp(buf, "REQ=", 4) == 0 )
	{
		int nChar, nOffset = 0, nFrame = 0;
		SdkStream.Evaluate( buf+4 );
		ui8* arrSector = (ui8*)buf; //[FILEINFO::SectorSize];
		ui8 arrFirstSector[FILEINFO::SectorSize];
	
		f = fbase;
		memset( arrSector, 0, FILEINFO::SectorSize );
		while ( ( nChar = SdkStream.GetChar() ) != -1 )
		{
			arrSector[nOffset++] = (unsigned char)nChar;
			if ( nOffset >= 512 )
			{
				if ( nFrame == 0 )
				{
					/* 	
							When we are going to write multiple sectors, do not write "ANS=" as first bytes
							because the host application will understand it, that the result is already written
							in the flash... So, we will write "____" instead of "ANS=" and after writing the
							last sector, we will rewind back to the first sector of "SDK.BIN" and write the 
							first sector again with "ANS=" control sequence.
					*/
					ui8 first[4];
					memcpy( arrFirstSector, arrSector, FILEINFO::SectorSize );
					memcpy( first, arrSector, 4 );
					memcpy( arrSector, "____", 4 );
				}
		
				// write
				_ASSERT_VALID( BIOS::DSK::Write(&f, arrSector) );

				memset( arrSector, 0, FILEINFO::SectorSize );

				nFrame++;
				nOffset = 0;
			}
		}

		if ( nFrame == 0 )
		{
			_ASSERT_VALID( BIOS::DSK::Write(&f, arrSector) );
		} else
		{
			_ASSERT_VALID( BIOS::DSK::Write(&f, arrSector) );
			// rewind to beginning of the SDK.BIN file
			f = fbase;
			_ASSERT_VALID( BIOS::DSK::Write(&f, arrFirstSector) );			
		}
		
		return;
	}

	if ( memcmp(buf, "ANS=", 4) == 0 )
	{
		return;
	}

	if ( buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 )
	{
		// empty file? maybe the connector application just created it
		bInit = TRUE;
		return;
	}

	// buffer mismatch
	m_wndMessage.Show(this, "SDK Warning", "SDK File Corrupted", RGB565(FF0000));
	bInit = TRUE; 
#endif
}

