
void closeFile();
void exitAndCloseFile(char *str, char *bound);
void addVariable(variable_t * head, int type);
void convertToCSV(char *filename);
void readHeader();
void readMeta();
void readVariable();
void readValueLabels();
void dataToCsvLong();
void dataToCsvFlat();
void readOver(int amt, char *msg);
void readWord(char *msg);
int readIntByte(char *msg);
int readIntByteNoOutput();
int readInt32(char *msg);
void readInt64(char *msg);
double readDouble(char *msg);
double readDoubleNoOuput();
