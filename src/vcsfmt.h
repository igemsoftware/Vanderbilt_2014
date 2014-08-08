#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

/*
	functions for breaking down DNA sequences into specially formatted files
	which work well with traditional line-based version control software
*/

#include "block_processing.h"		// for block processing methods

#define OUTPUT_SUFFIX ".vcsfmt"

// returns result_bytes_processed of total bytes read and bytes written
// result is 0 if good, -1 if error occurred
result_bytes_processed_pair vcsfmt (char * filename);	// if filetype supported, produces vcsfmt file of same name
// MUST call free_result_bytes_processed_pair_not on the return value! allocated memory should be freed!

// inverse of above
// returns result_bytes_processed of total bytes read and bytes written
// result is 0 if good, -1 if error occurred
result_bytes_processed_pair de_vcsfmt (char * filename);	// produces original file
// MUST call free_result_bytes_processed_pair_not on the return value! allocated memory should be freed!

// remove newlines, play with other metadata before going into dna; produces file in .dwn folder
// returns indices of of bytes in file where dna exists, ensures DNA is divisible by 3
dna_reading_indices pre_format_file_vcsfmt (char * filename);
// above for de_vcsfmt
dna_reading_indices de_pre_format_file_vcsfmt (char * filename);

// adds any required formatting to file after reconstituting from .vcsfmt
void post_format_file_vcsfmt (char * filename);
// above for de_vcsfmt
void de_post_format_file_vcsfmt (char * filename);

#endif /*___VCS_FMT_H___*/
