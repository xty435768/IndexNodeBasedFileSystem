#pragma once
#include <time.h>
#include <stdio.h>
#pragma warning(disable:4996)
class superNode
{
public:
	superNode();
	
	unsigned int INODE_NUM;				    // amount of inodes
	unsigned int FREE_INODE_NUM;			// amount of free inodes
	unsigned int BLOCK_NUM;				    // amount of blocks
	unsigned int FREE_BLOCK_NUM;			// amount of free blocks
	
	static unsigned BLOCK_SIZE;
	static unsigned INODE_SIZE;
	static unsigned SUPERBLOCK_SIZE;
	
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




