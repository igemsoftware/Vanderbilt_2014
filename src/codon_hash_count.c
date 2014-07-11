// modified from default gperf output
// 1) added __attribute__ ((unused)) to 'len' in function 'hash'
// 2) added ifdef guards
// 3) changed names of functions to 'in_codon_set' and 'codon_hash'
// 4) made codon_hash and in_word_set nonstatic
// 5) changed register variables to function arguments
// 6) added #include <string.h>
// 7) moved wordlist outside of in_codon_set, to the header, and made it non-static
// 8) removed 'register' from 'register const char *s wordlist[key]' at bottom
// 9) removed #defines, moved to header for accesibility by other functions

#include <string.h>							// for strncmp

#include "codon_hash_count.h"		// required

/* C code produced by gperf version 3.0.4 */
/* Command-line: gperf -c amino_codons  */
/* Computed positions: -k'1-3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35)					\
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40)			\
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44)				\
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48)				\
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52)				\
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56)				\
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60)				\
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65)				\
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69)				\
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73)				\
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77)				\
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81)				\
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85)				\
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89)				\
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93)			\
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98)				\
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102)		\
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106)		\
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110)		\
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114)		\
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118)		\
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122)		\
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
unsigned int
codon_hash (const char * str, unsigned int len __attribute__ ((unused)))
{
  unsigned char asso_values[] =
    {
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178,   4, 127,  10,  90,   2,
      178,  40,  45,   5, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178,  25,   0,   0, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178, 178, 178,
      178, 178, 178, 178, 178, 178, 178, 178
    };
  return asso_values[(unsigned char)str[2]+1] + asso_values[(unsigned char)str[1]+2] + asso_values[(unsigned char)str[0]];
}


const char * wordlist[] =
{
	"", "", "", "",
	"ATT",
	"",
	"ACT",
	"", "",
	"AGT",
	"CTT",
	"",
	"CCT",
	"",
	"AAT",
	"CGT",
	"", "", "", "",
	"CAT",
	"", "", "", "",
	"TTT",
	"",
	"TCT",
	"", "",
	"TGT",
	"", "", "", "",
	"TAT",
	"", "", "", "",
	"GTT",
	"",
	"GCT",
	"", "",
	"GGT",
	"", "", "",
	"ATG",
	"GAT",
	"ACG",
	"", "",
	"AGG",
	"CTG",
	"",
	"CCG",
	"",
	"AAG",
	"CGG",
	"", "", "", "",
	"CAG",
	"", "", "", "",
	"TTG",
	"",
	"TCG",
	"", "",
	"TGG",
	"", "", "", "",
	"TAG",
	"", "", "", "",
	"GTG",
	"",
	"GCG",
	"", "",
	"GGG",
	"", "", "",
	"ATC",
	"GAG",
	"ACC",
	"", "",
	"AGC",
	"CTC",
	"",
	"CCC",
	"",
	"AAC",
	"CGC",
	"", "", "", "",
	"CAC",
	"", "", "", "",
	"TTC",
	"",
	"TCC",
	"", "",
	"TGC",
	"", "", "", "",
	"TAC",
	"", "", "", "",
	"GTC",
	"ATA",
	"GCC",
	"ACA",
	"",
	"GGC",
	"AGA",
	"CTA",
	"",
	"CCA",
	"GAC",
	"AAA",
	"CGA",
	"", "", "", "",
	"CAA",
	"", "", "", "",
	"TTA",
	"",
	"TCA",
	"", "",
	"TGA",
	"", "", "", "",
	"TAA",
	"", "", "", "",
	"GTA",
	"",
	"GCA",
	"", "",
	"GGA",
	"", "", "", "",
	"GAA"
};


#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif

const char *
in_codon_set (const char * str, unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = codon_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          const char *s = wordlist[key];

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return s;
        }
    }
  return 0;
}