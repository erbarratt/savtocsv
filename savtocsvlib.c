#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "savtocsvcommon.h"
#include "savtocsvlib.h"

const int RECORD_TYPE_VARIABLE = 2;
const int RECORD_TYPE_VALUE_LABELS = 3;
const int RECORD_TYPE_VALUE_LABELS_INDEX = 4;
const int RECORD_TYPE_DOCUMENTS = 6;
const int RECORD_TYPE_ADDITIONAL = 7;
const int RECORD_TYPE_FINAL = 999;

const int COMPRESS_SKIP_CODE = 0;
const int COMPRESS_END_OF_FILE = 252;
const int COMPRESS_NOT_COMPRESSED = 253;
const int COMPRESS_ALL_BLANKS = 254;
const int COMPRESS_MISSING_VALUE = 255;

//Number of bytes really stored in each segment of a very long string variable.
	const int REAL_VLS_CHUNK = 255;

FILE *savPtr;
int cursor = 0;

unsigned char wordBuffer[4];
unsigned char textBuffer[100];
int intBuffer;
double doubleBuffer;

/**
* Main runthrough for file conversion
* @return void
*/
	void convertToCSV(char *filename){
		openFile(filename);
	}

/**
* Try to open the file passed in the -f option
* @return void
*/
	void openFile(char *filename){
		
		//try to open for read in binary mode
			savPtr = fopen(filename, "rb");
		
		//file passed isn't sav file
			if(strcmp(getFileExt(filename),"sav") != 0){
				printOutErr("Unable to open file: %s", filename);
				exit(1);
			}
		
		//can't open file passed
			else if(savPtr == NULL){
				printOutErr("Unable to open file: %s", filename);
				exit(1);
			}
			
		//log
			printOut("Opened .sav file: %s", filename, "cyan");
			
		//header
			readHeader();
		
	}
	
/**
* Close file
* @return void
*/
	void closeFile(){
		fclose(savPtr);
	}
	
/**
* Read from the sav file until the start of the data blocks
* @return void
*/
	void readHeader(){
		
		if(silent == false){
			printOut("Reading file header...", "", "cyan");
		}
		
		fseek(savPtr, 0, SEEK_SET);
		
		//get file type
			readWord("File Indetifier:\n\t");
			
			if (strcmp(wordBuffer, "$FL2") != 0){
				printOutErr("File must begin with chars $FL2 for a valid SPSS .sav file. Got: %s", wordBuffer);
				exit(EXIT_FAILURE);
			}
		
		//read SPSS Version text
			readOver(60, "Header: \n\t");
		
		//layout code should be 2 or 3
			readInt("Layout Code:\n\t");
			
			if(intBuffer != 2 && intBuffer != 3){
				printOutErr("Layout code not 2 or 3 after SPSS Version Text.", "");
				exit(EXIT_FAILURE);
			}
			
		// OBS
			readInt("OBS:\n	");
			
			//@72
			
		// compression
			readInt("Compression:\n\t");
			
			//@76
			
		// weight
			readInt("Weight:\n	");
			
			//@80
			
		// cases
			readInt("Number of Cases:\n	");
			
			//@84
		
		// compression bias
			readDouble("Compression Bias:\n	");
			
			//@92
			
		// creation date
			readOver(9, "Creation Date: \n\t");
			readOver(8, "Creation Time: \n\t");
			
			//@109
			
		// file label
			readOver(64, "File Label: \n\t");
			
			//@173
			
		// padding
			readOver(3, "Padding: \n\t");
			
			//@176
		
	}
	
/**
* Read a number of bytes to move file pointer along
* @param int amt Amount of bytes
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readOver(int amt, char *msg){
		
		memset(textBuffer,0,sizeof(textBuffer));
		
		fread(textBuffer, amt, 1, savPtr);
		if(debug && !silent){
			printf(msg);
			printf("%s \n\t<%d bytes read, %d bytes total>\n\n", textBuffer, amt, ftell(savPtr));
		}
		cursor += amt;
		
	}
	
/**
* Read 4 bytes as a string
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readWord(char *msg){
		fread(wordBuffer, 4, 1, savPtr);
		if(debug && !silent){
			printf(msg);
			printf("%s \n\t<4 bytes read, %d bytes total>\n\n", wordBuffer, ftell(savPtr));
		}
		cursor += 4;
	}
	
/**
* Read 4 bytes as an int 
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readInt(char *msg){
		fread(&intBuffer, sizeof(intBuffer), 1, savPtr);
		if(debug && !silent){
			printf(msg);
			printf("%d \n\t<4 bytes read, %d bytes total>\n\n", intBuffer, ftell(savPtr));
		}
		cursor += 4;
	}
	
	
/**
* Read 8 bytes as a double
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readDouble(char *msg){
		fread(&doubleBuffer, sizeof(doubleBuffer), 1, savPtr);
		if(debug && !silent){
			printf(msg);
			printf("%d \n\t<8 bytes read, %d bytes total>\n\n", doubleBuffer, ftell(savPtr));
		}
		cursor += 4;
	}
	
	