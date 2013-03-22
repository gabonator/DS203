#include "Manager.h"
#include <Source/Library/elf.h>

bool CWndUserManager::ElfGetInterpreter( char* strName, char* strInterpreter )
{
	CBufferedReader2 fw;
	if ( !fw.Open( strName ) )
		return false;

	Elf32_Ehdr elfHeader;
	Elf32_Phdr elfProgram;

	fw >> CStream(&elfHeader, sizeof(Elf32_Ehdr));

	_ASSERT( sizeof(Elf32_Phdr) == elfHeader.phentsize );

	fw.Seek( elfHeader.phoff );
	for (int i=0; i<elfHeader.phnum; i++)
	{
		fw >> CStream(&elfProgram, sizeof(Elf32_Phdr));
		if ( elfProgram.type == Elf32_PTypeInterpreter )
		{
			fw.Seek( elfProgram.offset );
			_ASSERT( elfProgram.filesz < 31 );
			fw >> CStream( strInterpreter, elfProgram.filesz );
			strInterpreter[elfProgram.filesz] = 0;
			fw.Close();
			return true;
		}
	}
	fw.Close();
	return false;
}


//ui8 pGlobal[1024];
/*
	BIOS::MEMORY::LinearStart();
	BIOS::MEMORY::LinearProgram( 0x08014000, bCode, 4 );
	BIOS::MEMORY::LinearFinish();
*/
// real: 7020 7047 7047 7047
// load: 7b20 7047 7047 7047
#define TCALL 

int TCALL test(void)
{
	return 123;
}

