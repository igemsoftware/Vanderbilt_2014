#ifndef ___STRING_PROCESSING_H___
#define ___STRING_PROCESSING_H___

#include "sequence_processing.h"

// http://stackoverflow.com/questions/9616296/
// whats-the-best-hash-for-utf-8-strings
#define DJB2_HASH_BEGIN 5381 // initial string hash
// returns the next hash value
#define DJB2_MAGIC_CONSTANT 5
static inline unsigned long djb2_hash_on_string_index(
  unsigned long instantaneous_hash, char * str, size_t cur_index) {
    // same as instantaneous_hash * 33 ^ str[cur_index]
    return ((instantaneous_hash << DJB2_MAGIC_CONSTANT) + instantaneous_hash) ^
           str[cur_index];
    // OPTIMIZATION: is using the function character-by-character inefficient?
}


// http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/
// Levenshtein_distance#C
#define MIN3(a, b, c) \
    ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
// precondition: strings are same size
static inline size_t get_levenshtein_distance(string_with_size * prev_string,
                                       string_with_size * cur_string) {
    size_t prev_index, cur_index, lastdiag, olddiag;
    // TODO: convert to static-allocated array of 80 bytes
    size_t matrix_column[prev_string->readable_bytes + 1];
    for (prev_index = 1; prev_index <= prev_string->readable_bytes;
         ++prev_index) {
        matrix_column[prev_index] = prev_index;
    }
    for (cur_index = 1; cur_index <= cur_string->readable_bytes; ++cur_index) {
        matrix_column[0] = cur_index;
        for (prev_index = 1, lastdiag = cur_index;
             prev_index <= prev_string->readable_bytes;
             ++prev_index) {
            olddiag = matrix_column[prev_index];
            matrix_column[prev_index] =
              MIN3(matrix_column[prev_index] + 1,
                   matrix_column[prev_index - 1] + 1,
                   lastdiag + (prev_string->string[prev_index - 1] ==
                                   cur_string->string[cur_index - 1]
                                 ? 0
                                 : 1));
            lastdiag = olddiag;
        }
    }
    return matrix_column[prev_string->readable_bytes] - 1;
}

#endif /*___STRING_PROCESSING_H___*/
