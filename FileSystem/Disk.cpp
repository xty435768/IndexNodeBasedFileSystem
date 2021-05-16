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
		printf("Free block number:%d\n", super.freeDataBlockNumber);
		printf("Inode number:%d\n", super.inodeNumber);
		printf("Free inode number:%d\n", super.freeInodeNumber);
		dbm.printBlockUsage(&super,diskFile);
		printf("Loading root inode...");
		if (setCurrentInode(0)) {
			printf("Successful!\n");
			printf("Root inode id:%d\n", currentInode.inode_id);
			printf("Root inode create time:%s", ctime((time_t const *)&(currentInode.inode_create_time)));
			printf("Root inode access time:%s", ctime((time_t const*)&(currentInode.inode_access_time)));
			printf("Root inode modify time:%s", ctime((time_t const*)&(currentInode.inode_modify_time)));
			printf("Root file size:%d\n", currentInode.fileSize);
			printf("Root file start at (direct block addresss):%d\n", currentInode.direct[0].to_int());
			Directory root = readFileEntriesFromDirectoryFile(currentInode);
			for (size_t i = 0; i < root.files.size(); i++)
			{
				printf("%s\t%d\n", root.files[i].fileName, root.files[i].inode_id);
			}
		}
		else {
			printf("Failed!\n");
			exit(4);
		}
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
		if (fileRead(&super, sizeof(superBlock), 1, file) != 1)
		{
			fclose(file);
			return false;
		}
		diskFile = file;
		dbm.freeptr = super.freeptr;
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

		//super.inodeNumber = INITIAL_INODE_NUMBER;
		//super.freeInodeNumber = INITIAL_INODE_NUMBER;
		//super.dataBlockNumber = INITIAL_DATA_BLOCK_NUMBER;
		//super.freeDataBlockNumber = INITIAL_DATA_BLOCK_NUMBER;
		//super.BLOCK_SIZE = INITIAL_BLOCK_SIZE;
		//super.INODE_SIZE = INITIAL_INODE_SIZE;
		////super.SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE - sizeof(magic_number) + 1;
		//super.SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE;
		//super.superBlockStart = sizeof(magic_number) - 1;
		//super.TOTAL_SIZE = INITIAL_DISK_SIZE;
		////super.inodeBitmapStart = super.superBlockStart + super.SUPERBLOCK_SIZE;
		////super.blockBitmapStart = super.inodeBitmapStart + (BITMAP_RESERVE_BITS + INITIAL_INODE_NUMBER) / 8;
		////super.inodeStart = super.blockBitmapStart + super.dataBlockNumber / 8;
		//
		//super.inodeStart = INITIAL_SUPERBLOCK_SIZE;
		//super.blockStart = super.inodeStart + super.inodeNumber * INITIAL_INODE_SIZE;
		dbm.initialize(&super, file);
		super.freeptr = dbm.freeptr;
		super.freeDataBlockNumber = dbm.getFreeBlock(file);
		//if (fileSeek(file, super.superBlockStart, SEEK_SET)) return false;
		//if (fileWrite(&super, sizeof(superBlock), 1, file) != 1) return false;
		diskFile = file;
		printf("Initializing root directory...\n");
		initializeRootDirectory();
		

		//fclose(file);
		return true;
	}
}

void Disk::initializeRootDirectory()
{
	Directory root_dir;
	root_dir.files.push_back(fileEntry(".", 0));
	root_dir.files.push_back(fileEntry("..", 0));
	int directoryFileSize = root_dir.files.size() * sizeof(fileEntry);
	Address rootDirectoryFile = allocateNewBlock(diskFile);
	if (rootDirectoryFile == Address(0)) {
		printf("Initialize root directory file failed!\n");
		exit(2);
	}
	for (int i = 0; i < root_dir.files.size(); i++)
	{
		memcpy(db.content + i * sizeof(fileEntry), &root_dir.files[i], sizeof(fileEntry));
		//fileSeek(diskFile, rootDirectoryFile.to_int() + i * sizeof(fileEntry), SEEK_SET);
		//fileWrite(&(root_dir.files[i]), sizeof(fileEntry), 1, diskFile);
	}
	db.write(rootDirectoryFile);
	Address directAddresses[1] = { rootDirectoryFile };
	int root_inode = super.allocateNewInode(root_dir.files.size() * sizeof(fileEntry), 0, directAddresses, NULL, diskFile);
	if (root_inode >= 0) {
		printf("Root directory initialized successfully!\n");
	}
}

