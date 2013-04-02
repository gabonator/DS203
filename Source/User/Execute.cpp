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

void Experiment()
{
	typedef int (TCALL *TMain)(void);

	ui32* pEntry = (ui32*)&test;
	BIOS::DBG::Print("%08x =%08x %08x %08x\n",
		pEntry, pEntry[0], pEntry[1], pEntry[2], pEntry[3]);

	ui32 addr = (ui32)&test;
	pEntry = (ui32*)(addr&~0xf);
	BIOS::DBG::Print("%08x=%08x %08x %08x %08x\n",
		pEntry, pEntry[0], pEntry[1], pEntry[2], pEntry[3]);

	pEntry = (ui32*)&test;
	int nRet = ((TMain) pEntry)();
	BIOS::DBG::Print("1/2 Return code=%d\n", nRet);

	ui32 myCode = 0x4770207b;
		
	BIOS::DBG::Print("%08x=%08x %08x %08x %08x\n",
		&myCode, myCode);

	ui32 dwPtr = ((ui32)&myCode) | 1;

	BIOS::DBG::Print("data=%08x code=%08x \n",
		&myCode, dwPtr );

	nRet = ((TMain) dwPtr)();
	BIOS::DBG::Print("2/2 Return code=%d\n", nRet);
}

void CWndUserManager::ElfExecute( char* strName )
{
	/*
		all variables used in this routine + (used by BIOS functions) must be placed 
		at memory location not colliding with loaded image! Same limitation apply to the
		code area in flash occupied by this function!
	*/
	BIOS::LCD::Clear(0);
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
		SecDynamic = 6,
		SecDynStr = 7, // we need to load symbol list before DynSym
		SecDynSym = 8,
		SecRelPlt = 9, // process as last
		SecInterp = 10,
		SecStringTab = 11
	};
	const char* arrSecNames[] = {"none", ".text", ".data", ".bss", ".plt", ".got", ".dynamic", ".dynstr", ".dynsym", ".rel.plt", ".interp", ".shstrtab"};

	int arrSectionIndex[COUNT(arrSecNames)];
	int arrSectionOffset[COUNT(arrSecNames)];
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
		/*
		BIOS::DBG::Print("Section%d '%s' ofs=%d addr=%08x len=%d\n", i, 
			strSectionNames+elfSection.name, elfSection.offset, elfSection.addr,
			elfSection.size);
			*/
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
			case SecDynamic:
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
					BIOS::DBG::Print("Relocation %08x <- %08x '%s'.", elfRelocation.r_offset, dwProcAddr, strSymbolName);
					if ( i < nSymbolCount-1 )
					{
						BIOS::DBG::Print("\n");
					}
					_ASSERT(dwProcAddr);
#ifndef _WIN32
					/*
					0x20000E84 (GOT[2]) -> 0x20000DAC (PLT[0])
					0x20000E84 (GOT[2]) <- new address
					*/
					ui32* pRelocation = (ui32*)elfRelocation.r_offset;
					*pRelocation = dwProcAddr;
#endif
				}
			}
		}
		BIOS::DBG::Print("\n");
	}

	BIOS::DBG::Print("Load ok. Jumping to entry %08x \n", elfHeader.entry);
	// TODO: set stack pointer!
	int nRet = BIOS::SYS::Execute( elfHeader.entry );
	BIOS::DBG::Print("Return code=%d.\n", nRet);
}
