#include "Chameleon.h"
#include "onion.h"

VM_CONTEXT *Chameleon::getVM( )
{
	return( &vmContext );
}

Chameleon::Chameleon( PINSTRUCTION_SET inst )
{
	
	vmContext.eax = 0;
	vmContext.ebx = 0;
	vmContext.ecx = 0;
	vmContext.edx = 0;
	vmContext.edi = 0;
	vmContext.esi = 0;
	vmContext.esp = ( STACK ) 4096;
	vmContext.ebp = 0;

	vmInstructions = *inst;
}

void Chameleon::Alter( PINSTRUCTION_SET inst )
{
	vmInstructions = *inst;
}

void *Chameleon::Call( unsigned char *vmBuffer, unsigned long vmSize )
{
	POINTER offset;
	unsigned int i = 0;

	vmContext.eip = 0;
	while( vmContext.eip < vmSize )
	{
		switch( vmBuffer[vmContext.eip] )
		{
		case opcode_add_long:
			if( vmInstructions.Add.RegisterLocation == 1 && vmInstructions.Add.UsesRegister )
			{
				for( i = 0; ( i + 1 ) < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmInstructions.Add.UsesRegister ), vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Add.RegisterLocation == 1 && !vmInstructions.Add.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 0; ( i + 1 ) < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Add.UsesRegister )
			{
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmInstructions.Add.UsesRegister ), vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}
			else
			{
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Add.Parameters] ), vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}

			break;
		case opcode_add:
			if( vmInstructions.Add.RegisterLocation == 1 && vmInstructions.Add.UsesRegister )
			{
				for( i = 0; ( i + 1 ) < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmInstructions.Add.UsesRegister ), vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Add.Parameters + 1;
			}
			else if( vmInstructions.Add.RegisterLocation == 1 && !vmInstructions.Add.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Add.Parameters + 1;
			}
			else if( vmInstructions.Add.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Add.UsesRegister );
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Add.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Add.Parameters] );
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Add.Parameters;
			}

			break;
		case opcode_add_register:
			if( vmInstructions.AddRegister.RegisterLocation == 1 && vmInstructions.Add.UsesRegister )
			{
				for( i = 0; ( i + 1 ) < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmInstructions.Add.UsesRegister ), *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Add.Parameters;
			}
			else if( vmInstructions.AddRegister.RegisterLocation == 1 && !vmInstructions.Add.UsesRegister )
			{
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + 1] ), *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Add.Parameters + 1;
			}
			else if( vmInstructions.Add.UsesRegister )
			{
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmInstructions.Add.UsesRegister ), *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + i] ) ) );

				vmContext.eip += vmInstructions.Add.Parameters;
			}
			else
			{
				for( i = 1; i < vmInstructions.Add.Parameters; i++ )
					add( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + 2] ), *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + i] ) ) );
				
				vmContext.eip += vmInstructions.Add.Parameters + 1;
			}

			break;
		case opcode_sub_long:
			if( vmInstructions.Sub.RegisterLocation == 1 && vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Sub.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Sub.RegisterLocation == 1 && !vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Sub.UsesRegister );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Sub.Parameters] );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}

			break;
		case opcode_sub:
			if( vmInstructions.Sub.RegisterLocation == 1 && vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Sub.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Sub.Parameters + 1;
			}
			else if( vmInstructions.Sub.RegisterLocation == 1 && !vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Sub.Parameters + 1;
			}
			else if( vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Sub.UsesRegister );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Sub.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Sub.Parameters] );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Sub.Parameters;
			}

			break;
		case opcode_sub_register:
			if( vmInstructions.SubRegister.RegisterLocation == 1 && vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Sub.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Sub.Parameters;
			}
			else if( vmInstructions.SubRegister.RegisterLocation == 1 && !vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Sub.Parameters + 1;
			}
			else if( vmInstructions.Sub.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Sub.UsesRegister );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + i] ) ) );

				vmContext.eip += vmInstructions.Sub.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Sub.Parameters; i++ )
					sub( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );
				
				vmContext.eip += vmInstructions.Sub.Parameters + 1;
			}

			break;
		case opcode_mul_long:
			if( vmInstructions.Mul.RegisterLocation == 1 && vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Mul.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Mul.RegisterLocation == 1 && !vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Mul.UsesRegister );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Mul.Parameters] );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}

			break;
		case opcode_mul:
			if( vmInstructions.Mul.RegisterLocation == 1 && vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Mul.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Mul.Parameters + 1;
			}
			else if( vmInstructions.Mul.RegisterLocation == 1 && !vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Mul.Parameters + 1;
			}
			else if( vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Mul.UsesRegister );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Mul.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Mul.Parameters] );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Mul.Parameters;
			}

			break;
		case opcode_mul_register:
			if( vmInstructions.MulRegister.RegisterLocation == 1 && vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Mul.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Mul.Parameters;
			}
			else if( vmInstructions.MulRegister.RegisterLocation == 1 && !vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Mul.Parameters + 1;
			}
			else if( vmInstructions.Mul.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Mul.UsesRegister );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + i] ) ) );

				vmContext.eip += vmInstructions.Mul.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Mul.Parameters; i++ )
					mul( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );
				
				vmContext.eip += vmInstructions.Mul.Parameters + 1;
			}

			break;
		case opcode_div_long:
			if( vmInstructions.Div.RegisterLocation == 1 && vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Div.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Div.RegisterLocation == 1 && !vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += 2 + sizeof( unsigned long );
			}
			else if( vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Div.UsesRegister );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Div.Parameters] );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += 1 + sizeof( unsigned long );
			}

			break;
		case opcode_div:
			if( vmInstructions.Div.RegisterLocation == 1 && vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Div.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Div.Parameters + 1;
			}
			else if( vmInstructions.Div.RegisterLocation == 1 && !vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + 1 + i] );

				vmContext.eip += vmInstructions.Div.Parameters + 1;
			}
			else if( vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Div.UsesRegister );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Div.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + vmInstructions.Div.Parameters] );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, vmBuffer[vmContext.eip + i] );

				vmContext.eip += vmInstructions.Div.Parameters;
			}

			break;
		case opcode_div_register:
			if( vmInstructions.DivRegister.RegisterLocation == 1 && vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Div.UsesRegister );
				for( i = 0; ( i + 1 ) < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Div.Parameters;
			}
			else if( vmInstructions.DivRegister.RegisterLocation == 1 && !vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );

				vmContext.eip += vmInstructions.Div.Parameters + 1;
			}
			else if( vmInstructions.Div.UsesRegister )
			{
				offset = ( POINTER ) GetRegister( vmInstructions.Div.UsesRegister );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + i] ) ) );

				vmContext.eip += vmInstructions.Div.Parameters;
			}
			else
			{
				offset = ( POINTER ) GetRegister( vmBuffer[vmContext.eip + 1] );
				for( i = 1; i < vmInstructions.Div.Parameters; i++ )
					div( ( REGISTER * ) offset, *( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + i] ) ) );
				
				vmContext.eip += vmInstructions.Div.Parameters + 1;
			}

			break;
		case opcode_mov_offset:
			if( vmInstructions.MovOffset.DoesExist == false )
			{
			}
			else if( vmInstructions.MovOffset.RegisterLocation == 1 )
			{
				offset = *( ( unsigned long * ) &vmBuffer[vmContext.eip + 2] );
				mov( ( POINTER * ) offset, *( GetRegister( vmBuffer[vmContext.eip + 1] ) ) );
			}
			else
			{
				offset = *( ( unsigned long * ) &vmBuffer[vmContext.eip + 1] );
				mov( ( POINTER * ) offset, *( GetRegister( vmBuffer[vmContext.eip + 1 + sizeof( unsigned long )] ) ) );
			}

			vmContext.eip += 2 + sizeof( unsigned long );
			break;
		case opcode_mov_register_long:
			if( vmInstructions.MovOffset.DoesExist == false )
			{
			}
			else if( vmInstructions.MovOffset.RegisterLocation == 1 )
			{
				*( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1] ) ) = *( ( unsigned long * ) &vmBuffer[vmContext.eip + 2] );
			}
			else
			{
				*( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + sizeof( unsigned long )] ) ) = *( ( unsigned long * ) &vmBuffer[vmContext.eip + 1] );
			}

			vmContext.eip += 2 + sizeof( unsigned long );
			break;
		case opcode_mov_register:
			if( vmInstructions.MovOffset.DoesExist == false )
			{
			}
			else if( vmInstructions.MovOffset.RegisterLocation == 1 )
			{
				*( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1] ) ) = vmBuffer[vmContext.eip + 2];
			}
			else
			{
				*( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 2] ) ) = vmBuffer[vmContext.eip + 1];
			}

			vmContext.eip += 3;
			break;
		case opcode_mov_register_pointer:
			if( vmInstructions.MovOffset.DoesExist == false )
			{
			}
			else if( vmInstructions.MovOffset.RegisterLocation == 1 )
			{
				*( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1] ) ) = **( ( unsigned long ** ) &vmBuffer[vmContext.eip + 2] );
			}
			else
			{
				*( ( unsigned long * ) GetRegister( vmBuffer[vmContext.eip + 1 + sizeof( unsigned long )] ) ) = **( ( unsigned long ** ) &vmBuffer[vmContext.eip + 1] );
			}

			vmContext.eip += 2 + sizeof( unsigned long );
			break;
		case opcode_test_long:
			test( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_test:
			test( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 3;
			break;
		case opcode_br:
			br( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_be:
			be( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_bne:
			bne( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_bg:
			bg( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_bge:
			bge( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_bl:
			bl( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_ble:
			ble( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_ldo:
			if( vmInstructions.Ldo.DoesExist == false )
			{
			}
			else if( vmInstructions.Ldo.RegisterLocation == 1 )
			{
				offset = *( ( unsigned long * ) &vmBuffer[vmContext.eip + 2] );
				ldo( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + 1] ), ( POINTER * ) offset );
			}
			else
			{
				offset = *( ( unsigned long * ) &vmBuffer[vmContext.eip + 1] );
				ldo( ( REGISTER * ) GetRegister( vmBuffer[vmContext.eip + 1 + sizeof( unsigned long )] ), ( POINTER * ) offset );
			}

			vmContext.eip += 2 + sizeof( unsigned long );
			break;
		case opcode_not:
			if( vmInstructions.Not.DoesExist == true )
				bnot( GetRegister( vmBuffer[vmContext.eip + 1] ) );

			vmContext.eip += 2;
			break;
		case opcode_long_or:
			if( vmInstructions.Or.DoesExist == true )
				bor( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_or:
			if( vmInstructions.Or.DoesExist == true )
				bor( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 3;
			break;
		case opcode_long_xor:
			if( vmInstructions.Xor.DoesExist == true )
				bxor( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_xor:
			if( vmInstructions.Xor.DoesExist == true )
				bxor( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 3;
			break;
		case opcode_and:
			if( vmInstructions.And.DoesExist == true )
				band( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 3;
			break;
		case opcode_long_and:
			if( vmInstructions.And.DoesExist == true )
				band( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_long_shr:
			if( vmInstructions.Shr.DoesExist == true )
				bshr( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_shr:
			if( vmInstructions.Shr.DoesExist == true )
				bshr( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 3;
			break;
		case opcode_long_shl:
			if( vmInstructions.Shl.DoesExist == true )
				bshl( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_shl:
			if( vmInstructions.Shl.DoesExist == true )
				bshl( GetRegister( vmBuffer[vmContext.eip + 1] ), vmBuffer[vmContext.eip + 2] );

			vmContext.eip += 3;
			break;
		case opcode_pushval_long:
			if( vmInstructions.Pushval.DoesExist == true )
				pushvallong( *( ( unsigned long * ) &vmBuffer[vmContext.eip + 1] ) );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_pushval:
			if( vmInstructions.Pushval.DoesExist == true )
				pushval( vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 2;
			break;
		case opcode_pushvalptr:
			if( vmInstructions.Pushvalptr.DoesExist == true )
				pushvalptr( ( POINTER * ) vmBuffer[vmContext.eip + 1] );

			vmContext.eip += 1 + sizeof( unsigned long );
			break;
		case opcode_pushregptr:
			if( vmInstructions.Pushregptr.DoesExist == true )
				pushregptr( ( REGISTER * ) **( ( unsigned long ** ) GetRegister( vmBuffer[vmContext.eip + 1] ) ) );

			vmContext.eip += 2;
			break;
		case opcode_pushreg:
			if( vmInstructions.Pushreg.DoesExist == true )
				pushreg( ( REGISTER * ) *( GetRegister( vmBuffer[vmContext.eip + 1] ) ) );

			vmContext.eip += 2;
			break;
		case opcode_call_kifastsystemcall:
			KiFastSystemCall( vmContext.esp - 4 );
			break;
		}
	}
	return( ( void * ) vmContext.eax );
}

REGISTER *Chameleon::GetRegister( unsigned char reg )
{
	switch( reg )
	{
	case reg_eax:
		return( &vmContext.eax );

	case reg_ax:
		return( ( REGISTER * ) &vmContext.ax );
		
	case reg_ah:
		return( ( REGISTER * ) &vmContext.ah );

	case reg_al:
		return( ( REGISTER * ) &vmContext.al );

	case reg_ebx:
		return( &vmContext.ebx );

	case reg_bx:
		return( ( REGISTER * ) &vmContext.bx );

	case reg_bh:
		return( ( REGISTER * ) &vmContext.bh );

	case reg_bl:
		return( ( REGISTER * ) &vmContext.bl );

	case reg_ecx:
		return( &vmContext.ecx );

	case reg_cx:
		return( ( REGISTER * ) &vmContext.cx );

	case reg_ch:
		return( ( REGISTER * ) &vmContext.ch );

	case reg_cl:
		return( ( REGISTER * ) &vmContext.cl );

	case reg_edx:
		return( &vmContext.edx );

	case reg_dx:
		return( ( REGISTER * ) &vmContext.dx );

	case reg_dh:
		return( ( REGISTER * ) &vmContext.dh );

	case reg_dl:
		return( ( REGISTER * ) &vmContext.dl );

	case reg_edi:
		return( &vmContext.edi );

	case reg_di:
		return( ( REGISTER * ) &vmContext.di );

	case reg_esi:
		return( &vmContext.esi );

	case reg_si:
		return( ( REGISTER * ) &vmContext.si );

	case reg_esp:
		return( &vmContext.esp );

	case reg_sp:
		return( ( REGISTER * ) &vmContext.sp );

	case reg_ebp:
		return( &vmContext.ebp );

	case reg_bp:
		return( ( REGISTER * ) &vmContext.bp );

	default:
		return( 0 );
	}
}

void Chameleon::add( REGISTER *vmRegister, unsigned long vmValue )
{
	if( vmInstructions.Add.DoesExist != false )
		*vmRegister = ( *vmRegister ^ vmValue ) | ( ( *vmRegister & vmValue ) << 1 );
}

void Chameleon::sub( REGISTER *vmRegister, unsigned long vmValue )
{
	if( vmInstructions.Sub.DoesExist != false )
		*vmRegister = ( *vmRegister ) + ( ~vmValue + 1 );
}

void Chameleon::mul( REGISTER *vmRegister, unsigned long vmValue )
{
	if( !vmValue )
		*vmRegister = 0;
	else if( vmValue % 2 )
		*vmRegister = ( *vmRegister << ( vmValue / 2 ) ) + 2;
	else
		*vmRegister = *vmRegister << ( vmValue / 2 );
}

void Chameleon::div( REGISTER *vmRegister, unsigned long vmValue )
{
	if( !vmValue )
		*vmRegister = 0;
	else if( vmValue % 2 )
		*vmRegister = ( *vmRegister >> ( vmValue / 2 ) ) + 2;
	else
		*vmRegister = ( *vmRegister >> ( vmValue / 2 ) );
}

void Chameleon::bnot( REGISTER *vmRegister )
{
	*vmRegister = ( *vmRegister * -1 ) - 1;
}

void Chameleon::bor( REGISTER *vmRegister, unsigned long vmValue )
{
	*vmRegister = *vmRegister | vmValue;
}

void Chameleon::bxor( REGISTER *vmRegister, unsigned long vmValue )
{
	*vmRegister = *vmRegister ^ vmValue;
}

void Chameleon::band( REGISTER *vmRegister, unsigned long vmValue )
{
	*vmRegister = *vmRegister & vmValue;
}

void Chameleon::bshl( REGISTER *vmRegister, unsigned long vmValue )
{
	*vmRegister = ( *vmRegister * ( vmValue * 2 ) );
}

void Chameleon::bshr( REGISTER *vmRegister, unsigned long vmValue )
{
	*vmRegister = ( *vmRegister / ( vmValue * 2 ) );
}

void Chameleon::mov( POINTER *realOffset, REGISTER vmRegister )
{
	*realOffset = vmRegister;
}

void Chameleon::ldo( REGISTER *vmRegister, POINTER *realOffset )
{
	*vmRegister = *realOffset;
}

void Chameleon::test( REGISTER *vmRegister, unsigned long vmValue )
{
	if( *vmRegister != vmValue )
		vmContext.eflags |= 0x860;

	if( *vmRegister == vmValue )
		vmContext.eflags |= 0x640;

	if( *vmRegister >= vmValue )
		vmContext.eflags |= 0x530;

	if( *vmRegister <= vmValue )
		vmContext.eflags |= 0x420;

	if( *vmRegister < vmValue )
		vmContext.eflags |= 0x380;

	if( *vmRegister > vmValue )
		vmContext.eflags |= 0x250;
}

void Chameleon::be( unsigned long vmOffset )
{
	if( vmContext.eflags == ( vmContext.eflags | 0x640 ) )
		vmContext.eip += vmOffset;
}

void Chameleon::bne( unsigned long vmOffset )
{
	if( vmContext.eflags == ( vmContext.eflags | 0x860 ) )
		vmContext.eip += vmOffset;
}

void Chameleon::bge( unsigned long vmOffset )
{
	if( vmContext.eflags == ( vmContext.eflags | 0x530 ) )
		vmContext.eip += vmOffset;
}

void Chameleon::bg( unsigned long vmOffset )
{
	if( vmContext.eflags == ( vmContext.eflags | 0x250 ) )
		vmContext.eip += vmOffset;
}

void Chameleon::ble( unsigned long vmOffset )
{
	if( vmContext.eflags == ( vmContext.eflags | 0x420 ) )
		vmContext.eip += vmOffset;
}

void Chameleon::bl( unsigned long vmOffset )
{
	if( vmContext.eflags == ( vmContext.eflags | 0x380 ) )
		vmContext.eip += vmOffset;
}

void Chameleon::br( unsigned long vmOffset )
{
	vmContext.eip += vmOffset;
}

void Chameleon::pushreg( REGISTER *vmRegister )
{
	vmContext.esp -= sizeof( unsigned long );
	*( ( unsigned long * ) vmContext.esp ) = ( unsigned long ) vmRegister;
}

void Chameleon::pushregptr( REGISTER *vmRegister )
{
	vmContext.esp -= sizeof( unsigned long );
	*( ( unsigned long * ) vmContext.esp ) = *vmRegister;
}

void Chameleon::pushval( POINTER vmValue )
{
	vmContext.esp -= 1;
	*( ( unsigned long * ) vmContext.esp ) = vmValue;
}

void Chameleon::pushvallong( POINTER vmValue )
{
	vmContext.esp -= sizeof( unsigned long );
	*( ( POINTER * ) vmContext.esp ) = vmValue;
}

void Chameleon::pushvalptr( POINTER *vmValue )
{
	vmContext.esp -= sizeof( unsigned long );
	*( ( unsigned long * ) vmContext.esp ) = *vmValue;
}

void Chameleon::pop( REGISTER *vmRegister )
{
	vmContext.esp += sizeof( unsigned long );
	*vmRegister = *( ( unsigned long * ) vmContext.esp - sizeof( unsigned long ) );
}

void Chameleon::retn( unsigned short vmValue )
{
	vmContext.esp -= sizeof( unsigned long );
	br( vmContext.esp - sizeof( unsigned long ) );
}