Address Disk::allocateNewBlock(FILE* file)
{
	Address ad = dbm.alloc(file);
	if (ad == Address(0))return ad;
	super.freeptr = dbm.freeptr;
	super.freeDataBlockNumber = dbm.getFreeBlock(file);
	if (!super.updateSuperBlock(file)) {
		printf("Update super block failed! Reverting...\n");
		dbm.free(ad, file);
		super.freeptr = dbm.freeptr;
		super.freeDataBlockNumber = dbm.getFreeBlock(file);
		return Address(0);
	}
	return ad;
}

bool Disk::freeBlock(Address addr, FILE* file)
{
	dbm.free(addr, file);
	super.freeptr = dbm.freeptr;
	super.freeDataBlockNumber = dbm.getFreeBlock(file);
	if (!super.updateSuperBlock(file)) {
		printf("Update super block failed!\n");
		exit(1);
	}
	return true;
}

bool Disk::setCurrentInode(int inode_id)
{
	if (fileSeek(diskFile, super.inodeStart + inode_id * super.INODE_SIZE, SEEK_SET)) return false;
	if (fileRead(&currentInode, sizeof iNode, 1, diskFile) != 1) return false;
	return true;
}

Directory Disk::readFileEntriesFromDirectoryFile(iNode inode)
{
	Directory d;
	fileEntry fe;
	unsigned fileSize = inode.fileSize;
	if (fileSize <= DIRECT_ADDRESS_NUMBER * super.BLOCK_SIZE)
	{
		for (int i = 1; i <= ceil((double)fileSize / super.BLOCK_SIZE); i++)
		{
			db.load(inode.direct[i - 1], diskFile);
			if (i == (int)ceil((double)fileSize / super.BLOCK_SIZE))
			{
				for (size_t j = 0; j < (fileSize % super.BLOCK_SIZE) / sizeof(fileEntry); j++)
				{
					memcpy(&fe, db.content + j * sizeof(fileEntry), sizeof(fileEntry));
					d.files.push_back(fe);
				}
			}
			else {
				for (size_t j = 0; j < super.BLOCK_SIZE / sizeof(fileEntry); j++)
				{
					memcpy(&fe, db.content + j * sizeof(fileEntry), sizeof(fileEntry));
					d.files.push_back(fe);
				}
			}
		}
	}
	else {
		for (size_t i = 0; i < DIRECT_ADDRESS_NUMBER; i++)
		{
			db.load(inode.direct[i], diskFile);
			for (size_t j = 0; j < super.BLOCK_SIZE / sizeof(fileEntry); j++)
			{
				memcpy(&fe, db.content + j * sizeof(fileEntry), sizeof(fileEntry));
				d.files.push_back(fe);
			}
		}
		IndirectDiskblock idb;
		idb.load(inode.indirect, diskFile);
		unsigned remainingFileSize = fileSize - DIRECT_ADDRESS_NUMBER * super.BLOCK_SIZE;
		unsigned remainingFullBlocks = remainingFileSize / super.BLOCK_SIZE;
		for (size_t i = 0; i < remainingFullBlocks; i++)
		{
			db.load(idb.addrs[i],diskFile);
			for (size_t j = 0; j < super.BLOCK_SIZE / sizeof(fileEntry); j++)
			{
				memcpy(&fe, db.content + j * sizeof(fileEntry), sizeof(fileEntry));
				d.files.push_back(fe);
			}
		}
		int lastRemainingEntriesNumber = (remainingFileSize % super.BLOCK_SIZE) / sizeof(fileEntry);
		db.load(idb.addrs[remainingFullBlocks],diskFile);
		for (size_t i = 0; i < lastRemainingEntriesNumber; i++)
		{
			memcpy(&fe, db.content + i * sizeof(fileEntry), sizeof(fileEntry));
			d.files.push_back(fe);
		}
	}
	return d;
}

