#include "savtocsvlib.h"

/** @var bool bigEndian flag to see if file was oringinally stored on Big Endian OS... */
	bool bigEndian = false;

/** @var FILE* savPtr File pointer for sav file */
	FILE* savPtr;

/** @var int cursor Internal library cursor */
	int cursor = 0;

/** @var char wordBuffer[4] Buffer for storing 4byte strings */
	char wordBuffer[4];

/** @var int8_t intByteBuffer Buffer for storing 8 bit / 1 byte ints */
	int8_t intByteBuffer;

/** @var int int32Buffer Buffer for storing 32 bit / 4 byte ints */
	int int32Buffer;

/** @var int64_t int64Buffer Buffer for storing 64 bit / 8 byte ints */
	int64_t int64Buffer;

/** @var double flt64Buffer Buffer for storing 64 bit / 8 byte floating point numbers */
	double flt64Buffer;
	
/** @var int compressionSwitch Compression on or not */
	int compressionSwitch;
	
/** @var double compressionBias Used to decode data */
	double compressionBias;
	
/** @var int numberOfCases Num cases in sav file */
	int numberOfCases = 0;
	
/** @var int numberOfVariables Num vars in sav file */
	int numberOfVariables = 0;

/** @var variable_t* variablesList Linked list of Variable structures */
	struct Variable * variablesList = NULL;

/**
* Close file
* @return void
*/
	void closeFile(){
		fclose(savPtr);
	}
	

/**
* Print error, close file and exit
* @return void
*/
	void exitAndCloseFile(char *str, char *bound){
		printOutErr(str, bound);
		closeFile();
		exit(EXIT_FAILURE);
	}
	
/**
* Add a variable to the Variables linked list
* @param variable_t * head Pointer to head of list
* @param int type Variable typecode to add one creation
* @return void
*/
	void addVariable(struct Variable * head, int type, int writeFormatCode) {
	
		struct Variable * current = head;
		
		//find last element
			while (current->next != NULL) {
				current = current->next;
			}
		
		//add new var
			current->next = (struct Variable *) malloc(sizeof(struct Variable));
			current->next->type = type;
			current->next->measure = 0;
			current->next->cols = 0;
			current->next->alignment = 0;
			current->next->writeFormatCode = writeFormatCode;
			current->next->next = NULL;
		
	}

/**
* Main run through method
* @return void
*/
	void convertToCSV(char *filename){
		
		//try to open for read in binary mode
			savPtr = fopen(filename, "rb");
			
		//file open?
			if (savPtr == NULL) {
				exitAndCloseFile("Unable to open file (permission denied, try sudo): %s", filename);
			}
		
		//file passed isn't sav file
			if(strcmp(getFileExt(filename),"sav") != 0){
				exitAndCloseFile("File not a .sav file: %s", filename);
			}
		
		//can't open file passed
			else if(savPtr == NULL){
				exitAndCloseFile("Unable to open file: %s", filename);
			}
			
		//log
			printOut("Opened .sav file: \n\t%s", filename, "cyan");
			
		//initialise linked list
			variablesList = (struct Variable*)malloc(sizeof(struct Variable));
			
			if (variablesList == NULL) {
			    exitAndCloseFile("Failed to allocate memory for variables.", "");
			}
			
			variablesList->type = 0;
			variablesList->measure = 0;
			variablesList->cols = 0;
			variablesList->alignment = 0;
			variablesList->next = NULL;
		
		//header
			readHeader();
			
		//meta
			readMeta();
			
		//data
			if(longCsv){
				dataToCsvLong();
			} else {
				dataToCsvFlat();
			}
			
		closeFile();
		
	}
	
/**
* Read from the sav file until the start of the data blocks
* @return void
*/
	void readHeader(){
		
		if(!silent){
			printOut("Reading file header:", "", "cyan");
		}
		
		//reset file pointer location to start
			fseek(savPtr, 0, SEEK_SET);
		
		//get file type
			readWord("File Identifier:");
			
			if (strcmp(wordBuffer, "$FL2") != 0){
				exitAndCloseFile("File must begin with chars $FL2 for a valid SPSS .sav file.", "");
			}
			
			//@4
		
		//read SPSS Version text
			readOver((size_t)60, "Header:");
			
			//@64
		
		//layout code should be 2 or 3
			int layout = readInt32("Layout Code:");
			if(layout != 2 && layout != 3){
				bigEndian = true;
				printOut("File stored as Big Endian found in layout code.", "", "yellow");
			}
			
			//@68
			
		// OBS
			readInt32("OBS:");
			
			//@72
			
		// compression
			compressionSwitch = readInt32("Compression:");
			
			//@76
			
		// weight
			readInt32("Weight:");
			
			//@80
			
		// cases
			numberOfCases = readInt32("Number of Cases:");
			
			//@84
		
		// compression bias
			compressionBias = readDouble("Compression Bias:");
			
			//@92
			
		// creation date
			readOver((size_t)9, "Creation Date:");
			readOver((size_t)8, "Creation Time:");
			
			//@109
			
		// file label
			readOver((size_t)64, "File Label:");
			
			//@173
			
		// padding
			readOver((size_t)3, "Padding:");
			
			//@176
			
		if(!silent){
			printOut("\t%s Cases found", intToStr32(numberOfCases), "cyan");
		}
		
	}
	
