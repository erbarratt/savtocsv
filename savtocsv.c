#include "savtocsvcommon.h"
#include "savtocsvlib.h"

int main(int argc, char *argv[]){
	
	//main parse of options before opening / reading file
		parseOpts(argc, argv);
	
	//open the file or exit
		convertToCSV(sav);
		
	//read file to validate
		
	//convert data to csvs
	
	exitSavtocsv();
	
	return 0;
	
}

