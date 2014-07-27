#include "block_processing.h"

extern inline string_with_size * delimit_block_by_line(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr, bool * is_within_orf, size_t * cur_orf_pos, char * current_codon_frame, bool is_final_block);

extern inline string_with_size * remove_newlines(string_with_size * input_block_with_size_ptr, string_with_size * output_block_with_size_ptr);
