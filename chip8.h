class Chip8
{
	private:
		typedef unsigned char byte;
		typedef unsigned short twoByte;

		static const int MEMSIZE = 0x1000; //4kb of memory
		static const int STACKSIZE = 16;   //16 stack levels 
		static const int NUM_REGS = 16;    //16 GP registers

		static const twoByte PROG_START_ADDR = 0x200;

		byte memory[MEMSIZE];

		byte V[NUM_REGS];         //general purpose registers, V[F] is used for flags
		twoByte ip;               //instruction pointer
		twoByte I;                //index register
		byte sp,                  //stack pointer
		     st,                  //sound timer (dec at 64hz)
		     dt;                  //delay timer (dec at 60hz)
		twoByte stack[STACKSIZE]; //the stack

		bool keys[16];            //holds the state of every key on the keyboard
		
		//initialise all internal storage components
		void memInit();
		void loadFont();

		//decode and run an instruction
		void execute(const twoByte &b);
	public:
	
		Chip8();
		void printMemory();	
		void loadRom(char *filePath);
		void runSingleInstruction(); //1 fetch-decode-execute cycle
};
