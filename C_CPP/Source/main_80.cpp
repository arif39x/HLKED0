#include "Chameleon.h"
#include "onion.h"
#pragma comment( lib, "Fury32.lib" )

int main( )
{
	INSTRUCTION_SET myInstructionSet;

	myInstructionSet.Add.RegisterLocation = 2;
	myInstructionSet.Add.Parameters = 3;
	myInstructionSet.Add.UsesRegister = reg_eax;
	myInstructionSet.Add.DoesExist = true;

	myInstructionSet.Sub.RegisterLocation = 1;
	myInstructionSet.Sub.Parameters = 2;
	myInstructionSet.Sub.UsesRegister = 0;
	myInstructionSet.Sub.DoesExist = true;

	myInstructionSet.Mul.RegisterLocation = 1;
	myInstructionSet.Mul.Parameters = 2;
	myInstructionSet.Mul.DoesExist = true;

	myInstructionSet.Div.RegisterLocation = 1;
	myInstructionSet.Div.Parameters = 2;
	myInstructionSet.Div.DoesExist = true;

	myInstructionSet.Ldo.RegisterLocation = 1;
	myInstructionSet.Ldo.DoesExist = true;

	myInstructionSet.MovOffset.RegisterLocation = 1;
	myInstructionSet.MovOffset.DoesExist = true;

	myInstructionSet.Pushval.DoesExist = true;

	Chameleon myCPU( &myInstructionSet );

	unsigned long y = 4;
	unsigned long x = 0;
	unsigned char vmBuffer[ ] = {
		opcode_ldo, reg_eax, 0x00, 0x00, 0x00, 0x00,
		opcode_add, 0x35, 0x30,
	};
	*( ( unsigned long * ) &vmBuffer[2] ) = ( unsigned long ) &y;


	myCPU.Call( vmBuffer, ( sizeof( vmBuffer ) / sizeof( vmBuffer[0] ) ) );

	myInstructionSet.Add.UsesRegister = 0;
	myInstructionSet.Add.Parameters = 2;
	myInstructionSet.Add.RegisterLocation = 1;
	myCPU.Alter( &myInstructionSet );
	unsigned char secondVMBuffer[ ] = {
		opcode_add, reg_ecx, 0x10,
		opcode_sub_register, reg_eax, reg_ecx,
		opcode_mul, reg_eax, 0x02,
		opcode_div, reg_eax, 0x04,
		opcode_mov_offset, reg_eax, 0x00, 0x00, 0x00, 0x00,

		//opcode_pushval_long, 0x00, 0x00, 0x00, 0x00,
		//opcode_pushval_long, 0xFF, 0xFF, 0xFF, 0xFF,
		//opcode_pushval_long, 0x01, 0x01, 0x00, 0x00,
		//opcode_call_kifastsystemcall
	};

	*( ( unsigned long * ) &secondVMBuffer[14] ) = ( unsigned long ) &x;

	myCPU.Call( secondVMBuffer, ( sizeof( secondVMBuffer ) / sizeof( secondVMBuffer[0] ) ) );

	return( x );
}