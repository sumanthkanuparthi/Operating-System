#include <defs.h>
#include <sys/ELFReader.h>


/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
uint64_t Parse_ELF_Executable(char *exeFileData,
			 struct Exe_Format *exeFormat)
{
  /* Simple sanity check */
 // if (exeFileData == NULL || exeFileLength <= 0 || exeFormat == NULL)
    if (exeFileData == NULL || exeFormat == NULL)
      return -1;

  elfHeader *elf = (elfHeader *) exeFileData;

  /* Check for magic number */
  if (elf->ident[0] != 0x7f || elf->ident[1] != 'E' || 
          elf->ident[2] != 'L' || elf->ident[3] != 'F') 
      return -1;

  /* Fail if not executable */
  if (elf->type != 0x2)
      return -1;

  /* Fail if not supported version */
  if (elf->version != 0x1)
      return -1;

  /* Fail if no valid program headers */
  if (elf->phnum < 0)
      return -1;

  /* Fail if not supported architecture (Intel) */
  if (elf->machine != 0x3e)
      return -1;

/*
  if (elf->entry > exeFileLength || elf->phoff > exeFileLength ||
          elf->sphoff > exeFileLength)
      return -1;


  if (elf->ehsize != sizeof(elfHeader))
      return -1;


  if (elf->phentsize != sizeof(programHeader))
      return -1;

*/
  exeFormat->entryAddr = elf->entry;
  exeFormat->numSegments = elf->phnum;
  programHeader *program =
      (programHeader *) (exeFileData + elf->phoff);

  int i;
  for (i = 0; i < elf->phnum; i++) {

    exeFormat->segmentList[i].offsetInFile = program->offset;
    exeFormat->segmentList[i].lengthInFile = program->fileSize;
    exeFormat->segmentList[i].startAddress = program->vaddr;
    exeFormat->segmentList[i].sizeInMemory = program->memSize;
    exeFormat->segmentList[i].protFlags = program->flags;
    program++;

  }

  return (uint64_t)elf->entry;
}
