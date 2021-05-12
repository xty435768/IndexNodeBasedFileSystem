#include "Disk.h"
#include <iostream>
using namespace std;
#pragma warning(disable:4996)

// all sizes are measured in "BYTES" 
#define INITIAL_DISK_SIZE  16 * 1024 * 1024
#define INITIAL_BLOCK_SIZE  1 * 1024
#define INITIAL_SUPERBLOCK_SIZE  1 * 1024
#define INITIAL_BITMAP_SIZE  2 * 1024
#define INITIAL_INODE_NUMBER  61

const char magic_number[] = "tyhrhsfs";

int fileSeek(FILE* stream, long offSet, int fromWhere)
{
	int r = fseek(stream, offSet, fromWhere);
	if (r) { perror("fseek()");  }
	return r;
}

FILE* fileOpen(const char* name, const char* mode) 
{
	FILE* file = fopen(name, mode);
	if (!file) { perror("fopen()");}
	return file;
}

size_t fileRead(void* buffer, size_t elementSize, size_t elementCount, FILE* file)
{
	size_t r = fread(buffer, elementSize, elementCount, file);
	if (r != elementCount) { 
		perror("fread()"); 
		if (feof(file)) printf("EOF error!\n");
		else printf("Error code: %d\n", ferror(file));
	}
	return r;
}

size_t fileWrite(const void* buffer, size_t elementSize, size_t elementCount, FILE* file)
{
	size_t r = fwrite(buffer, elementSize, elementCount, file);
	if (r != elementCount) {
		perror("fread()");
		if (feof(file)) printf("EOF error!\n");
		else printf("Error code: %d\n", ferror(file));
	}
	return r;
}

int filePutCharacter(int character, FILE *file)
{
	int r = fputc(0, file);
	if (r == -1)
	{
		perror("fputc()");
		printf("EOF Error! Code: %d\n", ferror(file));
	}
	return r;
}

Disk::Disk()
{

}

Disk::~Disk()
{

}

void Disk::run()
{
	loadDisk();
}

void Disk::loadDisk()
{
	if(!initializeDiskFile())
		exit(1);
	else {
		printf("It works!\n");
	}
}

bool Disk::initializeDiskFile()
{
	FILE* file = fileOpen("disk.dat", "rb+");
	if (file)
	{
		cout << "Disk file found!" << endl;
		if (fileSeek(file, 0, SEEK_SET)) return false;
		char* magic_number_test = new char[sizeof(magic_number)];
		if (fileRead(magic_number_test, sizeof(char), strlen(magic_number), file) != strlen(magic_number)) 
			return false;
		magic_number_test[sizeof(magic_number) - 1] = '\0';
		return !strcmp(magic_number_test, magic_number);
	}
	else
	{
		cout << "Disk file not found! Initial a new file!" << endl;
		file = fileOpen("disk.dat", "w");
		if (fileSeek(file, INITIAL_DISK_SIZE - 1, SEEK_CUR)) return false;
		if (filePutCharacter(0, file)) return false;
		cout << "Create file successful!" << endl;
		fclose(file);
		file = fileOpen("disk.dat", "rb+");
		if (fileSeek(file, 0, SEEK_SET)) return false;
		if (fileWrite(magic_number,sizeof(magic_number),1,file) != 1) return false;



		return true;
	}
}

superNode::superNode()
{

}
