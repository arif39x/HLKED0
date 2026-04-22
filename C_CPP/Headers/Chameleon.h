static const unsigned char reg_eax = 0x01;
static const unsigned char reg_ax = 0x02;
static const unsigned char reg_ah = 0x03;
static const unsigned char reg_al = 0x04;

static const unsigned char reg_ebx = 0x05;
static const unsigned char reg_bx = 0x06;
static const unsigned char reg_bh = 0x07;
static const unsigned char reg_bl = 0x08;

static const unsigned char reg_ecx = 0x09;
static const unsigned char reg_cx = 0x0A;
static const unsigned char reg_ch = 0x0B;
static const unsigned char reg_cl = 0x0C;

static const unsigned char reg_edx = 0x0D;
static const unsigned char reg_dx = 0x0E;
static const unsigned char reg_dh = 0x0F;
static const unsigned char reg_dl = 0x10;

static const unsigned char reg_edi = 0x11;
static const unsigned char reg_di = 0x12;

static const unsigned char reg_esi = 0x13;
static const unsigned char reg_si = 0x14;

static const unsigned char reg_esp = 0x15;
static const unsigned char reg_sp = 0x16;

static const unsigned char reg_ebp = 0x17;
static const unsigned char reg_bp = 0x18;

static const unsigned char opcode_add = 0x19;
static const unsigned char opcode_add_long = 0x1A;
static const unsigned char opcode_add_register = 0x47;

static const unsigned char opcode_sub = 0x1B;
static const unsigned char opcode_sub_long = 0x1C;
static const unsigned char opcode_sub_register = 0x49;

static const unsigned char opcode_mul = 0x1D;
static const unsigned char opcode_mul_long = 0x1E;
static const unsigned char opcode_mul_register = 0x4A;

static const unsigned char opcode_div = 0x1F;
static const unsigned char opcode_div_long = 0x20;
static const unsigned char opcode_div_register = 0x4B;

static const unsigned char opcode_mov_offset = 0x21;
static const unsigned char opcode_mov_register = 0x22;
static const unsigned char opcode_mov_register_long = 0x45;
static const unsigned char opcode_mov_register_pointer = 0x46;

static const unsigned char opcode_ldo = 0x23;
static const unsigned char opcode_ldo_long = 0x24;

static const unsigned char opcode_test = 0x25;
static const unsigned char opcode_test_long = 0x26;

static const unsigned char opcode_pushval_long = 0x27;
static const unsigned char opcode_pushval = 0x28;
static const unsigned char opcode_pushvalptr = 0x29;
static const unsigned char opcode_pushregptr = 0x2A;
static const unsigned char opcode_pushreg = 0x2B;

static const unsigned char opcode_br = 0x2C;
static const unsigned char opcode_be = 0x2D;
static const unsigned char opcode_bne = 0x2E;
static const unsigned char opcode_bg = 0x2F;
static const unsigned char opcode_bge = 0x30;
static const unsigned char opcode_bl = 0x31;
static const unsigned char opcode_ble = 0x32;

static const unsigned char opcode_test_ptr = 0x33;

static const unsigned char opcode_not = 0x34;
static const unsigned char opcode_xor = 0x35;
static const unsigned char opcode_or = 0x36;
static const unsigned char opcode_and = 0x37;
static const unsigned char opcode_shl = 0x38;
static const unsigned char opcode_shr = 0x39;

static const unsigned char opcode_long_xor = 0x40;
static const unsigned char opcode_long_or = 0x41;
static const unsigned char opcode_long_and = 0x42;
static const unsigned char opcode_long_shl = 0x43;
static const unsigned char opcode_long_shr = 0x44;

static const unsigned char opcode_call_kifastsystemcall = 0x48;

typedef unsigned long POINTER;
typedef unsigned long REGISTER;
typedef unsigned long FLAGS;
typedef unsigned long STACK;

