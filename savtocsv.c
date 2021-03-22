#include "common.h"
#include "savtocsvlib.h"
#include "savtocsvcommon.h"

void parseOpts(int argc, char *argv[]);

bool debug = false;

int main(int argc, char *argv[]){
	
	//main parse of options before opening / reading file
		parseOpts(argc, argv);
	
	//open the file or exit
		convertToCSV(sav);
	
	return 0;
	
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
					printf("version 1.2 ");
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
			lineLimit = 1000000;
			char *lltxt = "1000000";
			printOut("CSV Line Length default: \n\t%s", lltxt, "yellow");
		}
	
}
