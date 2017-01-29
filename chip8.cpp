//References: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#dispcoords

#include <iostream>
#include "chip8.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>

Chip8::Chip8()
{
	memInit();
	//TODO: Random seed
	srand(198276376);
}

void Chip8::loadFont()
{
	//TODO: Load font to memory s. at 0	
	unsigned char font[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	}; //80 bytes long (16 x 5b chars)

	//copy the bytes into memory, starting at 0
	for(int i = 0; i < 80; i++)
	{
		memory[i] = font[i];
	}

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
		V[i] = 0;

		//CAREFULL, assert stacksize = num V
		stack[i] = 0;    
	}
	ip = PROG_START_ADDR; //where programs start
	sp = 0;
	st = 0;
	dt = 0;

	loadFont();
	std::cout << "Memory initialised." << std::endl;
}

void Chip8::printMemory()
{
	std::cout << "Beginning memory dump." << std::endl;
	std::cout << "--------------------" << std::endl;

	std::cout << "Registers" << std::endl;
	for(int i = 0; i < NUM_REGS; i++)
	{
		 printf("V[%X]: 0x%04X\n", i, V[i]);
	}

	std::cout << "\nStack" << std::endl;
	for(int i = 0; i < STACKSIZE; i++)
	{
		printf("%X: 0x%04X\n", i, stack[i]);
	}

	printf("\nStack pointer: 0x%02X\n", sp);
	printf("Sound Timer: 0x%02X\n", st);
	printf("Delay Timer: 0x%02X\n", dt);	

	printf("IP: 0x%04X\n", ip);

	std::cout << std::endl << "Main Memory:" << std::endl;

	int linewidth = 16;

	//pretty print memory
	for(int i = 0; i < MEMSIZE/linewidth; i++)
	{
		printf("%04X: ", linewidth*i);

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
	int leftNibble = (instruction & 0xF000) >> 12;

	//get both register indexes
	//they're always in the same place
	//in the opcode
	int x = (instruction & 0xF00) >> 8;
	int y = (instruction & 0xF0) >> 4;

	switch(leftNibble)
	{
		case 0x0: //we are ignoring 0nn SYS addr, it is no longer used
			switch(instruction & 0xFF)
			{
				case 0xE0: //CLS
					//TODO: clear the display
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
			if(V[x] == instruction & 0xFF)
			{
				ip += 2;
			}
			break;

		case 0x4: //SNE Vx, byte
			if(V[x] != instruction & 0xFF)
			{
				ip += 2;
			}
			break;

		case 0x5: //SE Vx, Vy
			if(V[x] == V[y])
			{
				ip += 2;
			}
			break;

		case 0x6: //LD Vx, byte
			V[x] = instruction & 0xFF;
			break;

		case 0x7: //ADD Vx, byte
			V[x] += instruction & 0xFF;
			break;
		case 0x8: //for operations involving both registers

			switch(instruction & 0xF)
			{	
				case 0: //LD Vx, Vy
					V[x] = V[y];	
					break;

				case 1: //OR Vx, Vy
					V[x] |= V[y];
					break;

				case 2: //AND Vx, Vy
					V[x] &= V[y];
					break;

				case 3: //XOR Vx, Vy
					V[x] ^= V[y];
					break;

				case 4: //ADD Vx, Vy
					{
						twoByte result = V[x] + V[y];
						
						//set V[F] = overflow
						result > 0xFF ? V[0xF] = 1 : V[0xF] = 0;
						
						//store the lowest 8 bits of the result in v[x]
						V[x] = result & 0xFF;
					}
					break;

				case 5: //SUB Vx, Vy
					V[x] > V[y] ? V[0xF] = 1 : V[0xF] = 0;

					V[x] -= V[y];
					break;

				case 6: //SHR Vx {, Vy}
					V[0xF] = V[x] & 0x1;
					V[x] >>= 1;
					break;

				case 7: //SUBN Vx, Vy
					V[y] > V[x] ? V[0xF] = 1 : V[0xF] = 0;

					V[x] = V[y] - V[x];
					break;

				case 0xE: //SHL Vx
					//use 0x80 to get just the msb
					V[0xF] = V[x] & 0x80;

					V[x] <<= 1;
					break;
			}
			break;

		case 0x9: //SNE Vx, Vy
			if(V[x] != V[y])
			{
				ip += 2;
			}
			break;

		case 0xA: //LD I, addr
			I = instruction & 0xFFF;
			break;

		case 0xB: //JP V0, addr
			ip = V[0] + (instruction & 0xFFF);
			break;

		case 0xC: //RND Vx, byte
			V[x] = std::rand() & (instruction & 0xFF);
			break;

		case 0xD: //DRW Vx, Vy, nibble
			//TODO: draw sprite at I at x, y
			
			//for n byte sprite
			std::cout << "Draw Sprite" << std::endl;
			break;

		case 0xE:
			switch(instruction & 0xFF)
			{
				case 0x9E: //SKP Vx
					if(keys[V[x]])
					{
						ip += 2;
					}
					break;
					
				case 0xA1: //SKNP Vx
					if(!keys[V[x]])
					{
						ip += 2;
					}
					break;
			}
			break;

		case 0xF:
			switch(instruction & 0xFF)
			{
				case 7: //LD Vx, DT
					V[x] = dt;
					break;

				case 0xA: //LD Vx, K
					//TODO
					break;

				case 0x15: //LD DT, Vx
					dt = V[x];
					break;

				case 0x18: //LD ST, Vx
					st = V[x];
					break;

				case 0x1E: //ADD I, Vx
					I += V[x];
					break;

				case 0x29: //LS F, Vx
					//TODO: load font sprite rep. value Vx
					std::cout << "LS F, Vx" << std::endl;
				        ip -= 2; //run same instruction again
					break;

				case 0x33: //LD B, Vx
					{
						int val = V[x], mod = 0;

						for(int i = 2; i >= 0; i--)
						{
							mod = val % 10;

							memory[I + i] = mod;
						
							val -= mod;
							val /= 10;
						}
					}
					break;

				case 0x55: //LD [I], Vx
					for(int i = 0; i <= V[x]; i++)
					{
						memory[I + i] = V[i];
					}
					break;

				case 0x65: //LD Vx, [I]
					for(int i = 0; i <= V[x]; i++)
					{
						V[i] = memory[I + i];
					}
					break;
			}
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
	instruction = memory[ip];
	instruction <<= 8;
	instruction |= memory[ip+1];
	
	printf("\nip: 0x%04X", ip);
	printf("\nrunning opcode: %04X\n", instruction);

	ip += 2; //inc ip
	execute(instruction);
}
