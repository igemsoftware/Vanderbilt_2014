#ifndef ___STRING_PROCESSING_H___
#define ___STRING_PROCESSING_H___

#include "sequence_processing.h"

// http://stackoverflow.com/questions/9616296/
// whats-the-best-hash-for-utf-8-strings
#define DJB2_HASH_BEGIN 5381 // initial string hash
// returns the next hash value
// TODO: javadoc
#define DJB2_MAGIC_CONSTANT 5
unsigned long int djb2_hash_on_string_index(
  unsigned long int instantaneous_hash, char * str, unsigned long long cur_index);

// TODO: javadoc
// http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/
// Levenshtein_distance#C
#define MIN3(a, b, c) \
    ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
// precondition: strings are same size
unsigned long long get_levenshtein_distance(string_with_size * prev_string,
                                string_with_size * cur_string);

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
} levenshtein_string_edit_operation;

typedef struct {
    unsigned long long * cur_cell;
    unsigned long long cur_x;
    unsigned long long cur_y;
    unsigned long long max_y;
} levenshtein_matrix_state;

bool three_not_null(unsigned long long * x, unsigned long long * y, unsigned long long * z);

unsigned long long min_of_non_null_three(unsigned long long * x, unsigned long long * y, unsigned long long * z);

// TODO: javadoc, including that as a precondition all up/left/up&left moves
// are
// still within the array, and that cur_cell is NOT the base array index, it
// is
// the address of the active cell
// mention that also advances cur cell in an atomic fashion which is kinda
// cool
levenshtein_string_edit_operation
  get_next_levenshtein_operation_and_advance(levenshtein_matrix_state * lms);

// http://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm
// returns GSList * of chars inserted/deleted/substituted and at which
// positions
// NOTE: FREES STRING_WITH_SIZES GIVEN AS INPUT
GSList * get_levenshtein_edits_and_free(string_with_size * prev_string,
                                        string_with_size * cur_string);

char convert_leven_op_to_char(levenshtein_string_edit_operation cur_op);

typedef struct {
    string_with_size * sws;
    unsigned long long index;
} string_with_size_and_index;

void write_leven_char_to_index_of_string(
  levenshtein_string_edit_operation * cur_op,
  string_with_size_and_index * swsai);

string_with_size * format_and_free_levenshtein_list_to_string_with_size(
  GSList * operations_backtrace);

#endif /*___STRING_PROCESSING_H___*/
