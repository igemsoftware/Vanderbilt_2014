#ifndef ___STRING_PROCESSING_H___
#define ___STRING_PROCESSING_H___

#include "block_processing.h"

/*
http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/
Levenshtein_distance#C
*/

#define MIN3(a, b, c) \
    ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

// OPTIMIZATION: everything in this function
// precondition: strings are same size
inline size_t get_levenshtein_distance(string_with_size * prev_string,
                                       string_with_size * cur_string) {
    size_t prev_index, cur_index, lastdiag, olddiag;
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
