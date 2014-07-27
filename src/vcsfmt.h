#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

#include "block_processing.h"		// for block processing methods

#define OUTPUT_SUFFIX ".vcsfmt"

// 18446744073709551615 is max size of size_t on my system
// use (size_t) -1 to find max size of size_t on per-system basis
// TODO: this is important because dna data files on the genome level can get very long

//returns -1 if failed, otherwise returns size in bytes of produced file
int vcsfmt(char * filename);	// if filetype supported, produces vcsfmt file of same name

// inverse of above
// returns -1 if failed, otherwise returns size in bytes of produced file
int de_vcsfmt(char * filename);	// produces original file

// remove newlines, play with other metadata before going into dna; produces file in .darwin folder
// returns indices of of bytes in file where dna exists, ensures DNA is divisible by 3
dna_reading_indices pre_format_file(char * filename);

// adds any required formatting to file after reconstituting from .vcsfmt
void post_format_file(char * filename);

// open file, return pointer
FILE * open_file(char* filename);

// read block_size bytes from file into output_str
// modifies input_str_wih_size->cur_size to be number of bytes read from file
inline string_with_size * read_block(FILE * input_file, string_with_size * input_str_with_size){
	input_str_with_size->cur_size = fread(input_str_with_size->string,
																				sizeof(char),
																				input_str_with_size->full_size,
																				input_file);
	return input_str_with_size;
}

// perform some processing on block and write to file
// modifies output_str_wih_size->cur_size to be number of bytes written to file
inline string_with_size * process_block(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr, bool * is_within_orf, size_t * cur_orf_pos, char * current_codon_frame, bool is_final_block){
	return delimit_block_by_line(input_block_with_size_ptr,
															 output_block_with_size_ptr,
															 is_within_orf, cur_orf_pos,
															 current_codon_frame,
															 is_final_block);
}

// inverse of above
inline string_with_size * de_process_block(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr){
	return remove_newlines(input_block_with_size_ptr, output_block_with_size_ptr);
}

inline string_with_size * write_block(FILE * output_file, string_with_size * output_block_with_size_ptr){
	output_block_with_size_ptr->cur_size = fwrite(output_block_with_size_ptr->string,
																						sizeof(char),
																						output_block_with_size_ptr->cur_size,
																						output_file);
	return output_block_with_size_ptr;
}

FILE * create_outfile(char * filename); // create file, return pointer

#endif /*___VCS_FMT_H___*/
