#include <Source/Core/Sdk.h>
//#include <Source/Library/ihex.h>

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

/*
bool CMainWnd::UpdateAvailable()
{
}

bool CMainWnd::UpdateApply()
{
}
*/
/*
	if ( pSender == &m_itmReset && code == ToWord('l', 'e') )
	{
		MainWnd.m_wndConfirm.Show( this, "Message", "Connect probe to ground\nReady?", RGB565(ffff00), "Yes", "No");
		return;
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Yes" )
	{
		m_proReset.m_pName = "Wait...";
		m_itmReset.Invalidate();

		m_nResetPhase = 0;
		MainWnd.m_wndConfirm.Hide();
		return;
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"No" )
	{
		MainWnd.m_wndConfirm.Hide();
		return;
	}

*/

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t		Elf32_Sword;
typedef uint32_t	Elf32_Word;
typedef uint32_t	Elf32_Size;

#pragma pack(push)
#pragma pack(2)
typedef struct {
        unsigned char   ident[16];
        Elf32_Half      type;
        Elf32_Half      machine;
        Elf32_Word      version;
        Elf32_Addr      entry;
        Elf32_Off       phoff;
        Elf32_Off       shoff;
        Elf32_Word      flags;
        Elf32_Half      ehsize;
        Elf32_Half      phentsize;
        Elf32_Half      phnum;
        Elf32_Half      shentsize;
        Elf32_Half      shnum;
        Elf32_Half      shtrndx;
} Elf32_Ehdr;

typedef struct {
	Elf32_Word	type;		/* Entry type. */
	Elf32_Off	offset;	/* File offset of contents. */
	Elf32_Addr	vaddr;	/* Virtual address in memory image. */
	Elf32_Addr	paddr;	/* Physical address (not used). */
	Elf32_Size	filesz;	/* Size of contents in file. */
	Elf32_Size	memsz;	/* Size of contents in memory. */
	Elf32_Word	flags;	/* Access permission flags. */
	Elf32_Size	align;	/* Alignment in memory and file. */
} Elf32_Phdr;
#pragma pack(pop)


