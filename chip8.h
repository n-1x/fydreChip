//File: chip8.h
//Author: Nicholas J D Dean
//Date Created: 2017-01-27
//------------------------------------
//This will emulate all actions 
//of the chip 8. Howver to be used it will require some
//functions to be called externally to deal with things
//such as timing, keyboard and display.
//
//Firstly, you must call decTimers() at 60Hz.
//
//Secondly, you must call getDisplay() when drawFlag() == true 
//to retrieve the graphical output.
//
//Finally, you have to send it an array of booleans that tracks
//the state of the 16 keys that should be attached.
//
//Once these systems are in place, loadRom() then call
//runSingleCycle() as often as possible.

class Chip8
{
	private:
		typedef unsigned char byte;
		typedef unsigned short twoByte;

		static const int MEMSIZE = 0x1000; //4kb of memory
		static const int STACKSIZE = 16;   //16 stack levels 
		static const int NUM_REGS = 16;    //16 GP registers

		static const int DISP_WIDTH = 64,
	                    DISP_HEIGHT = 32;

		static const twoByte PROG_START_ADDR = 0x200;

		byte memory[MEMSIZE];

		byte V[NUM_REGS];         //general purpose registers, V[F] is used for flags
		twoByte pc;               //instruction pointer
		twoByte I;                //index register
		byte sp,                  //stack pointer
		     st,                  //sound timer (dec at 64hz)
		     dt;                  //delay timer (dec at 60hz)
		twoByte stack[STACKSIZE]; //the stack

		bool keys[16],            //holds the state of every key on the keyboard
		     display[64*32];      //on/off state of all the pixels
		bool screenUpdateNeeded;

		//initialise all internal storage components
		void memInit();
		void loadFont();

		//decode and run an instruction
		void execute(const twoByte &b);
	public:
		Chip8();

		//functions for writing stuff to stdout
		//only used for debugging

		void printStack();
		void printRegs();
		void printMainMemory();
		void printAllMemory();	
		void printDisplay();

		void loadRom(const char *filePath);
		void runSingleCycle(); //1 fetch-decode-execute cycle
		void runInstruction(const twoByte &instruction);  

		void decTimers(); //decrement the delay and sound timers
		bool drawFlag();  //does the screen need to be redrawn
		void setKeys(bool keyStates[16]); //assign the state of all keys
		bool *getDisplay();
};