/**
* Read from the sav file until the start of the data blocks
* @return void
*/
	void readMeta(){
	
		if(!silent){
			printOut("Reading meta data:", "", "cyan");
		}
	
		bool stop = false;
		while (!stop) {
		
			if(debug){
				printOut("-------------------------", "", "blue");
				printOut("-------------------------", "", "blue");
			}
			
			int recordType = readInt32("Record type:");
			
			switch (recordType) {
				
				// Variable Record (2)
					case RECORD_TYPE_VARIABLE:
						readVariable();
					break;
						
				// Value and labels (3)
					case RECORD_TYPE_VALUE_LABELS:
						readValueLabels();
					break;
						
				// Read and parse document records (6)
					case RECORD_TYPE_DOCUMENTS:
						
						{
							// number of variables
								int numberOfLines = readInt32("Number of Docs Vars:");
							
							// read the lines
								int i;
								for (i = 0; i < numberOfLines; i++) {
									readOver((size_t)80, "Doc Content:");
								}
							
						}
						
					break;
						
				// Read and parse additional records (7)
					case RECORD_TYPE_ADDITIONAL:
					
						{
						
							int subtype = readInt32("SubType:");
							//@4
							
							int size = readInt32("Size:");
							//@8
						
							int count = readInt32("Count:");
							//@12
							
							int datalen = size * count;
							
							switch (subtype) {
								
								// SPSS Record Type 7 Subtype 3 - Source system characteristics
									case 3:
										readOver((size_t)32, "Source system characteristics:");
									break;
									
								// SPSS Record Type 7 Subtype 4 - Source system floating pt constants
									case 4:
										readOver((size_t)24, "Source system floating pt constants:");
									break;
									
								// SPSS Record Type 7 Subtype 5 - Variable sets
									case 5:
										readOver((size_t)datalen, "Variable Sets:");
									break;
									
								// SPSS Record Type 7 Subtype 6 - Trends date information
									case 6:
										readOver((size_t)datalen, "Trends Date Info:");
									break;
									
								// SPSS Record Type 7 Subtype 7 - Multi response groups
									case 7:
										readOver((size_t)datalen, "Multi Response Groups:");
									break;
								
								// SPSS Record Type 7 Subtype 11 - Variable meta SPSS bits...
									case 11:
										
										if (size != 4) {
											exitAndCloseFile("Error reading record type 7 subtype 11: bad data element length [%s]. Expecting 4.", intToStr32(size));
										}
										
										if ((count % 3) != 0) {
											exitAndCloseFile("Error reading record type 7 subtype 11: number of data elements [%s] is not a multiple of 3.", intToStr32(size));

										}
									
										//go through vars and set meta
											struct Variable * current = variablesList;
											current = current->next;
											
											int i;
											for(i = 0; i < count/3; ++i){
												
												if(debug){
													printOut("~~~Var Meta~~~", "", "magenta");
													printOut("\n~~~Var Type: %s \n", intToStr32(current->type), "yellow");
												}
												current->measure =  readInt32("~~~Var Measure:");
												current->cols =  readInt32("~~~Var Cols:");
												current->alignment =  readInt32("~~~Var Alignment:");
												
												current = current->next;
	
											}
										
									break;
								
								// SPSS Record Type 7 Subtype 13 - Extended names
									case 13:
										readOver((size_t)datalen, "Extended Names:");
									break;
								
								// SPSS Record Type 7 Subtype 14 - Extended strings
									case 14:
										readOver((size_t)datalen, "Extended Strings:");
									break;
								
								// SPSS Record Type 7 Subtype 16 - Number Of Cases
									case 16:
									
										readInt32("Byte Order:");
										readOver(4, "Skip:");
										readInt32("Count:");
										readOver(4, "Skip:");
										
									break;
								
								// SPSS Record Type 7 Subtype 17 - Dataset Attributes
									case 17:
										readOver((size_t)datalen, "Dataset Attributes:");
									break;
								
								// SPSS Record Type 7 Subtype 18 - Variable Attributes
									case 18:
										readOver((size_t)datalen, "Variable Attributes:");
									break;
								
								// SPSS Record Type 7 Subtype 19 -  Extended multiple response groups
									case 19:
										readOver((size_t)datalen, "Extended multiple response groups:");
									break;
								
								// SPSS Record Type 7 Subtype 20 -  Encoding, aka code page
									case 20:
										readOver((size_t)datalen, "Encoding, aka code page:");
									break;
								
								// SPSS Record Type 7 Subtype 21 - Extended value labels
									case 21:
										readOver((size_t)datalen, "Extended value labels:");
									break;
								
								// SPSS Record Type 7 Subtype 22 - Missing values for long strings
								
									case 22:
										readOver((size_t)datalen, "Missing values for long strings:");
									break;
									
								// SPSS Record Type 7 Subtype 23 - Sort Index information
									case 23:
										readOver((size_t)datalen, "Sort Index information:");
									break;
								
								// SPSS Record Type 7 Subtype 24 - XML info
									case 24:
										readOver((size_t)datalen, "XML info:");
									break;
								
								// Other info
									default:
										readOver((size_t)datalen, "Misc info:");
									break;
							}
							
						}
						
					break;
						
				// Finish
					case RECORD_TYPE_FINAL:
						
						stop = true;
					
						int test = readInt32("Test for final rec type:");
						
						if (test != 0) {
							exitAndCloseFile("Error reading record type 999: Non-zero value found.", "");
						}
						
					break;
					
				default:
					exitAndCloseFile("Read error: invalid record type [%s]", intToStr32(recordType));
				break;
			
			}
			
		}
		
		if(!silent){
			printOut("\t%s Variables found", intToStr32(numberOfVariables), "cyan");
		}
	
	}

