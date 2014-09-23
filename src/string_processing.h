#ifndef ___STRING_PROCESSING_H___
#define ___STRING_PROCESSING_H___

#include "sequence_processing.h"

// http://stackoverflow.com/questions/9616296/
// whats-the-best-hash-for-utf-8-strings
#define DJB2_HASH_BEGIN 5381 // initial string hash
// returns the next hash value
// TODO: javadoc
#define DJB2_MAGIC_CONSTANT 5
static inline unsigned long int djb2_hash_on_string_index(
  unsigned long int instantaneous_hash, char * str, size_t cur_index) {
    // same as instantaneous_hash * 33 ^ str[cur_index]
    return ((instantaneous_hash << DJB2_MAGIC_CONSTANT) + instantaneous_hash) ^
           (unsigned long int) str[cur_index];
    // OPTIMIZATION: is using the function character-by-character inefficient?
}

// TODO: javadoc
// http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/
// Levenshtein_distance#C
#define MIN3(a, b, c) \
    ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
// precondition: strings are same size
static inline size_t get_levenshtein_distance(string_with_size * prev_string,
                                              string_with_size * cur_string) {
    size_t olddiag;
    // TODO: convert to static-allocated array
    size_t * matrix_column =
      malloc(sizeof(size_t) * (prev_string->readable_bytes + 1));
    for (size_t prev_index = 1; prev_index <= prev_string->readable_bytes;
         ++prev_index) {
        matrix_column[prev_index] = prev_index;
    }
    for (size_t cur_index = 1; cur_index <= cur_string->readable_bytes;
         ++cur_index) {
        matrix_column[0] = cur_index;
        for (size_t prev_index = 1, lastdiag = cur_index;
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
    size_t ret = matrix_column[prev_string->readable_bytes] - 1;
    free(matrix_column);
    return ret;
}

/*
  * insert: left one
  * delete: up one
  * sub: up & left
  * match: up & left
  */
typedef enum {
    leven_insertion,
    leven_deletion,
    leven_substitution,
    leven_matching
} levenshtein_string_edit_operation;

// TODO: javadoc, including that as a precondition all up/left/up&left moves are
// still within the array, and that cur_cell is NOT the base array index, it is
// the address of the active cell
static inline levenshtein_string_edit_operation
  get_next_levenshtein_operation(short int * cur_cell, size_t max_y) {
    short int * insert = cur_cell - 1, * delete = cur_cell - max_y,
                * sub = cur_cell - max_y - 1;
    short int minimum = MIN3(*insert, *delete, *sub);
    // order of checking is arbitrary
    if (minimum == *sub) {
        if (*cur_cell == *sub) { // if same value
            return leven_matching;
        } else {
            return leven_substitution;
        }
    } else if (minimum == *delete) {
        return leven_deletion;
    } else {                    // required to be insertion
        return leven_insertion;
    }
}

// http://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm
// returns GSList * of chars inserted/deleted/substituted and at which
// positions
static inline GSList * get_levenshtein_edits(string_with_size * prev_string,
                                             string_with_size * cur_string) {
    // alias variables and allocate matrix
    size_t m = prev_string->readable_bytes, n = cur_string->readable_bytes;
    char * s = prev_string->string, * t = cur_string->string;
    short int * lmat =
      malloc(sizeof(short int) * (m + 1) * (n + 1)); // 2D array

#ifdef DEBUG
    for (size_t i = 0; i <= m; ++i) {
        for (size_t j = 0; j <= n; ++j) {
            TWO_D_ARRAY_INDEX(lmat, i, j, n + 1) = 0;
        }
    }
#endif

    // initialize lmat
    for (size_t i = 0; i <= m; ++i) {
        TWO_D_ARRAY_INDEX(lmat, i, 0, n + 1) = i;
    }
    // for (size_t j = 0; j <= n; ++j) {
    //     TWO_D_ARRAY_INDEX(lmat, 0, j, n) = j;
    // }
#ifdef DEBUG
    for (size_t i = 0; i <= m; ++i) {
        for (size_t j = 0; j <= n; ++j) {
            PRINT_ERROR_SIZE_T_NO_NEWLINE(
              (size_t) TWO_D_ARRAY_INDEX(lmat, i, j, n + 1));
            PRINT_ERROR_NO_NEWLINE(",");
        }
        PRINT_ERROR_NEWLINE();
    }
    PRINT_ERROR_NEWLINE();
#endif
    for (size_t j = 1; j <= n; ++j) {
        for (size_t i = 1; i <= m; ++i) {
            if (s[i - 1] == t[j - 1]) { // changed from quoted version of alg
                // because these strings are zero-based
                TWO_D_ARRAY_INDEX(lmat, i, j, n) =
                  TWO_D_ARRAY_INDEX(lmat, i - 1, j - 1, n);
            } else {
                TWO_D_ARRAY_INDEX(lmat, i, j, n) =
                  MIN3(TWO_D_ARRAY_INDEX(lmat, i - 1, j, n) + 1,
                       TWO_D_ARRAY_INDEX(lmat, i, j - 1, n) + 1,
                       TWO_D_ARRAY_INDEX(lmat, i - 1, j - 1, n) + 1);
            }
        }
    }
#ifdef DEBUG
    for (size_t i = 0; i < m + 1; ++i) {
        for (size_t j = 0; j < n + 1; ++j) {
            PRINT_ERROR_SIZE_T_NO_NEWLINE(
              (size_t) TWO_D_ARRAY_INDEX(lmat, i, j, n));
            PRINT_ERROR_NO_NEWLINE(",");
        }
        PRINT_ERROR_NEWLINE();
    }
    PRINT_ERROR_NEWLINE();
    PRINT_ERROR_SIZE_T_NO_NEWLINE(
      (size_t) TWO_D_ARRAY_INDEX(lmat, m, n, n));
    PRINT_ERROR(" IS FINAL LEVENSHTEIN");
#endif

    // // http://csc260project.hoguer.com/
    // // GSList * operations_backtrace = NULL;
    // short int * cur_cell = &TWO_D_ARRAY_INDEX(lmat, m, n, n); // start at end
    // levenshtein_string_edit_operation cur_op;
    // while (cur_cell != lmat) {  // while current is cell is not start
    //     cur_op = get_next_levenshtein_operation(cur_cell, n);
    //     if (cur_op == leven_matching) {
    //         PRINT_ERROR_NO_NEWLINE("M");
    //     } else if (cur_op == leven_substitution) {
    //         PRINT_ERROR_NO_NEWLINE("S");
    //     } else if (cur_op == leven_deletion) {
    //         PRINT_ERROR_NO_NEWLINE("D");
    //     } else {                // required to be insertion
    //         PRINT_ERROR_NO_NEWLINE("I");
    //     }
    //     PRINT_ERROR_NEWLINE();
    // }

    // delete allocated mem (except for list)
    free(lmat);

    return NULL;
}

#endif /*___STRING_PROCESSING_H___*/
