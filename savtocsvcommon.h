#include <stdbool.h>

extern bool silent;
extern bool debug;
extern char *sav;
extern char *csv;
extern int lineLimit;

void exitSavtocsv();
void colorSet(char *col);
void printOut(char *str, char *bound, char *col);
void printOutErr(char *str, char *bound);
extern char *getFileExt(char *filename);
void parseOpts(int argc, char *argv[]);