/**
 * SPSS Record Type 2 - Variable information
 * @throws \Exception
 * @return void
 */
	void readVariable()
	{
		
		int typeCode = readInt32("---Var Type Code:");
		//if numeric, type code here = 0
		//if string, type code is length of string.
		
		//@4
		
		//if TYPECODE is -1, record is a continuation of a string var
			if(typeCode == -1) {
			
				//read and ignore the next 24 bytes
					readOver(24, "---String Continuation Var Skip 24:");
					
		//otherwise normal var
			} else {
				
				numberOfVariables++;
				
				// read label flag
					int hasLabel = readInt32("---Var Has Label:");
					//could throw exception here as missing label?
					
					//@8
				
				// read missing value format code
					int missingValueFormatCode = readInt32("---Missing Format Code:");
					
					if (abs(missingValueFormatCode) > 3) {
						exitAndCloseFile("Error reading variable Record: invalid missing value format code [%s]. Range is -3 to 3.", intToStr32(missingValueFormatCode));
					}
					
					//@12
				
				// read print format code
					readInt32("---Print Format Code:");
				
					//@16
				
				// read write format code
					int writeFormatCode = readInt32("---Write Format Code:");
				
					//@20
				
				// read varname
					readOver((size_t)8, "---Var Short Name:");
				
					//@28
				
				// read label length and label only if a label exists
					if (hasLabel == 1) {
						
						int labelLength = readInt32("---Label Length:");
						//@32
						
						//need to ensure we read word-divisable amount of bytes
							int rem = 4-(labelLength % 4);
							if(rem == 4){
								rem = 0;
							}
							
							readOver((size_t)labelLength, "---Label:");
							readOver((size_t)rem, "---label Skip:");
						
					}
				
				// missing values
					if (missingValueFormatCode != 0) {
						int i;
						for (i = 0; i < abs(missingValueFormatCode); ++i) {
							readInt64("---Missing Values:");
						}
					}
					
					addVariable(variablesList, typeCode, writeFormatCode);
				
			}
		
	}

/**
 * SPSS Record Type 3 - Value labels
 * @return void
 */
	void readValueLabels()
	{
	
		// number of labels
			int numberOfLabels = readInt32("+++Number of Labels:");
			//@4
	
		// labels
			int i;
			for (i = 0; i < numberOfLabels; i++) {
	
				// read the label value
					double labelValue = readDouble("+++Value:");
					//@8
				
				// read the length of a value label
				// the following byte in an unsigned integer (max value is 60)
					int8_t labelLength = readIntByte("+++Label Length:");
					//int8_t max = 60;
				
					//if (labelLength > max) {
					//	exitAndCloseFile("The length of a value label(%s) must be less than 60.", doubleToStr(labelValue));
					//}
				
				//need to ensure we read word-divisable amount of bytes
					int rem = 8-((labelLength+1) % 8);
					if(rem == 8){
						rem = 0;
					}
					
					readOver((size_t)labelLength, "+++Label:");
					readOver((size_t)rem, "+++Label Skip:");
				
			}
	
		// read type 4 record (that must follow type 3!)
		// record type
			int recordTypeCode = readInt32("+++Record Type Code (Should be 4):");
			if (recordTypeCode != 4) {
				exitAndCloseFile("Error reading Variable Index record: bad record type [%s]. Expecting Record Type 4.", intToStr32(recordTypeCode));
			}
	
		// number of variables to add to?
			int numVars = readInt32("+++Number of Variables:");
	
		// variableRecord indexes
			int j;
			for (j = 0; j < numVars; j++) {
				
				readInt32("+++Var Index:");
			
			}
	
	}
	
