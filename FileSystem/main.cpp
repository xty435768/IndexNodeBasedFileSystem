#include "Disk.h"
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

Address a() {
	return Address(-1);
}

int main() 
{
	//Disk disk;
	//disk.run();
	char str[] = "\\hello\\world\\hhh.txt";
	char* token = strtok(str, "\\");
	cout << token << endl;
	char* token2 = strtok(NULL, "\\");
	cout << token2 << endl;
	char* token3 = strtok(NULL, "\\");
	cout << token3 << endl;
	char* token4 = strtok(NULL, "\\");
	cout << token4 << endl;
	char* token5 = strtok(str, "\\");
	cout << token5 << endl;
	cout << strcmp(token, "hello");
	return 0;
}