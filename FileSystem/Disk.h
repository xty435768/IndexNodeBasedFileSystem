#pragma once
#include <time.h>
#include <stdio.h>
#pragma warning(disable:4996)
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




