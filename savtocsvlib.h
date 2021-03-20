
void convertToCSV(char *filename);
void openFile(char *filename);
void closeFile();
void readHeader();
void readMeta();
void readVariable();
void readValueLabels();
void readOver(int amt, char *msg);
void readWord(char *msg);
int readIntByte(char *msg);
int readInt32(char *msg);
void readInt64(char *msg);
void readDouble(char *msg);