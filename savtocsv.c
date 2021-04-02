#include "common.h"

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