/**
 * Convert data to long format csv's
 * @return void
 */
	void dataToCsvLong(){
	
		//initialise counters for the loops
			int fileNumber = 1;
			int caseid = 1;
			int rowCount = 1;
		
		//cluster for compression reads
			int8_t cluster[8] = {0,0,0,0,0,0,0,0};
			int clusterIndex = 8;
		
		//how many rows overall?
			int totalRows = numberOfVariables * numberOfCases;
			
		//progress tracking
			int progressDivision;
			int progressCount = 0;
			
			if(totalRows < lineLimit){
				progressDivision = totalRows / 20;
			} else {
				progressDivision = lineLimit / 20;
			}
			
		//work out files and file pointers
			int filesAmount;
			
			if(totalRows > lineLimit){
				filesAmount = (totalRows / lineLimit) + 1;
			} else {
				filesAmount = 1;
			}
			FILE * csvs[filesAmount];
		
		//first filename
			char filename[100] = "";
			strcat(filename, csv);
			strcat(filename, intToStr32(fileNumber));
			strcat(filename, ".csv");
		
		//first file
			csvs[0] = fopen(filename, "w");
			
		//can we open and edit?
			if (csvs[0] == NULL) {
				exitAndCloseFile("Unable to open file (permission denied, try sudo): %s", filename);
			}
		
		if(!silent){
			printOut("Building Long CSV:", "", "cyan");
			printOut("\t%s", filename, "cyan");
		}
		
		double numData;
		
		int i;
		for(i = 1; i <= numberOfCases; i++){
			
			//loop through vars, skipping head of list
				struct Variable * current = variablesList;
				current = current->next;
			
				int variableId = 1;
				int j;
				for(j = 0; j < numberOfVariables; j++){
					
					if(current->type != 0){
					
						int charactersToRead = (current->writeFormatCode >> 16) & 0xFF; //byte 2
						
						double blocksToRead = floorf( (((float)charactersToRead - 1) / 8) + 1 );
						
						while (blocksToRead > 0) {
							
							if (compressionSwitch > 0) {
							
								if(clusterIndex > 7){
						
									cluster[0] = readIntByteNoOutput();
									cluster[1] = readIntByteNoOutput();
									cluster[2] = readIntByteNoOutput();
									cluster[3] = readIntByteNoOutput();
									cluster[4] = readIntByteNoOutput();
									cluster[5] = readIntByteNoOutput();
									cluster[6] = readIntByteNoOutput();
									cluster[7] = readIntByteNoOutput();
									
									clusterIndex = 0;
								
								}
								
								// convert byte to an unsigned byte in an int
									int byteValue = (0x000000FF & (int)cluster[clusterIndex]);
							
									clusterIndex++;
								
								switch (byteValue) {
									
									// skip this code
										case COMPRESS_SKIP_CODE:
									// all blanks
										case COMPRESS_ALL_BLANKS:
										break;
									
									// end of file, no more data to follow. This should not happen.
										case COMPRESS_END_OF_FILE:
											exitAndCloseFile("Error reading data: unexpected end of compressed data file (cluster code 252)", "");
										break;
										
									// data cannot be compressed, the value follows the cluster
										case COMPRESS_NOT_COMPRESSED:
										
											{
										
												// read a maximum of 8 characters but could be less if this is the last block
													size_t blockStringLength = (size_t)MIN(8, (float)charactersToRead);
													
												// append to existing value
													readOver((size_t)blockStringLength, "");
													
												// if this is the last block, skip the remaining dummy byte(s) (in the block of 8 bytes)
													if (charactersToRead < 8) {
														readOver((size_t)(8 - charactersToRead), "");
													}
												// update the characters counter
													charactersToRead -= (int)blockStringLength;
												
											}
												
										break;
									
									// system missing value
										case COMPRESS_MISSING_VALUE:
											exitAndCloseFile("Error reading data: unexpected SYSMISS for string variable", "");
										break;
									
									// 1-251 value is code minus the compression BIAS (normally always equal to 100)
										default:
											exitAndCloseFile("Error reading data: unexpected compression code for string variable", "");
										break;
								}
							
							//UNCOMPRESSED DATA
								} else {
								
									// read a maximum of 8 characters but could be less if this is the last block
										size_t blockStringLength = (size_t)MIN(8, (float)charactersToRead);
										
									// append to existing value
										readOver(blockStringLength, "");
										
									// update counter
										charactersToRead -= (int)blockStringLength;
										
								}
							
							blocksToRead--;
						
						}
						
						continue;
					
					}
					
					bool insertNull = false;
					numData = 0;
					
					if(compressionSwitch > 0){
					
						if(clusterIndex > 7){
						
							cluster[0] = readIntByteNoOutput();
							cluster[1] = readIntByteNoOutput();
							cluster[2] = readIntByteNoOutput();
							cluster[3] = readIntByteNoOutput();
							cluster[4] = readIntByteNoOutput();
							cluster[5] = readIntByteNoOutput();
							cluster[6] = readIntByteNoOutput();
							cluster[7] = readIntByteNoOutput();
							
							clusterIndex = 0;
						
						}
						
						// convert byte to an unsigned byte in an int
							int byteValue = (0x000000FF & (int)cluster[clusterIndex]);
							
							clusterIndex++;
						
						switch (byteValue) {
							
							// skip this code
								case COMPRESS_SKIP_CODE:
								break;
								
							// end of file, no more data to follow. This should not happen.
								case COMPRESS_END_OF_FILE:
									exitAndCloseFile("Error reading data: unexpected end of compressed data file (cluster code 252)", "");
								break;
								
							// data cannot be compressed, the value follows the cluster
								case COMPRESS_NOT_COMPRESSED:
									numData = readDoubleNoOuput();
								break;
								
							// all blanks
								case COMPRESS_ALL_BLANKS:
									numData = 0;
								break;
								
							// system missing value
								case COMPRESS_MISSING_VALUE:
									//used to be 'NULL' but LOAD DATA INFILE requires \N instead, otherwise a '0' get's inserted instead
									insertNull = true;
								break;
								
							// 1-251 value is code minus the compression BIAS (normally always equal to 100)
								default:
									numData = byteValue - compressionBias;
								break;
							
						}
					
					} else {
						numData = readDoubleNoOuput();
					}
					
					//write to file
					
						if(includeRowIndex){
							
							fprintf(csvs[fileNumber-1],"%d,",rowCount);
							
						}
							
						if(insertNull){
						
							fprintf(csvs[fileNumber-1],"%d,%d,\\N\n", caseid, variableId);
						
						} else if (dubIsInt(numData)) {
							
							fprintf(csvs[fileNumber-1],"%d,%d,%d\n", caseid, variableId, (int)numData);
						
						} else {
							
							fprintf(csvs[fileNumber-1],"%d,%d,%f\n", caseid, variableId, numData);
							
						}
					
					//switch to new file
					if(rowCount % lineLimit == 0){
						
						//close current file
							fclose(csvs[fileNumber-1]);
							
						//finish progressCount output
							if(progressCount < 20 && !silent){
								int x;
								for(x = 0; x <= (20 - progressCount); x++){
									printf("#");
								}
								fflush(stdout);
							}
							
							progressCount = 0;
						
						//make and open new file
							fileNumber++;
						
							char filenameHere[100] = "";
						
							strcat(filenameHere, csv);
							strcat(filenameHere, intToStr32(fileNumber));
							strcat(filenameHere, ".csv");
						
							csvs[fileNumber-1] = fopen(filenameHere,"w");
							
							if (csvs[fileNumber-1] == NULL) {
								exitAndCloseFile("Unable to open file (permission denied, try sudo): %s", filenameHere);
							}
						
						if(!silent){
							printOut("\nBuilding Long CSV:", "", "cyan");
							printOut("\t%s", filenameHere, "cyan");
						}
						
					} else if (rowCount % progressDivision == 0){
					
						if(!silent){
							printf("#");
							fflush(stdout);
						}
						
						progressCount++;
					
					}
					
					//current = current->next;
					variableId++;
					rowCount++;
				
				}
			
			caseid++;
		
		}
		
		//finish progressCount output
			if(progressCount < 20 && !silent){
				int x;
				for(x = 0; x <= (20 - progressCount); x++){
					printf("#");
				}
				fflush(stdout);
			}
		
		if(!silent){
			printOut("\nWrote %s rows.", intToStr32(totalRows), "green");
			printOut("Wrote %s files.", intToStr32(filesAmount), "green");
		}
		
		//close current file
			fclose(csvs[fileNumber-1]);
	
	}
	
