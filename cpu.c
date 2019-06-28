#include "cpu.h"
#include "bit_functions.h"
#include "memory_system.h"
#include <stdio.h>
#include <stdlib.h>

int registers[16]; //holds the 16 registers
int cpsr; //current program status register

void set_reg(int reg, int value){
	registers[reg] = value;
}

int get_reg(int reg){
	return registers[reg];
}
	
int get_cpsr(){
	return cpsr;
}

void show_regs(){
	for(int reg = 0; reg < 16; reg++){
		printf("reg%d: 0x%08x\n", reg, registers[reg]);
	}
}

/* 
implicitely returns register and address for instructions
that follow pattern opcode (0-7), reg (8-15), address (16-31) 
*/
void get_reg_address(int instr, int* reg, int* address){
	*reg = instr >> 16 & 0xff;
	*address = instr & 0xffff;
}

// exits program if register invalid.
void check_reg(int reg){
	if (reg > 15){
			printf("register is out of bounds\n");
			exit(1);
	}
}

// exits program if address invalid.
void check_address(int address){
	if (address > 1023){
			printf("address is out of bounds\n");
			exit(1);
	}
}

/* 
implicitely returns destReg, reg1, reg for instructions
that follow pattern opcode DestReg (8-15), Reg1 (16-23), Reg2 (24-31)
*/
void get_dest_source(int instr, int* destReg, int* reg1, int* reg2){
	*destReg = instr >> 16 & 0xff;
	*reg1 = instr >> 8 & 0xff;
	*reg2 = instr & 0xff;
}

/* 
implicitely returns address for instructions
that follow pattern opcode address (8-31) 
*/
void get_address(int instr, int* address){
	*address = instr & 0xffffff;
}

// executes arithematic instructions
void alu(int opcode, int instruction, int* dest, int* reg1, int* reg2){
	get_dest_source(instruction, dest, reg1, reg2);
	check_reg(*dest);
	check_reg(*reg1);
	check_reg(*reg2);
	
	switch(opcode){
		case ADD:
			registers[*dest] = registers[*reg1] + registers[*reg2];
			printf("opcode: ADD\n");
			break;
				
		case SUB:
			registers[*dest] = registers[*reg1] - registers[*reg2];
			printf("opcode: SUB\n");
			break;
				
		case MUL:
			registers[*dest] = registers[*reg1] * registers[*reg2];
			printf("opcode: MUL\n");
			break;
				
		case DIV:
			registers[*dest] = registers[*reg1] / registers[*reg2];
			printf("opcode: DIV\n");
			break;
	}
	
	printf("reg: %d val: 0x%08x, op_reg: %d val: 0x%08x, op_reg: %d  val: 0x%08x\n", 
						*dest, registers[*dest], *reg1, registers[*reg1], *reg2, registers[*reg2]);
}

// stores PC to LR when branching.
void link(int* address){
	registers[LR] = registers[PC];
	registers[PC] = *address;
}

void branch(int* address){
	registers[PC] = *address;
}

void compare(int reg1, int reg2){
	if(reg1 > reg2){
		// set the GT, clear all else.
		bit_set(&cpsr, GT);
		bit_clear(&cpsr, Z);
		bit_clear(&cpsr, LT);
	}
	else if(reg1 < reg2){
		// set the LT, clear all else.
		bit_set(&cpsr, LT);
		bit_clear(&cpsr, Z);
		bit_clear(&cpsr, GT);
	}
	else{ // if reg1 = reg2
		bit_set(&cpsr, Z);
		bit_clear(&cpsr, LT);
		bit_clear(&cpsr, GT);
	}
}

