#include "Disk.h"
#include <iostream>
using namespace std;
#pragma warning(disable:4996)


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
		printf("Load file successful!!\n");
		printf("Block size:%d\n", super.BLOCK_SIZE);
		printf("Block number:%d\n", super.dataBlockNumber);
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
		if (strcmp(magic_number_test, magic_number))
		{
			cout << "Invalid file!!" << endl;
			return false;
		}
		if (fileSeek(file, sizeof(magic_number) - 1, SEEK_SET)) return false;
		if (fileRead(&super, sizeof(superNode), 1, file) != 1)
		{
			fclose(file);
			return false;
		}
		diskFile = file;
		delete[] magic_number_test;
		return true;
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

		super.inodeNumber = INITIAL_INODE_NUMBER;
		super.freeInodeNumber = INITIAL_INODE_NUMBER;
		super.dataBlockNumber = INITIAL_DATA_BLOCK_NUMBER;
		super.freeDataBlockNumber = INITIAL_DATA_BLOCK_NUMBER;
		super.BLOCK_SIZE = INITIAL_BLOCK_SIZE;
		super.INODE_SIZE = INITIAL_BLOCK_SIZE;
		super.SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE - sizeof(magic_number) + 1;
		super.superBlockStart = sizeof(magic_number) - 1;
		super.inodeBitmapStart = super.superBlockStart + super.SUPERBLOCK_SIZE;
		super.blockBitmapStart = super.inodeBitmapStart + (BITMAP_RESERVE_BITS + INITIAL_INODE_NUMBER) / 8;
		super.inodeStart = super.blockBitmapStart + super.dataBlockNumber / 8;
		super.blockStart = super.inodeStart + super.inodeNumber * super.BLOCK_SIZE;
		if (fileSeek(file, sizeof(magic_number) - 1, SEEK_SET)) return false;
		if (fileWrite(&super, sizeof(superNode), 1, file) != 1) return false;
		return true;
	}
}

superNode::superNode()
{

}


Diskblock::Diskblock(int addrInt)
{
	load(addrInt);
}

Diskblock::Diskblock(Address addr)
{
	load(addr);
}

void Diskblock::load(int addrInt)
{
	FILE* file = fileOpen(DISK_PATH, "rb+");
	fileSeek(file, addrInt, SEEK_SET);
	fileRead(content, sizeof content, 1, file);
	fclose(file);
}

void Diskblock::load(Address addr)
{
	int addrInt = addr.to_int();
	load(addrInt);
}

void Diskblock::write(int addrInt)
{
	FILE* file = fileOpen(DISK_PATH, "rb+");
	fileSeek(file, addrInt, SEEK_SET);
	fileWrite(content, sizeof content, 1, file);
	fclose(file);
}

void Diskblock::write(Address addr)
{
	int addrInt = addr.to_int();
	write(addrInt);
}

void IndirectDiskblock::load(Address a)
{
	int addrInt = a.to_int();
	FILE* file = fileOpen(DISK_PATH, "rb+");
	memset(addr, 0, sizeof(addr));
	fileSeek(file, addrInt, SEEK_SET);
	fileRead(addr, 3, NUM_INDIRECT_ADDRESSES, file);
	fclose(file);
}

iNode::iNode()
{

}