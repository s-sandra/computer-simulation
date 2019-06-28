# include "bit_functions.h"
# include "memory.h"
# include <stdio.h>

// global matrix of memory, 32 by 32 bits or 1024 bytes.
unsigned char mem[32][32];

// obtains a given byte from memory given a 32 bit row and col, where one bit is set.
unsigned char mem_get(unsigned int row_selected, unsigned int col_selected){
	return mem[bit_find(row_selected)][bit_find(col_selected)];
}

// places a given byte into memory at a given address.
void mem_put(unsigned int row_selected, unsigned int col_selected, unsigned char newMem) {
	mem[bit_find(row_selected)][bit_find(col_selected)] = newMem;
}

// prints out the entire contents of the memory chip.
void mem_dump(){
	
	// gets each row
	for(int i = 0; i < 32; i++){
		
		// gets every 8th column
		for(int j = 0; j < 32; j+=8){
			printf("0x%04x (0d%04d) 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n" ,
					i * 32 + j, i * 32 + j,
					mem[i][j], mem[i][j + 1], mem[i][j + 2],
					mem[i][j + 3], mem[i][j + 4], mem[i][j + 5],
					mem[i][j + 6], mem[i][j + 7]);
		}
	}
}

