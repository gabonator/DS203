// elftest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>

#define EI_NIDENT 16

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t		Elf32_Sword;
typedef uint32_t	Elf32_Word;
typedef uint32_t	Elf32_Size;

typedef struct {
        unsigned char   ident[EI_NIDENT];
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

typedef Elf32_Ehdr elfHeader;
typedef Elf32_Phdr programHeader;

Elf32_Ehdr outHdr;
Elf32_Phdr outProg[8];
int outCount = 0;

/* Section header.  */

typedef struct
{
  Elf32_Word    name;                /* Section name (string tbl index) */
  Elf32_Word    type;                /* Section type */
  Elf32_Word    flags;               /* Section flags */
  Elf32_Addr    addr;                /* Section virtual addr at execution */
  Elf32_Off     offset;              /* Section file offset */
  Elf32_Word    size;                /* Section size in bytes */
  Elf32_Word    link;                /* Link to another section */
  Elf32_Word    info;                /* Additional section information */
  Elf32_Word    addralign;           /* Section alignment */
  Elf32_Word    entsize;             /* Entry size if section holds table */
} Elf32_Shdr;

typedef Elf32_Shdr sectionHeader;

sectionHeader secMapping;

int Parse_ELF_Executable(void* exeFileData)
{   
   //var declaration
   int i;
   
   //map an elfHeader to the exeFileData
   elfHeader* eh = (elfHeader*) exeFileData;
   //check the elf-identification
   if(eh->ident[0] != 0x7F &&
         eh->ident[1] != 'E' &&
         eh->ident[2] != 'L' &&
         eh->ident[3] != 'F')
   {
      //TODO is it necessary, that the other bytes are checked too?
      printf("\nnot an executable file\n");
      return 0;
   }
   if(eh->ident[4] == 0 ||
         eh->ident[4] == 2)
   {
      printf("\ninvalid class or 64bit object\n");
      return 0;
   }

   memcpy(&outHdr, eh, sizeof(elfHeader));
   outHdr.phnum = 0;
   outHdr.phoff = sizeof(elfHeader);
   outHdr.shnum = 0;
   outHdr.shoff = 0;
   
   //check header for correct data
   /*if(eh->type != 2)
   { //executable type - first we only implement exec files
      Print("\nunimplemented executable type: %d\n", eh->type);
      return ENOEXEC;
   }*/
   /*if(eh->machine != 2)
   { //machine type - we implement this on x86 machines
      Print("\nunsupported machine type: %d\n", eh->machine);
      return ENOEXEC;
   }*/
   
   printf("num segments = %d\n", eh->phnum);
   printf("entry addr = %08x\n", eh->entry);

   //fill in data to exeFormat
   //- set nr. of exeSegments
   //- get exeSegments
   if(eh->phoff == 0)
   {
      printf("\nno program header available\n");
      return 0;
   }
   
   outCount = 0;
   int nextProgramHeaderOffset = (eh->phoff);
   unsigned long lLastOffset = 0;
   for(i=0; i<eh->phnum; i++)
   {
      if(i >= 128)
      { //the number of segments must be less or equals the maximum numbers set for the exeFormat
         printf("\nmaximum number of segments exceeded\n");
         return 0;
      }
      
      //map the programHeader to the exeFileData
      programHeader* ph = (programHeader*) ((char*)exeFileData+nextProgramHeaderOffset);
   
	  printf("Program %d: ", i);

	  switch (ph->vaddr)
	  {
	  case 0x20003000: printf("RAM"); break;
	  case 0x0804C000: printf("ROM"); break;
	  case 0x0800C000: printf("ENTRY-SLOT1"); break;
	  case 0x08014000: printf("ENTRY-SLOT2"); break;
	  case 0x0801C000: printf("ENTRY-SLOT3"); break;
	  case 0x08024000: printf("ENTRY-SLOT4"); break;
		
	  }
	  printf("\n");
	  printf("  type %d (%s)\n", ph->type, ph->type == 1 ? "LOAD" : "?");
	  printf("  offset %d\n", ph->offset);
	  printf("  lengthInFile %d\n", ph->filesz);
	  printf("  startAddress %08x\n", ph->vaddr);
	  printf("  sizeInMemory %d\n", ph->memsz);
	  printf("  protFlags %d %c%c%c\n", ph->flags, (ph->flags & 1) ? 'X' : ' ', (ph->flags & 2) ? 'W' : ' ', (ph->flags & 4) ? 'R' : ' ');
      
	  if ( ph->offset == 0 || ph->paddr == lLastOffset )
	  {
		  printf("  - SKIPPING -\n");
	  } else {
		  memcpy(&outProg[outCount++], ph, sizeof(programHeader));
		  lLastOffset = ph->paddr; // ignore bss section
	  }
#if 0
	  int nOfs = ph->offset;
	  char strName[] = "dump0.pro";
	  strName[4] = '0' + i;
	  FILE *f = fopen(strName, "wb");
	  fwrite( (char*)exeFileData+nOfs, ph->filesz, 1, f); 
	  fclose(f);
#endif
      //get the address of the next programHeader
      nextProgramHeaderOffset += eh->phentsize;
   }
#if 0
   int nextSectionHeaderOffset = (eh->shoff);
   for(i=0; i<eh->shnum; i++)
   {
      if(i >= 128)
      { //the number of segments must be less or equals the maximum numbers set for the exeFormat
         printf("\nmaximum number of section exceeded\n");
         return 0;
      }
      
      //map the programHeader to the exeFileData
      sectionHeader* ph = (sectionHeader*) ((char*)exeFileData+nextSectionHeaderOffset);
	  if ( i== eh->shtrndx )
		memcpy(&secMapping, ph, sizeof(sectionHeader));
   
	  printf("Section %d\n", i);

	  printf("  addr 0x%08x\n", ph->addr);
	  printf("  offset %d\n", ph->offset);
	  printf("  size %d\n", ph->size);
      

	  int nOfs = ph->offset;
	  char strName[] = "dump0.sec";
	  strName[4] = 'A' + i;
	  FILE *f = fopen(strName, "wb");
	  fwrite( (char*)exeFileData+nOfs, ph->size, 1, f); 
	  fclose(f);
      //get the address of the next programHeader
      nextSectionHeaderOffset += eh->shentsize;
   }
#endif
   outHdr.phnum  = outCount;
   return 1;
}

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{
	//FILE* f = fopen("C:\\Data\\Devel\\Github\\ds203\\Bin\\APP_G251_1.elf", "rb");
	if (argc<3)
	{
		printf("error.\n");
		return 1;
	}
	FILE* f = fopen(argv[1], "rb");

	// set the file pointer to end of file	
	fseek( f, 0, SEEK_END );
	// get the file size
	int nSize= ftell( f );
	rewind(f);

	unsigned char* buf = (unsigned char*)malloc(nSize);
	fread(buf, 1, nSize, f);
	fclose(f);

	Parse_ELF_Executable(buf);

	outHdr.shnum = 0;
	outHdr.shtrndx = 0;
	/*
	int nMapOfs = secMapping.offset;
	int nMapSiz = secMapping.size;

	int nLastPrg = sizeof(Elf32_Ehdr) + outCount*sizeof(Elf32_Phdr); 
	outHdr.shoff = nLastPrg;
	nLastPrg += + sizeof(Elf32_Shdr);

	for (int i=0; i<outCount; i++)
		nLastPrg += outProg[i].filesz;
	secMapping.offset = nLastPrg;
	*/
	// realign;
	f = fopen(argv[2], "wb");
	fwrite(&outHdr, sizeof(Elf32_Ehdr), 1, f);
	int nOffset = sizeof(Elf32_Ehdr) + outCount*sizeof(Elf32_Phdr) /*+ sizeof(Elf32_Shdr)*/;
	int nOldOfs[8], nOldSize[8];
	for (int i=0; i<outCount; i++)
	{
		nOldOfs[i] = outProg[i].offset;
		nOldSize[i] = outProg[i].filesz;
		outProg[i].offset = nOffset;
		nOffset += outProg[i].filesz;
		fwrite(&outProg[i], sizeof(Elf32_Phdr), 1, f);
	}
	//fwrite(&secMapping, sizeof(secMapping), 1, f);
	for (int i=0; i<outCount; i++)
	{
		fwrite(buf + nOldOfs[i], nOldSize[i], 1, f);
	}
	//fwrite(buf + nMapOfs, nMapSiz, 1, f);

	fclose(f);

	free(buf);
	return 0;
}

