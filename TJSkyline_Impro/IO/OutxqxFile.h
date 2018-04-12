#ifndef out_x
#define out_x

#include"../xqx/transxqx.h"
#include <stdarg.h>
void OutxqxFile(char *filename);
void outFile(char *filename);
int outFILEIn(char *filename, int NowrecordNum, int LastrecordNum);

int TimetoIndex(char *Tx);

void printfRecord(struct solution *Nsol, int i);
int write_log (FILE* pFile, int timet, const char *format, ...);
extern FILE *LogFile;
#endif // out_x


