# include "memory_system.h"
# include "memory.h"
# include "decoder.h"
# include <stdio.h>

// takes address and splits into two, five bit pieces
void split_address(int address, unsigned char* lower, unsigned char* upper){
	*lower = address & DECODER_BITMASK;
	*upper = (address >> DECODER_SHIFTER) & DECODER_BITMASK;
}

void decode_address(int address, unsigned int* decoded_lower, unsigned int* decoded_upper){
	unsigned char lower;
	unsigned char upper;
	
	split_address(address, &lower, &upper);
	*decoded_lower = decoder(lower); // sets the bit that corresponds to the row number.
	*decoded_upper = decoder(upper); // sets the bit that corresponds to the col number.
}
	
void memory_store(int address, unsigned char value){
	unsigned int decoded_lower;
	unsigned int decoded_upper;
	decode_address(address, &decoded_lower, &decoded_upper);
	
	// places the decoded address into memory
	mem_put(decoded_upper, decoded_lower, value);
}

unsigned char memory_fetch(int address){
	unsigned int decoded_lower;
	unsigned int decoded_upper;
	
	decode_address(address, &decoded_lower, &decoded_upper);
	return mem_get(decoded_upper, decoded_lower); // gets a byte from memory
}

unsigned int memory_fetch_word(int address){
	
	// call memory_fetch 4 times, because a word is 4 bytes
	unsigned int byte_1 = memory_fetch(address);
	unsigned int byte_2 = memory_fetch(address + 1);
	unsigned int byte_3 = memory_fetch(address + 2);
	unsigned int byte_4 = memory_fetch(address + 3);
	return byte_1 << 24 | byte_2 << 16 | byte_3 << 8 | byte_4; // big endian
}

void memory_dump(int start_address, int num_bytes){
	int end_address = start_address + num_bytes;
	
	if(start_address % 8 != 0){ // if address isn't divisible by 8
		printf("                "); //spaces for unwanted memory address
		for(int i = 0; i < start_address % 8; i++)
		printf("     "); //spaces for unwanted bytes in memory
	}
	
	// gets each row
	for(int i = start_address; i < end_address; i++){
		if(i % 8 == 0){
			printf("\n0x%04x (0d%04d) 0x%02x ", i, i, memory_fetch(i));
		}
		else{
			printf("0x%02x ", memory_fetch(i));
		}
	}
	printf("\n");	
}

void memory_store_word(int address, unsigned int value){
	// value is 32 bit quantity.
	// calls memory store 4 times, 8 bits at a time.
	unsigned char byte1 = value >> 24; //gets first 8 bits
	unsigned char byte2 = (value >> 16) & 0xff;
	unsigned char byte3 = (value >> 8) & 0xff;
	unsigned char byte4 = value & 0xff;
	
	memory_store(address, byte1);
	address += 1;
	memory_store(address, byte2);
	address += 1;
	memory_store(address, byte3);
	address += 1;
	memory_store(address, byte4);
}

void load_memory(char *filename){
	FILE* fp = fopen(filename, "r");
	int word;
	int address;
	
	fscanf(fp, "%x", &address);

	while(1){
		if (fscanf(fp, "%x", &word) == EOF) { 
				break; // exit loop if EOF reached
		}
		else {
			memory_store_word(address, word);
			address += 4; // increment address by 4 for each word.
		}
	}
}

void memory_fill(int start_address, unsigned char value, int num_bytes){
	for(int i = 0; i < num_bytes; i++){
		memory_store(start_address + i, value);
	}
}
