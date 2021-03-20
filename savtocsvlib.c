#include "common.h"
#include "savtocsvlib.h"
#include "savtocsvcommon.h"


#define RECORD_TYPE_VARIABLE 2
#define RECORD_TYPE_VALUE_LABELS 3
#define RECORD_TYPE_VALUE_LABELS_INDEX 4
#define RECORD_TYPE_DOCUMENTS 6
#define RECORD_TYPE_ADDITIONAL 7
#define RECORD_TYPE_FINAL 999

#define COMPRESS_SKIP_CODE 0
#define COMPRESS_END_OF_FILE 252
#define COMPRESS_NOT_COMPRESSED 253
#define COMPRESS_ALL_BLANKS 254
#define COMPRESS_MISSING_VALUE 255

//Number of bytes really stored in each segment of a very long string variable.
#define REAL_VLS_CHUNK 255

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

/**
* Main run through for file conversion
* @return void
*/
	void convertToCSV(char *filename){
		openFile(filename);
		closeFile();
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
			printOut("Opened .sav file: \n\t%s", filename, "cyan");
			
		//header
			readHeader();
			
		//meta
			readMeta();
		
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
		
		//reset file pointer location to start
			fseek(savPtr, 0, SEEK_SET);
		
		//get file type
			readWord("File Identifier:");
			
			if (strcmp(wordBuffer, "$FL2") != 0){
				printOutErr("File must begin with chars $FL2 for a valid SPSS .sav file.", "");
				exit(EXIT_FAILURE);
			}
			
			//@4
		
		//read SPSS Version text
			readOver(60, "Header:");
			
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
			readInt32("Compression:");
			
			//@76
			
		// weight
			readInt32("Weight:");
			
			//@80
			
		// cases
			readInt32("Number of Cases:");
			
			//@84
		
		// compression bias
			readDouble("Compression Bias:");
			
			//@92
			
		// creation date
			readOver(9, "Creation Date:");
			readOver(8, "Creation Time:");
			
			//@109
			
		// file label
			readOver(64, "File Label:");
			
			//@173
			
		// padding
			readOver(3, "Padding:");
			
			//@176
		
	}
	
/**
* Read from the sav file until the start of the data blocks
* @return void
*/
	void readMeta(){
	
		bool stop = false;
		while (!stop) {
		
			printOut("-------------------------", "", "blue");
			printOut("-------------------------", "", "blue");
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
								for (int i = 0; i < numberOfLines; i++) {
									readOver(80, "Doc Content:");
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
										//readSpecificInfo(); //@32
										readOver(32, "Source system characteristics:");
									break;
									
								// SPSS Record Type 7 Subtype 4 - Source system floating pt constants
									case 4:
										//readSpecificFloatInfo(); //@24
										readOver(24, "Source system floating pt constants:");
									break;
									
								// SPSS Record Type 7 Subtype 5 - Variable sets
									case 5:
										readOver(datalen, "Variable Sets:");
									break;
									
								// SPSS Record Type 7 Subtype 6 - Trends date information
									case 6:
										readOver(datalen, "Trends Date Info:");
									break;
									
								// SPSS Record Type 7 Subtype 7 - Multi response groups
									case 7:
										readOver(datalen, "Multi Response Groups:");
									break;
								
								// SPSS Record Type 7 Subtype 11 - Variable meta SPSS bits...
									case 11:
										
										if (size != 4) {
											printOutErr("Error reading record type 7 subtype 11: bad data element length [%s]. Expecting 4.", intToStr32(size));
											exitSavtocsv();
										}
										
										if ((count % 3) != 0) {
											printOutErr("Error reading record type 7 subtype 11: number of data elements [%s] is not a multiple of 3.", intToStr32(size));
											exitSavtocsv();
										}
										
										for(int i = 1; i <= count/3; ++i){
											readInt32("Var Measure:");
											readInt32("Var Cols:");
											readInt32("Var Alignment:");
										}
										
									break;
								
								// SPSS Record Type 7 Subtype 13 - Extended names
									case 13:
										readOver(datalen, "Extended Names:");
									break;
								
								// SPSS Record Type 7 Subtype 14 - Extended strings
									case 14:
										readOver(datalen, "Extended Strings:");
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
										readOver(datalen, "Dataset Attributes:");
									break;
								
								// SPSS Record Type 7 Subtype 18 - Variable Attributes
									case 18:
										readOver(datalen, "Variable Attributes:");
									break;
								
								// SPSS Record Type 7 Subtype 19 -  Extended multiple response groups
									case 19:
										readOver(datalen, "Extended multiple response groups:");
									break;
								
								// SPSS Record Type 7 Subtype 20 -  Encoding, aka code page
									case 20:
										readOver(datalen, "Encoding, aka code page:");
									break;
								
								// SPSS Record Type 7 Subtype 21 - Extended value labels
									case 21:
										readOver(datalen, "Extended value labels:");
									break;
								
								// SPSS Record Type 7 Subtype 22 - Missing values for long strings
								
									case 22:
										readOver(datalen, "Missing values for long strings:");
									break;
									
								// SPSS Record Type 7 Subtype 23 - Sort Index information
									case 23:
										readOver(datalen, "Sort Index information:");
									break;
								
								// SPSS Record Type 7 Subtype 24 - XML info
									case 24:
										readOver(datalen, "XML info:");
									break;
								
								// Other info
									default:
										readOver(datalen, "Misc info:");
									break;
							}
							
						}
						
					break;
						
				// Finish
					case RECORD_TYPE_FINAL:
						
						stop = true;
					
						int test = readInt32("Test for final rec type:");
						
						if (test != 0) {
							//throw new \Exception("Error reading record type 999: Non-zero value found.");
						}
						
					break;
					
				default:
					// throw new 	\Exception("Read error: invalid record type [" . $recordType . "]");
				break;
			
			}
			
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
				
				// read label flag
					int hasLabel = readInt32("---Var Has Label:");
					//could throw exception here as missing label?
					
					//@8
				
				// read missing value format code
					int missingValueFormatCode = readInt32("---Missing Format Code:");
					
					if (abs(missingValueFormatCode) > 3) {
						//throw new \Exception("Error reading variable Record: invalid missing value format code [" . missingValueFormatCode . "]. Range is -3 to 3.");
					}
					
					//@12
				
				// read print format code
					readInt32("---Print Format Code:");
				
					//@16
				
				// read write format code
					readInt32("---Write Format Code:");
				
					//@20
				
				// read varname
					readOver(8, "---Var Short Name:");
				
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
							
							readOver(labelLength+rem, "---Label:");
						
					}
				
				// missing values
					if (missingValueFormatCode != 0) {
						for (int i = 0; i < abs(missingValueFormatCode); ++i) {
							readInt64("---Missing Values:");
						}
					}
				
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
			for (int i = 0; i < numberOfLabels; i++) {
	
				// read the label value
					readDouble("+++Value:");
					//@8
				
				// read the length of a value label
				// the following byte in an unsigned integer (max value is 60)
					int8_t labelLength = readIntByte("+++Label Length:");
					int8_t max = 60;
				
					if (labelLength > max) {
						//throw new    \Exception("The length of a value label({$labelLength}) must be less than 256: $!");
					}
				
				//need to ensure we read word-divisable amount of bytes
					int rem = 8-((labelLength+1) % 8);
					if(rem == 8){
						rem = 0;
					}
					
					readOver(labelLength+rem, "+++Label:");
				
				// read the label
					//readOver(labelLength, "Value Label:");
					
				// value labels are stored in chunks of 8-bytes with space allocated
				// for length+1 characters
				// --> we need to skip unused bytes in the last chunk
					//if ((labelLength + 1) % 8) {
					//	readOver(8 - ((labelLength + 1) % 8), "Value Label Skip:");
					//}
				
			}
	
		// read type 4 record (that must follow type 3!)
		// record type
			int recordTypeCode = readInt32("+++Record Type Code (Should be 4):");
			if (recordTypeCode != 4) {
				//throw new \Exception("Error reading Variable Index record: bad record type [" . $recordTypeCode . "]. Expecting Record Type 4.");
			}
	
		// number of variables to add to?
			int numberOfVariables = readInt32("+++Number of Variables:");
	
		// variableRecord indexes
			for (int i = 0; i < numberOfVariables; i++) {
				
				readInt32("+++Var Index:");
			
			}
	
	}
	
