#include "string_processing.h"

unsigned long int djb2_hash_on_string_index(
  unsigned long int instantaneous_hash, char * str, size_t cur_index) {
    return ((instantaneous_hash * 33) ^ (unsigned long int)str[cur_index]);
    // OPTIMIZATION: is using the function character-by-character inefficient?
    // can we act upon larger blocks of data?
}

size_t get_levenshtein_distance(string_with_size * prev_string,
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

bool three_not_null(size_t * x, size_t * y, size_t * z) {
    return x != NULL || y != NULL || z != NULL;
}

size_t min_of_non_null_three(size_t * x, size_t * y, size_t * z) {
    if (x != NULL) {
        if (y != NULL) {
            if (z != NULL) {
                return MIN3(*x, *y, *z);
            } else {
                return MIN(*x, *y);
            }
        } else {
            if (z != NULL) {
                return MIN(*x, *z);
            } else {
                return *x;
            }
        }
    } else {
        if (y != NULL) {
            if (z != NULL) {
                return MIN(*y, *z);
            } else {
                return *y;
            }
        } else {
            return *z; // assumed not all three == NULL
        }
    }
}

levenshtein_string_edit_operation
  get_next_levenshtein_operation_and_advance(levenshtein_matrix_state * lms) {
    size_t * insert_ptr = NULL, *delete_ptr = NULL, *sub_ptr = NULL;
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
        return leven_complete; // done; at first element
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

GSList * get_levenshtein_edits_and_free(string_with_size * prev_string,
                                        string_with_size * cur_string) {
    // alias variables and allocate matrix
    // m is rows, n is columns
    size_t m = prev_string->readable_bytes + 1,
           n = cur_string->readable_bytes + 1;
    char * s = prev_string->string, *t = cur_string->string;
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
        } else { // leven_complete is only available option
                 // #ifdef DEBUG
                 //             PRINT_ERROR("COMPLETE");
                 // #endif
            free(cur_op);
        }
    }
    // delete allocated mem (except for list)
    free(lmat);
    free_string_with_size(prev_string);
    free_string_with_size(cur_string);
    return g_slist_reverse(operations_backtrace);
}

char convert_leven_op_to_char(levenshtein_string_edit_operation cur_op) {
    switch (cur_op) {
    case leven_matching:
        return 'M';
        break;
    case leven_substitution:
        return 'S';
        break;
    case leven_insertion:
        return 'I';
        break;
    case default:               // required to be leven_complete
        return 'C';
        break;
    }
}

typedef struct {
    string_with_size * sws;
    size_t index;
} string_with_size_and_index;

void write_leven_char_to_index_of_string(
  levenshtein_string_edit_operation * cur_op,
  string_with_size_and_index * swsai) {
    swsai->sws->string[swsai->index] = convert_leven_op_to_char(*cur_op);
    ++swsai->index;
}

string_with_size *
  format_and_free_levenshtein_list_to_string_with_size(
    GSList * operations_backtrace) {
    string_with_size * sws_to_return =
      make_new_string_with_size(g_slist_length(operations_backtrace));
    string_with_size_and_index swsai;
    swsai.sws = sws_to_return;
    swsai.index = 0;
    g_slist_foreach(
      operations_backtrace, (GFunc)write_leven_char_to_index_of_string, &swsai);
    // assured, because size in memory was initialized with size of list
    swsai.sws->readable_bytes = swsai.sws->size_in_memory;
    g_slist_free_full(operations_backtrace, free);
    return sws_to_return;
}
