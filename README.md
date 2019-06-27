# Computer Simulation

## Description
This program emulates RISC architecture, with the ability to load, fetch, decode and execute machine code as well as run a small set of commands. It has two main components.

### CPU Simulation
 The CPU emulator has a total of 16 registers, each storing 32 bits. It also has a current program status register, with three flags, defined as Z (zero, or equal to), LT (less than) and GT (greater than).

R0 -  R12: general registers.<br>
R13: stack pointer (not used in this simulation)<br>
R14: link register<br>
R15: program counter<br>

The CPU recognizes 16 instructions, each 32 bits long. Its instruction set is a simplified version of the ARM ISA. The first eight bits of each instruction is reserved for the opcode.

|opcode|instruction|format|purpose|
|---|---|---|---|
|1|LDR|(bits 9 - 16) destination register (bits 17 - 32) source address|loads register with contents of address.|
|2|LDI|(bits 9 - 16) destination register (bits 17 - 32) value|loads register with an immediate value.|
|3|LDX|(bits 9 - 16) destination register (bits 17 - 24) offset (bits 25 - 32) source register|loads the contents of the offsetted address contained in the source register into the destination register.|
|4|STR|(bits 9 - 16) source register (bits 17 - 32) destination address|stores the contents of register into address.|
|5|ADD|(bits 9 - 16) destination register (bits 17 - 24) operand 1 (bits 25 - 32) operand 2|stores sum of operand 1 and 2 in register.|
|6|SUB|(bits 9 - 16) destination register (bits 17 - 24) operand 1 (bits 25 - 32) operand 2|stores difference between operand 1 and 2 in register|
|7|MUL|(bits 9 - 16) destination register (bits 17 - 24) operand 1 (bits 25 - 32) operand 2|stores product of operand 1 and 2 in register.|
|8|DIV|(bits 9 - 16) destination register (bits 17 - 24) operand 1 (bits 25 - 32) operand 2|stores quotient of operand 1 and 2 in register.|
|9|CMP|(bits 17 - 24) register 1 (bits 25 - 32) register 2|compares contents of register 1 against contents of register 2. Sets status register flags accordingly. Bits 9 -16 are unused.|
|10|B|(bits 9 - 32) address|sets the program counter to the branch address.|
|11|BEQ|(bits 9 - 32) address|sets the program counter to the branch address if CMP is equal (Z is set).|
|12|BNE|(bits 9 - 32) address|sets the program counter to the branch address if CMP is not equal (Z is cleared).|
|13|BLT|(bits 9 - 32) address|sets the program counter to the branch address if CMP is less than (LT is set).|
|14|BGT|(bits 9 - 32) address|sets the program counter to the branch address if CMP is greater than (GT is set).|
|15|MOV|(bits 17 - 24) destination register (bits 25 - 32) source register|moves the contents of one register to another. Bits 9 - 16 are unused.||
|16|BL|(bits 9 - 32) address |sets the program counter to the branch address and stores the return address in the link register.|

## Memory Simulation
The simulated, big-endian memory system is comprised of a 32 by 32 byte memory matrix. An address for a byte is expressed as a 10 bit quantity, where the first five bits represent the column and the last five the row. The decoder splits the address in half and converts the row and column binary values to one-hot form (as input signals), which are then used to select a byte. The memory system can fetch and store both one byte and four byte quantities.

## Interface
The computer simulation accepts six different commands.

|command|description|
|---|---|
|load|asks for a machine code file name to load into memory.|
|set_reg|asks for a register and the new content for the register.|
|step|executes instruction at the program counter, printing the program counter, current program status register and details about the instruction.|
|step_n| executes n number of instruction from the program counter, printing the program counter, current program status register and details about each instruction.
|regs|prints the contents of all registers.|
|dump|prints each byte of the memory chip, in groups of eight hexadecimals.|

## Simulation Machine Code Format
The first line of each machine code file begins with the memory address of the program. Each subsequent line contains a word that represents data or an instruction.

