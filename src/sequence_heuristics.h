// provides line-delimiter heuristics for use by vcsfmt
#ifndef ___SEQUENCE_HEURISTICS_H___
#define ___SEQUENCE_HEURISTICS_H___

#include "utilities.h"					// bool, size_t, strncmp
#include "codon_hash_count.h"		// provide hash to files

#define CODON_LENGTH 3					// codon length, in bases

extern const char * start_codons[];
bool is_start_codon(char * codon);

extern const char * stop_codons[];
bool is_stop_codon(char * codon);

// byte to codon transformations
// OPTIMIZATION: make this inline if compiler doesn't do it already
// encode each codon into different byte with perfect hash
// for compression and identification of synonymic codons
// precondition: codon MUST be three characters from A/C/G/T
// postcondition: returns null char if not found
char get_byte_from_codon(char * codon);

const char * get_codon_from_byte(char byte);

#endif /*___SEQUENCE_HEURISTICS_H___*/
