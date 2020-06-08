#ifndef FILE_H
#define FILE_H

#include <fstream>


#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

struct BinaryFile
{
	std::ifstream file;
	unsigned int endianness;
};

void openBinaryFile(char *fileName, BinaryFile *file, unsigned int endianness);
void closeBinaryFile(BinaryFile *file);

void readToBuffer(char *buffer, unsigned int byteAmount, BinaryFile *file);
unsigned char readUByte(BinaryFile *file);
unsigned short readUWord(BinaryFile *file);
unsigned int readUDword(BinaryFile *file);

char readByte(BinaryFile *file);
short readWord(BinaryFile *file);
int readDword(BinaryFile *file);

#endif