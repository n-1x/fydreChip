class Chip8
{
	private:
		typedef unsigned char byte;
		typedef unsigned short twoByte;

		static const int MEMSIZE = 4096;
		static const int STACKSIZE = 16;
		static const int NUM_REGS = 16;

		static const twoByte PROG_START_ADDR = 0x200;

		byte memory[MEMSIZE];

		twoByte registers[NUM_REGS];  //general purpose registers
		twoByte ip;          //instruction pointer
		twoByte I;           //Holds an instruction
		byte sp,             //stack pointer
		     st,             //sound timer (dec at 64hz)
		     dt;             //delay timer (dec at 60hz)
		twoByte stack[STACKSIZE];
		
		//initialise all internal storage components
		void memInit();

		//decode and run an instruction
		void execute(const twoByte &b);
	public:
	
		Chip8();
		void printMemory();	
		void loadRom(char *filePath);
		void runSingleInstruction(); //1 fetch-decode-execute cycle
};
