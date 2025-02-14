#include <iostream>

#define MEMORY_MAX (1 << 8)

u_int8_t memory[MEMORY_MAX];

enum Registers
{
	A, 		// Akkumulator
	D,		// Datenregister
	PL,		// Pointer-Low
	PH,		// Pointer-High
	FLG,	// Flagregister
	PC,		// ProgramCounter
	SP,		// Stackpointer
	COUNT
};

enum Flags
{
	B = 1 << 0,
	C = 1 << 1,
	E = 1 << 2,
	Z = 1 << 3
};

enum Opcodes
{
	NOP,
	MOV,
	INA,
	STO,
	LOD,
	ALU,
	JIT,	// Jump if true
	JIF,	// Jump if false
	PUSH,
	POP,
	TAK,
	PRN,
	CLS,
	HLT,
	TRP,
	RES
};

// allways uses A and D regs
enum ALU_Opcodes 
{
	ADD,
	SUB,
	INC,
	DEC,
	CMP,
	NOT,
	AND,
	NAND,
	OR,
	NOR,
	XOR,
	XNOR,
	SHL,
	SHR,
	ROL,
	ROR
};

// the registers actually only hold a nibble but c++ can't represent that.
u_int8_t registers[Registers::COUNT];

void read_image_file(FILE* file)
{
    fread(memory, sizeof(u_int8_t), MEMORY_MAX, file);
}

int read_image(const char* image_path)
{
    FILE* file = fopen(image_path, "rb");
    if (!file) { return 0; };
    read_image_file(file);
    fclose(file);
    return 1;
}

void memory_read(u_int8_t address, u_int8_t val)
{
    memory[address] = val;
}

u_int8_t memory_read(u_int8_t address)
{
    return memory[address];
}

int main(int argc, const char* argv[])
{
	if (argc == 0)
	{
		printf("MM4001 [memory-image]\n");
		exit(2);
	}

	if (!read_image(argv[1]))
	{
		printf("failed to load image: %s\n", argv[1]);
		exit(1);
	}

	for (int j = 1; j < argc; ++j)
	{
		if (!read_image(argv[j]))
		{
			printf("failed to load image: %s\n", argv[j]);
			exit(1);
		}
	}

	bool isRunning = true;

	registers[Registers::PC] = 0;
	
	while (isRunning)
	{
		u_int8_t instruction = memory_read(registers[Registers::PC]++);
		u_int8_t opcode = instruction >> 4;
		u_int8_t E1 = (instruction & 0x0c) >> 2;
		u_int8_t E2 = instruction & 0x03;

		system("clear");
		printf("Reg A: %i\n", registers[Registers::A]);
		printf("Reg d: %i\n", registers[Registers::D]);
		printf("Inst: %04X\n", instruction);
		printf("E1: %i ", E1);
		printf("E2: %i\n", E2);
		getchar();

		switch (opcode)
		{
		case Opcodes::NOP:
			break;

		case Opcodes::INA:
			registers[Registers::A] = instruction & 0x0F;
			break;

		case Opcodes::MOV:
			registers[E1] = registers[E2];
			break;
		
		case Opcodes::HLT:
			isRunning = false;
		default:
			//bad OP
			break;
		}

		if (registers[Registers::PC] > 255)
		{
			isRunning = false;
		}
	}

	return EXIT_SUCCESS;
}
