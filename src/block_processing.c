#include "block_processing.h"


extern inline string_with_size * delimit_block_by_line (string_with_size * input_block_with_size_,
																												string_with_size * output_block_with_size,
																												bool * is_within_orf,
																												size_t * cur_orf_pos,
																												char * current_codon_frame,
																												bool is_final_block);

extern inline string_with_size * remove_newlines (string_with_size * input_block_with_size,
																									string_with_size * output_block_with_size);

extern inline string_with_size * read_block (FILE * input_file,
																						 string_with_size * input_string_with_size);

extern inline string_with_size * process_block_vcsfmt (string_with_size * input_block_with_size,
																											 string_with_size * output_block_with_size,
																											 bool * is_within_orf,
																											 size_t * cur_orf_pos,
																											 char * current_codon_frame,
																											 bool is_final_block);

extern inline string_with_size * de_process_block_vcsfmt (string_with_size * input_block_with_size,
																													string_with_size * output_block_with_size);

extern inline string_with_size * write_block (FILE * output_file,
																							string_with_size * output_block_with_size);
