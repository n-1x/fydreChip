//File: chip8.cpp
//Author: Nicholas J D Dean
//Date Created: 2017-01-27
//References:
//  http://devernay.free.fr/hacks/chpc8/C8TECH10.HTM#dispcoords

#include <iostream>
#include "chip8.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <time.h>

Chip8::Chip8()
{
	printf("\nWelcome to Fydrechip!\n");
	printf("---------------------------\n");
	memInit();

	//seed with current time
	srand(time(NULL));

	screenUpdateNeeded = false;
	
	//initialise the display, all pixels start off
	for(int i = 0; i < DISP_WIDTH * DISP_HEIGHT; i++)
	{
		display[i] = false;
	}
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

//this will prepare the memory such
//that a rom can be loaded and run after
//this function's operation.
//This means that is also has to load
//the font, which has been split to a
//separate function
void Chip8::memInit()
{
	//initialise main memory
	for(int i = 0; i < MEMSIZE; i++)
	{
		memory[i] = 0;
	}

	//initialise all registers
	for(int i = 0; i < NUM_REGS; i++)
	{
		V[i] = 0;

		//CAREFULL, assert stacksize = num V
		stack[i] = 0;    
	}

	pc = PROG_START_ADDR; //where programs start
	sp = 0;
	st = 0;
	dt = 0;

	printf("Memory initialised.\n");
	loadFont();
	printf("Font loaded.\n");
}

void Chip8::printStack()
{
	printf("\nStack");

	for(int i = 0; i < STACKSIZE; i++)
	{
		printf("%X: 0x%04X", i, stack[i]);

		//print an arrow to show
		//where sp is pointing
		if(i == sp)
		{
			printf(" <-- ");
		}
		printf("\n");
	}
	printf("\n");
}

void Chip8::printRegs()
{
	printf("Registers");

	for(int i = 0; i < NUM_REGS; i++)
	{
		 printf("V[%X]: 0x%04X\n", i, V[i]);
	}

	//print single values
	printf("\nSP: 0x%02X\n", sp);
	printf("ST: 0x%02X\n", st);
	printf("DT: 0x%02X\n", dt);	
	printf("IP: 0x%04X\n", pc);
	printf("I: 0x%04X\n\n", I);
}

void Chip8::printMainMemory()
{
	std::cout << std::endl << "Main Memory:" << std::endl;

	int linewidth = 16;
	//pretty print main memory
	for(int i = 0; i < MEMSIZE/linewidth; i++)
	{
		printf("%04X: ", linewidth*i);

		for(int j = i*linewidth; j < linewidth * (i + 1) - 1; j += 2)
		{
			printf("%02X %02X ", int(memory[j]), int(memory[j+1]));
		}
		printf("\n");
	}
}

void Chip8::printAllMemory()
{
	printf("Beginning memory dump.");

	printStack();
	printRegs();
	printMainMemory();

	printf("End of memory dump.");
}

//print a representation of the internal
//copy of the display to stdout
void Chip8::printDisplay()
{
	std::cout << "\nPrinting display." << std::endl;
	for(int y = 0; y < DISP_HEIGHT; y++) //rows
	{
		int yVal = y * DISP_WIDTH;

		for(int x = 0; x < DISP_WIDTH; x++) //columns
		{
			//print a 1 for an on pixel, 0 for off
			if(display[yVal + x])
			{
				std::cout << "1";
			}	
			else
			{
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
}

//takes the file at the given path and loads it into
//memory starting at PROG_START_ADDR
void Chip8::loadRom(const char *filePath)
{
	//open the file from the end
	std::ifstream file(filePath, std::ios::in |     //input 
			                       std::ios::binary | //binary mode
				     		           std::ios::ate);    //start at end
	std::streampos size;

	if (file.is_open())
	{
		size = file.tellg(); //started at end, so tellg is size

		//if rom fits in memory
		if(size <= MEMSIZE - PROG_START_ADDR)
		{
			//go to beginning
			file.seekg(0, std::ios::beg);

			file.read((char *)memory + PROG_START_ADDR, size);
			file.close();

			printf("Rom successfully loaded. Size: %d bytes\n", size);
		}
	}
	else 
	{
		std::cout << "Unable to load file." << std::endl;
	}
}

//runs any instruction given to it
void Chip8::execute(const twoByte &instruction)
{
	int leftTribble = (instruction & 0xF000) >> 12;

	//get both register indexes
	//they're always in the same place
	//in the opcode
	int x = (instruction & 0xF00) >> 8;
	int y = (instruction & 0xF0) >> 4;
	int nn = instruction & 0xFF;   //final byte of instruction
	int nnn = instruction & 0xFFF; //final 12 bits ^

	switch(leftTribble)
	{
		case 0x0: //we are ignoring 0nn SYS addr, it is no longer used
			switch(nn)
			{
				case 0xE0: //CLS
					for(int i = 0; i < DISP_WIDTH * DISP_HEIGHT; i++)
					{
						display[i] = false;
					}
					screenUpdateNeeded = true;
					break;

				case 0xEE: //RET
					pc = stack[sp];
					--sp;
					break;
			}
			break;

		case 0x1: //JP addr
			pc = nnn;
			break;

		case 0x2: //CALL addr
			++sp;
			stack[sp] = pc;
			pc = nnn;
			break;
		
		case 0x3: //SE Vx, byte
			if(V[x] == nn)
			{
				pc += 2;
			}
			break;

		case 0x4: //SNE Vx, byte
			if(V[x] != nn)
			{
				pc += 2;
			}
			break;

		case 0x5: //SE Vx, Vy
			if(V[x] == V[y])
			{
				pc += 2;
			}
			break;

		case 0x6: //LD Vx, byte
			V[x] = nn; 
			break;

		case 0x7: //ADD Vx, byte
			V[x] += nn;
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
						twoByte result = V[x];
						result += V[y];
						
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
					V[0xF] = (V[x] & 0x80) >> 7;
					
					//multpcly by 2
					V[x] <<= 1;
					break;
			}
			break;

		case 0x9: //SNE Vx, Vy
			if(V[x] != V[y])
			{
				pc += 2;
			}
			break;

		case 0xA: //LD I, addr
			I = nnn;
			break;

		case 0xB: //JP V0, addr
			pc = V[0] + nnn;
			break;

		case 0xC: //RND Vx, byte
			V[x] = std::rand() & nn;
			break;

		case 0xD: //DRW Vx, Vy, nibble
			{
				//length of sprite in bytes
				int spriteLen = instruction & 0xF; 
				
				//draw location is stored in V[x], V[y]
				//stored location is in I
				
				V[0xF] = 0; //no collision yet

				for(int row = 0; row < spriteLen; ++row)
				{
					unsigned int line = memory[I + row];

					//loop is backwards because the sprite is
					//written right to left due to how the single bits
					//are being taken out of the instruction
					for(int col = 7; col >= 0; --col)
					{
						//get correct row (%s are for wrapping)
						//adding a V is the offset
						int index = (((row + V[y]) % DISP_HEIGHT) * DISP_WIDTH);
						//add x position
						index += (col + V[x]) % DISP_WIDTH;	

						int bit = line & 1;

						if((bit == 1) && display[index])
						{
							V[0xF] = 1;
						}
						display[index] ^= bit;
						
						line >>= 1;

					}
				}
			}			  
			screenUpdateNeeded = true;
			break;

		case 0xE:
			switch(nn)
			{
				case 0x9E: //SKP Vx
					if(keys[V[x]])
					{
						pc += 2;
					}
					break;
					
				case 0xA1: //SKNP Vx
					if(!keys[V[x]])
					{
						pc += 2;
					}
					break;
			}
			break;

		case 0xF:
			switch(nn)
			{
				case 7: //LD Vx, DT
					V[x] = dt;
					break;

				case 0xA: //LD Vx, K
					pc -= 2; //stop program progressing
					{
						bool found = false;
						int counter = 0;

						//check all keys
						while(!found && counter < 16)
						{
							found = keys[counter] == true;
							
							//found a key, store value in V[x]
							if(found) 
							{
								V[x] = counter;
								pc += 2; //resume progression	
							}
							++counter;
						}
					}
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
					I = V[x] * 5; //each char is 5 bytes
					break;

				case 0x33: //LD B, Vx
					{
						int val = V[x], mod = 0;

						for(int i = 2; i >= 0; --i)
						{
							mod = val % 10;

							memory[I + i] = mod;
						
							val -= mod;
							val /= 10;
						}
					}
					break;

				case 0x55: //LD [I], Vx
					for(int i = 0; i <= x; ++i)
					{
						memory[I + i] = V[i];
					}
					break;

				case 0x65: //LD Vx, [I]
					for(int i = 0; i <= x; ++i)
					{
						V[i] = memory[I + i];
					}
					break;
			}
			break;

		default:
			printf("Unknown instruction: 0x%04X\n", instruction );
			break;

	}
}

void Chip8::runSingleCycle()
{
	//fetch, then send to execute()
	twoByte instruction = 0;
	
	//take first byte, left shift it, then or with second byte
	//all instructions are two bytes
	instruction = memory[pc];
	instruction <<= 8;
	instruction |= memory[pc+1];
	
	//printf("\nExecuting instruction: %04X\nIP: %04X\n", instruction, pc);
	pc += 2; //inc pc

	execute(instruction);
}

void Chip8::decTimers()
{
	if(dt > 0)
	{
		--dt;
	}

	if(st > 0)
	{
		--st;
	}
}

void Chip8::setKeys(bool keyStates[16])
{
	for(int i = 0; i < 16; ++i)
	{
		keys[i] = keyStates[i];
	}
}

void Chip8::runInstruction(const twoByte &instruction)
{
	execute(instruction);
}

bool Chip8::drawFlag()
{
	bool temp = screenUpdateNeeded;

	//here I'm assuming that if something asks for the draw flag,
	//then it will draw the screen if it's true, so I can reset the
	//flag.
	if(screenUpdateNeeded)
	{
		screenUpdateNeeded = false;	
	}
	return temp;
}

bool *Chip8::getDisplay()
{
	return display;
}
