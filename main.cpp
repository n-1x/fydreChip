#include <iostream>
#include <fstream>
#include "chip8.h"

using namespace std;

int main(int argc, char *argv[])
{
	Chip8 c;
	
	cout << "Loading rom..." << endl;
	c.loadRom(argv[1]);
	c.printMemory();

	cout << "Beginning execution." << endl;
	while(true)
	{
 		c.runSingleInstruction();
	}
	//c.printMemory();	

	return 0;
}