/**
 * Convert data to flat format csv's
 * @return void
 */
	void dataToCsvFlat(){
	
		int fileNumber = 1;
		
		//cluster for compression reads
			int8_t cluster[8] = {0,0,0,0,0,0,0,0};
			int clusterIndex = 8;
		
		//progress tracking
			int progressDivision;
			int progressCount = 0;
			
			if(numberOfCases < lineLimit){
				progressDivision = numberOfCases / 20;
			} else {
				progressDivision = lineLimit / 20;
			}
		
		//work out files and file pointers
			int filesAmount;
			
			if(numberOfCases > lineLimit){
				filesAmount = (numberOfCases / lineLimit) + 1;
			} else {
				filesAmount = 1;
			}
			
			FILE * csvs[filesAmount];
		
		//first filename
			char filename[100] = "";
			strcat(filename, csv);
			strcat(filename, intToStr32(fileNumber));
			strcat(filename, ".csv");
		
		//first file
			csvs[0] = fopen(filename, "w");
			
		//try to open
			if (csvs[0] == NULL) {
				exitAndCloseFile("Unable to open file (permission denied, try sudo): %s", filename);
			}
		
		if(!silent){
			printOut("Building Flat CSV:", "", "cyan");
			printOut("\t%s", filename, "cyan");
		}
		
		double numData;
		
		int i;
		for(i = 1; i <= numberOfCases; i++){
			
			//loop through vars, skipping head of list
				struct Variable * current = variablesList;
				current = current->next;
				
				if(includeRowIndex){
					fprintf(csvs[fileNumber-1],"%d,", i);
				}
				
				int j;
				for(j = 0; j < numberOfVariables; j++){
					
					//current->type
					
					if(current->type != 0){
					
						int charactersToRead = (current->writeFormatCode >> 16) & 0xFF; //byte 2
						
						double blocksToRead = floorf( (((float)charactersToRead - 1) / 8) + 1 );
						
						while (blocksToRead > 0) {
							
							if (compressionSwitch > 0) {
							
								if(clusterIndex > 7){
						
									cluster[0] = readIntByteNoOutput();
									cluster[1] = readIntByteNoOutput();
									cluster[2] = readIntByteNoOutput();
									cluster[3] = readIntByteNoOutput();
									cluster[4] = readIntByteNoOutput();
									cluster[5] = readIntByteNoOutput();
									cluster[6] = readIntByteNoOutput();
									cluster[7] = readIntByteNoOutput();
									
									clusterIndex = 0;
								
								}
								
								// convert byte to an unsigned byte in an int
									int byteValue = (0x000000FF & (int)cluster[clusterIndex]);
							
									clusterIndex++;
								
								switch (byteValue) {
									
									// skip this code
										case COMPRESS_SKIP_CODE:
									// all blanks
										case COMPRESS_ALL_BLANKS:
										break;
									
									// end of file, no more data to follow. This should not happen.
										case COMPRESS_END_OF_FILE:
											exitAndCloseFile("Error reading data: unexpected end of compressed data file (cluster code 252)", "");
										break;
										
									// data cannot be compressed, the value follows the cluster
										case COMPRESS_NOT_COMPRESSED:
										
											{
										
												// read a maximum of 8 characters but could be less if this is the last block
													size_t blockStringLength = (size_t)MIN(8, (float)charactersToRead);
													
												// append to existing value
													readOver((size_t)blockStringLength, "");
													
												// if this is the last block, skip the remaining dummy byte(s) (in the block of 8 bytes)
													if (charactersToRead < 8) {
														readOver((size_t)(8 - charactersToRead), "");
													}
												// update the characters counter
													charactersToRead -= (int)blockStringLength;
												
											}
											
										break;
									
									// system missing value
										case COMPRESS_MISSING_VALUE:
											exitAndCloseFile("Error reading data: unexpected SYSMISS for string variable", "");
										break;
									
									// 1-251 value is code minus the compression BIAS (normally always equal to 100)
										default:
											exitAndCloseFile("Error reading data: unexpected compression code for string variable", "");
										break;
								}
							
							//UNCOMPRESSED DATA
								} else {
								
									// read a maximum of 8 characters but could be less if this is the last block
										size_t blockStringLength = (size_t)MIN(8, (float)charactersToRead);
										
									// append to existing value
										readOver(blockStringLength, "");
										
									// update counter
										charactersToRead -= (int)blockStringLength;
										
								}
							
							blocksToRead--;
						
						}
						
						continue;
					
					}
					
					numData = 0;
					bool insertNull = false;
					
					if(compressionSwitch > 0){
					
						if(clusterIndex > 7){
						
							cluster[0] = readIntByteNoOutput();
							cluster[1] = readIntByteNoOutput();
							cluster[2] = readIntByteNoOutput();
							cluster[3] = readIntByteNoOutput();
							cluster[4] = readIntByteNoOutput();
							cluster[5] = readIntByteNoOutput();
							cluster[6] = readIntByteNoOutput();
							cluster[7] = readIntByteNoOutput();
							
							clusterIndex = 0;
						
						}
						
						// convert byte to an unsigned byte in an int
							int byteValue = (0x000000FF & (int)cluster[clusterIndex]);
							
							clusterIndex++;
						
						switch (byteValue) {
							
							// skip this code
								case COMPRESS_SKIP_CODE:
								break;
								
							// end of file, no more data to follow. This should not happen.
								case COMPRESS_END_OF_FILE:
									exitAndCloseFile("Error reading data: unexpected end of compressed data file (cluster code 252)", "");
								break;
								
							// data cannot be compressed, the value follows the cluster
								case COMPRESS_NOT_COMPRESSED:
									numData = readDoubleNoOuput();
								break;
								
							// all blanks
								case COMPRESS_ALL_BLANKS:
									numData = 0;
								break;
								
							// system missing value
								case COMPRESS_MISSING_VALUE:
									//used to be 'NULL' but LOAD DATA INFILE requires \N instead, otherwise a '0' get's inserted instead
									insertNull = true;
								break;
								
							// 1-251 value is code minus the compression BIAS (normally always equal to 100)
								default:
									numData = byteValue - compressionBias;
								break;
							
						}
					
					} else {
						numData = readDoubleNoOuput();
					}
					
					//write to file
					
						if(j > 0){
							fprintf(csvs[fileNumber-1],",");
						}
					
						if(insertNull){
							
							fprintf(csvs[fileNumber-1],"\\N");
						
						} else if (dubIsInt(numData)) {
							
							fprintf(csvs[fileNumber-1],"%d",(int)numData);
						
						} else {
							
							fprintf(csvs[fileNumber-1],"%f",numData);
						
						}
				
				}
				
				//newline
					fprintf(csvs[fileNumber-1],"\n");
				
				//switch to new file
					if(i % lineLimit == 0){
						
						//close current file
							fclose(csvs[fileNumber-1]);
							
						//finish progressCount output
							if(progressCount < 20 && !silent){
								int x;
								for(x = 0; x <= (20 - progressCount); x++){
									printf("#");
								}
								fflush(stdout);
							}
							
							progressCount = 0;
						
						//make and open new file
							fileNumber++;
						
							char filenameHere[100] = "";
						
							strcat(filenameHere, csv);
							strcat(filenameHere, intToStr32(fileNumber));
							strcat(filenameHere, ".csv");
						
							csvs[fileNumber-1] = fopen(filenameHere,"w");
							
							if (csvs[fileNumber-1] == NULL) {
								exitAndCloseFile("Unable to open file (permission denied, try sudo): %s", filenameHere);
							}
						
						if(!silent){
							printOut("\nBuilding Flat CSV:", "", "cyan");
							printOut("\t%s", filenameHere, "cyan");
						}
						
					} else if (i % progressDivision == 0){
					
						if(!silent){
							printf("#");
							fflush(stdout);
						}
						
						progressCount++;
					
					}
		
		}
		
		//finish progressCount output
			if(progressCount < 20 && !silent){
				int x;
				for(x = 0; x <= (20 - progressCount); x++){
					printf("#");
				}
				fflush(stdout);
			}
		
		if(!silent){
			printOut("\nWrote %s rows.", intToStr32(numberOfCases), "green");
			printOut("Wrote %s files.", intToStr32(filesAmount), "green");
		}
		
		//close current file
			fclose(csvs[fileNumber-1]);
	
	}
	
