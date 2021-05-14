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
	//d.files = new file[1000];
	cout << sizeof(short) << endl;
	return 0;
}