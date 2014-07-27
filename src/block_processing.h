#ifndef ___BLOCK_PROCESSING_H___
#define ___BLOCK_PROCESSING_H___

#include "sequence_heuristics.h" // provide codon sequence data to files

// chosen so that maximum binblock_size is 8192, a power of 2 (heuristic)
#define BLOCK_SIZE 7928 				// used in file I/O

// only look for orfs above 60 bases long (heuristic)
#define MIN_ORF_LENGTH 60	// convert this to program option at some point, not compile-time definition

// output block maximum size
// maximum possible size of output block, assuming every possible 60-char sequence is an orf (which won't happen)
// two newlines per orf in the worst possible case
#define BINBLOCK_SIZE (size_t)(BLOCK_SIZE*(1 + 2 / (double)MIN_ORF_LENGTH))

// heavy lifting

inline string_with_size * delimit_block_by_line(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr, bool * is_within_orf, size_t * cur_orf_pos, char * current_codon_frame, bool is_final_block){

	output_block_with_size_ptr->cur_size = 0;
	for (size_t codon_index = 0; codon_index < input_block_with_size_ptr->cur_size; ++codon_index){
		// read in next base
		current_codon_frame[CODON_LENGTH - 1] = input_block_with_size_ptr->string[codon_index];
		// if all three slots are filled
		// since the first base is only null at start/end of ORF or at beginning of run
		if (current_codon_frame[0] != '\0'){ // branch predictions should be good on this one
			if (*is_within_orf){							 // same here
			  if (*cur_orf_pos >= MIN_ORF_LENGTH - CODON_LENGTH && is_stop_codon(current_codon_frame)){
					// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
					for (size_t base_index = 0; base_index < CODON_LENGTH; ++base_index){
						output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size + base_index] =
							current_codon_frame[base_index];
						current_codon_frame[base_index] = '\0'; // nullify to read in more characters
					}
					output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size + CODON_LENGTH] = '\n';
					// output_block_with_size_ptr->cur_size INCREMENTED AT END OF LOOP
					output_block_with_size_ptr->cur_size += CODON_LENGTH;
					*is_within_orf = false;	// NOTE THE DIFFERENCE HERE FROM BELOW
					*cur_orf_pos = 0;
				}
				else{
					output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size] =
						current_codon_frame[0];
					++*cur_orf_pos;
					// output_block_with_size_ptr->cur_size INCREMENTED AT END OF LOOP
				}
			}
			else{
				if (is_start_codon(current_codon_frame)){
					// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
					output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size] = '\n';
					for (size_t base_index = 0; base_index < CODON_LENGTH; ++base_index){
						// order reversed; newline inserted BEFORE orf
						output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size + base_index + 1] = current_codon_frame[base_index];
						current_codon_frame[base_index] = '\0'; // nullify to read in more characters
					}
					// output_block_with_size_ptr->cur_size INCREMENTED AT END OF LOOP
					output_block_with_size_ptr->cur_size += CODON_LENGTH;
					output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size + CODON_LENGTH] = '\n';
					*is_within_orf = true; // NOTE THE DIFFERENCE HERE FROM ABOVE
					*cur_orf_pos = 3;
				}
				else{
					// output_block_with_size_ptr->cur_size INCREMENTED AT END OF LOOP
					output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size] =
						current_codon_frame[0];
				}
			}
			++output_block_with_size_ptr->cur_size;
		}
		// no attempt made to deal with non-dna characters since it is assumed that pre-formatting removes those
		
		// shuffle bases over
		// under assumption that current_codon_frame is of length CODON_LENGTH
		// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
		for (size_t base_index = 0; base_index < CODON_LENGTH - 1; ++base_index){
			current_codon_frame[base_index] = current_codon_frame[base_index + 1];
		}
		// leaves first two codons in current_codon_frame pointer for next block to use
	}

	// if this is the last block, eject the last two bases which would have otherwise been saved for next block
	if (is_final_block){
		// OPTIMIZATION: unroll this loop since CODON_LENGTH is known and universally constant
		// not really that much of an optimization though since this is a miniscule calculation
		for (size_t base_index = 0; base_index < CODON_LENGTH - 1; ++base_index){
			output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size + base_index] =
				current_codon_frame[base_index];
		}
		output_block_with_size_ptr->cur_size += CODON_LENGTH - 1;
	}

	return output_block_with_size_ptr;
}

inline string_with_size * remove_newlines(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr){
	output_block_with_size_ptr->cur_size = 0;
	for (size_t bytes_read = 0; bytes_read < input_block_with_size_ptr->cur_size; ++bytes_read){
		if (input_block_with_size_ptr->string[bytes_read] != '\n'){
			output_block_with_size_ptr->string[output_block_with_size_ptr->cur_size] =
				input_block_with_size_ptr->string[bytes_read];
			++output_block_with_size_ptr->cur_size;
		}
	}
	return output_block_with_size_ptr;
}


#endif /*___BLOCK_PROCESSING_H___*/
