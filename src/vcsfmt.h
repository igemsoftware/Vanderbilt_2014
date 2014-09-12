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

#endif /*___VCS_FMT_H___*/
