#include "Disk.h"
#include <iostream>
#include <string.h>
using namespace std;

Address a() {
	return Address(-1);
}

int main() 
{
	Disk disk;
	disk.run();
	DiskblockManager dm;
	char c = 'd';
	cout << (int)c << endl;
	cout << (c >> (7 - 5)) % 2 << endl;
	sizeof(char);
	sizeof(fileEntry);
	Address v = a();
	char a = 25;
	char b = (1 << (7 - 5));
	printf("%d\n", (int)(a | b)); //������λ����Ϊ1�����29
	printf("%d\n", (int)(a & (~(1 << (7 - 3))))); //������λ����Ϊ0�����Ϊ9
	//dm.initialize();
	//cout << dm.freeptr.to_int() << endl;
	//dm.freeptr = INITLAL_FREEPTR;
	//dm.printBlockUsage();
	//Address newBlock = dm.alloc();
	//cout << "address of new block: " << newBlock.to_int() << endl;
	//dm.printBlockUsage();
	//dm.free(newBlock);
	//dm.printBlockUsage();
}