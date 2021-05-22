#include "Disk.h"
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

Address a() {
	return Address(-1);
}

double division(int a, int b)
{
	if (b == 0)
	{
		throw "Division by zero condition!";
	}
	return ((double)a / b);
}


int main() 
{
	Disk disk;
	disk.run();
	//Address* b = new Address[5];
	//cout << b << endl;
	//cout << b[2].to_int() << endl;
	//cout << &b[2] << endl;
	//b[2] = a();
	//cout << b << endl;
	//cout << b[2].to_int() << endl;
	//cout << &b[2] << endl;
	//Diskblock db;
	//cout << sizeof db.content << endl;
	//int x = 50;
	//int y = 0;
	//double z = 0;

	//try {
	//	z = division(x, y);
	//	cout << z << endl;
	//}
	//catch (const char* msg) {
	//	cerr << msg << endl;
	//	cout << 'x' << x << endl;
	//}

	return 0;
}