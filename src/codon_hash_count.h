#ifndef ___CODON_HASH_COUNT_H___
#define ___CODON_HASH_COUNT_H___

unsigned int codon_hash(const char * str, unsigned int len);

const char * in_codon_set (const char * str, unsigned int len);

extern const char * wordlist[];

#endif /*___CODON_HASH_COUNT_H___*/
