#include "savtocsvlib.h"

void parseOpts(int argc, char *argv[]);

int main(int argc, char *argv[]){
	
	//main parse of options before opening / reading file
		parseOpts(argc, argv);
	
	//open the file or exit
		convertToCSV(sav);
	
	if(!silent){
		printf("\n");
	}
	
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
	
	// If the first character of optstring is '-', then each nonoption argv-element is handled as if
	// it were the argument of an option with character code 1. (This is used by programs that were written to expect options and other argv-elements in any order and that care about the ordering of the two.)
		if(argc == 2){
			
			//check for version output or help
				while ((opt = getopt(argc, argv, "-vh")) != -1) {
					
					if(opt == 'v'){
						printf("%s", ANSI_COLOR_GREEN);
						printf("savtocsv ");
						printf("%s", ANSI_COLOR_RESET);
						printf("version ");
						printf("%s", ANSI_COLOR_YELLOW);
						printf("1.9.1 ");
						printf("%s", ANSI_COLOR_RESET);
						printf("2021-03-28\n");
						exit(0);
					}
					
					else if(opt == 'h'){
						
						printOut("\n----------SAV To CSV Help----------\n", "", "green");
						
						printf("%s", ANSI_COLOR_YELLOW);
						printf("Usage:\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printOut("\tcommand [options] [arguments]\n", "", "magenta");
						
						printf("%s", ANSI_COLOR_YELLOW);
						printf("Options:\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-f\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet the input .sav filename (eg file.sav)\n");
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-o\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet the output csv prefix (appended by x.csv where x is filenumber determined by Line Limit) ");
						printf("%s", ANSI_COLOR_YELLOW);
						printf("[default: out]\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-l\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet Line Limit per csv file.  ");
						printf("%s", ANSI_COLOR_YELLOW);
						printf("[default: 1000000]\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-s\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet silent mode for no output.\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-d\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet debug mode for additional output. Will not output if Silent mode on.\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-F\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet csv output format to flat instead of long.\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-R\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tSet csv output to include row index.\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-v\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tOutput version. Must be sole option.\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("%s", ANSI_COLOR_GREEN);
						printf("\t-h\t");
						printf("%s", ANSI_COLOR_RESET);
						printf("\tOutput help. Must be sole option.\n");
						printf("%s", ANSI_COLOR_RESET);
						
						printf("\n");
						
						exit(0);
					}
					
				}
			
		}
	
	//reset getopt index for next while loop
		optind = 1;
	
	//check for silent first
		while ((opt = getopt(argc, argv, "-sfoldFR")) != -1) {
			
			if(opt == 's'){
				silent = true;
			}
			
		}
	
	//reset getopt index for next while loop
		optind = 1;
	
	//ullo
		printOut("\n----------SAV To CSV----------", "", "green");
	
	//if it's not -v or -h then is the num of args correct?
		if(argc <= 2){
			printOutErr("Missing required options.", "");
			printOutErr("Usage: savtocsv [-v] | [-f] [file...] [-o] [file...] [-l] [int] [-sdFR]", "");
			exit(EXIT_FAILURE);
		}
	
	//go through normal options
		while ((opt = getopt(argc, argv, "-f:o:l:svdFR")) != -1) {
			
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
					
				//csv file format
					case 'F':
						longCsv = false;
					break;
					
				//output row index for each row
					case 'R':
						includeRowIndex = true;
					break;
					
				//how pany lines per csv?
					case 'l':
						
						lineLimit = atoi(optarg);
						if(lineLimit == 0){
							printOutErr("-l argument must be number", optarg);
							exit(EXIT_FAILURE);
						} else {
							printOut("CSV Line Length set to: \n\t%s", optarg, "magenta");
						}
					
					break;
					
				//option not in optstring
					case '?':
					default:
						printOutErr("Option not in option list of -f -o -l", "");
						exit(EXIT_FAILURE);
				
			}
			
		}
	
	//check sav file option
		if(sav == NULL){
			printOutErr("Missing required option -f", "");
			exit(EXIT_FAILURE);
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
		
	//flat file or long file
		if(longCsv){
			printOut("CSV file format default: \n\tLong.", "", "yellow");
		} else {
			printOut("CSV file format set: \n\tFlat", "", "magenta");
		}
	
	//flat file or long file
		if(!includeRowIndex){
			printOut("CSV include row index default: \n\tFALSE.", "", "yellow");
		} else {
			printOut("CSV include row index set: \n\tTRUE", "", "magenta");
		}
	
}
