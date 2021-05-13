#include "Disk.h"
#include <iostream>
#include <string.h>
using namespace std;

int main() 
{
	Disk disk;
	disk.run();
	iNode in;
	cout << sizeof(in) << endl;
	Directory d;
	cout << sizeof(d) << endl;
	return 0;
}