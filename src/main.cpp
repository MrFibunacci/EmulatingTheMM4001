#include <iostream>

#define MEMORY_MAX (1 << 8)

u_int8_t memory[MEMORY_MAX];
u_int8_t ram[MEMORY_MAX];

enum Reg
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
	B = 1 << 3,	// borrow (overflow on subtraction)
	C = 1 << 2,	// carry (overflow on addition)
	G = 1 << 1,	// greater (on cmp)
	E = 1 << 0  // equal (on cmp)
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
u_int8_t registers[Reg::COUNT];

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

void memory_write(u_int8_t address, u_int8_t val)
{
    memory[address] = val;
}

u_int8_t memory_read(u_int8_t address)
{
    return memory[address];
}

void ram_write(u_int8_t address, u_int8_t val)
{
    ram[address] = val;
}

u_int8_t ram_read(u_int8_t address)
{
    return ram[address];
}

// return both Pointer registers as one 8-Bit value
u_int8_t pointerHL()
{
	return (u_int8_t) (registers[Reg::PH] << 4) + registers[Reg::PL];
}

// save 8-Bit val to both Pointer registers
void pointerHL_store(u_int8_t val)
{
	registers[Reg::PH] = val >> 4;
	registers[Reg::PL] = val & 0x0f;
}

void autoIncDecPointer(u_int8_t E)
{
	u_int8_t pointerReg = pointerHL();

	if (E == 1)
	{
		pointerHL_store(++pointerReg);
	} else if (E == 2) 
	{
		pointerHL_store(--pointerReg);
	}
}

void printRAM()
{
	printf("RAM Content (16x16): \n");
	for (int i = 0; i < MEMORY_MAX; i++)
	{
		printf("%01x ", ram[i]);
		if(i%16 == 15) {printf("\n");}
	}
	printf("\n\n");
}

void setFlag(Flags flag, bool val = 1) 
{
	if(val) {
		registers[Reg::FLG] = registers[Reg::FLG] | flag;
	} else {
		registers[Reg::FLG] = registers[Reg::FLG] & ~(flag);
	}
}