void CMainWnd::SdkUartProc()
{
	static char buffer[128];
	static int npos = 0;
	static CSdkStreamProvider SdkStream;

#if 0
	static bool bApplied = false;
	static long lLastTest = 0;
	int nTick = BIOS::SYS::GetTick();
	if ( lLastTest == 0 )
		lLastTest = nTick;
	if ( !bApplied && nTick - lLastTest > 10000 )
	{
		lLastTest = nTick;

		char* pBuf = (char*)BIOS::DSK::GetSharedBuffer();
//		memclr(pBuf, 0, 512);
		strcpy(pBuf, "Toto je zaujimavy experiment 0123456789!!!!");

		BIOS::FAT::Init();
		BIOS::FAT::Open("test.txt", BIOS::DSK::IoWrite);
		BIOS::FAT::Write((ui8*)pBuf);
		BIOS::FAT::Close(strlen(pBuf));

		CBufferedReader fw;
		if ( fw.Open( (char*)"FW      HEX" ) )
		{
			bApplied = true;
			m_wndMessage.Show(this, "Update utility", "FW file found!", RGB565(FFFF00));

			IHexRecord irec;	
			uint16_t addressOffset = 0x00;
			uint32_t address = 0x0;
			int ihexError;
			int err;
			BIOS::MEMORY::LinearStart();

			while ((ihexError = Read_IHexRecord(&irec, fw)) == IHEX_OK) 
			{
				switch(irec.type)
				{
					case IHEX_TYPE_00:    /**< Data Record */
						address = (((uint32_t) addressOffset) << 16 )+ irec.address;
						err = !BIOS::MEMORY::LinearProgram( address, irec.data, irec.dataLen );

						if(err)
						{
							m_wndMessage.Show(this, "Update utility", "Flashing error!", RGB565(FF0000));
							fw.Close();
							return;
						}
					break;

					case IHEX_TYPE_04:    /**< Extended Linear Address Record */
						addressOffset = (((uint16_t) irec.data[0]) << 8 ) + irec.data[1];		
					break;

					case IHEX_TYPE_01:    /**< End of File Record */
					case IHEX_TYPE_05:    /**< Start Linear Address Record */
					break;

					case IHEX_TYPE_02:    /**< Extended Segment Address Record */
					case IHEX_TYPE_03:    /**< Start Segment Address Record */
						m_wndMessage.Show(this, "Update utility", "Invalid HEX!", RGB565(FF0000));
						fw.Close();
					return;
				}

				if ( irec.type == IHEX_TYPE_01 )
					break;
			}

			BIOS::MEMORY::LinearFinish();
			m_wndMessage.Show(this, "Update utility", "Flashing done!", RGB565(00FF00));

			fw.Close();
		}

		if ( fw.Open( (char*)"FW      ELF" ) )
		{
			bApplied = true;
			m_wndMessage.Show(this, "Update utility", "FW file found!", RGB565(FFFF00));
			Elf32_Ehdr elfHeader;
			Elf32_Phdr elfProgram[4];

			fw >> CStream(&elfHeader, sizeof(Elf32_Ehdr));

			_ASSERT( sizeof(Elf32_Phdr) == elfHeader.phentsize );
			_ASSERT( elfHeader.phnum <= (int) COUNT(elfProgram) );
			for ( int i = 0; i < elfHeader.phnum; i++ )
				fw >> CStream(&elfProgram[i], sizeof(Elf32_Phdr));

			int nFileOffset = sizeof(Elf32_Ehdr) + elfHeader.phnum * sizeof(Elf32_Phdr);

			for ( int i = 0; i < elfHeader.phnum; i++ )
			{
				if ( elfProgram[i].offset == 0 )
				{
					// first program in regular file has offset set to 0, don't know why..
					continue;
				}

				BIOS::MEMORY::LinearStart();

				ui8 buffer[32];
				while ( nFileOffset < (int)elfProgram[i].offset )
				{
					int nToRead = elfProgram[i].offset - nFileOffset;
					if ( nToRead > 32 )
						nToRead = 32;
					fw >> CStream( buffer, nToRead );
					nFileOffset += nToRead;
				}
				_ASSERT( nFileOffset == (int)elfProgram[i].offset );

				int nOfs = 0;
				int nAddress = elfProgram[i].paddr;
				for ( nOfs = 0; nOfs < (int)elfProgram[i].filesz; )
				{
					int nToRead = 32;
					if ( nOfs + nToRead > (int)elfProgram[i].filesz )
						nToRead = elfProgram[i].filesz - nOfs;
					fw >> CStream( buffer, nToRead );
					nFileOffset += nToRead;
					nOfs += nToRead;
					if ( !BIOS::MEMORY::LinearProgram( nAddress, buffer, nToRead ) )
					{
						m_wndMessage.Show(this, "Update utility", "Flashing error!", RGB565(FF0000));
						fw.Close();
						return;
					}
					nAddress += nToRead;
				}

				memset( buffer, 0, sizeof(buffer) );

				for ( ; nOfs < (int)elfProgram[i].memsz; )
				{
					int nToRead = 32;
					if ( nOfs + nToRead > (int)elfProgram[i].memsz )
						nToRead = elfProgram[i].memsz - nOfs;
					nOfs += nToRead;
					if ( !BIOS::MEMORY::LinearProgram( nAddress, buffer, nToRead ) )
					{
						m_wndMessage.Show(this, "Update utility", "Flashing error!", RGB565(FF0000));
						fw.Close();
						return;
					}
				}
				BIOS::MEMORY::LinearFinish();
			}

			m_wndMessage.Show(this, "Update utility", "Flashing done!", RGB565(00FF00));
			fw.Close();
		}
	}
#endif
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

