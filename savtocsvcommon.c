#include "common.h"
#include "savtocsvcommon.h"

bool silent = false;
bool longCsv = true;
char *sav = NULL;
char *csv = "out";
int lineLimit = 0;

char ANSI_COLOR_RED[] = "\x1b[31m";
char ANSI_COLOR_GREEN[] = "\x1b[32m";
char ANSI_COLOR_YELLOW[] = "\x1b[33m";
char ANSI_COLOR_BLUE[] = "\x1b[34m";
char ANSI_COLOR_MAGENTA[] = "\x1b[35m";
char ANSI_COLOR_CYAN[] = "\x1b[36m";
char ANSI_COLOR_RESET[] = "\x1b[0m";

/**
* Close the program with a newline
* @return void
*/
	void exitSavtocsv(){
		
		if(!silent){
			//printf("\n");
		}
		exit(EXIT_FAILURE);
	
	}

/**
* Set the colour output of the console
* @param char *col The chosen colour
* @return void
*/
	void colorSet(char *col){
		
		if (strcmp(col, "red") == 0){
			
			printf(ANSI_COLOR_RED);
				
		} else if (strcmp(col, "yellow") == 0){
			
			printf(ANSI_COLOR_YELLOW);
				
		} else if (strcmp(col, "green") == 0){
			
			printf(ANSI_COLOR_GREEN);
				
		} else if (strcmp(col, "blue") == 0){
			
			printf(ANSI_COLOR_BLUE);
				
		} else if (strcmp(col, "cyan") == 0){
			
			printf(ANSI_COLOR_CYAN);
				
		} else if (strcmp(col, "magenta") == 0){
			
			printf(ANSI_COLOR_MAGENTA);
			
		} else if (strcmp(col, "reset") == 0){
			
			printf(ANSI_COLOR_RESET);
				
		} else {
			
			printf(ANSI_COLOR_RESET);
				
		}
		
	}

/**
* Print out based on silent switch
* @param char *str The printf() message
* @param char *bound Any additional string to pass to printf()
* @param char *col The chosen colour
* @return void
*/
	void printOut(char *str, char *bound, char *col){
		
		if(!silent){
			colorSet(col);
			printf(str, bound);
			puts(ANSI_COLOR_RESET);
		}
		
	}
	
/**
* Print out to stderr based on silent switch
* @param char *str The printf() message
* @param char *bound Any additional string to pass to fprintf()
* @return void
*/
	void printOutErr(char *str, char *bound){
		
		if(!silent){
			fprintf(stderr, ANSI_COLOR_RED);
			fprintf(stderr, str, bound);
			fprintf(stderr, ANSI_COLOR_RESET);
			fprintf(stderr,"\n");
		}
		
	}
	
/**
* Get file extension
* @param char *filename The filename in question
* @return char
*/
	char *getFileExt(char *filename){
		
		char *dot = strrchr(filename, '.');
		if(!dot || dot == filename) return "";
		return dot + 1;
		
	}

/**
* Turn 32 bit int into string
* @param int num The int
* @return char*
*/
	char* intToStr32(int num){
	
		int length = snprintf( NULL, 0, "%d", num );
		char* str = malloc( length + 1 );
		snprintf( str, length + 1, "%d", num );
		return str;
		
	}
	
/**
* Turn 64 bit int into string
* @param int64_t num The int
* @return char*
*/
	char* intToStr64(int64_t num){
	
		int length = snprintf( NULL, 0, "%" PRId64, num );
		char* str = malloc( length + 1 );
		snprintf( str, length + 1, "%" PRId64, num );
		return str;
		
	}
	
/**
* Turn double into string
* @param double num The int
* @return char*
*/
	char* doubleToStr(double num){
	
		int length = snprintf( NULL, 0, "%f", num );
		char* str = malloc( length+1);
		snprintf( str, length + 1, "%f", num );
		return str;
		
	}