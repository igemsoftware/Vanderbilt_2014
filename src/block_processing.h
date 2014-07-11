#ifndef ___BLOCK_PROCESSING_H___
#define ___BLOCK_PROCESSING_H___

#include "utilities.h"					 // for required utilities
#include "sequence_heuristics.h" // provide codon sequence data to files

void hash_and_delimit_block_by_line(string_with_size * input_block_with_size, string_with_size * output_block_with_size, bool * is_within_orf, size_t * cur_orf_pos);

void unhash_and_remove_newlines(string_with_size * input_block_with_size, string_with_size * output_block_with_size);



// CHARACTER ESCAPING
// certain special characters such as \r and \n are used to delimit lines in the resulting vcsfmt file
// but the hash for codons may produce unsigned ints corresponding to those ascii codes
// so we must make sure to divert them to an unused space in the wordlist array
extern char special_chars[]; // initialization in block_processing.c

extern char special_char_escapes[]; // initialized by create_array_for_special_chars()

int create_special_char_escapes(); // finds empty spaces in the hash wordlist and assigns to special chars
// run once at beginning of vcsfmt; wish it could be done at compile time but C can't do that
// SOURCE OF ERROR : transcription errors due to misusing this function

char escape_special_chars(char input_byte); // if char is a special char, replace it with something else

char de_escape_special_chars(char input_byte); // if char escaped, turn it back
// END CHARACTER ESCAPING

#endif /*___BLOCK_PROCESSING_H___*/
