#include "Disk.h"
#include <iostream>
#include <cstdio>

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

void Disk::parse(char* str)
{
	char* command = strtok(str, " ");
	if (command == NULL) {
		cout << "no command error" << endl;
		return;
	}
	
	if (!strcmp(command, "createFile")) {
		char* path = strtok(NULL, " ");
		if (path == NULL) {
			cout << "lack of path" << endl;
			return;
		}
		char* size = strtok(NULL, " ");
		if (size == NULL) {
			cout << "lack of size" << endl;
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		//TODO: implement creating file
		

	}
	else if (!strcmp(command, "createDir") || !strcmp(command, "mkdir")) {
		char* path = strtok(NULL, " ");
		if (path == NULL) {
			cout << "lack of path" << endl;
			return;
		}
		if (strlen(path) > MAXIMUM_FILENAME_LENGTH - 1) {
			printf("The directory name is too long! Maximum length: %d.\n", MAXIMUM_FILENAME_LENGTH - 1);
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}

		if (!regex_match(string(path), fileNamePattern))
		{
			printf("Your file name does not meet the specification. "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores\n");
			return;
		}
		vector<string> pathList = stringSplit(string(path), "/");
		int inode_id_ptr = currentInode.inode_id;
		for (size_t i = 0; i < pathList.size(); i++)
		{
			if (pathList[i] == "") continue;
			iNode inode_ptr = super.loadInode(inode_id_ptr,diskFile);
			Directory dir = readFileEntriesFromDirectoryFile(inode_ptr);
			short nextDirectoryInode = dir.findInFileEntries(pathList[i].c_str());
			if (nextDirectoryInode != -1) {
				inode_id_ptr = nextDirectoryInode;
			}
			else {
				inode_id_ptr = createUnderInode(inode_ptr, pathList[i].c_str());
				if (inode_id_ptr == -1) {
					printf("Create directory failed!\n");
					return;
				}
				printf("Create directory successful!\n");
				return;
			}
		    
		}
		printf("dir already exists!\n");
		return;
		

	}
	else if (!strcmp(command, "deleteFile")) {
		char* path = strtok(NULL, " ");
		if (path == NULL) {
			cout << "lack of path" << endl;
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return; 
		}
		//TODO: implement deleting file

	}
	else if (!strcmp(command, "deleteDir") || !strcmp(command, "rmdir")) {
		char* path = strtok(NULL, " ");
		if (path == NULL) {
			cout << "lack of path" << endl;
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		iNode directoryToBeDelete = super.loadInode(locateInodeFromPath(string(path)), diskFile);
		if (directoryToBeDelete.inode_id == currentInode.inode_id) {
			printf("You cannot delete current working directory: ");
			printCurrentDirectory("\n");
			return;
		}
		recursiveDeleteDirectory(directoryToBeDelete);
	}
	else if (!strcmp(command, "changeDir") || !strcmp(command, "cd")) {
		char* path = strtok(NULL, " ");
		if (path == NULL) {
			cout << "lack of path" << endl;
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		if (changeDirectory(path)) {
			printf("Directory has changed to %s\n", path);
		}
		else
		{
			printf("Change directory failed!\n", path);
		}
		
	}
	else if (!strcmp(command, "dir")) {
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		listDirectory(currentInode);
		printf("Current directory file size: %d\n", currentInode.fileSize);
	}
	else if (!strcmp(command, "cp")) {
		char* path1 = strtok(NULL, " ");
		if (path1 == NULL) {
			cout << "lack of original path" << endl;
			return;
		}
		char* path2 = strtok(NULL, " ");
		if (path2 == NULL) {
			cout << "lack of target path" << endl;
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		//TODO: implementing coping file or dir

	}
	else if (!strcmp(command, "sum")) {
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		dbm.printBlockUsage(&super, diskFile);
		printf("Inode usage: %d/%d (%.2f%%)\n", super.inodeNumber - super.freeInodeNumber, super.inodeNumber, 100 * (super.inodeNumber - super.freeInodeNumber) / (double)super.inodeNumber);
	}
	else if (!strcmp(command, "cat")) {
		char* path = strtok(NULL, " ");
		if (path == NULL) {
			cout << "lack of path" << endl;
			return;
		}
		char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			cout << "more arguments than expected" << endl;
			return;
		}
		//TODO: implementing printing the content of file.

	}
	else if (!strcmp(command, "cls")) {
	    system("cls");
    }
	else 
    {
	    printf("Unknown command! Please check again!\n");
    }
}

Disk::Disk()
{
	fileNamePattern = regex("^([a-z]|[A-Z]|[_/]|[0-9])*$");
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
			printf("Root inode create time:%s\n", currentInode.getCreateTime().c_str());
			printf("Root inode access time:%s\n", currentInode.getAccessTime().c_str());
			printf("Root inode modify time:%s\n", currentInode.getModifiedTime().c_str());
			printf("Root file size:%d\n", currentInode.fileSize);
			printf("Root file start at (direct block addresss):%d\n", currentInode.direct[0].to_int());
			

			while (true) {
				printCurrentDirectory();
				printf("#");
				char command[1024];
				scanf("%[^\n]", &command);
				getchar();
				if (!strcmp(command, "exit"))break;
				parse(command);
				currentInode = super.loadInode(currentInode.inode_id, diskFile);
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
	//if (fileSeek(diskFile, super.inodeStart + inode_id * super.INODE_SIZE, SEEK_SET)) return false;
	//if (fileRead(&currentInode, sizeof iNode, 1, diskFile) != 1) return false;
	iNode inode = super.loadInode(inode_id);
	if (inode.inode_id != -1) {
		memcpy(&currentInode, &inode, sizeof(iNode));
		return true;
	}
	else return false;
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
				db.write(inode.direct[i - 1]);
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

int Disk::createUnderInode(iNode& parent, const char* name, bool isDir)
{
	/*1、只占用直接块，不需要申请新块
	2、只占用直接块，需要申请新块
	3、一开始占用直接块，但加了之后需要申请间接块
	4、占用间接块，不需要申请新块
	5、占用间接块，需要申请新块*/
	if (super.freeInodeNumber == 0)
	{
		printf("No free index-node left!\n");
		return -1;
	}
	if (parent.fileSize == MAXIMUM_FILE_SIZE) {
		printf("This directory has reached its maximum size!\n");
		return -1;
	}
	bool checkDuplicate = false;
	Directory parent_dir = readFileEntriesFromDirectoryFile(parent);
	for (size_t i = 0; i < parent_dir.files.size(); i++)
	{
		if (!strcmp(parent_dir.files[i].fileName, name))
		{
			checkDuplicate = true;
			break;
		}
	}
	if (checkDuplicate) {
		printf("File/Directory with same name is exist: %s\nPlease change another name!\n",name);
		return -1;
	}
	unsigned newFileSizeOfCurrentDirectory = parent.fileSize + sizeof(fileEntry);
	int newInode = -1;
	if (parent.fileSize < super.BLOCK_SIZE * DIRECT_ADDRESS_NUMBER)
	{
		if (parent.fileSize % super.BLOCK_SIZE != 0) {
			//1、只占用直接块，父目录文件不需要申请新块，需要给新文件夹一个block
			int block_required = 0; //只是父文件夹的需求量
			if (freeBlockCheck(block_required, name))
			{
				//应用新文件(夹)的更改
				if(isDir)
					newInode = applyChangesForNewDirectory(parent, name);
				else {
					//TODO: applyChangesForNewFile(blablabla)
				}
				if (newInode == -1)return -1;
				//应用父文件夹的更改
				Directory parent_dir = readFileEntriesFromDirectoryFile(parent);
				parent_dir.files.push_back(fileEntry(name, newInode));
				parent.fileSize += sizeof(fileEntry);
				parent.updateModifiedTime();
				super.writeInode(parent, diskFile);
				writeFileEntriesToDirectoryFile(parent_dir, parent);
			}
			else return -1;
		}
		else
		{
			//2、只占用直接块，需要申请新块
			int block_required = 1;
			if (freeBlockCheck(block_required, name))
			{
				//应用新文件(夹)的更改
				if (isDir)
					newInode = applyChangesForNewDirectory(parent, name);
				else {
					//TODO: applyChangesForNewFile(blablabla)
				}
				if (newInode == -1)return -1;
				//应用父文件夹的更改
				Directory parent_dir = readFileEntriesFromDirectoryFile(parent);
				parent_dir.files.push_back(fileEntry(name, newInode));
				parent.fileSize += sizeof(fileEntry);
				parent.updateModifiedTime();
				Address newBlockForParentDirectory = allocateNewBlock(diskFile);
				parent.direct[parent.fileSize / super.BLOCK_SIZE] = newBlockForParentDirectory;
				super.writeInode(parent, diskFile);
				writeFileEntriesToDirectoryFile(parent_dir, parent);
			}
			else return -1;
		}
	}
	else if (parent.fileSize == super.BLOCK_SIZE * DIRECT_ADDRESS_NUMBER) 
	{
		//3、一开始占用直接块，但加了之后需要申请间接块
		int block_required = 2;
		if (freeBlockCheck(block_required, name))
		{
			//应用新文件(夹)的更改
			if (isDir)
				newInode = applyChangesForNewDirectory(parent, name);
			else {
				//TODO: applyChangesForNewFile(blablabla)
			}
			if (newInode == -1) return -1;
			//应用父文件夹的更改
			Directory parent_dir = readFileEntriesFromDirectoryFile(parent);
			parent_dir.files.push_back(fileEntry(name, newInode));
			parent.fileSize += sizeof(fileEntry);
			parent.updateModifiedTime();
			Address newIndirectAddressBlockForParentDirectory = allocateNewBlock(diskFile);
			Address newIndirectBlockForParentDirectory = allocateNewBlock(diskFile);
			parent.indirect = newIndirectAddressBlockForParentDirectory;
			IndirectDiskblock idb;
			idb.addrs[0] = newIndirectBlockForParentDirectory;
			idb.write(parent.indirect, diskFile);
			super.writeInode(parent, diskFile);
			writeFileEntriesToDirectoryFile(parent_dir, parent);
		}
		else return -1;
	}
	else
	{
		if (parent.fileSize % super.BLOCK_SIZE != 0) {
			//4、占用间接块，不需要申请新块
			int block_required = 0;
			if (freeBlockCheck(block_required, name)) {
				//应用新文件(夹)的更改
				if (isDir)
					newInode = applyChangesForNewDirectory(parent, name);
				else {
					//TODO: applyChangesForNewFile(blablabla)
				}
				if (newInode == -1) return -1;
				//应用父文件夹的更改
				Directory parent_dir = readFileEntriesFromDirectoryFile(parent);
				parent_dir.files.push_back(fileEntry(name, newInode));
				parent.fileSize += sizeof(fileEntry);
				parent.updateModifiedTime();
				super.writeInode(parent, diskFile);
				writeFileEntriesToDirectoryFile(parent_dir, parent);
			}
			else return -1;
		}
		else {
			//5、占用间接块，需要申请新块
			int block_required = 1;
			if (freeBlockCheck(block_required, name)) {
				//应用新文件(夹)的更改
				if (isDir)
					newInode = applyChangesForNewDirectory(parent, name);
				else {
					//TODO: applyChangesForNewFile(blablabla)
				}
				if (newInode == -1) return -1;
				//应用父文件夹的更改
				Directory parent_dir = readFileEntriesFromDirectoryFile(parent);
				parent_dir.files.push_back(fileEntry(name, newInode));
				parent.fileSize += sizeof(fileEntry);
				parent.updateModifiedTime();
				Address newIndirectBlockForParentDirectory = allocateNewBlock(diskFile);
				IndirectDiskblock idb;
				idb.load(parent.indirect,diskFile);
				idb.addrs[parent.fileSize / super.BLOCK_SIZE - DIRECT_ADDRESS_NUMBER] = newIndirectBlockForParentDirectory;
				idb.write(parent.indirect, diskFile);
				super.writeInode(parent, diskFile);
				writeFileEntriesToDirectoryFile(parent_dir, parent);
			}
			else return -1;
		}
	}
	return newInode;
}

short Disk::applyChangesForNewDirectory(iNode parent, const char* name)
{
	// return: new inode id for new directory
	Address newBlockForNewDirectory = allocateNewBlock(diskFile);
	Address newBlocks[1] = { newBlockForNewDirectory };
	int newInodeForNewDirectory = super.allocateNewInode(2 * sizeof(fileEntry), parent.inode_id, newBlocks, NULL, diskFile);
	Directory dir;
	dir.files.push_back(fileEntry(".", newInodeForNewDirectory));
	dir.files.push_back(fileEntry("..", parent.inode_id));
	if (!writeFileEntriesToDirectoryFile(dir, super.loadInode(newInodeForNewDirectory)))
	{
		printf("Failed to write file entries to disk for new directory!\n");
		super.freeInode(newInodeForNewDirectory, diskFile);
		freeBlock(newBlockForNewDirectory, diskFile);
		return -1;
	}
	return newInodeForNewDirectory;
}

bool Disk::freeBlockCheck(int blockRequirementOfparent, const char* newDirectoryName)
{
	int newDirectoryBlockRequirment = 1;
	int newDirectoryInodeRequirment = 1;
	printf("Need %d new block(s) for current directory, %d inode(s) and %d block(s) for new directory: %s \n", blockRequirementOfparent, newDirectoryInodeRequirment, newDirectoryBlockRequirment, newDirectoryName);
	printf("Free block: %d.\n", super.freeDataBlockNumber);
	if (newDirectoryBlockRequirment + blockRequirementOfparent < super.freeDataBlockNumber) {
		return true;
	}
	else
	{
		printf("No enough blocks left in this disk!\n");
		return false;
	}
}

void Disk::listDirectory(iNode directory_inode)
{
	Directory dir = readFileEntriesFromDirectoryFile(directory_inode);
	printf("\nCurrent working directory: ");
	printCurrentDirectory();
	printf("\nFile Name\tFile Size(Bytes)\tCreate Time\t\t\tModified Time\t\t\tInode ID\n");
	iNode in;
	for (size_t i = 0; i < dir.files.size(); i++)
	{
		in = super.loadInode(dir.files[i].inode_id);
		printf("%s%s%d\t\t\t%s\t%s\t%d\n", dir.files[i].fileName, (strlen(dir.files[i].fileName) >= 8 ? "\t" : "\t\t"), in.fileSize, in.getCreateTime().c_str(), in.getModifiedTime().c_str(), in.inode_id);
	}
	printf("\n");
}

void Disk::printCurrentDirectory(const char* end)
{
	printf("%s", getFullFilePath(currentInode, end).c_str());
}

string Disk::getFullFilePath(iNode inode, const char* end)
{
	string result = "/";
	stack<string> directories;
	while (inode.inode_id != 0)
	{
		directories.push(getFileName(inode));
		inode = super.loadInode(inode.parent, diskFile);
	}
	while (!directories.empty()) {
		result += (directories.top() + "/");
		directories.pop();
	}
	result += (" " + string(end));
	return result;
}



string Disk::getFileName(iNode inode)
{
	if (inode.inode_id == 0) {
		return string("");
	}
	iNode parent_inode = super.loadInode(inode.parent, diskFile);
	Directory parent_dir = readFileEntriesFromDirectoryFile(parent_inode);
	for (size_t i = 0; i < parent_dir.files.size(); i++)
	{
		if (parent_dir.files[i].inode_id == inode.inode_id)
			return string(parent_dir.files[i].fileName);
	}
	printf("No such file or directory!\n");
	exit(5);

}

bool Disk::changeDirectory(const char* destination)
{
	Directory current_dir = readFileEntriesFromDirectoryFile(currentInode);
	if (destination[0] == '/')
		destination += 1;
	size_t i;
	for (i = 0; i < current_dir.files.size(); i++)
	{
		if (!strcmp(current_dir.files[i].fileName, destination))
		{
			currentInode = super.loadInode(current_dir.files[i].inode_id, diskFile);
			return true;
		}
	}
	printf("No such directory!\n");
	return false;
}

vector<string> Disk::stringSplit(const string& str, const string& pattern)
{
	regex re(pattern);
	return vector<string> {
		sregex_token_iterator(str.begin(), str.end(), re, -1),sregex_token_iterator()
	};
}

short Disk::locateInodeFromPath(std::string path)
{
	//当前工作目录为起点
	vector<string> pathList = stringSplit(path, "/");
	short inode_id_ptr = currentInode.inode_id;
	for (size_t i = 0; i < pathList.size(); i++)
	{
		if (pathList[i] == "") continue;
		iNode inode_ptr = super.loadInode(inode_id_ptr, diskFile);
		Directory dir = readFileEntriesFromDirectoryFile(inode_ptr);
		short nextDirectoryInode = dir.findInFileEntries(pathList[i].c_str());
		if (nextDirectoryInode != -1) {
			inode_id_ptr = nextDirectoryInode;
		}
		else return -1;
	}
	return inode_id_ptr;
}

void Disk::recursiveDeleteDirectory(iNode inode)
{
	if (inode.fileSize == 2 * sizeof(fileEntry)) {
		string filePath = getFullFilePath(inode);
		if (deleteFile(inode))
		{
			printf("File/Directory deleted successful: %s\n", filePath.c_str());
		}
		else {
			printf("File/Directory deleted failed: %s\n", filePath.c_str());
		}
		return;
	}
	Directory dir = readFileEntriesFromDirectoryFile(inode);
	for (size_t i = 0; i < dir.files.size(); i++)
	{
		if (!strcmp(dir.files[i].fileName, ".") || !strcmp(dir.files[i].fileName, ".."))
			continue;
		recursiveDeleteDirectory(super.loadInode(dir.files[i].inode_id, diskFile));
	}
	string filePath = getFullFilePath(inode);
	if (deleteFile(inode))
	{
		printf("File/Directory deleted successful: %s\n", filePath.c_str());
	}
	else {
		printf("File/Directory deleted failed: %s\n", filePath.c_str());
	}
	return;
}

bool Disk::deleteFile(iNode inode)
{
	int blockCount = (int)ceil((double)inode.fileSize / super.BLOCK_SIZE);
	if (blockCount <= DIRECT_ADDRESS_NUMBER) {
		for (size_t i = 0; i < blockCount; i++)
		{
			freeBlock(inode.direct[i], diskFile);
		}
	}
	else
	{
		for (size_t i = 0; i < DIRECT_ADDRESS_NUMBER; i++)
		{
			freeBlock(inode.direct[i], diskFile);
		}
		IndirectDiskblock idb;
		idb.load(inode.indirect);
		int remainDataBlock = blockCount - DIRECT_ADDRESS_NUMBER;
		for (size_t i = 0; i < remainDataBlock; i++)
		{
			freeBlock(idb.addrs[i]);
		}
		freeBlock(inode.indirect);
	}
	super.freeInode(inode.inode_id, diskFile);
	iNode parent_inode = super.loadInode(inode.parent, diskFile);
	Directory parent_dir = readFileEntriesFromDirectoryFile(parent_inode);
	for (size_t i = 0; i < parent_dir.files.size(); i++)
	{
		if (parent_dir.files[i].inode_id == inode.inode_id)
		{
			parent_dir.files.erase(parent_dir.files.begin() + i);
		}
	}
	int parent_block_count = (int)ceil((double)parent_inode.fileSize / super.BLOCK_SIZE);
	if (parent_block_count <= DIRECT_ADDRESS_NUMBER)
	{
		if (parent_inode.fileSize % super.BLOCK_SIZE == sizeof(fileEntry)) {
			freeBlock(parent_inode.direct[parent_block_count - 1], diskFile);
		}
	}
	else {
		IndirectDiskblock idb;
		idb.load(parent_inode.indirect, diskFile);
		if (parent_inode.fileSize == DIRECT_ADDRESS_NUMBER * super.BLOCK_SIZE + sizeof(fileEntry)) {
			freeBlock(idb.addrs[parent_block_count - 1 - DIRECT_ADDRESS_NUMBER], diskFile);
			freeBlock(parent_inode.indirect, diskFile);
			parent_inode.indirect = Address(0);
		}
		else if (parent_inode.fileSize % super.BLOCK_SIZE == sizeof(fileEntry)) {
			freeBlock(idb.addrs[parent_block_count - 1 - DIRECT_ADDRESS_NUMBER], diskFile);
			idb.addrs[parent_block_count - 1 - DIRECT_ADDRESS_NUMBER] = Address(0);
		}
	}
	parent_inode.fileSize -= sizeof(fileEntry);
	if (!super.writeInode(parent_inode, diskFile)) { printf("Failed to update parent inode (id: %d)!\n", parent_inode.inode_id); return false; }
	if (!writeFileEntriesToDirectoryFile(parent_dir, parent_inode)) { printf("Failed to update parent directory file (inode id: %d)!\n", parent_inode.inode_id); return false; }
	return true;
}

iNode superBlock::loadInode(short id,FILE* file)
{
	bool specify_FILE_object = file != NULL;
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	iNode inode;
	if (fileSeek(file, inodeStart + id * INODE_SIZE, SEEK_SET)) return iNode(0, -1, -1);
	if (fileRead(&inode, sizeof iNode, 1, file) != 1) return iNode(0, -1, -1);
	if (!specify_FILE_object) fclose(file);
	return inode;
}

bool superBlock::writeInode(iNode inode, FILE* file)
{
	bool specify_FILE_object = file != NULL;
	if (!specify_FILE_object) file = fileOpen(DISK_PATH, "rb+");
	if (fileSeek(file, inodeStart + inode.inode_id * INODE_SIZE, SEEK_SET)) return false;
	if (fileWrite(&inode, sizeof iNode, 1, file) != 1)return false;
	if (!specify_FILE_object) fclose(file);
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

iNode::iNode(unsigned fileSize, int parent, int inode_id, bool isDir)
{
	updateCreateTime();
	updateModifiedTime();
	updateAccessTime();
	this->fileSize = fileSize;
	this->parent = parent;
	this->inode_id = inode_id;
	this->isDir = isDir;
}

void iNode::updateCreateTime()
{
	time(&inode_create_time);
}

void iNode::updateModifiedTime()
{
	time(&inode_access_time);
}

void iNode::updateAccessTime()
{
	time(&inode_modify_time);
}

string iNode::getCreateTime()
{
	string time_str = ctime((time_t const*)&(inode_create_time));
	time_str = time_str.substr(0, time_str.size() - 1);
	return time_str;
}

string iNode::getModifiedTime()
{
	string time_str = ctime((time_t const*)&(inode_modify_time));
	time_str = time_str.substr(0, time_str.size() - 1);
	return time_str;
}

string iNode::getAccessTime()
{
	string time_str = ctime((time_t const*)&(inode_access_time));
	time_str = time_str.substr(0, time_str.size() - 1);
	return time_str;
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

short Directory::findInFileEntries(const char* name)
{
	for (size_t i = 0; i < files.size(); i++)
	{
		if (!strcmp(files[i].fileName, name))
			return files[i].inode_id;
	}
	return -1;
}
