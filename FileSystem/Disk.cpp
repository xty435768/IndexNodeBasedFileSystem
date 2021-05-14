#include "Disk.h"
#include <iostream>
using namespace std;
#pragma warning(disable:4996)


int fileSeek(FILE* file, long offSet, int fromWhere, bool error_close_require)
{
	int r = fseek(file, offSet, fromWhere);
	if (r) { perror("fseek()"); if (error_close_require)fclose(file);}
	return r;
}

FILE* fileOpen(const char* name, const char* mode, bool error_close_require)
{
	FILE* file = fopen(name, mode);
	if (!file) { perror("fopen()"); if (error_close_require)fclose(file);}
	return file;
}

size_t fileRead(void* buffer, size_t elementSize, size_t elementCount, FILE* file, bool error_close_require)
{
	size_t r = fread(buffer, elementSize, elementCount, file);
	if (r != elementCount) { 
		perror("fread()"); 
		if (feof(file)) printf("EOF error!\n");
		else printf("Error code: %d\n", ferror(file));
		if (error_close_require)fclose(file);
	}
	return r;
}

size_t fileWrite(const void* buffer, size_t elementSize, size_t elementCount, FILE* file, bool error_close_require)
{
	size_t r = fwrite(buffer, elementSize, elementCount, file);
	if (r != elementCount) {
		perror("fread()");
		if (feof(file)) printf("EOF error!\n");
		else printf("Error code: %d\n", ferror(file));
		if (error_close_require)fclose(file);
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
	if (!loadDisk())
		exit(1);
	else {
		printf("Load file successful!!\n");
		printf("Block size:%d\n", super.BLOCK_SIZE);
		printf("Block number:%d\n", super.dataBlockNumber);
		dbm.printBlockUsage(&super,diskFile);
		fclose(diskFile);
	}
}


bool Disk::loadDisk()
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
		//fclose(file);
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
		super.INODE_SIZE = INITIAL_INODE_SIZE;
		//super.SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE - sizeof(magic_number) + 1;
		super.SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE;
		super.superBlockStart = sizeof(magic_number) - 1;
		super.TOTAL_SIZE = INITIAL_DISK_SIZE;
		//super.inodeBitmapStart = super.superBlockStart + super.SUPERBLOCK_SIZE;
		//super.blockBitmapStart = super.inodeBitmapStart + (BITMAP_RESERVE_BITS + INITIAL_INODE_NUMBER) / 8;
		//super.inodeStart = super.blockBitmapStart + super.dataBlockNumber / 8;
		
		super.inodeStart = INITIAL_SUPERBLOCK_SIZE;
		super.blockStart = super.inodeStart + super.inodeNumber * INITIAL_INODE_SIZE;
		if (fileSeek(file, sizeof(magic_number) - 1, SEEK_SET)) return false;
		if (fileWrite(&super, sizeof(superNode), 1, file) != 1) return false;
		dbm.initialize(&super,file);
		diskFile = file;

		//fclose(file);
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

void IndirectDiskblock::load(Address a,FILE* file)
{
	bool specify_FILE_object = file != NULL;
	int addrInt = a.to_int();
	if(!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	memset(addrs, 0, sizeof(addrs));
	fileSeek(file, addrInt, SEEK_SET);
	fileRead(addrs, 3, NUM_INDIRECT_ADDRESSES, file);
	if (!specify_FILE_object) fclose(file);
}

void IndirectDiskblock::write(Address blockAddr,FILE* file)  // still untested
{
	bool specify_FILE_object = file != NULL;
	int addrInt = blockAddr.to_int();
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	fileSeek(file, addrInt, SEEK_SET);
	fileWrite(addrs, sizeof addrs, 1, file);
	if (!specify_FILE_object) fclose(file);
}

iNode::iNode()
{

}

void DiskblockManager::initialize(superNode* super, FILE* file)
{

	freeptr = super->blockStart;
	IndirectDiskblock iblock;
	iblock.load(Address(super->blockStart),file);
	iblock.addrs[0] = freeptr;
	iblock.write(Address(super->blockStart),file);
	for (int i = super->blockStart / super->BLOCK_SIZE + 1; i < super->TOTAL_SIZE / super->BLOCK_SIZE; i++) {
		Address freeAddr = i * super->BLOCK_SIZE;
		free(freeAddr,file);
	}
}

Address DiskblockManager::alloc()
{
	// load the current linked-list block
	IndirectDiskblock iblock;
	Address blockAddr = freeptr.block_addr();
	iblock.load(blockAddr);

	// no free blocks
	if (freeptr.offset().to_int() == 0) {
		perror("out of disk memory");
		exit(1);
	}
	Address freeAddr = iblock.addrs[freeptr.offset().to_int() / 3];
	iblock.addrs[freeptr.offset().to_int() / 3].from_int(0);
	if (freeptr.offset().to_int() == 3) {
		freeptr = iblock.addrs[0] + (NUM_INDIRECT_ADDRESSES - 1) * 3;
		memset(iblock.addrs, 0, sizeof iblock.addrs);
		iblock.write(blockAddr);
	}
	else {
		iblock.write(blockAddr);
		freeptr = freeptr - 3;
	}
	return freeAddr;
}

void DiskblockManager::free(Address freeAddr, FILE* file)  // addr is the freed or afterused block address
{
	// load the current linked-list block
	IndirectDiskblock iblock;
	Address blockAddr = freeptr.block_addr();
	iblock.load(blockAddr,file);

	// if free pointer not points to the last address in the block
	if (freeptr.offset().to_int() != (NUM_INDIRECT_ADDRESSES - 1) * 3) {
		freeptr = freeptr + 3;
		iblock.addrs[freeptr.offset().to_int() / 3] = freeAddr;
		iblock.write(blockAddr,file);
	}
	else {
		Address newNodeAddr = iblock.addrs[NUM_INDIRECT_ADDRESSES - 1];  // get the last free-address in the block.
		cout << "new linked-list block" << newNodeAddr.to_int() << endl;
		iblock.load(newNodeAddr,file);  
		memset(iblock.addrs, 0, sizeof iblock.addrs);
		iblock.addrs[0] = blockAddr;
		iblock.addrs[1] = freeAddr;
		freeptr.from_int(newNodeAddr.to_int() + 3);
		iblock.write(newNodeAddr,file);
	}
}

void DiskblockManager::printBlockUsage(superNode* super,FILE* file)
{
	int freeBlock = 0, linkedListBlock= 0;
	Address ptr = freeptr.block_addr();
	IndirectDiskblock iblock;
	iblock.load(ptr,file);
	do
	{
		linkedListBlock += 1;
		ptr = iblock.addrs[0];
		iblock.load(ptr,file);
	} while (ptr != iblock.addrs[0]);
	linkedListBlock += 1;
	freeBlock = (linkedListBlock - 1) * 339 + freeptr.offset().to_int() / 3;
	cout << "free block: " << freeBlock << endl;
	cout << "blocks for linked list: " << linkedListBlock << endl;
	cout << "blocks used for data: " << (super->TOTAL_SIZE - super->SUPERBLOCK_SIZE - super->INODE_SIZE * super->inodeNumber) / super->BLOCK_SIZE
		- freeBlock - linkedListBlock << endl;
	cout << "blocks used for Inodes: " << (super->INODE_SIZE * super->inodeNumber) / super->BLOCK_SIZE << endl;
	cout << "blocks used for superblock: " << super->SUPERBLOCK_SIZE / super->BLOCK_SIZE << endl;
}