bool Disk::writeFileEntriesToDirectoryFile(Directory d, iNode inode)
{
	unsigned fileSize = inode.fileSize;
	if (fileSize <= DIRECT_ADDRESS_NUMBER * super.BLOCK_SIZE)
	{
		for (int i = 1; i <= ceil((double)fileSize / super.BLOCK_SIZE); i++)
		{
			if (i == (int)ceil((double)fileSize / super.BLOCK_SIZE))
			{
				for (int j = (i - 1) * super.BLOCK_SIZE / sizeof(fileEntry); j < d.files.size(); j++)
				{
					memcpy(db.content + (j % (super.BLOCK_SIZE / sizeof(fileEntry))) * sizeof(fileEntry), &d.files[j], sizeof(fileEntry));
				}
				db.write(inode.direct[i]);
			}
			else
			{
				for (int j = (i - 1) * super.BLOCK_SIZE / sizeof(fileEntry); j < i * super.BLOCK_SIZE / sizeof(fileEntry); j++) 
				{
					memcpy(db.content + (j % (super.BLOCK_SIZE / sizeof(fileEntry))) * sizeof(fileEntry), &d.files[j], sizeof(fileEntry));
				}
				db.write(inode.direct[i - 1]);
			}
		}
	}
	else {
		for (size_t i = 0; i < DIRECT_ADDRESS_NUMBER; i++)
		{
			for (size_t j = 0; j < super.BLOCK_SIZE / sizeof(fileEntry); j++)
			{
				memcpy(db.content + j * sizeof(fileEntry), &d.files[i * (super.BLOCK_SIZE / sizeof(fileEntry)) + j], sizeof(fileEntry));
			}
			db.write(inode.direct[i]);
		}
		IndirectDiskblock idb;
		idb.load(inode.indirect, diskFile);
		unsigned remainingFileSize = fileSize - DIRECT_ADDRESS_NUMBER * super.BLOCK_SIZE;
		unsigned remainingFullBlocks = remainingFileSize / super.BLOCK_SIZE;
		for (size_t i = 0; i < remainingFullBlocks; i++)
		{
			for (size_t j = 0; j < super.BLOCK_SIZE / sizeof(fileEntry); j++)
			{
				memcpy(db.content + j * sizeof(fileEntry), &d.files[DIRECT_ADDRESS_NUMBER * super.BLOCK_SIZE / sizeof(fileEntry) + i * (super.BLOCK_SIZE / sizeof(fileEntry)) + j], sizeof(fileEntry));
			}
			db.write(idb.addrs[i]);
		}
		unsigned lastRemainingEntriesNumber = (remainingFileSize % super.BLOCK_SIZE) / sizeof(fileEntry);
		for (size_t i = 0; i < lastRemainingEntriesNumber; i++)
		{
			memcpy(db.content + i * sizeof(fileEntry), &d.files[(DIRECT_ADDRESS_NUMBER + remainingFullBlocks) * super.BLOCK_SIZE / sizeof(fileEntry) + i], sizeof(fileEntry));
		}
		db.write(idb.addrs[remainingFullBlocks]);
	}
	return true;
}


