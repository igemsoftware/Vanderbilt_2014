#ifndef ___CODON_HASH_COUNT_H___
#define ___CODON_HASH_COUNT_H___

// pilfered from gperf output
#define TOTAL_KEYWORDS 64
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 177
/* maximum key range = 174, duplicates = 0 */

unsigned int codon_hash (const char * str, unsigned int len);

const char * in_codon_set (const char * str, unsigned int len);

extern const char * wordlist[];

#endif /*___CODON_HASH_COUNT_H___*/
