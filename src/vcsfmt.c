#include "vcsfmt.h"							// required

result_bytes_processed * initialize_result_bytes_processed_ptr(result_bytes_processed * rbp){
	rbp->result = 0;
	mpz_init(rbp->bytes_processed); // value set to 0
	return rbp;
}

void free_result_bytes_processed_ptr(result_bytes_processed * rbp){
	mpz_clear(rbp->bytes_processed);
	free(rbp);
}

result_bytes_processed * add_to_bytes_processed_ptr(result_bytes_processed * rbp, unsigned long int added_bytes){
	mpz_add_ui(rbp->bytes_processed,rbp->bytes_processed,added_bytes);
	return rbp;
}

void print_bytes_processed_ptr(result_bytes_processed * rbp, FILE * outstream){
	mpz_out_str(outstream,
							10,								// base 10
							rbp->bytes_processed);
}

void free_result_bytes_processed_pair(result_bytes_processed_pair rbpp){
	free_result_bytes_processed_ptr(rbpp.bytes_read);
	free_result_bytes_processed_ptr(rbpp.bytes_written);
}


result_bytes_processed_pair vcsfmt(char * filename){	
	pre_format_file(filename);

	unsigned long int cur_bytes_read = 0;
	// alloc'd on heap to avoid stack overflow
	// TODO: this variable currently unused
	result_bytes_processed * total_bytes_read = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed_ptr(total_bytes_read); // set to 0
	unsigned long int cur_bytes_written = 0;
	result_bytes_processed * total_bytes_written = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed_ptr(total_bytes_written); // set to 0

	result_bytes_processed_pair returnpair;
	returnpair.bytes_read = total_bytes_read;
	returnpair.bytes_written = total_bytes_written;
	
	FILE * input_file = open_file(filename);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(input_file,
																									"Error in creating input file.",
																									total_bytes_read->result = -1,
																									returnpair);

	// create filename long enough to concatenate filename and suffix
	char * output_file_name = (char*) malloc((strlen(filename) +
																						strlen(OUTPUT_SUFFIX) +
																						1) * sizeof(char));
	// create output filename
	strcpy(output_file_name,filename);
	strcat(output_file_name,OUTPUT_SUFFIX);
	FILE * output_file = create_outfile(output_file_name);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(output_file,
																									"Error in creating output file.",
																									total_bytes_written->result = -1,
																									returnpair);

	// allocate mem for input block
	string_with_size * input_block_with_size_ptr = make_new_string_with_size_ptr(BLOCK_SIZE);
	// same for output
	string_with_size * output_block_with_size_ptr = make_new_string_with_size_ptr(BINBLOCK_SIZE);

	bool is_within_orf = false;	// file begins outside of orf
	size_t cur_orf_pos = 0;
	char current_codon_frame[CODON_LENGTH] = {'\0'}; // begins with zero current codons

	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block and add to bytes processed
		add_to_bytes_processed_ptr(total_bytes_read,
															 cur_bytes_read =
															 read_block(input_file,
																					input_block_with_size_ptr)->cur_size);
		// process and write block
		add_to_bytes_processed_ptr(total_bytes_written,
															 cur_bytes_written =
															 write_block(output_file,
																					 process_block(input_block_with_size_ptr,
																												 output_block_with_size_ptr,
																												 &is_within_orf,
																												 &cur_orf_pos,
																												 current_codon_frame,
																												 feof(input_file)))->cur_size);
	}

	// cleanup allocated memory
	free(output_file_name);	// this is absolutely tiny but there's no reason not to free it
	free_string_with_size_ptr(input_block_with_size_ptr);
	free_string_with_size_ptr(output_block_with_size_ptr);

	// error handling
	if (ferror(input_file) && !feof(input_file)){
		PRINT_ERROR("Error in reading from input file.");
		total_bytes_read->result = -1;
	}
	else if (ferror(output_file) && !feof(input_file)){
		PRINT_ERROR("Error in writing to output file.");
		total_bytes_written->result = -1;
	}
	else if (feof(input_file)){
		total_bytes_read->result = 0;	// redundant, but here for clarity
		total_bytes_written->result = 0;
	}
	else{
		PRINT_ERROR("Unknown error in vcsfmt.c");
		total_bytes_read->result = -1;
		total_bytes_written->result = -1;
	}
	
	// close open handles
	fclose(input_file);
	fclose(output_file);

	post_format_file(filename);
	return returnpair;
}

