#include <iostream>
#include <fstream>
#include "chip8.h"

using namespace std;

int main(int argc, char *argv[])
{
	Chip8 c;
	
	cout << "loading rom..." << endl;
	c.loadRom(argv[1]);
	c.printMemory();
 	c.runSingleInstruction();
	c.printMemory();	

	return 0;
}
