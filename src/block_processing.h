#ifndef ___BLOCK_PROCESSING_H___
#define ___BLOCK_PROCESSING_H___

/*
	functions to process strings of dna data in sequential blocks
*/

#include "sequence_processing.h" // provide codon sequence data to files

// chosen so that maximum BINBLOCK_SIZE is 8192, a power of 2 (heuristic for fast file I/O)
#define BLOCK_SIZE 7928 				// used in file I/O

// only look for orfs above 60 bases long (heuristic)
#define MIN_ORF_LENGTH 60	// TODO: convert this to program option at some point, not compile-time definition

// output block maximum size
// maximum possible size of output block, assuming every possible 60-char sequence is an orf (which won't happen)
// two newlines per orf in the worst possible case
#define BINBLOCK_SIZE (size_t) (BLOCK_SIZE * (1 + 2 / (double) MIN_ORF_LENGTH))

// heavy lifting

// open file, return pointer
FILE * open_file_read (char * filename);
FILE * create_outfile (char * filename); // create file, return pointer

inline string_with_size * delimit_block_by_line (string_with_size * input_block_with_size,
																								 string_with_size * output_block_with_size,
																								 bool * is_within_orf,
																								 size_t * cur_orf_pos,
																								 char * current_codon_frame,
																								 bool is_final_block){
	output_block_with_size->readable_bytes = 0;
	for (size_t codon_index = 0;
			 codon_index < input_block_with_size->readable_bytes;
			 ++codon_index){
		// read in next base
		current_codon_frame[CODON_LENGTH - 1] = input_block_with_size->string[codon_index];
		// if all three slots are filled
		// since the first base is only null at start/end of ORF or at beginning of run
		if (current_codon_frame[0] != '\0'){ // branch predictions should be good on this one
			if (*is_within_orf){							 // same here
			  if (*cur_orf_pos >= MIN_ORF_LENGTH - CODON_LENGTH && is_stop_codon(current_codon_frame)){
					// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
					for (size_t base_index = 0;
							 base_index < CODON_LENGTH;
							 ++base_index){
						output_block_with_size->string[output_block_with_size->readable_bytes + base_index] =
							current_codon_frame[base_index];
						current_codon_frame[base_index] = '\0'; // nullify to read in more characters
					}
					output_block_with_size->string[output_block_with_size->readable_bytes + CODON_LENGTH] = '\n';
					// output_block_with_size->readable_bytes INCREMENTED AT END OF LOOP
					output_block_with_size->readable_bytes += CODON_LENGTH;
					*is_within_orf = false;	// NOTE THE DIFFERENCE HERE FROM BELOW
					*cur_orf_pos = 0;
				}
				else{
					// ORF length must be multiple of CODON_LENGTH (of 3)
					// we could simply check if current orf length is a multiple of 3, but that becomes very slow at scale
					// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
					for (size_t base_index = 0;
							 base_index < CODON_LENGTH;
							 ++base_index){
						output_block_with_size->string[output_block_with_size->readable_bytes + base_index] = current_codon_frame[base_index];
						current_codon_frame[base_index] = '\0';
					}
					*cur_orf_pos += 3;
					// output_block_with_size->readable_bytes INCREMENTED AT END OF LOOP
					output_block_with_size->readable_bytes += 2;
				}
			}
			else{
				if (is_start_codon(current_codon_frame)){
					// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
					output_block_with_size->string[output_block_with_size->readable_bytes] = '\n';
					for (size_t base_index = 0;
							 base_index < CODON_LENGTH;
							 ++base_index){
						// order reversed; newline inserted BEFORE orf
						output_block_with_size->string[output_block_with_size->readable_bytes + base_index + 1] = current_codon_frame[base_index];
						current_codon_frame[base_index] = '\0'; // nullify to read in more characters
					}
					// output_block_with_size->readable_bytes INCREMENTED AT END OF LOOP
					output_block_with_size->readable_bytes += CODON_LENGTH;
					output_block_with_size->string[output_block_with_size->readable_bytes + CODON_LENGTH] = '\n';
					*is_within_orf = true; // NOTE THE DIFFERENCE HERE FROM ABOVE
					*cur_orf_pos = 3;
				}
				else{
					// output_block_with_size->readable_bytes INCREMENTED AT END OF LOOP
					output_block_with_size->string[output_block_with_size->readable_bytes] =
						current_codon_frame[0];
				}
			}
			++output_block_with_size->readable_bytes;
		}
		// no attempt made to deal with non-dna characters since it is assumed that pre-formatting removes those
		
		// shuffle bases over
		// under assumption that current_codon_frame is of length CODON_LENGTH
		// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
		for (size_t base_index = 0;
				 base_index < CODON_LENGTH - 1;
				 ++base_index){
			current_codon_frame[base_index] = current_codon_frame[base_index + 1];
		}
		current_codon_frame[CODON_LENGTH - 1] = '\0'; // nullify final
		// leaves first two codons in current_codon_frame pointer for next block to use
	}

	// if this is the last block, eject the last two bases which would have otherwise been saved for next block
	if (is_final_block){
		// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
		// not really that much of an optimization though since this is a miniscule calculation
		for (size_t base_index = 0;
				 base_index < CODON_LENGTH - 1;
				 ++base_index){
			if (output_block_with_size->string[output_block_with_size->readable_bytes + base_index] != '\0'){
				output_block_with_size->string[output_block_with_size->readable_bytes + base_index] =
					current_codon_frame[base_index];
			}
		}
		output_block_with_size->readable_bytes += CODON_LENGTH - 1;
	}

	return output_block_with_size;
}

