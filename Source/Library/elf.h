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
