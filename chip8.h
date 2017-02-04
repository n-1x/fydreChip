//File: chip8.h
//Author: Nicholas J D Dean
//Date Created: 2017-01-27
//------------------------------------
//This will emulate all actions 
//of the chip 8. Howver to be used it will require some
//functions to be called externally to deal with things
//such as timing, keyboard and display.
//
//Firstly, decTimers() must be called at 60Hz.
//
//Secondly, getDisplay() must be called when drawFlag() == true 
//to retrieve the graphical output. It should be noted that 
//calling drawFlag() sets it to false, because it is assumed
//that if drawFlag() == true, you will draw the screen.
//
//Finally, the object must me sent an array of 16 booleans that 
//holds the state of the keys.
//
//Once these systems are in place, loadRom() then call
//runSingleCycle() as often as possible.
//
//Use soundFlag() to determine if the sound should currently be playing

class Chip8
{
	private:
		typedef unsigned char byte;
		typedef unsigned short twoByte;

		static const int MEMSIZE = 0x1000; //4kb of memory
		static const int STACKSIZE = 16;   //16 stack levels 
		static const int NUM_REGS = 16;    //16 GP registers

		static const int DISP_WIDTH = 64,
					     DISP_HEIGHT = 32,
		                 DISP_SIZE = DISP_WIDTH * DISP_HEIGHT;

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
		     display[DISP_SIZE];  //on/off state of all the pixels
		bool screenUpdateNeeded;

		//initialise all internal storage components
		void loadFont();

		//decode and run an instruction
		void execute(const twoByte &b);
	public:
		Chip8();

		//reset everything to it's default state
		//rom will need to be reloaded
		void hardReset();

		//functions for writing stuff to stdout
		//only used for debugging
		void printStack();
		void printRegs();
		void printMainMemory();
		void printAllMemory(); //prints all except display
		void printDisplay();

		void loadRom(const char *filePath);

		void runSingleCycle(); //1 fetch-decode-execute cycle

		//executes the given instruction
		void runInstruction(const twoByte &instruction);  

		//decrement the delay and sound timers
		void decTimers(); 

		//does the screen need to be redrawn
		bool drawFlag();

		//should the sound be playing
		bool soundFlag(); 		
		
		 //assign the state of all keys
		void setKeys(bool keyStates[16]);		
		
		//return the value of all pixels
		bool *getDisplay(); 
};
