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

/** @var struct Variable Variable structure definition */
	struct Variable{
		int type;
		int measure;
		int cols;
		int alignment;
		struct Variable * next;
	};

void closeFile();
void exitAndCloseFile(char *str, char *bound);
void addVariable(struct Variable * head, int type);
void convertToCSV(char *filename);
void readHeader();
void readMeta();
void readVariable();
void readValueLabels();
void dataToCsvLong();
void dataToCsvFlat();
void readOver(size_t amt, char *msg);
void readWord(char *msg);
int8_t readIntByte(char *msg);
int8_t readIntByteNoOutput();
int readInt32(char *msg);
void readInt64(char *msg);
double readDouble(char *msg);
double readDoubleNoOuput();
