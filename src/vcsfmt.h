#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

#include <stdio.h>							// for FILE

#define OUTPUT_SUFFIX ".vcsfmt"

// 18446744073709551615 is max size of size_t on my system
// use (size_t) -1 to find max size of size_t on per-system basis

// used to return a char string, along with size information
typedef struct{
	char * string;
	size_t cur_size;							// current number of useful bytes this is storing
	size_t full_size;							// full size of char * in bytes
} string_with_size;							// NOT null-terminated by default!

//returns -1 if failed, otherwise returns size in bytes of produced file
int vcsfmt(char * filename, size_t block_size);	// if filetype supported, produces vcsfmt file of same name

FILE * open_file(char* filename); // open file, return pointer

void read_block(FILE * input_file, string_with_size * input_str_with_size); // read block_size bytes from file into output_str
// modifies input_str_wih_size.cur_size to be number of bytes read from file

string_with_size * process_block(string_with_size * active_str_with_size); // perform some processing on block

FILE * create_outfile(char * filename); // create file, return pointer

void append_to_outfile(FILE * output_file, string_with_size * output_str_with_size); // write bytes to file, returns bytes written
// modifies output_str_wih_size.cur_size to be number of bytes written to file

#endif /*___VCS_FMT_H___*/