/**
* Read a number of bytes and print but not store in memory
* @param int amt Amount of bytes
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readOver(size_t amt, char *msg){
			
		//only initialise with blank data if debug, otherwise doesn't matter.
			if(debug){
				
				//read amounts into temp var
				char temp[amt+1];
			
				int i;
				for(i = 0; i < (int)amt; i++){
					temp[i] = ' ';
				}
				
				temp[amt] = '\0';
				
				size_t read = fread(&temp, (size_t)amt, 1, savPtr);
				
				if(read != amt){
					exitAndCloseFile("Failed to read %d bytes in readOver()", intToStr32((int)amt));
				}
				
				if(!silent){
					printOut(msg, "", "yellow");
					printOut("\t%s", temp, "magenta");
					printf("\t<%d bytes read, %d bytes total>\n\n", (int)amt, cursor);
				}
				
			} else {
				fseek(savPtr, (int)amt, SEEK_CUR);
			}
			
		cursor += (int)amt;
		
	}
	
/**
* Read 4 bytes as a string
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readWord(char *msg){
	
		//read into mem loc of word buffer. Word buffer always 4 in length, so no need to clear
			size_t read = fread(&wordBuffer, 4, 1, savPtr);
			
			if(read != 1){
				exitAndCloseFile("Failed to read 4 bytes in readWord()", "");
			}
	
			cursor += 4;
	
		//output for debug info
			if(debug && !silent){
				printOut(msg, "", "yellow");
				printOut("\t%s", wordBuffer, "magenta");
				printf("\t<4 bytes read, %d bytes total>\n\n", cursor);
			}
			
	}

/**
* Read 1 byte as an int
* @param char *msg Message to prepend to debug output
* @return void
*/
	int8_t readIntByte(char *msg){
		
		//read 4 bytes into memory location of int32buffer
		size_t read = fread(&intByteBuffer, 1, 1, savPtr);
		
		if(read != 1){
			exitAndCloseFile("Failed to read 1 bytes in readIntByte()", "");
		}
		
		cursor += 1;
		
		//output for debug info
		if(debug && !silent){
			printOut(msg, "", "yellow");
			printf("\t%d\n", intByteBuffer);
			printf("\t<1 byte read, %d bytes total>\n\n", cursor);
		}
		
		return intByteBuffer;
		
	}
	