/**
* Read a number of bytes and print but not store in memory
* @param int amt Amount of bytes
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readOver(int amt, char *msg){
		
		//read amounts into temp var
			char temp[amt+1];
			fread(&temp, amt, 1, savPtr);
			cursor += amt;
		
		//output for debug info
			if(debug && !silent){
				printOut(msg, "", "yellow");
				printOut("\t%s", temp, "magenta");
				printf("\t<%d bytes read, %d bytes total>\n\n", amt, cursor);
			}
		
	}
	
/**
* Read 4 bytes as a string
* @param char *msg Message to prepend to debug output
* @return void
*/
	void readWord(char *msg){
	
		//read into mem loc of word buffer. Word buffer always 4 in length, so no need to clear
			fread(&wordBuffer, 4, 1, savPtr);
	
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
	int readIntByte(char *msg){
		
		//read 4 bytes into memory location of int32buffer
		fread(&intByteBuffer, 1, 1, savPtr);
		
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
* Read 4 bytes as an int 
* @param char *msg Message to prepend to debug output
* @return void
*/
	int readInt32(char *msg){
	
		//read 4 bytes into memory location of int32buffer
			fread(&int32Buffer, 4, 1, savPtr);
	
			cursor += 4;
		
		//if file been stored on a big endian system (as found in header), swap bytes for 32 bits (4 bytes) in the buffer
			if(bigEndian){
				__bswap_32(int32Buffer);
			}
	
		//output for debug info
			if(debug && !silent){
				printOut(msg, "", "yellow");
				printOut("\t%d", (char *)int32Buffer, "magenta");
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
	
		fread(&int64Buffer, 8, 1, savPtr);
	
		cursor += 4;
	
		//if file been stored on a big endian system (as found in header), swap bytes for 64 bits (8 bytes) in the buffer
			if(bigEndian){
				__bswap_64(int64Buffer);
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
	void readDouble(char *msg){
		
		fread(&flt64Buffer, 8, 1, savPtr);
		
		cursor += 8;
		
		//if file been stored on a big endian system (as found in header), swap bytes for 64 bits (8 bytes) in the buffer
		if(bigEndian){
			__bswap_64(flt64Buffer);
		}
		
		if(debug && !silent){
			printOut(msg, "", "yellow");
			printOut("\t%s", doubleToStr(flt64Buffer), "magenta");
			printf("\t<8 bytes read, %d bytes total>\n\n", cursor);
		}
		
		
	}
	
	