superBlock::superBlock()
{
	inodeNumber = INITIAL_INODE_NUMBER;
	freeInodeNumber = INITIAL_INODE_NUMBER;
	dataBlockNumber = INITIAL_DATA_BLOCK_NUMBER;
	freeDataBlockNumber = INITIAL_DATA_BLOCK_NUMBER;
	BLOCK_SIZE = INITIAL_BLOCK_SIZE;
	INODE_SIZE = INITIAL_INODE_SIZE;
	//super.SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE - sizeof(magic_number) + 1;
	SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE;
	superBlockStart = sizeof(magic_number) - 1;
	TOTAL_SIZE = INITIAL_DISK_SIZE;
	//super.inodeBitmapStart = super.superBlockStart + super.SUPERBLOCK_SIZE;
	//super.blockBitmapStart = super.inodeBitmapStart + (BITMAP_RESERVE_BITS + INITIAL_INODE_NUMBER) / 8;
	//super.inodeStart = super.blockBitmapStart + super.dataBlockNumber / 8;
	
	inodeStart = INITIAL_SUPERBLOCK_SIZE;
	blockStart = inodeStart + inodeNumber * INITIAL_INODE_SIZE;
	memset(inodeMap, 0, sizeof(inodeMap));
}

int superBlock::allocateNewInode(unsigned fileSize, int parent, Address direct[], Address* indirect, FILE* file)
{
	bool specify_FILE_object = file != NULL;

	if (freeInodeNumber == 0) {
		printf("No free index-node can be allocated!\n");
		return -1;
	}
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	int arrayIndex;
	int offset;
	for (size_t i = 0; i < inodeNumber; i++)
	{
		arrayIndex = i / 8;
		offset = i % 8;
		if ((inodeMap[arrayIndex] >> (7 - offset)) % 2 == 0) {
			break;
		}
	}
	inodeMap[arrayIndex] |= (1 << (7 - offset));
	iNode inode(fileSize, parent, arrayIndex * 8 + offset);
	if (fileSize <= DIRECT_ADDRESS_NUMBER * BLOCK_SIZE)
	{
		for (size_t i = 1; i <= ceil((double)fileSize / BLOCK_SIZE); i++)
		{
			inode.direct[i - 1] = direct[i - 1];
		}
	}
	else {
		for (size_t i = 1; i <= DIRECT_ADDRESS_NUMBER; i++)
		{
			inode.direct[i - 1] = direct[i - 1];
		}
		inode.indirect = *indirect;
	}
	
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	if (fileSeek(file, inodeStart + inode.inode_id * INODE_SIZE,SEEK_SET)) return -2;
	if (fileWrite(&inode, sizeof iNode, 1, file) != 1) return -2;
	if (!specify_FILE_object) { fclose(file); file = NULL; }
	freeInodeNumber--;
	if (!updateSuperBlock(file)) { 
		printf("Update super block failed!\n");
		return -3; 
	}
	return arrayIndex * 8 + offset;
}

bool superBlock::freeInode(int inodeid, FILE* file)
{
	int arrayIndex = inodeid / 8;
	int offset = inodeid % 8;
	inodeMap[arrayIndex] &= (~(1 << (7 - offset)));
	freeInodeNumber++;
	return updateSuperBlock(file);
}

bool superBlock::updateSuperBlock(FILE* file)
{
	bool specify_FILE_object = file != NULL;
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	if (fileSeek(file, superBlockStart, SEEK_SET)) return false;
	if (fileWrite(this, sizeof superBlock, 1, file) != 1) return false;
	if (!specify_FILE_object) { fclose(file); }
	return true;
}


void Diskblock::refreshContent()
{
	memset(content, 0, sizeof content);
}

Diskblock::Diskblock(int addrInt)
{
	load(addrInt);
}

Diskblock::Diskblock(Address addr)
{
	load(addr);
}

