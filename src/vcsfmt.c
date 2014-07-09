#include <stdlib.h>							// for malloc
#include <string.h>							// for strlen/strcpy/strcat
#include <stdbool.h>						// for bool variables

#include "vcsfmt.h"							// required
#include "utilities.h"					// for compiler macros
#include "sequence_heuristics.h" // for codon sequence data

inline int vcsfmt(char * filename, size_t block_size){
	FILE * input_file = open_file(filename);
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(input_file,"Error in creating input file.\n");

	// create filename long enough to concatenate filename and suffix
	char * output_file_name = (char*) malloc((strlen(filename) +
																						strlen(OUTPUT_SUFFIX) +
																						1) * sizeof(char));
	// create output filename
	strcpy(output_file_name,filename);
	strcat(output_file_name,OUTPUT_SUFFIX);
	FILE * output_file = create_outfile(output_file_name);
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(output_file,"Error in creating output file.\n");

	// allocate mem for block
	string_with_size * stream_block = (string_with_size *) malloc(sizeof(string_with_size));
	stream_block->string = malloc((block_size) * sizeof(char));
	stream_block->full_size = block_size * sizeof(char);
	stream_block->cur_size = 0;		// no data written yet
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(stream_block->string,"Stream memory block not successfully allocated.\n");

	size_t cur_bytes_read = 0;
	size_t total_bytes_read = 0;
	size_t cur_bytes_written = 0;
	size_t total_bytes_written = 0;

	bool * is_within_orf = (bool*) malloc(sizeof(bool));
	*is_within_orf = false;				// file begins outside of orf
	char previous_two_bases[CODON_LENGTH - 1] = {'\0','\0'}; // used so that start/stop codons not broken across blocks

	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block
		read_block(input_file, stream_block);
		cur_bytes_read = stream_block->cur_size;
		total_bytes_read += cur_bytes_read;
		// process and write block
		cur_bytes_written = process_and_write_block(output_file,
																								stream_block,
																								is_within_orf,
																								previous_two_bases);
		total_bytes_written += cur_bytes_written;
	}
	// cur_bytes_read and cur_bytes_written are assigned differently,
	// one as the return value of a function and one as a value within the string_with_size struct,
	// because while reading a block is always <= block_size,
	// writing can be and usually is > block_size due to the newlines and other info inserted

	// cleanup mem
	free(output_file_name);
	free(stream_block->string);
	free(stream_block);
	free(is_within_orf);
	// close open handles
	fclose(input_file);
	fclose(output_file);

	// error handling
	if (ferror(input_file) && !feof(input_file)){
		PRINT_ERROR("Error in reading from input file.\n");
		return -1;
	}
	else if (ferror(output_file) && !feof(input_file)){
		PRINT_ERROR("Error in writing to output file.\n");
		return -1;
	}
	else{													// if reached EOF successfully
		// diagnostics
		printf("%zu",total_bytes_read);
		printf(" bytes read.\n");
		printf("%zu",total_bytes_written);
		printf(" bytes written.\n");
		return 0;
	}
}

inline FILE * open_file(char * filename){
	FILE * input_file = fopen(filename,"r");
	// reject if incorrect filetype
	return input_file;
}

inline void read_block(FILE * input_file, string_with_size * input_str_with_size){
	input_str_with_size->cur_size = fread(input_str_with_size->string,sizeof(char),input_str_with_size->full_size,input_file);
}

inline size_t process_and_write_block(FILE* output_file, string_with_size * input_block_with_size, bool * is_within_orf, char * previous_two_bases){
	size_t current_index = 0;			// index into char array
	size_t end_line_index = 0;		// index after beginning/end of orf is found
	size_t bytes_written = 0;
	char temp_char_for_newline;		// used to temporarily store byte which is used to hold the newline character when dumping (end_line_index - current_index) of the block to file

	// currently nonfunctional code
	
	current_index = handle_previous_two_bases(input_block_with_size, is_within_orf);
	while (current_index != input_block_with_size->block_size){
		end_line_index = get_end_of_line(input_block_with_size, current_index, is_within_orf);
		bytes_written += fwrite(input_block_with_size->string + current_index,
														sizeof(char),
														end_line_index - current_index,
														output_file);
		current_index = end_line_index;
	}

	// TODO:
	// compress output into single-byte amino acid codes (so we can have space in the output block to insert newlines and stuff)
	// make input block_size a multiple of 3 (but as close to [and below] a multiple of two as possible) so we don't have to worry about cutting off orfs
	// copy over input blocks; while doing so, convert to amino acid bytes (after checking if they're orfs)
	// the above requires creating a table of 64 characters (bytes) to be used to represent all 64 possible codons, and then making orf-finding a process of checking whether that byte matches up to a list of known delimiting bytes
	
#if defined DEBUG
	fprintf(stderr,"write cur_size: ");
	fprintf(stderr,"%zu",input_block_with_size->cur_size);
	fprintf(stderr,"\nwrite full_size: ");
	fprintf(stderr,"%zu",input_block_with_size->full_size);
	fprintf(stderr,"\n---------------\n");
#endif

	return bytes_written;
}

inline FILE * create_outfile(char* filename){
	FILE * output_file = fopen(filename,"w");
	return output_file;
}
