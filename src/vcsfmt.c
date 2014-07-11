#include "vcsfmt.h"							// required

int vcsfmt(char * filename){	
	pre_format_file(filename);
	
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

	// allocate mem for input block
	string_with_size * input_stream_block = (string_with_size *) malloc(sizeof(string_with_size));
	input_stream_block->string = malloc((BLOCK_SIZE) * sizeof(char));
	input_stream_block->full_size = BLOCK_SIZE * sizeof(char);
	input_stream_block->cur_size = 0;		// no data written yet
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(input_stream_block->string,"Stream memory block not successfully allocated.\n");
	// same for output
	string_with_size * output_stream_block = (string_with_size *) malloc(sizeof(string_with_size));
	output_stream_block->string = malloc((BLOCK_SIZE) * sizeof(char));
	output_stream_block->full_size = BLOCK_SIZE * sizeof(char);
	output_stream_block->cur_size = 0;		// no data written yet
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(output_stream_block->string,"Stream memory block not successfully allocated.\n");


	size_t cur_bytes_read = 0;
	size_t total_bytes_read = 0;
	size_t cur_bytes_written = 0;
	size_t total_bytes_written = 0;

	bool * is_within_orf = (bool*) malloc(sizeof(bool));
	*is_within_orf = false;				// file begins outside of orf
	size_t * cur_orf_pos = (size_t *) malloc(sizeof(size_t));
	*cur_orf_pos = 0;

	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block
		read_block(input_file, input_stream_block);
		cur_bytes_read = input_stream_block->cur_size;
		total_bytes_read += cur_bytes_read;
		// process block
		process_block(input_stream_block,
									output_stream_block,
									is_within_orf,
									cur_orf_pos);
		// write block
		write_block(output_file, output_stream_block);
		cur_bytes_written = output_stream_block->cur_size;
		total_bytes_written += cur_bytes_written;
	}
	// cur_bytes_read and cur_bytes_written are assigned differently,
	// one as the return value of a function and one as a value within the string_with_size struct,
	// because while reading a block is always <= BLOCK_SIZE,
	// writing can be and usually is > BLOCK_SIZE due to the newlines and other info inserted

	// cleanup mem
	free(output_file_name);
	free(input_stream_block->string);
	free(input_stream_block);
	free(is_within_orf);

	bool error_occurred = false;
	
	// error handling
	if (ferror(input_file) && !feof(input_file)){
		PRINT_ERROR("Error in reading from input file.\n");
		error_occurred = true;
	}
	else if (ferror(output_file) && !feof(input_file)){
		PRINT_ERROR("Error in writing to output file.\n");
	}
	else if (feof(input_file)){
		// diagnostics
		printf("%zu",total_bytes_read);
		printf(" bytes read.\n");
		printf("%zu",total_bytes_written);
		printf(" bytes written.\n");
		error_occurred = false;			// redundant, but here for clarity
	}
	else{
		PRINT_ERROR("Unknown error in vcsfmt.c\n");
		error_occurred = true;
	}

	// close open handles
	fclose(input_file);
	fclose(output_file);

	if (error_occurred){
		return -1;
	}
	else{
		return total_bytes_written;
	}
}

int de_vcsfmt(char * filename){	
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

	// allocate mem for input block
	string_with_size * input_stream_block = (string_with_size *) malloc(sizeof(string_with_size));
	input_stream_block->string = malloc((BINBLOCK_SIZE) * sizeof(char));
	input_stream_block->full_size = BINBLOCK_SIZE * sizeof(char);
	input_stream_block->cur_size = 0;		// no data written yet
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(input_stream_block->string,"Stream memory block not successfully allocated.\n");
	// same for output
	string_with_size * output_stream_block = (string_with_size *) malloc(sizeof(string_with_size));
	output_stream_block->string = malloc((BLOCK_SIZE) * sizeof(char));
	output_stream_block->full_size = BLOCK_SIZE * sizeof(char);
	output_stream_block->cur_size = 0;		// no data written yet
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(output_stream_block->string,"Stream memory block not successfully allocated.\n");


	size_t cur_bytes_read = 0;
	size_t total_bytes_read = 0;
	size_t cur_bytes_written = 0;
	size_t total_bytes_written = 0;
	
	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block
		read_block(input_file, input_stream_block);
		cur_bytes_read = input_stream_block->cur_size;
		total_bytes_read += cur_bytes_read;
		// process block
		de_process_block(input_stream_block,
										 output_stream_block);
		// write block
		write_block(output_file, output_stream_block);
		cur_bytes_written = output_stream_block->cur_size;
		total_bytes_written += cur_bytes_written;
	}
	// cur_bytes_read and cur_bytes_written are assigned differently,
	// one as the return value of a function and one as a value within the string_with_size struct,
	// because while reading a block is always <= BLOCK_SIZE,
	// writing can be and usually is > BLOCK_SIZE due to the newlines and other info inserted

	// cleanup mem
	free(output_file_name);
	free(input_stream_block->string);
	free(input_stream_block);

	bool error_occurred = false;
	
	// error handling
	if (ferror(input_file) && !feof(input_file)){
		PRINT_ERROR("Error in reading from input file.\n");
		error_occurred = true;
	}
	else if (ferror(output_file) && !feof(input_file)){
		PRINT_ERROR("Error in writing to output file.\n");
	}
	else if (feof(input_file)){
		// diagnostics
		printf("%zu",total_bytes_read);
		printf(" bytes read.\n");
		printf("%zu",total_bytes_written);
		printf(" bytes written.\n");
		error_occurred = false;			// redundant, but here for clarity
	}
	else{
		PRINT_ERROR("Unknown error in vcsfmt.c\n");
		error_occurred = true;
	}

	// close open handles
	fclose(input_file);
	fclose(output_file);

	if (error_occurred){
		return -1;
	}
	else{
		return total_bytes_written;
	}
}


dna_reading_indices pre_format_file(char * filename __attribute__ ((unused))){
	// does nothing right now
	dna_reading_indices active_dna_reading_indices;
	active_dna_reading_indices.begin_index = 0;
	active_dna_reading_indices.end_index = 0;
	return active_dna_reading_indices;
}

FILE * open_file(char * filename){
	FILE * input_file = fopen(filename,"r");
	// reject if incorrect filetype
	return input_file;
}

void read_block(FILE * input_file, string_with_size * input_str_with_size){
	input_str_with_size->cur_size = fread(input_str_with_size->string,sizeof(char),input_str_with_size->full_size,input_file);
}

void process_block(string_with_size * input_block_with_size, string_with_size * output_block_with_size, bool * is_within_orf, size_t * cur_orf_pos){
	hash_and_delimit_block_by_line(input_block_with_size, output_block_with_size, is_within_orf, cur_orf_pos);
}

void de_process_block(string_with_size * input_block_with_size, string_with_size * output_block_with_size){
	unhash_and_remove_newlines(input_block_with_size, output_block_with_size);
}


void write_block(FILE * output_file, string_with_size * output_block_with_size){
	output_block_with_size->cur_size = fwrite(output_block_with_size->string,
																						sizeof(char),
																						output_block_with_size->cur_size,
																						output_file);
}

FILE * create_outfile(char* filename){
	FILE * output_file = fopen(filename,"wb");
	return output_file;
}
