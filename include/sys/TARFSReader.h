#ifndef TARFSREADER_H
#define TARFSREADER_H


#include <sys/TARFSReader.h>
#include "stdio.h"
#include<defs.h>
char* ReadFile(char *fileName);
int8_t IsValidFile(char *fileName);
#endif
