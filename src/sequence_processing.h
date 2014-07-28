// provides line-delimiter heuristics for use by vcsfmt
#ifndef ___SEQUENCE_PROCESSING_H___
#define ___SEQUENCE_PROCESSING_H___

/*
	encapsulates many possibly-changing functions to analyze dna strings
*/

#include "utilities.h"					// bool, size_t, strncmp

#define CODON_LENGTH 3					// codon length, in bases

extern const char * start_codons[];
bool is_start_codon(char * codon);

extern const char * stop_codons[];
bool is_stop_codon(char * codon);

#endif /*___SEQUENCE_PROCESSING_H___*/
