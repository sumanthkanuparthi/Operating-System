
#ifndef ELFREADER_H
#define ELFREADER_H

#include <defs.h>


/*
 * ELF header at the beginning of the executable.
 */


typedef uint64_t elf_addr;
typedef uint64_t elf_off;
typedef uint16_t elf_half;
typedef uint32_t elf_word;
typedef int32_t  elf_sword;
typedef uint64_t elf_size;
typedef uint64_t elf_xword;
typedef int64_t  elf_sxword;


typedef struct {
    unsigned  char	ident[16];
    elf_half	type;
    elf_half	machine;
    elf_word	version;
    elf_addr	entry;
    elf_off	phoff;
    elf_off	sphoff;
    elf_word	flags;
    elf_half	ehsize;
    elf_half	phentsize;
    elf_half	phnum;
    elf_half	shentsize;
    elf_half	shnum;
    elf_half	shstrndx;
} elfHeader;


typedef struct {
        unsigned char   e_ident[32];
        elf_half      e_type;
        elf_half      e_machine;
        elf_word     e_version;
        elf_addr      e_entry;
        elf_off       e_phoff;
        elf_off       e_shoff;
        elf_word      e_flags;
        elf_half      e_ehsize;
        elf_half      e_phentsize;
        elf_half      e_phnum;
        elf_half      e_shentsize;
        elf_half      e_shnum;
        elf_half      e_shtrndx;
} Elf64_Ehdr;


/*
 * An entry in the ELF program header table.
 * This describes a single segment of the executable.
 */

/*
typedef struct {
    Elf64_Word   type;
    unsigned  int   offset;
    unsigned  int   vaddr;
    unsigned  int   paddr;
    unsigned  int   fileSize;
    unsigned  int   memSize;
    unsigned  int   flags;
    unsigned  int   alignment;
} programHeader;

*/
typedef struct
{
	elf_word  type; /* Type of segment */
	elf_word  flags; /* Segment attributes */
	elf_off offset; /* Offset in file */
	elf_addr vaddr; /* Virtual address in memory */
	elf_addr paddr; /* Reserved */
	elf_xword fileSize; /* Size of segment in file */
	elf_xword memSize; /* Size of segment in memory */
	elf_xword alignment; /* Alignment of segment */
} programHeader;


/*
 * Bits in flags field of programHeader.
 * These describe memory permissions required by the segment.
 */
#define PF_R	0x4	 /* Pages of segment are readable. */
#define PF_W	0x2	 /* Pages of segment are writable. */
#define PF_X	0x1	 /* Pages of segment are executable. */

/*
 * A segment of an executable.
 * It specifies a region of the executable file to be loaded
 * into memory.
 */
struct Exe_Segment {
    uint64_t offsetInFile;	 /* Offset of segment in executable file */
    uint64_t lengthInFile;	 /* Length of segment data in executable file */
    uint64_t startAddress;	 /* Start address of segment in user memory */
    uint64_t sizeInMemory;	 /* Size of segment in memory */
    int protFlags;		 /* VM protection flags; combination of VM_READ,VM_WRITE,VM_EXEC */
};

/*
 * Maximum number of executable segments we allow.
 * Normally, we only need a code segment and a data segment.
 * Recent versions of gcc (3.2.3) seem to produce 3 segments.
 */
#define EXE_MAX_SEGMENTS 3

/*
 * A struct concisely representing all information needed to
 * load an execute an executable.
 */
struct Exe_Format {
    struct Exe_Segment segmentList[EXE_MAX_SEGMENTS]; /* Definition of segments */
    int numSegments;		/* Number of segments contained in the executable */
    uint64_t entryAddr;	 	/* Code entry point address */
};

uint64_t Parse_ELF_Executable(char *exeFileData,
			 struct Exe_Format *exeFormat);

#endif  