// fetches, decodes and executes a single instruction.
void step(){
	
	//fetches the instruction.
	int instruction = memory_fetch_word(registers[PC]); //PC is macro, 15th element in registers
	
	// extracts the opcode. Instructions are 32 bits, first 8 are opcode
	int opcode = instruction >> 24;
	
	printf("PC: 0x%08x, inst: 0x%08x ", registers[PC], instruction);
	
	// decodes and executes instruction based on opcode
	switch (opcode){
		int reg = 0;
		int address = 0;
		int immediate = 0;
		int dest = 0;
		int reg1 = 0;
		int reg2 = 0;
		int offset = 0;
		int srcReg = 0;
		int destReg = 0;
		
		case LDR:
			// LDR reg (8-15), address (16-31)
			get_reg_address(instruction, &reg, &address);
			check_reg(reg);
			check_address(address);
			registers[reg] = memory_fetch_word(address); // gets value from address, loads into reg.
			printf("opcode: LDR\nreg: %d, reg val: 0x%08x, address: 0x%08x\n", reg, registers[reg], address);
			registers[PC] += 4; // increments the program counter to the next instruction (size of 4 bytes)
			break;
			
		case LDI: // LDI reg (8-15), immediate value (16-31)
			get_reg_address(instruction, &reg, &immediate);
			check_reg(reg);
			registers[reg] = immediate; //loads immediate into reg.
			printf("opcode: LDI\nreg: %d, reg val: 0x%08x, immediate: 0x%04x\n", reg, registers[reg], immediate);
			registers[PC] += 4;
			break;
			
		case LDX: // LDX DestReg (8-15), offset (16-23), reg (24-31)
			get_dest_source(instruction, &dest, &offset, &reg);
			int offsetted = registers[reg] + offset;
			check_reg(dest);
			check_reg(reg);
			check_address(offsetted);
			registers[dest] = memory_fetch_word(offsetted); // reg contains an address
			printf("opcode: LDX\nreg: %d, reg val: 0x%08x, address: 0x%04x\n", dest, registers[dest], offsetted);
			registers[PC] += 4;
			break;
			
		case STR: // STR reg (8-15), address (16-31)
			get_reg_address(instruction, &reg, &address);
			check_reg(reg);
			check_address(address);
			memory_store_word(address, registers[reg]); // places reg into address.
			printf("opcode: STR\nreg: %d, reg val: 0x%08x, address: 0x%04x\n", reg, registers[reg], address);
			registers[PC] += 4;
			break;
			
		case ADD: // ADD DestReg (8-15), Reg1 (16-23), Reg2 (24-31)
			alu(opcode, instruction, &dest, &reg1, &reg2);
			registers[PC] += 4;
			break;
			
		case SUB: // SUB DestReg (8-15), Reg1 (16-23), Reg2 (24-31)
			alu(opcode, instruction, &dest, &reg1, &reg2);
			registers[PC] += 4;
			break;
			
		case MUL: // MUL DestReg (8-15), Reg1 (16-23), Reg2 (24-31)
			alu(opcode, instruction, &dest, &reg1, &reg2);
			registers[dest] = reg1 * reg2;
			registers[PC] += 4;
			break;
			
		case DIV: // DIV DestReg (8-15), Reg1 (16-23), Reg2 (24-31)
			alu(opcode, instruction, &dest, &reg1, &reg2);
			registers[PC] += 4;
			break;
			
		case CMP: // CMP Reg1 (16-23), Reg2 (24-31)
			get_dest_source(instruction, &dest, &reg1, &reg2);
			check_reg(dest);
			check_reg(reg1);
			check_reg(reg2);
			compare(registers[reg1], registers[reg2]);
			printf("opcode: CMP\nreg: %d val: 0x%08x to reg: %d val: 0x%08x\n", reg1, registers[reg1], reg2, registers[reg2]);
			registers[PC] += 4;
			break;
			
		case B:   // B Address (8-31)
			get_address(instruction, &address);
			check_address(address);
			branch(&address);
			printf("opcode: B\nbranched to 0x%08x\n", address);
			break;
			
		case BEQ: // BEQ Address (8-31)
			get_address(instruction, &address);
			check_address(address);
			
			if(bit_test(cpsr, Z)){
				branch(&address);
				printf("opcode: BEQ\nbranched to 0x%08x\n", address);
			}
			else{
				registers[PC] += 4;
			}
			
			break;
			
		case BNE: // BNE Address (8-31)
			get_address(instruction, &address);
			check_address(address);
			
			if(!bit_test(cpsr, Z)){
				branch(&address);
				printf("opcode: BNE\nbranched to 0x%08x\n", address);
			}
			else{
				registers[PC] += 4;
			}
			
			break;
		
		case BLT: // BLT Address (8-31)
			get_address(instruction, &address);
			check_address(address);
			
			if(bit_test(cpsr, LT)){
				branch(&address);
				printf("opcode: BLT\nbranched to 0x%08x\n", address);
			}
			else{
				registers[PC] += 4;
			}
			
			break;
			
		case BGT: // BGT Address (8-31)
			get_address(instruction, &address);
			check_address(address);
			if(bit_test(cpsr, GT)){
				branch(&address);
				printf("opcode: BGT\nbranched to 0x%08x\n", address);
			}
			else{
				registers[PC] += 4;
			}
			
			break;
			
		case MOV: // MOV DestReg (16-23), Reg1 (24-31)
			get_dest_source(instruction, &dest, &destReg, &srcReg);
			check_reg(srcReg);
			check_reg(destReg);
			registers[destReg] = registers[srcReg];
			printf("opcode: MOV\ndestReg: %d reg val: 0x%08x, srcReg: %d regVal: 0x%08x\n", 
					destReg, registers[destReg], srcReg, registers[srcReg]);
			registers[PC] += 4;
			break;
			
		case BL:  // BL Address (8-31)
			get_address(instruction, &address);
			check_address(address);
			link(&address);
			printf("opcode: BL\nbranched to 0x%08x\n", address);
			break;
			
		default:
			printf("opcode: %d invalid\n", opcode);
			exit(1);
	}
	printf("CPSR: 0x%02x\n", cpsr);
}

// calls step() n times
void step_n(int n){
	for(int i = 0; i < n; i++){
		step();
	}
 }

