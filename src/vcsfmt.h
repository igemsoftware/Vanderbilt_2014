#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

/*
    functions for breaking down DNA sequences into specially formatted files
    which work well with traditional line-based version control software
*/

#include "block_processing.h" // for block processing methods

#define OUTPUT_SUFFIX ".vcsfmt"

 // if filetype supported, produces vcsfmt file of same name
void vcsfmt(char * filename);
void de_vcsfmt(char * filename); // produces original file


// remove newlines, play with other metadata before going into dna; produces
// file in .dwn folder
void pre_format_file_vcsfmt(char * filename);
void de_pre_format_file_vcsfmt(char * filename);

// adds any required formatting to file after reconstituting from .vcsfmt
void post_format_file_vcsfmt(char * filename);
// above for de_vcsfmt
void de_post_format_file_vcsfmt(char * filename);

#endif /*___VCS_FMT_H___*/