result_bytes_processed_pair de_vcsfmt(char * filename){
	de_pre_format_file(filename);

	unsigned long int cur_bytes_read = 0;
	// alloc'd on heap to avoid stack overflow
	// TODO: this variable currently unused
	result_bytes_processed * total_bytes_read = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed_ptr(total_bytes_read); // set to 0
	unsigned long int cur_bytes_written = 0;
	result_bytes_processed * total_bytes_written = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed_ptr(total_bytes_written); // set to 0

	result_bytes_processed_pair returnpair;
	returnpair.bytes_read = total_bytes_read;
	returnpair.bytes_written = total_bytes_written;
	
	FILE * input_file = open_file(filename);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(input_file,
																									"Error in creating input file.",
																									total_bytes_read->result = -1,
																									returnpair);

	// create filename long enough to concatenate filename and suffix
	char * output_file_name = (char*) malloc((strlen(filename) +
																						strlen(OUTPUT_SUFFIX) +
																						1) * sizeof(char));
	// create output filename
	strcpy(output_file_name,filename);
	strcat(output_file_name,OUTPUT_SUFFIX);
	FILE * output_file = create_outfile(output_file_name);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(output_file,
																									"Error in creating output file.",
																									total_bytes_written->result = -1,
																									returnpair);

	// allocate mem for input block
	string_with_size * input_block_with_size_ptr = make_new_string_with_size_ptr(BINBLOCK_SIZE);
	// both are BINBLOCK_SIZE because BINBLOCK_SIZE was calculated to be efficient at file I/O (at 8192 as I write this)
	// one might think BLOCK_SIZE should be used here, but that would only work if, as in the worst-case scenario,
	// two newlines are inserted for an ORF for every 60 bases, and would produce a segfault if not
	// since output is always smaller than input (due to removal of newlines), no segfault occurs
	string_with_size * output_block_with_size_ptr = make_new_string_with_size_ptr(BINBLOCK_SIZE);
	
	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block
		add_to_bytes_processed_ptr(total_bytes_read,
															 cur_bytes_read =
															 read_block(input_file,
																					input_block_with_size_ptr)->cur_size);
		// process and write block
		add_to_bytes_processed_ptr(total_bytes_written,
															 cur_bytes_written =
															 write_block(output_file,
																					 de_process_block(input_block_with_size_ptr,
																														output_block_with_size_ptr))->cur_size);
	}

	// cleanup mem
	free(output_file_name);
	free_string_with_size_ptr(input_block_with_size_ptr);
	free_string_with_size_ptr(output_block_with_size_ptr);

	// error handling
	if (ferror(input_file) && !feof(input_file)){
		PRINT_ERROR("Error in reading from input file.");
		total_bytes_read->result = -1;
	}
	else if (ferror(output_file) && !feof(input_file)){
		PRINT_ERROR("Error in writing to output file.");
		total_bytes_written->result = -1;
	}
	else if (feof(input_file)){
		total_bytes_read->result = 0; // redundant, but here for clarity
		total_bytes_written->result = 0;
	}
	else{
		PRINT_ERROR("Unknown error in vcsfmt.c");
		total_bytes_read->result = -1;
		total_bytes_written->result = -1;
	}

	// close open handles
	fclose(input_file);
	fclose(output_file);

	// apply final formatting
	de_post_format_file(filename);
	return returnpair;
}

dna_reading_indices pre_format_file(char * filename){
	// does nothing right now
	dna_reading_indices active_dna_reading_indices;
	active_dna_reading_indices.begin_index = (size_t) filename[0];
	active_dna_reading_indices.end_index = 0;
	return active_dna_reading_indices;
}

dna_reading_indices de_pre_format_file(char * filename){
	// does nothing right now
	dna_reading_indices active_dna_reading_indices;
	active_dna_reading_indices.begin_index = (size_t) filename[0];
	active_dna_reading_indices.end_index = 0;
	return active_dna_reading_indices;
}

void post_format_file(char * filename){
	// does nothing right now
	printf("%s\n",filename);
}

void de_post_format_file(char * filename){
	// does nothing right now
	printf("%s\n",filename);
}

FILE * open_file(char * filename){
	FILE * input_file = fopen(filename,"r");
	// reject if incorrect filetype
	return input_file;
}

extern inline string_with_size * read_block(FILE * input_file, string_with_size * input_str_with_size);

extern inline string_with_size * process_block(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr, bool * is_within_orf, size_t * cur_orf_pos, char * current_codon_frame, bool is_final_block);

extern inline string_with_size * de_process_block(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr);

extern inline string_with_size * write_block(FILE * output_file, string_with_size * output_block_with_size_ptr);

FILE * create_outfile(char* filename){
	FILE * output_file = fopen(filename,"wb");
	return output_file;
}
