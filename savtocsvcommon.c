#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "savtocsvcommon.h"

bool silent = false;
bool debug = false;
char *sav = NULL;
char *csv = "out";
int lineLimit = 0;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**
* Close the program with a newline
* @return void
*/
	void exitSavtocsv(){
		
		if(!silent){
			printf("\n");
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
		
		if(silent == false){
			colorSet(col);
			printf(str, bound);
			puts(ANSI_COLOR_RESET);
			//printf("\n");
		}
		
	}
	
/**
* Print out to stderr based on silent switch
* @param char *str The printf() message
* @param char *bound Any additional string to pass to fprintf()
* @return void
*/
	void printOutErr(char *str, char *bound){
		
		if(silent == false){
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
* Print out based on silent switch
* @param int argc Count of arguments
* @param argv *argv Array of pointers of arguments
* @return void
*/
	void parseOpts(int argc, char *argv[]){
		
		int opt;
		
		//check for version output
		// If the first character of optstring is '-', then each nonoption argv-element is handled as if 
		// it were the argument of an option with character code 1. (This is used by programs that were written to expect options and other argv-elements in any order and that care about the ordering of the two.) 
			if(argc == 2){
				
				while ((opt = getopt(argc, argv, "-v")) != -1) {
					if(opt == 'v'){
						printf(ANSI_COLOR_GREEN);
						printf("savtocsv ");
						printf(ANSI_COLOR_RESET);
						printf("version ");
						printf(ANSI_COLOR_YELLOW);
						printf("version 1.0 ");
						printf(ANSI_COLOR_RESET);
						printf("2021-03\n");
						exitSavtocsv();
					}
				}
				
			}
			
		//reset getopt index for next while loop
			optind = 1;
		
		//check for silent first
			while ((opt = getopt(argc, argv, "-sfodl")) != -1) {
				
				if(opt == 's'){
					silent = true;
				}
					
			}
			
		//reset getopt index for next while loop
			optind = 1;
		
		//ullo
			printOut("\n----------SAV To CSV----------", "", "green");
		
		//if it's not -v then is the num of args correct?
			if(argc < 2){
				printOutErr("Missing required options.", "");
				printOutErr("Usage: savtocsv [-v] | [-f] [file...] [-o] [file...] [-l] [int] [-s]", "");
				exitSavtocsv();
			}
	
		while ((opt = getopt(argc, argv, "-f:o:l:svd")) != -1) {
			
			switch (opt) {
				
				//get file pointer
					case 'f':
						sav = optarg;
						printOut("Input file set: \n\t%s", optarg, "magenta");
					break;
					
				//get output filename
					case 'o':
						csv = optarg;
					break;
					
				//silent switch for stdout
					case 's':
						silent = true;
					break;
					
				//debug switch for stdout
					case 'd':
						debug = true;
					break;
					
				//how pany lines per csv?
					case 'l':
					
						lineLimit = atoi(optarg);
						if(lineLimit == 0){
							printOutErr("-l argument must be number", optarg);
							exitSavtocsv();
						} else {
							printOut("CSV Line Length set to: \n\t%s", optarg, "magenta");
						}
						
					break;
					
				//option not in optstring
					case '?':
					
						printOutErr("Option not in option list of -f -o -l", "");
						exitSavtocsv();
						
					break;
					
			}
			
		}


		
		//check sav file option
			if(sav == NULL){
				printOutErr("Missing required option -f", "");
				exitSavtocsv();
			}

        //output csv prefix
            if(strcmp(csv, "out") == 0){
                printOut("Output file prefix default: \n\tout", "", "yellow");
            } else {
	            printOut("Output file prefix set: \n\t%s", csv, "magenta");
            }
			
		//check line limit or set default
			if(lineLimit == 0){
				lineLimit = 100000;
				char *lltxt = "100000";
				printOut("CSV Line Length default: \n\t%s", lltxt, "yellow");
			}
		
	}