typedef struct _VM_CONTEXT {
	union {
		REGISTER eax;
		union
		{
			unsigned char al;
			unsigned char ah;
		};
		unsigned short ax;
	};

	union {
		REGISTER ebx;
		union
		{
			unsigned char bl;
			unsigned char bh;
		};
		unsigned short bx;
	};

	union {
		REGISTER ecx;
		union
		{
			unsigned char cl;
			unsigned char ch;
		};
		unsigned short cx;
	};

	union {
		REGISTER edx;
		union
		{
			unsigned char dl;
			unsigned char dh;
		};
		unsigned short dx;
	};

	union {
		REGISTER esi;
		unsigned short si;
	};

	union {
		REGISTER edi;
		unsigned short di;
	};

	union {
		REGISTER ebp;
		unsigned short bp;
	};

	union {
		REGISTER esp;
		unsigned short sp;
	};

	REGISTER eip;

	FLAGS eflags;
} VM_CONTEXT;

typedef struct _INSTRUCTION
{
	bool DoesExist;
	unsigned char RegisterLocation;
	unsigned short Parameters;

	unsigned char UsesRegister;
} INSTRUCTION;

typedef struct _INSTRUCTION_SET
{
	INSTRUCTION Add;
	INSTRUCTION AddRegister;

	INSTRUCTION Sub;
	INSTRUCTION SubRegister;

	INSTRUCTION Mul;
	INSTRUCTION MulRegister;

	INSTRUCTION Div;
	INSTRUCTION DivRegister;

	INSTRUCTION MovOffset;
	INSTRUCTION MovRegister;
	INSTRUCTION MovRegisterPointer;

	INSTRUCTION Ldo;
	INSTRUCTION Test;

	INSTRUCTION Pushval;
	INSTRUCTION Pushvalptr;
	INSTRUCTION Pushreg;
	INSTRUCTION Pushregptr;

	INSTRUCTION Br;
	INSTRUCTION Be;
	INSTRUCTION Bne;
	INSTRUCTION Bg;
	INSTRUCTION Bge;
	INSTRUCTION Bl;
	INSTRUCTION Ble;

	INSTRUCTION Shr;
	INSTRUCTION Shl;
	INSTRUCTION Xor;
	INSTRUCTION Not;
	INSTRUCTION And;
	INSTRUCTION Or;
} INSTRUCTION_SET, *PINSTRUCTION_SET;

class Chameleon
{
private:
	VM_CONTEXT vmContext;
	INSTRUCTION_SET vmInstructions;

	char stack[8192];

	void add( REGISTER *vmRegister, unsigned long vmValue );
	void sub( REGISTER *vmRegister, unsigned long vmValue );

	void mul( REGISTER *vmRegister, unsigned long vmValue );
	void div( REGISTER *vmRegister, unsigned long vmValue );

	void bnot( REGISTER *vmRegister );
	void bor( REGISTER *vmRegister, unsigned long vmValue );
	void bxor( REGISTER *vmRegister, unsigned long vmValue );
	void band( REGISTER *vmRegister, unsigned long vmValue );
	void bshl( REGISTER *vmRegister, unsigned long vmValue );
	void bshr( REGISTER *vmRegister, unsigned long vmValue );

	void mov( POINTER *realOffset, REGISTER vmRegister );
	void ldo( REGISTER *vmRegister, POINTER *vmOffset );

	void test( REGISTER *vmRegister, unsigned long vmValue );
	void be( unsigned long vmOffset );
	void bne( unsigned long vmOffset );
	void bg( unsigned long vmOffset );
	void bge( unsigned long vmOffset );
	void bl( unsigned long vmOffset );
	void ble( unsigned long vmOffset );
	void br( unsigned long vmOffset );

	void pushreg( REGISTER *vmRegister );
	void pushregptr( REGISTER *vmRegister );
	void pushval( POINTER vmValue );
	void pushvalptr( POINTER *vmValue );
	void pushvallong( POINTER vmValue );
	
	void pop( REGISTER *vmRegister );
	void retn( unsigned short vmValue );

	REGISTER *GetRegister( unsigned char reg );
public:
	VM_CONTEXT *getVM( );
	void *Call( unsigned char *buffer, unsigned long size );
	void Alter( PINSTRUCTION_SET inst );

	Chameleon( PINSTRUCTION_SET inst );
};