inline string_with_size * remove_newlines (string_with_size * input_block_with_size,
																					 string_with_size * output_block_with_size){
	output_block_with_size->readable_bytes = 0;
	for (size_t bytes_read = 0;
			 bytes_read < input_block_with_size->readable_bytes;
			 ++bytes_read){
		if (input_block_with_size->string[bytes_read] != '\n'){
			output_block_with_size->string[output_block_with_size->readable_bytes] =
				input_block_with_size->string[bytes_read];
			++output_block_with_size->readable_bytes;
		}
	}
	return output_block_with_size;
}

// read block_size bytes from file into output_str
// modifies input_str_wih_size->readable_bytes to be number of bytes read from file
inline string_with_size * read_block (FILE * input_file,
																			string_with_size * input_string_with_size){
	input_string_with_size->readable_bytes = fread(input_string_with_size->string,
																				sizeof(char),
																				input_string_with_size->size_in_memory,
																				input_file);
	return input_string_with_size;
}

// perform some processing on block and write to file
// modifies output_str_wih_size->readable_bytes to be number of bytes written to file
inline string_with_size * process_block_vcsfmt (string_with_size * input_block_with_size,
																								string_with_size * output_block_with_size,
																								bool * is_within_orf,
																								size_t * cur_orf_pos,
																								char * current_codon_frame,
																								bool is_final_block){
	return delimit_block_by_line(input_block_with_size,
															 output_block_with_size,
															 is_within_orf,
															 cur_orf_pos,
															 current_codon_frame,
															 is_final_block);
}

// inverse of above
inline string_with_size * de_process_block_vcsfmt (string_with_size * input_block_with_size,
																									 string_with_size * output_block_with_size){
	return remove_newlines(input_block_with_size,
												 output_block_with_size);
}

inline string_with_size * write_block (FILE * output_file,
																			 string_with_size * output_block_with_size){
	output_block_with_size->readable_bytes = fwrite(output_block_with_size->string,
																						sizeof(char),
																						output_block_with_size->readable_bytes,
																						output_file);
	return output_block_with_size;
}

#endif /*___BLOCK_PROCESSING_H___*/
