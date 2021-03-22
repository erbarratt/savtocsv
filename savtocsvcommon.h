#include "common.h"

extern bool debug;
extern bool silent;
extern bool longCsv;
extern bool includeRowIndex;
extern char *sav;
extern char *csv;
extern int lineLimit;

extern char ANSI_COLOR_RED[];
extern char ANSI_COLOR_GREEN[];
extern char ANSI_COLOR_YELLOW[];
extern char ANSI_COLOR_BLUE[];
extern char ANSI_COLOR_MAGENTA[];
extern char ANSI_COLOR_CYAN[];
extern char ANSI_COLOR_RESET[];

void colorSet(char *col);
void printOut(char *str, char *bound, char *col);
void printOutErr(char *str, char *bound);
extern char *getFileExt(char *filename);
char* intToStr8(int8_t num);
char* intToStr32(int num);
char* intToStr64(int64_t num);
char* doubleToStr(double num);
