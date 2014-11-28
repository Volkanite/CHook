#include "mlde32.h"

#define       O_UNIQUE           0
#define       O_PREFIX           1
#define       O_IMM8             2
#define       O_IMM16            3
#define       O_IMM24            4
#define       O_IMM32            5
#define       O_IMM48            6
#define       O_MODRM            7
#define       O_MODRM8           8
#define       O_MODRM32          9
#define       O_EXTENDED        10
#define       O_WEIRD           11
#define       O_ERROR           12

int __declspec(naked) __cdecl mlde32(char *buf)
{
	__asm {
		; ****************
			; *Opcode types *
			; ****************

			//.code

			//public C mlde32
			; int __cdecl mlde32(void *codeptr);
	mlde32:
		pushad

			cld
			xor  edx, edx

			mov  esi, [esp + (8 * 4) + 4]
			mov  ebp, esp

			; 256 bytes, index - compressed opcode type table
			push 01097F71Ch
			push 0F71C6780h
			push 017389718h
			push 0101CB718h
			push 017302C17h
			push 018173017h
			push 0F715F547h
			push 04C103748h
			push 0272CE7F7h
			push 0F7AC6087h
			push 01C121C52h
			push 07C10871Ch
			push 0201C701Ch
			push 04767602Bh
			push 020211011h
			push 040121625h
			push 082872022h
			push 047201220h
			push 013101419h
			push 018271013h
			push 028858260h
			push 015124045h
			push 05016A0C7h
			push 028191812h
			push 0F2401812h
			push 019154127h
			push 050F0F011h
			mov  ecx, 015124710h
			push ecx
			push 011151247h
			push 010111512h
			push 047101115h
			mov  eax, 012472015h
			push eax
			push eax
			push 012471A10h
			add  cl, 10h
			push ecx
			sub  cl, 20h
			push ecx

			xor  ecx, ecx
			dec  ecx

			; code starts
		__ps : inc  ecx
			   mov  edi, esp
		   __go : lodsb
				  mov  bh, al
			  __ft : mov  ah, [edi]
					 inc  edi
					 shr  ah, 4
					 sub  al, ah
					 jnc  __ft

					 mov  al, [edi - 1]
					 and  al, 0Fh

					 cmp  al, O_ERROR
					 jnz  __i7

					 pop  edx
					 not  edx

				 __i7 : inc  edx
						cmp  al, O_UNIQUE
						jz   __t_exit

						cmp  al, O_PREFIX
						jz   __ps

						add  edi, 51h; (___ettbl - ___ttbl)

						cmp  al, O_EXTENDED
						jz   __go

						mov  edi, [ebp + (8 * 4) + 4]

					__i6:  inc  edx
						   cmp  al, O_IMM8
						   jz   __t_exit
						   cmp  al, O_MODRM
						   jz   __t_modrm
						   cmp  al, O_WEIRD
						   jz   __t_weird

					   __i5 : inc  edx
							  cmp  al, O_IMM16
							  jz   __t_exit
							  cmp  al, O_MODRM8
							  jz   __t_modrm

						  __i4 : inc  edx
								 cmp  al, O_IMM24
								 jz   __t_exit

							 __i3 : inc  edx
								__i2 : inc  edx

									   pushad
									   mov  al, 66h
									   repnz scasb
									   popad
									   jnz  __c32

								   __d2 : dec  edx
										  dec  edx

									  __c32 : cmp  al, O_MODRM32
											  jz   __t_modrm
											  sub  al, O_IMM32
											  jz   __t_imm32

										  __i1 : inc  edx

											 __t_exit :
		mov  esp, ebp
			mov[esp + (7 * 4)], edx
			popad
			ret

			; *********************************
			; *PROCESS THE MOD / RM BYTE       *
			; **
			; *7    6 5          3 2    0 *
			; *| MOD | Reg / Opcode | R / M | *
			; **
			; *********************************
		__t_modrm:
		lodsb
			mov  ah, al
			shr  al, 7
			jb   __prmk
			jz   __prm

			add  dl, 4

			pushad
			mov  al, 67h
			repnz scasb
			popad
			jnz  __prm

		__d3 : sub  dl, 3

			   dec  al
		   __prmk : jnz  __t_exit
					inc  edx
					inc  eax
				__prm :
		and  ah, 00000111b

			pushad
			mov  al, 67h
			repnz scasb
			popad
			jz   __prm67chk

			cmp  ah, 04h
			jz   __prmsib

			cmp  ah, 05h
			jnz  __t_exit

		__prm5chk :
		dec  al
			jz   __t_exit
		__i42 : add  dl, 4
				jmp  __t_exit

			__prm67chk :
		cmp  ax, 0600h
			jnz  __t_exit
			inc  edx
			jmp  __i1

		__prmsib :
		cmp  al, 00h
			jnz  __i1
			lodsb
			and  al, 00000111b
			sub  al, 05h
			jnz  __i1
			inc  edx
			jmp  __i42

			; ****************************
			; *PROCESS WEIRD OPCODES    *
			; **
			; *Fucking test(F6h / F7h)   *
			; **
			; ****************************
		__t_weird:
		test byte ptr[esi], 00111000b
			jnz  __t_modrm

			mov  al, O_MODRM8

			shr  bh, 1
			adc  al, 0
			jmp  __i5

			; *********************************
			; *PROCESS SOME OTHER SHIT       *
			; **
			; *Fucking mov(A0h / A1h / A2h / A3h) *
			; **
			; *********************************
		__t_imm32:
		sub  bh, 0A0h

			cmp  bh, 04h
			jae  __d2

			pushad
			mov  al, 67h
			repnz scasb
			popad
			jnz  __chk66t

		__d4 : dec  edx
			   dec  edx

		   __chk66t :
		pushad
			mov  al, 66h
			repnz scasb
			popad
			jz   __i1
			jnz  __d2
	}
}