/**
* Read 1 byte as an int
* @return void
*/
	int8_t readIntByteNoOutput(){
		
		//read 4 bytes into memory location of int32buffer
		size_t read = fread(&intByteBuffer, 1, 1, savPtr);
		
		if(read != 1){
			exitAndCloseFile("Failed to read 1 bytes in readIntByteNoOutput()", "");
		}
		
		cursor += 1;
		
		return intByteBuffer;
		
	}
	
/**
* Read 4 bytes as an int 
* @param char *msg Message to prepend to debug output
* @return void
*/
	int readInt32(char *msg){
	
		//read 4 bytes into memory location of int32buffer
			size_t read = fread(&int32Buffer, 4, 1, savPtr);
	
			if(read != 1){
				exitAndCloseFile("Failed to read 4 bytes in readInt32()", "");
			}
	
			cursor += 4;
		
		//if file been stored on a big endian system (as found in header), swap bytes for 32 bits (4 bytes) in the buffer
			if(bigEndian){
				bswap_32(int32Buffer);
			}
	
		//output for debug info
			if(debug && !silent){
				printOut(msg, "", "yellow");
				printOut("\t%s", intToStr32(int32Buffer), "magenta");
				printf("\t<4 bytes read, %d bytes total>\n\n", cursor);
			}
		
		return int32Buffer;
		
	}
	