bool getFlag(Flags flag)
{
	u_int8_t flagPos;

	if (flag == Flags::B) {flagPos = 3;}
	else if (flag == Flags::C) {flagPos = 2;}
	else if (flag == Flags::G) {flagPos = 1;}
	else if (flag == Flags::E) {flagPos = 0;}

	return (registers[Reg::FLG] >> flagPos) & 0x1;
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

	registers[Reg::PC] = 0;
	
	while (isRunning)
	{
		u_int8_t instruction = memory_read(registers[Reg::PC]++);
		u_int8_t opcode = instruction >> 4;
		u_int8_t E1 = (instruction & 0x0c) >> 2;
		u_int8_t E2 = instruction & 0x03;

		system("clear");
		printRAM();
		printf("Reg A: %01X\n", registers[Reg::A]);
		printf("Reg d: %01X\n", registers[Reg::D]);
		printf("Reg PH/L: %02X", pointerHL());
		printf(" - Adress value: %01X\n", ram_read(pointerHL()));
		printf("Reg SP: %01X\n", registers[Reg::SP]);
		printf("Reg PC: %01X\n", registers[Reg::PC]);
		printf("Reg FLG: %01X\n", registers[Reg::FLG]);
		printf("B C G E\n");
		printf("%i %i %i %i", getFlag(Flags::B), getFlag(Flags::C), getFlag(Flags::G), getFlag(Flags::E));
		printf("\nInst: %02X\n", instruction);
		printf("E1: %i ", E1);
		printf("E2: %i\n", E2);
		getchar();

		switch (opcode)
		{
		case Opcodes::NOP:
			break;

		case Opcodes::INA:
			registers[Reg::A] = instruction & 0x0F;
			break;

		case Opcodes::MOV:
			registers[E1] = registers[E2];
			break;

		case Opcodes::STO:
			ram_write(pointerHL(), registers[E2]);
			autoIncDecPointer(E1);
			break;

		case Opcodes::LOD:
			registers[E1] = ram_read(pointerHL());
			autoIncDecPointer(E2);
			break;

		case Opcodes::ALU:
			switch ((E1 << 2)+E2)
			{
			case ALU_Opcodes::ADD:
				registers[Reg::A] = registers[Reg::A] + registers[Reg::D];
				if (registers[Reg::A] >> 4 == true)  { setFlag(Flags::C); }
				registers[Reg::A] = registers[Reg::A] & 0x0f;
				break;
			case ALU_Opcodes::SUB:
				registers[Reg::A] = registers[Reg::A] - registers[Reg::D];
				if (registers[Reg::A] >> 4 & 0x1 == true)  { setFlag(Flags::B); }
				registers[Reg::A] = registers[Reg::A] & 0x0f;
				break;
			case ALU_Opcodes::INC:
				registers[Reg::A] = registers[Reg::A] + 1;
				if (registers[Reg::A] >> 4 == true)  { setFlag(Flags::C); }
				registers[Reg::A] = registers[Reg::A] & 0x0f;
				break;
			case ALU_Opcodes::DEC:
				registers[Reg::A] = registers[Reg::A] - 1;
				if (registers[Reg::A] >> 4 & 0x1 == true)  { setFlag(Flags::B); }
				registers[Reg::A] = registers[Reg::A] & 0x0f;
				break;
			case ALU_Opcodes::CMP:
				if (registers[Reg::A] > registers[Reg::D]) { setFlag(Flags::G); }
				if (registers[Reg::A] == registers[Reg::D]) { setFlag(Flags::E); }
				break;
			case ALU_Opcodes::NOT:
				registers[Reg::A] = ~registers[Reg::A];
				break;
			case ALU_Opcodes::AND:
				registers[Reg::A] = registers[Reg::A] & registers[Reg::D];
				break;
			case ALU_Opcodes::NAND:
				registers[Reg::A] = ~(registers[Reg::A] & registers[Reg::D]);
				break;
			case ALU_Opcodes::OR:
				registers[Reg::A] = registers[Reg::A] | registers[Reg::D];
				break;
			case ALU_Opcodes::NOR:
				registers[Reg::A] = ~(registers[Reg::A] | registers[Reg::D]);
				break;
			case ALU_Opcodes::XOR:
				registers[Reg::A] = registers[Reg::A] ^ registers[Reg::D];
				break;
			case ALU_Opcodes::XNOR:
				registers[Reg::A] = ~(registers[Reg::A] ^ registers[Reg::D]);
				break;
			case ALU_Opcodes::SHL:
				registers[Reg::A] = registers[Reg::A] << registers[Reg::D];
				break;
			case ALU_Opcodes::SHR:
				registers[Reg::A] = registers[Reg::A] >> registers[Reg::D];
				break;
			case ALU_Opcodes::ROL:
				registers[Reg::A] = (registers[Reg::A] << registers[Reg::D])|(registers[Reg::A] >> (4 - registers[Reg::D]));
				break;
			case ALU_Opcodes::ROR:
				registers[Reg::A] = (registers[Reg::A] >> registers[Reg::D])|(registers[Reg::A] << (4 - registers[Reg::D]));
				break;
			default:
				break;
			}
			break;

		case Opcodes::JIT:
			if (getFlag((Flags)((E1 << 2)+E2))) { registers[Reg::PC] = pointerHL(); }
			break;
			// TODO: should flag be reset after jump occours?
		case Opcodes::JIF:
			if (!getFlag((Flags)((E1 << 2)+E2))) { registers[Reg::PC] = pointerHL(); }
			break;
		case Opcodes::HLT:
			isRunning = false;
			break;
		default:
			//bad OP
			break;
		}

		if (registers[Reg::PC] > 255)
		{
			isRunning = false;
		}
	}

	return EXIT_SUCCESS;
}