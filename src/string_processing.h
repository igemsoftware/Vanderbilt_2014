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
    // TODO: convert to static-allocated array since we know how big it will be
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
    leven_matching,
    leven_complete
}
levenshtein_string_edit_operation;

typedef struct {
    size_t * cur_cell;
    size_t cur_x;
    size_t cur_y;
    size_t max_y;
} levenshtein_matrix_state;

static inline bool three_not_null(size_t * x, size_t * y, size_t * z) {
    return x != NULL || y != NULL || z != NULL;
}

static inline size_t min_of_non_null_three(size_t * x, size_t * y, size_t * z) {
    if (x != NULL) {
        if (y != NULL) {
            if (z != NULL) {
                return MIN3(*x, *y, *z);
            } else {
                return MIN(*x, *y);
            }
        } else {
            if (z != NULL){
                return MIN(*x, *z);
            } else {
                return *x;
            }
        }
    } else {
        if (y != NULL) {
            if (z != NULL){
                return MIN(*y, *z);
            } else {
                return *y;
            }
        } else {
            return *z;          // assumed not all three == NULL
        }
    }
}

// TODO: javadoc, including that as a precondition all up/left/up&left moves
// are
// still within the array, and that cur_cell is NOT the base array index, it
// is
// the address of the active cell
// mention that also advances cur cell in an atomic fashion which is kinda
// cool
static inline levenshtein_string_edit_operation
  get_next_levenshtein_operation_and_advance(levenshtein_matrix_state * lms) {
    size_t * insert_ptr = NULL, * delete_ptr = NULL, * sub_ptr = NULL;
    // bounds checking
    if (lms->cur_y > 0) {
        insert_ptr = lms->cur_cell - 1;
        if (lms->cur_x > 0) {
            sub_ptr = lms->cur_cell - lms->max_y - 1;
        }
    }
    if (lms->cur_x > 0) {
        delete_ptr = lms->cur_cell - lms->max_y;
    }
    size_t minimum;
    if (!three_not_null(insert_ptr, delete_ptr, sub_ptr)) {
        return leven_complete;  // done; at first element
    } else {
        minimum = min_of_non_null_three(insert_ptr, delete_ptr, sub_ptr);
    }
    // order of checking is arbitrary
    if (NULL != sub_ptr && minimum == *sub_ptr) {
        --lms->cur_x;
        --lms->cur_y;
        if (*lms->cur_cell == *sub_ptr) { // if same value
            lms->cur_cell = sub_ptr;
            return leven_matching;
        } else {
            lms->cur_cell = sub_ptr;
            return leven_substitution;
        }
    } else if (NULL != delete_ptr && minimum == *delete_ptr) {
        lms->cur_cell = delete_ptr;
        --lms->cur_x;
        return leven_deletion;
    } else { // required to be insertion, required to != NULL
        --lms->cur_y;
        lms->cur_cell = insert_ptr;
        return leven_insertion;
    }
}

// http://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm
// returns GSList * of chars inserted/deleted/substituted and at which
// positions
static inline GSList * get_levenshtein_edits(string_with_size * prev_string,
                                             string_with_size * cur_string) {
    // alias variables and allocate matrix
    // m is rows, n is columns
    size_t m = prev_string->readable_bytes + 1,
           n = cur_string->readable_bytes + 1;
    char * s = prev_string->string, * t = cur_string->string;
    size_t * lmat = malloc(sizeof(size_t) * m * n); // 2D array

    // initialize lmat
    for (size_t k = 0; k < m * n; ++k) {
        lmat[k] = 0;
    }
    for (size_t i = 0; i < m; ++i) {
        TWO_D_ARRAY_INDEX(lmat, i, 0, n) = i;
    }
    for (size_t j = 0; j < n; ++j) {
        TWO_D_ARRAY_INDEX(lmat, 0, j, n) = j;
    }

    for (size_t j = 1; j < n; ++j) {
        for (size_t i = 1; i < m; ++i) {
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

    // http://csc260project.hoguer.com/
    GSList * operations_backtrace = NULL;
    levenshtein_matrix_state lms;
    lms.cur_x = m - 1;
    lms.cur_y = n - 1;
    lms.max_y = n;
    lms.cur_cell = &TWO_D_ARRAY_INDEX(lmat, m - 1, n - 1, n); // start at end
    levenshtein_string_edit_operation * cur_op;
    while (lms.cur_cell != lmat) { // while current is cell is not start
        cur_op = malloc(sizeof(levenshtein_string_edit_operation));
        *cur_op = get_next_levenshtein_operation_and_advance(&lms);
        if (*cur_op == leven_matching) {
// #ifdef DEBUG
//             PRINT_ERROR("M");
// #endif
            operations_backtrace =
              g_slist_prepend(operations_backtrace, cur_op);
        } else if (*cur_op == leven_substitution) {
// #ifdef DEBUG
//             PRINT_ERROR("S");
// #endif
            operations_backtrace =
              g_slist_prepend(operations_backtrace, cur_op);
        } else if (*cur_op == leven_deletion) {
// #ifdef DEBUG
//             PRINT_ERROR("D");
// #endif
            operations_backtrace =
              g_slist_prepend(operations_backtrace, cur_op);
        } else if (*cur_op == leven_insertion) {
// #ifdef DEBUG
//             PRINT_ERROR("I");
// #endif
            operations_backtrace =
              g_slist_prepend(operations_backtrace, cur_op);
        } else {                // leven_complete is only available option
// #ifdef DEBUG
//             PRINT_ERROR("COMPLETE");
// #endif
            free(cur_op);
        }
    }

    // delete allocated mem (except for list)
    free(lmat);

    return operations_backtrace;
}

#endif /*___STRING_PROCESSING_H___*/
