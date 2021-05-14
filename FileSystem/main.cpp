#include "Disk.h"
#include <iostream>
#include <string.h>
using namespace std;

int main() 
{
	Disk disk;
	disk.run();
	DiskblockManager dm;
	//dm.initialize();
	//cout << dm.freeptr.to_int() << endl;
	dm.freeptr = INITLAL_FREEPTR;
	dm.printBlockUsage();
	Address newBlock = dm.alloc();
	cout << "address of new block: " << newBlock.to_int() << endl;
	dm.printBlockUsage();
	dm.free(newBlock);
	dm.printBlockUsage();
}