void CWndUserManager::ElfExecute( char* strName )
{
	typedef int (TCALL *TMain)(void);
	ui16 arrCode[] = {0x7B20, 0x7047, 0x7047, 0x7047};

	ui16* pEntry = (ui16*)&test;
	BIOS::DBG::Print("%08x = %04x %04x %04x %04x\n",
		pEntry, pEntry[0], pEntry[1], pEntry[2], pEntry[3]);
	int nRet = ((TMain) pEntry)();
	BIOS::DBG::Print("1/2 Return code=%d\n", nRet);

	pEntry = arrCode;
	BIOS::DBG::Print("%08x = %04x %04x %04x %04x\n",
		pEntry, pEntry[0], pEntry[1], pEntry[2], pEntry[3]);

	nRet = ((TMain) pEntry)();
	BIOS::DBG::Print("2/2 Return code=%d\n", nRet);

	/*
	typedef int (TCALL *TMain)(void);

	memset(pGlobal, 0x90, 1024);
	ui8* pRAM = pGlobal+128;
	ui8* pSrc = (ui8*)&test;
	memcpy( pRAM, pSrc, 1024);

	ui8* pEntry = pSrc;
	BIOS::DBG::Print("Data:%02x %02x %02x %02x.%02x %02x %02x %02x\n",
		pEntry[0], pEntry[1], pEntry[2], pEntry[3],
		pEntry[4], pEntry[5], pEntry[6], pEntry[7]);
	
	int nRet = ((TMain) pEntry)();
	BIOS::DBG::Print("1/2 Return code=%d\n", nRet);

	pEntry = pRAM;
	BIOS::DBG::Print("Data:%02x %02x %02x %02x.%02x %02x %02x %02x\n",
		pEntry[0], pEntry[1], pEntry[2], pEntry[3],
		pEntry[4], pEntry[5], pEntry[6], pEntry[7]);

	nRet = ((TMain) pEntry)();
	BIOS::DBG::Print("2/2 Return code=%d\n", nRet);
	*/
}
#if 0

	/*
		all variables used in this routine + (used by BIOS functions) must be placed 
		at memory location not colliding with loaded image! Same limitation apply to the
		code area in flash occupied by this function!
	*/
	BIOS::DBG::Print("Executing ELF image\n");
	CBufferedReader2 fw;
	if ( !fw.Open( strName ) )
		return;

	Elf32_Ehdr elfHeader;
	Elf32_Shdr elfSection;

	fw >> CStream(&elfHeader, sizeof(Elf32_Ehdr));
	int nStringTableSectionOfs = elfHeader.shoff + elfHeader.shtrndx * sizeof(Elf32_Shdr);

	fw.Seek( nStringTableSectionOfs );
	fw >> CStream(&elfSection, sizeof(Elf32_Shdr));
	int nStringTableOfs = elfSection.offset;
	int nStringTableLen = elfSection.size;

	_ASSERT( nStringTableLen < 128 );
	char strSectionNames[128];
	char* strSymbolNames = strSectionNames;
	fw.Seek(nStringTableOfs);
	fw >> CStream(strSectionNames, nStringTableLen);

	enum SecType {
		SecNone = 0,
		SecText = 1,
		SecData = 2,
		SecBss = 3,
		SecPlt = 4,
		SecGot = 5,
		SecDynStr = 6, // we need to load symbol list before DynSym
		SecDynSym = 7,
		SecRelPlt = 8, // process as last
		SecInterp = 9,
		SecStringTab = 10
	};
	const char* arrSecNames[] = {"none", ".text", ".data", ".bss", ".plt", ".got", ".dynstr", ".dynsym", ".rel.plt", ".interp", ".shstrtab"};

	int arrSectionIndex[11];
	int arrSectionOffset[11];
	for (int i=0; i<COUNT(arrSectionIndex); i++)
	{
		arrSectionIndex[i] = -1;
		arrSectionOffset[i] = -1;
	}
	fw.Seek( elfHeader.shoff );
	for (int i=0; i<elfHeader.shnum; i++)
	{
		//fw.Seek( elfHeader.shoff + i * sizeof(Elf32_Shdr) );
		fw >> CStream(&elfSection, sizeof(Elf32_Shdr));
		char* strSectionName = strSectionNames + elfSection.name;

		SecType sectionType = SecNone;
		for (int j=0; j<COUNT(arrSecNames); j++)
			if ( strcmp( strSectionName, arrSecNames[j] ) == 0 )
			{
				sectionType = (SecType)j;
				break;
			}

		_ASSERT( sectionType > 0 );
		arrSectionIndex[sectionType] = i;
		arrSectionOffset[sectionType] = elfSection.offset;

		BIOS::DBG::Print("Section%d '%s' ofs=%d addr=%08x len=%d\n", i, 
			strSectionNames+elfSection.name, elfSection.offset, elfSection.addr,
			elfSection.size);
	}

	for ( int i=0; i<(int)COUNT(arrSectionIndex); i++)
	{
		if ( arrSectionIndex[i] == -1 )
			continue;

		fw.Seek( elfHeader.shoff + arrSectionIndex[i] * sizeof(Elf32_Shdr) );
		fw >> CStream(&elfSection, sizeof(Elf32_Shdr));

		BIOS::DBG::Print("%s>", arrSecNames[i]);
		switch ( i )
		{
			case SecText:
			case SecData:
			case SecPlt:
			case SecGot:
			{
				BIOS::DBG::Print("Flashing %d bytes at %08x.", elfSection.size, elfSection.addr );
				fw.Seek( elfSection.offset );
				//LoadSection( fw, elfSection.addr, elfSection.size );
#ifndef _WIN32
				// section length should be aligned to 32bits!?
				ui8* pWriteTo = (ui8*)elfSection.addr;
				for (int i=0; i<(int)elfSection.size; i++)
				{
					ui8 bData;
					fw >> bData;
					*pWriteTo++ = bData;
				}
#endif
				break;
			}
			case SecBss:
			{
				BIOS::DBG::Print("Filling %d bytes at %08x.", elfSection.size, elfSection.addr );
#ifndef _WIN32
				// section length should be aligned to 32bits!?
				ui8* pWriteTo = (ui8*)elfSection.addr;
				for (int i=0; i<(int)elfSection.size; i++)
					*pWriteTo++ = 0;
#endif
				//ZeroSection( elfSection.addr, elfSection.size );
				break;
			}
			case SecDynStr:
			{
				_ASSERT( elfSection.size < sizeof(strSectionNames) );
				fw.Seek( elfSection.offset );
				fw >> CStream( strSymbolNames, elfSection.size );
				break;
			}
			case SecRelPlt:
			{
				BIOS::DBG::Print("Matching imports...\n");
				_ASSERT( arrSectionOffset[SecDynSym] != -1 );
				_ASSERT( arrSectionOffset[SecDynStr] != -1 );

				int nSymbolCount = elfSection.size/sizeof(Elf32_Rel);
				for (int i=0; i<nSymbolCount; i++)
				{
					Elf32_Rel elfRelocation;
					Elf32_Sym elfSymbol;

					fw.Seek( elfSection.offset + i * sizeof(Elf32_Rel) );
					fw >> CStream(&elfRelocation, sizeof(Elf32_Rel));

					_ASSERT( (elfRelocation.r_info & 0xff) == 0x16 );
					int nIndex = elfRelocation.r_info >> 8;
					
					fw.Seek( arrSectionOffset[SecDynSym] + nIndex*sizeof(Elf32_Sym) );
					fw >> CStream(&elfSymbol, sizeof(Elf32_Sym)); 
					char* strSymbolName = strSymbolNames + elfSymbol.st_name;
					ui32 dwProcAddr = BIOS::SYS::GetProcAddress( strSymbolName );
					BIOS::DBG::Print("Relocation '%s' %08x <- %08x.\n", strSymbolName, elfRelocation.r_offset, dwProcAddr);
					_ASSERT(dwProcAddr);
#ifndef _WIN32
					ui32* pRelocation = (ui32*)elfRelocation.r_offset;
					*pRelocation = dwProcAddr;
#endif
				}
			}
		}
		BIOS::DBG::Print("\n");
	}

	BIOS::DBG::Print("Finished loading\n");
	BIOS::DBG::Print("Jumping to entry %08x \n", elfHeader.entry);
#ifndef _WIN32

	ui8* pEntry = (ui8*)elfHeader.entry;

	//ui8 bCode[4] = {0x7b, 0x20, 0x70, 0x47};
	ui8 bCode[4] = {0x70, 0x20, 0x70, 0x47};
	pEntry[0] = bCode[1];
	pEntry[1] = bCode[0];
	pEntry[2] = bCode[3];
	pEntry[3] = bCode[2];

	BIOS::DBG::Print("Data:%02x %02x %02x %02x.%02x %02x %02x %02x\n",
		pEntry[0], pEntry[1], pEntry[2], pEntry[3],
		pEntry[4], pEntry[5], pEntry[6], pEntry[7]);

	pEntry = (ui8*)&test;
	BIOS::DBG::Print("Verify @ %08x \n", pEntry);
	BIOS::DBG::Print("Data:%02x %02x %02x %02x.%02x %02x %02x %02x \n",
		pEntry[0], pEntry[1], pEntry[2], pEntry[3],
		pEntry[4], pEntry[5], pEntry[6], pEntry[7]);

	ui8	btest[32];
	memcpy( btest, pEntry, 32);
	pEntry = btest;

	int nRet = ((int (*)(void)) pEntry)();
	//int nRet = ((int (*)(void)) elfHeader.entry)();
	BIOS::DBG::Print("Return code=%d\n", nRet);

#endif


//	BIOS::SYS::Execute( elfHeader.entry );
	BIOS::DBG::Print("Done.\n"); // shouldn't get here
}
#endif