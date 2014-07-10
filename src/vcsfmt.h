#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

#include <stdio.h>							// for FILE
#include <stdbool.h>						// for booleans

#define OUTPUT_SUFFIX ".vcsfmt"

// OPTIMIZATION: make these functions inline

// 18446744073709551615 is max size of size_t on my system
// use (size_t) -1 to find max size of size_t on per-system basis
// this is important because dna data files on the genome level can get very long

// used to return a char string, along with size information
typedef struct{
	char * string;
	size_t cur_size;							// current number of useful bytes this is storing
	size_t full_size;							// full size of char * in bytes
} string_with_size;							// NOT null-terminated by default!

typedef struct{
	size_t begin_index;
	size_t end_index;
} dna_reading_indices;

#define MIN_ORF_LENGTH 60				// convert this to program option at some point

#define BLOCK_SIZE 8190 				// used in file I/O
// we need to take in DNA blocks as multiples of 3 characters, and someone told me it should be close to a multiple of 2; hence 8190
//returns -1 if failed, otherwise returns size in bytes of produced file
int vcsfmt(char * filename);	// if filetype supported, produces vcsfmt file of same name

#define BINBLOCK_SIZE 2730			// 1/3 of BLOCK_SIZE, so that we don't overflow when expanding, counting newlines
// inverse of above
//returns -1 if failed, otherwise returns size in bytes of produced file
int de_vcsfmt(char * filename);	// produces original file

dna_reading_indices pre_format_file(char * filename); // remove newlines, play with other metadata before going into dna; produces file in .darwin folder
// returns indices of of bytes in file where dna exists, ensures DNA is divisible by 3

FILE * open_file(char* filename); // open file, return pointer

void read_block(FILE * input_file, string_with_size * input_str_with_size); // read block_size bytes from file into output_str
// modifies input_str_wih_size->cur_size to be number of bytes read from file

void process_block(string_with_size * input_block_with_size, string_with_size * output_block_with_size, bool * is_within_orf, size_t * cur_orf_pos);
// perform some processing on block and write to file
// modifies output_str_wih_size->cur_size to be number of bytes written to file

void de_process_block(string_with_size * input_block_with_size, string_with_size * output_block_with_size); // inverse of above

void write_block(FILE * output_file, string_with_size * output_block_with_size);

FILE * create_outfile(char * filename); // create file, return pointer

// CHARACTER ESCAPING
// certain special characters such as \r and \n are used to delimit lines in the resulting vcsfmt file
// but the hash for codons may produce unsigned ints corresponding to those ascii codes
// so we must make sure to divert them to an unused space in the wordlist array
#define NUM_SPECIAL_CHARS 2

extern char special_chars[NUM_SPECIAL_CHARS]; // initialization in vcsfmt.c

char special_char_escapes[NUM_SPECIAL_CHARS]; // initialized by create_array_for_special_chars()

int create_special_char_escapes(); // finds empty spaces in the hash wordlist and assigns to special chars
// run once at beginning of vcsfmt; wish it could be done at compile time but C can't do that
// SOURCE OF ERROR : transcription errors due to misusing this function

char escape_special_chars(char input_byte); // if char is a special char, replace it with something else

char de_escape_special_chars(char input_byte); // if char escaped, turn it back
// END CHARACTER ESCAPING

#endif /*___VCS_FMT_H___*/
