#include "file.h"

// File IO stuff


unsigned int endianness()
{
	unsigned int endianness;

	unsigned int tester = 1;
	char *ptr = (char*)&tester;

	if (*ptr == 1) endianness = LITTLE_ENDIAN;
	else endianness = BIG_ENDIAN;

	return endianness;
}


void openBinaryFile(char *fileName, BinaryFile *file, unsigned int endianness)
{
	file->endianness = endianness;
	file->file = std::ifstream(fileName, std::ios_base::binary);
}

void closeBinaryFile(BinaryFile *file)
{
	file->file.close();
}

void readToBuffer(char *buffer, unsigned int byteAmount, BinaryFile *file)
{
	file->file.read(buffer, byteAmount);
}

unsigned char readUByte(BinaryFile *file)
{
	unsigned char byte;

	file->file.read((char*)&byte, sizeof(unsigned char));

	return byte;
}

char readByte(BinaryFile *file)
{
	char byte;

	file->file.read(&byte, sizeof(char));

	return byte;
}


unsigned short readUWord(BinaryFile *file)
{
	unsigned short number;
	file->file.read((char*)&number, sizeof(unsigned short));

	if (endianness() != file->endianness)
	{
		unsigned short final = 0;
		unsigned short mask = 0xFF;

		final = number & mask;

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		return final;
	}

	return number;
}

short readWord(BinaryFile *file)
{
	short number;
	file->file.read((char*)&number, sizeof(short));

	if (endianness() != file->endianness)
	{
		unsigned short temp_number = *(unsigned short*)&number;


		unsigned short final = 0;
		unsigned short mask = 0xFF;

		final = temp_number & mask;

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);


		short signed_final = *(short*)&final;
		return signed_final;
	}

	return number;
}

unsigned int readUDword(BinaryFile *file)
{
	unsigned int number;
	file->file.read((char*)&number, sizeof(unsigned int));

	if (endianness() != file->endianness)
	{
		unsigned int final = 0;
		unsigned int mask = 0xFF;

		final = number & mask;

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		number >>= 8;
		final <<= 8;

		final = final | (number & mask);

		return final;
	}

	return number;
}


int readDword(BinaryFile *file)
{
	int number;
	file->file.read((char*)&number, sizeof(int));

	if (endianness() != file->endianness)
	{
		unsigned int temp_number = *(unsigned int*)&number;

		unsigned int final = 0;
		unsigned int mask = 0xFF;

		final = temp_number & mask;

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);

		temp_number >>= 8;
		final <<= 8;

		final = final | (temp_number & mask);

		int signed_final = *(int*)&final;

		return signed_final;
	}

	return number;
}