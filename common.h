#ifndef SAVTOCSV_H

#define SAVTOCSV_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include <stdint.h>
	
	#define MIN(a,b) (((a)<(b))?(a):(b))
	
	#ifdef _MSC_VER
	
		#define bswap_32(x) _byteswap_ulong(x)
		#define bswap_64(x) _byteswap_uint64(x)
	
	#elif defined(__APPLE__)
	
		// Mac OS X / Darwin features
		#include <libkern/OSByteOrder.h>
		#define bswap_32(x) OSSwapInt32(x)
		#define bswap_64(x) OSSwapInt64(x)
	
	#elif defined(__sun) || defined(sun)
	
		#include <sys/byteorder.h>
		#define bswap_32(x) BSWAP_32(x)
		#define bswap_64(x) BSWAP_64(x)
	
	#elif defined(__FreeBSD__)
	
		#include <sys/endian.h>
		#define bswap_32(x) bswap32(x)
		#define bswap_64(x) bswap64(x)
	
	#elif defined(__OpenBSD__)
	
		#include <sys/types.h>
		#define bswap_32(x) swap32(x)
		#define bswap_64(x) swap64(x)
	
	#elif defined(__NetBSD__)
	
		#include <sys/types.h>
		#include <machine/bswap.h>
		#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
		#define bswap_32(x) bswap32(x)
		#define bswap_64(x) bswap64(x)
		#endif
	
	#else
	
		#include <byteswap.h>
	
	#endif

	//common
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
		char *getFileExt(char *filename);
		bool dubIsInt(double val);
		char* intToStr8(int8_t num);
		char* intToStr32(int num);
		char* intToStr64(int64_t num);
		char* doubleToStr(double num);
		void parseOpts(int argc, char *argv[]);
	
	//lib
	
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
		
		/** @var struct Variable Variable structure definition */
			struct Variable{
				int type;
				int measure;
				int cols;
				int alignment;
				int writeFormatCode;
				struct Variable * next;
			};
		
		void closeFile();
		void exitAndCloseFile(char *str, char *bound);
		void addVariable(struct Variable * head, int type, int writeFormatCode);
		void convertToCSV(char *filename);
		void readHeader();
		void readMeta();
		void readVariable();
		void readValueLabels();
		void dataToCsvLong();
		void dataToCsvFlat();
		void readOver(size_t amt, char *msg);
		void readWord(char *msg);
		uint8_t readIntByte(char *msg);
		int8_t readIntBytes(char *msg);
		uint8_t readIntByteNoOutput();
		int8_t readIntByteSNoOutput();
		int readInt32(char *msg);
		void readInt64(char *msg);
		double readDouble(char *msg);
		double readDoubleNoOuput();

#endif