void Diskblock::load(int addrInt, FILE* file)
{
	refreshContent();
	bool specify_FILE_object = file != NULL;
	if(!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	fileSeek(file, addrInt, SEEK_SET);
	fileRead(content, sizeof content, 1, file);
	if (!specify_FILE_object) fclose(file);
}

void Diskblock::load(Address addr, FILE* file)
{
	int addrInt = addr.to_int();
	load(addrInt, file);
}

void Diskblock::write(int addrInt, FILE* file)
{
	bool specify_FILE_object = file != NULL;
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	fileSeek(file, addrInt, SEEK_SET);
	fileWrite(content, sizeof content, 1, file);
	if (!specify_FILE_object) fclose(file);
	refreshContent();
}

void Diskblock::write(Address addr, FILE* file)
{
	int addrInt = addr.to_int();
	write(addrInt, file);
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

iNode::iNode(unsigned fileSize, int parent, int inode_id)
{
	time(&inode_create_time);
	time(&inode_access_time);
	time(&inode_modify_time);
	this->fileSize = fileSize;
	this->parent = parent;
	this->inode_id = inode_id;
}

void DiskblockManager::initialize(superBlock* super, FILE* file)
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

Address DiskblockManager::alloc(FILE* file)
{
	// load the current linked-list block
	IndirectDiskblock iblock;
	Address blockAddr = freeptr.block_addr();
	iblock.load(blockAddr,file);

	// no free blocks
	if (freeptr.offset().to_int() == 0) {
		//perror("out of disk memory");
		//exit(1);
		printf("out of disk memory!\n");
		return Address(0);
	}
	Address freeAddr = iblock.addrs[freeptr.offset().to_int() / 3];
	iblock.addrs[freeptr.offset().to_int() / 3].from_int(0);
	if (freeptr.offset().to_int() == 3) {
		freeptr = iblock.addrs[0] + (NUM_INDIRECT_ADDRESSES - 1) * 3;
		memset(iblock.addrs, 0, sizeof iblock.addrs);
		iblock.write(blockAddr,file);
	}
	else {
		iblock.write(blockAddr,file);
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

void DiskblockManager::printBlockUsage(superBlock* super,FILE* file)
{
	/*int freeBlock = 0, linkedListBlock= 0;
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
	freeBlock = (linkedListBlock - 1) * 339 + freeptr.offset().to_int() / 3;*/
	int linkedListBlock = getLinkedListBlock(file);
	int freeBlock = getFreeBlock(linkedListBlock);
	cout << "free block: " << freeBlock << endl;
	cout << "blocks for linked list: " << linkedListBlock << endl;
	cout << "blocks used for data: " << (super->TOTAL_SIZE - super->SUPERBLOCK_SIZE - super->INODE_SIZE * super->inodeNumber) / super->BLOCK_SIZE
		- freeBlock - linkedListBlock << endl;
	cout << "blocks used for Inodes: " << (super->INODE_SIZE * super->inodeNumber) / super->BLOCK_SIZE << endl;
	cout << "blocks used for superblock: " << super->SUPERBLOCK_SIZE / super->BLOCK_SIZE << endl;
}

int DiskblockManager::getFreeBlock(FILE* file)
{
	return (getLinkedListBlock(file) - 1) * 339 + freeptr.offset().to_int() / 3;
}

int DiskblockManager::getFreeBlock(int linkedListBlock)
{
	return (linkedListBlock - 1) * 339 + freeptr.offset().to_int() / 3;
}

int DiskblockManager::getLinkedListBlock(FILE* file)
{
	int linkedListBlock = 0;
	Address ptr = freeptr.block_addr();
	IndirectDiskblock iblock;
	iblock.load(ptr, file);
	do
	{
		linkedListBlock += 1;
		ptr = iblock.addrs[0];
		iblock.load(ptr, file);
	} while (ptr != iblock.addrs[0]);
	linkedListBlock += 1;
	return linkedListBlock;
}

fileEntry::fileEntry(const char* fname, short inode_id)
{
	strcpy(fileName, fname);
	this->inode_id = inode_id;
}