/**
* Read 8 bytes as a double
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readInt64(char *msg){
	
		size_t read = fread(&int64Buffer, 8, 1, savPtr);
		
		if(read != 1){
			exitAndCloseFile("Failed to read 8 bytes in readInt64()", "");
		}
	
		cursor += 8;
	
		//if file been stored on a big endian system (as found in header), swap bytes for 64 bits (8 bytes) in the buffer
			if(bigEndian){
				bswap_64(int64Buffer);
			}
		
		if(debug && !silent){
			printOut(msg, "", "yellow");
			printOut("\t%s", intToStr64(int64Buffer), "magenta");
			printf("\t<8 bytes read, %d bytes total>\n\n", cursor);
		}
		
		
	}
	
/**
* Read 8 bytes as a double
* @param char *msg Message to prepend to debug output
* @return void
*/
	double readDouble(char *msg){
		
		size_t read = fread(&flt64Buffer, 8, 1, savPtr);
		
		if(read != 1){
			exitAndCloseFile("Failed to read 8 bytes in readDouble()", "");
		}
		
		cursor += 8;
		
		//if file been stored on a big endian system (as found in header), swap bytes for 64 bits (8 bytes) in the buffer
		if(bigEndian){
			bswap_64(flt64Buffer);
		}
		
		if(debug && !silent){
			printOut(msg, "", "yellow");
			printOut("\t%s", doubleToStr(flt64Buffer), "magenta");
			printf("\t<8 bytes read, %d bytes total>\n\n", cursor);
		}
		
		return flt64Buffer;
		
	}
	
/**
* Read 8 bytes as a double
* @return void
*/
	double readDoubleNoOuput(){
		
		size_t read = fread(&flt64Buffer, 8, 1, savPtr);
		
		if(read != 1){
			exitAndCloseFile("Failed to read 8 bytes in readDoubleNoOutput()", "");
		}
		
		cursor += 8;
		
		//if file been stored on a big endian system (as found in header), swap bytes for 64 bits (8 bytes) in the buffer
		if(bigEndian){
			bswap_64(flt64Buffer);
		}
		
		return flt64Buffer;
		
	}
	
	
