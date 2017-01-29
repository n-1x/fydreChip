//References: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#dispcoords

#include <iostream>
#include "chip8.h"
#include <cstdio>
#include <fstream>

Chip8::Chip8()
{
	memInit();
}

void Chip8::memInit()
{
	//initialise main memory
	for(int i = 0; i < MEMSIZE; i++)
	{
		memory[i] = 0;
	}

	for(int i = 0; i < NUM_REGS; i++)
	{
		registers[i] = 0;

		//CAREFULL, assert stacksize = num registers
		stack[i] = 0;    
	}
	ip = PROG_START_ADDR; //where programs start
	sp = 0;
	st = 0;
	dt = 0;

	std::cout << "Memory initialised." << std::endl;
}

void Chip8::printMemory()
{
	std::cout << "Beginning memory dump." << std::endl;
	std::cout << "--------------------" << std::endl;

	std::cout << "Registers" << std::endl;
	for(int i = 0; i < NUM_REGS; i++)
	{
		 printf("V[%X]: %04X\n", i, registers[i]);
	}

	std::cout << "\nStack" << std::endl;
	for(int i = 0; i < STACKSIZE; i++)
	{
		printf("%X: %04X\n", i, stack[i]);
	}

	printf("\nStack pointer: %02X\n", sp);
	printf("\nSound Timer: %02X\n", st);
	printf("\nDelay Timer: %02X\n", dt);	

	printf("\nIP: %04X\n", ip);

	std::cout << std::endl << "Main Memory:" << std::endl;

	int linewidth = 16;

	//pretty print memory
	for(int i = 0; i < MEMSIZE/linewidth; i++)
	{
		printf("%08X: ", linewidth*i);

		for(int j = i*linewidth; j < linewidth * (i + 1) - 1; j += 2)
		{
			printf("%02X%02X ", int(memory[j]), int(memory[j+1]));
		}

		std::cout << std::endl;
	}
	
	std::cout << std::endl << "End of memory dump." << std::endl;	
}

void Chip8::loadRom(char *filePath)
{
	std::ifstream file(filePath, std::ios::in | 
			             std::ios::binary | 
				     std::ios::ate);
	std::streampos size;

	if (file.is_open())
	{
		size = file.tellg();

		if(size <= MEMSIZE - PROG_START_ADDR)
		{
			file.seekg(0, std::ios::beg);
			file.read((char *)memory + PROG_START_ADDR, size);
			file.close();

			std::cout << "Rom loaded into memory. Size:" << size << "b" << std::endl;
		}
	}
	else 
	{
		std::cout << "Unable to load file." << std::endl;
	}
}

void Chip8::execute(const twoByte &instruction)
{
	printf("Parsing instruction: %04X\n", instruction);
	
	int leftNibble = (instruction & 0xF000) >> 12;
	int regIndex = 0, regIndex2 = 0; //indexes of registers, used by some opcodes

	switch(leftNibble)
	{
		case 0x0: //we are ignoring 0nn SYS addr, it is no longer used
			switch(instruction & 0xFF)
			{
				case 0xE0: //CLS
					//clear the display
					break;

				case 0xEE: //RET
					ip = stack[sp--];
					break;
			}
			break;

		case 0x1: //JP addr
			ip = instruction & 0xFFF;
			break;

		case 0x2: //CALL addr
			sp += 1;
			sp = ip;
			ip = instruction & 0xFFF;
			break;
		
		case 0x3: //SE Vx, byte
			regIndex = (instruction & 0xF00) >> 8;

			if(registers[regIndex] == instruction & 0xFF)
			{
				ip += 2;
			}
			break;

		case 0x4: //SNE Vx, byte
			regIndex = (instruction & 0xF00) >> 8;

			if(registers[regIndex] != instruction & 0xFF)
			{
				ip += 2;
			}
			break;

		case 0x5: //SE Vx, Vy
			regIndex = (instruction & 0xF00) >> 8;
			regIndex2 = (instruction & 0x0F0) >> 4;
				
			if(registers[regIndex] == registers[regIndex2])
			{
				ip += 2;
			}
			break;

		case 0x6: //LD Vx, byte
			regIndex = (instruction & 0xF00) >> 8;

			registers[regIndex] = instruction & 0xFF;
			break;

		default:
			std::cout << "Unknown instruction." << std::endl;
			break;

	}
}

void Chip8::runSingleInstruction()
{
	//fetch, then send to execute()
	twoByte instruction = 0;
	
	//take first byte, left shift it, then or with second byte
	//all instructions are two bytes
	instruction = (memory[ip] << 8) | memory[ip+1];
	ip += 2; //inc ip


	execute(instruction);
}
