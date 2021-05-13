#pragma once
#include <time.h>
#include <stdio.h>
#include <string.h>
#pragma warning(disable:4996)

// measured in "BYTES" 
#define INITIAL_DISK_SIZE  16 * 1024 * 1024
#define INITIAL_BLOCK_SIZE  1 * 1024
#define INITIAL_SUPERBLOCK_SIZE  1 * 1024
#define INITIAL_BITMAP_SIZE  2 * 1024
#define INITIAL_INODE_NUMBER  61
#define INITIAL_DATA_BLOCK_NUMBER 2040 * 8
#define DISK_PATH "dist.dat"
#define NUM_INDIRECT_ADDRESSES 341
// measured in "BITS"
#define BITMAP_RESERVE_BITS 3

int fileSeek(FILE* stream, long offSet, int fromWhere);
FILE* fileOpen(const char* name, const char* mode);
size_t fileRead(void* buffer, size_t elementSize, size_t elementCount, FILE* file);
size_t fileWrite(const void* buffer, size_t elementSize, size_t elementCount, FILE* file);
int filePutCharacter(int character, FILE* file);

const char magic_number[] = "tyhrhsfs";

struct Address
{
	unsigned char addr[3];
	Address() {
		memset(addr, 0, sizeof addr);
	}
	Address(int addrInt) {
		from_int(addrInt);
	}
	int to_int() {
		return (int)addr[0] * 1 + (int)addr[1] * 256 + (int)addr[2] * 256 * 256;
	}
	void from_int(int addrInt) {
		addr[0] = addrInt % 256;
		addrInt /= 256;
		addr[1] = addrInt % 256;
		addrInt /= 256;
		addr[2] = addrInt % 256;
	}
};

class Distblock {  // can get the content from or write the content to a specific disk block
public:
	unsigned char content[1024];
	Distblock() {
		memset(content, 0, sizeof content);
	}
	Distblock(int addrInt); 
	Distblock(Address addr); 
	void load(int addrInt);// load a block from dist given an address to the content buffer
	void load(Address addr);// load a block from dist given an address to the content buffer
	void write(int addrInt);  // write the content buffer to the specific disk block
	void write(Address addr);  // write the content buffer to the specific disk block

};

class IndirectDiskblock {
public:
	Address addr[NUM_INDIRECT_ADDRESSES];  // addresses loaded from an indirect disk block
	int numAddress; // number of valid addresses since not all the 341 addr are used
	void load(Address a);  //load addresses given an indirect block address
};

class superNode
{
public:
	superNode();
	
	unsigned inodeNumber;				    // amount of inodes
	unsigned freeInodeNumber;				// amount of free inodes
	unsigned dataBlockNumber;				// amount of blocks
	unsigned freeDataBlockNumber;	        // amount of free blocks
	
	unsigned BLOCK_SIZE;
	unsigned INODE_SIZE;
	unsigned SUPERBLOCK_SIZE;
	
	int free_block_SP;					// free block stack pointer
	int *free_block;					// free block stack

	unsigned superBlockStart;
	unsigned inodeBitmapStart;
	unsigned blockBitmapStart;
	unsigned inodeStart;
	unsigned blockStart;
	
	//get inode count
	//get block count
	//get inode free
	//get block free
};

class iNode
{
public:
	unsigned short inode_id;
	unsigned short inode_link_count;
	unsigned int inode_size;
	time_t inode_change_time;
	time_t inode_access_time;
	time_t inode_modify_time;
	int direct[10];
	int indirect;

private:

};

class Disk
{
public:
	Disk();
	~Disk();
	FILE* diskFile;
	//get superNodeStart
	//get inode bitmap start
	//get inode block start
	//get data bitmap start
	//get data block start

	void run();
	void loadDisk();
	bool initializeDiskFile();


private:
	superNode super;
};

class DiskblockManager {
private:
	Address freeptr;
public:
	void initialize();  // initialize when the disk is created
	Address alloc();  // allocate a free block and return the free block address
	void free(Address addr);  // recycle the unused block and push it to the stack
};
