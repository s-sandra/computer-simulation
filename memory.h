unsigned char mem_get(unsigned int row_selected, unsigned int col_selected);
// each row is a series of bits. The bit that is set is the row that is selected.
void mem_put(unsigned int row_selected, unsigned int col_selected, unsigned char b);
void mem_dump();
