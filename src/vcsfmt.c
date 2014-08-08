#include "vcsfmt.h"							// required

// OPTIMIZATION: parallelize this
// maintain queue of read-in and processed blocks to be written; read/process and write are done concurrently
result_bytes_processed_pair vcsfmt (char * filename){	
	pre_format_file_vcsfmt(filename);
	unsigned long int cur_bytes_read = 0;
	// alloc'd on heap to avoid stack overflow
	// TODO: total_bytes_read currently unused
	result_bytes_processed * total_bytes_read = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed(total_bytes_read); // set to 0
	unsigned long int cur_bytes_written = 0;
	result_bytes_processed * total_bytes_written = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed(total_bytes_written); // set to 0

	result_bytes_processed_pair returnpair;
	returnpair.bytes_read = total_bytes_read;
	returnpair.bytes_written = total_bytes_written;
	
	FILE * input_file = open_file_read(filename);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(input_file,
																									"Error in creating input file.",
																									total_bytes_read->result = -1,
																									returnpair);

	// create filename long enough to concatenate filename and suffix
	char * output_file_name = (char *) malloc((strlen(filename) +
																						strlen(OUTPUT_SUFFIX) +
																						1) * sizeof(char));
	// create output filename
	strcpy(output_file_name,
				 filename);
	strcat(output_file_name,
				 OUTPUT_SUFFIX);
	FILE * output_file = create_file_binary_write(output_file_name);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(output_file,
																									"Error in creating output file.",
																									total_bytes_written->result = -1,
																									returnpair);

	// allocate mem for input block
	string_with_size * input_block_with_size = make_new_string_with_size(BLOCK_SIZE);
	// same for output
	string_with_size * output_block_with_size = make_new_string_with_size(BINBLOCK_SIZE);

	bool is_within_orf = false;	// file begins outside of orf
	size_t cur_orf_pos = 0;
	char current_codon_frame[CODON_LENGTH] = {'\0'}; // begins with zero current codons

	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block and add to bytes processed
		add_to_bytes_processed(total_bytes_read,
															 cur_bytes_read =
															 read_block(input_file,
																					input_block_with_size)->readable_bytes);
		// process and write block
		add_to_bytes_processed(total_bytes_written,
															 cur_bytes_written =
															 write_block(output_file,
																					 process_block_vcsfmt(input_block_with_size,
																												 output_block_with_size,
																												 &is_within_orf,
																												 &cur_orf_pos,
																												 current_codon_frame,
																												 feof(input_file)))->readable_bytes);
	}

	// cleanup allocated memory
	free(output_file_name);	// this is absolutely tiny but there's no reason not to free it
	free_string_with_size(input_block_with_size);
	free_string_with_size(output_block_with_size);

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

	post_format_file_vcsfmt(filename);
	return returnpair;
}

result_bytes_processed_pair de_vcsfmt (char * filename){
	de_pre_format_file_vcsfmt(filename);

	unsigned long int cur_bytes_read = 0;
	// alloc'd on heap to avoid stack overflow
	// TODO: this variable currently unused
	result_bytes_processed * total_bytes_read = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed(total_bytes_read); // set to 0
	unsigned long int cur_bytes_written = 0;
	result_bytes_processed * total_bytes_written = (result_bytes_processed *) malloc(sizeof(result_bytes_processed));
	initialize_result_bytes_processed(total_bytes_written); // set to 0

	result_bytes_processed_pair returnpair;
	returnpair.bytes_read = total_bytes_read;
	returnpair.bytes_written = total_bytes_written;
	
	FILE * input_file = open_file_read(filename);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(input_file,
																									"Error in creating input file.",
																									total_bytes_read->result = -1,
																									returnpair);

	// create filename long enough to concatenate filename and suffix
	char * output_file_name = (char *) malloc((strlen(filename) +
																						strlen(OUTPUT_SUFFIX) +
																						1) * sizeof(char));
	// create output filename
	strcpy(output_file_name,
				 filename);
	strcat(output_file_name,
				 OUTPUT_SUFFIX);
	FILE * output_file = create_file_binary_write(output_file_name);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(output_file,
																									"Error in creating output file.",
																									total_bytes_written->result = -1,
																									returnpair);

	// allocate mem for input block
	string_with_size * input_block_with_size = make_new_string_with_size(BINBLOCK_SIZE);
	// both are BINBLOCK_SIZE because BINBLOCK_SIZE was calculated to be efficient at file I/O (at 8192 as I write this)
	// one might think BLOCK_SIZE should be used here, but that would only work if, as in the worst-case scenario,
	// two newlines are inserted for an ORF for every 60 bases, and would produce a segfault if not
	// since output is always smaller than input (due to removal of newlines), no segfault occurs
	string_with_size * output_block_with_size = make_new_string_with_size(BINBLOCK_SIZE);
	
	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block
		add_to_bytes_processed(total_bytes_read,
															 cur_bytes_read =
															 read_block(input_file,
																					input_block_with_size)->readable_bytes);
		// process and write block
		add_to_bytes_processed(total_bytes_written,
															 cur_bytes_written =
															 write_block(output_file,

																					 de_process_block_vcsfmt(input_block_with_size,
																														output_block_with_size))->readable_bytes);
	}

	// cleanup mem
	free(output_file_name);
	free_string_with_size(input_block_with_size);
	free_string_with_size(output_block_with_size);

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
	de_post_format_file_vcsfmt(filename);
	return returnpair;
}

dna_reading_indices pre_format_file_vcsfmt (char * filename){
	// does nothing right now
	dna_reading_indices active_dna_reading_indices;
	active_dna_reading_indices.begin_index = (size_t) filename[0];
	active_dna_reading_indices.end_index = 0;
	return active_dna_reading_indices;
}

dna_reading_indices de_pre_format_file_vcsfmt (char * filename){
	// does nothing right now
	dna_reading_indices active_dna_reading_indices;
	active_dna_reading_indices.begin_index = (size_t) filename[0];
	active_dna_reading_indices.end_index = 0;
	return active_dna_reading_indices;
}

void post_format_file_vcsfmt (char * filename){
	// does nothing right now
	PRINT_ERROR(filename);
}

void de_post_format_file_vcsfmt (char * filename){
	// does nothing right now
	PRINT_ERROR(filename);
}
