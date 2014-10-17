#ifndef ___VCS_CMP_H___
#define ___VCS_CMP_H___

/*
    functions and utilities for implementing character-by-character
    comparison on .vcsfmt-formatted files
*/

#include "block_processing.h"

// TODO: allow modification of this, find reasoning for choice of value
#define LEVENSHTEIN_CHECK_CHARS 80
#define LEVENSHTEIN_CHECK_THRESHOLD 20

// STRING_ID
// TODO: javadoc
typedef struct {
    mpz_t line_number;
    // used because canonical djb2 uses unsigned long int
    unsigned long int str_hash;
    unsigned long int
      str_length; // arbitrary choice of length variable bit width
    string_with_size * first_k_chars;
    bool is_orf;
} line_id;
line_id * line_id_set_is_orf(line_id * sid, bool set_is_orf);
line_id * line_id_set_str_hash(line_id * sid, unsigned long int str_hash);
line_id * line_id_set_str_length(line_id * sid, unsigned long int str_length);
line_id * line_id_set_line_number(line_id * sid, mpz_t * number_to_set_to);
line_id * line_id_initialize_line_number(line_id * sid);
line_id * line_id_set_first_k_chars(line_id * sid,
                                    string_with_size * str_k_chars);
line_id * make_line_id(unsigned long int str_hash,
                       unsigned long int str_length);
line_id * make_line_id_given_string_with_size(unsigned long int str_hash,
                                              unsigned long int str_length,
                                              string_with_size * str_k_chars);
line_id * clone_line_id_with_string_null(line_id * base);
void free_line_id(void * arg);
bool line_id_equal(line_id * a, line_id * b);

// QUEUE_PROCESSING
/**
 * @brief:
 *
 */
// TODO: rename struct so intended usage is clear, mention foreach loop
typedef struct {
    void * data;
    bool boolean;
} boolean_and_data;

// TODO: javadoc all of this
// modify LINES_ABOVE_BELOW_TO_SEARCH, not QUEUE_HASH_CRITICAL_SIZE
#define LINES_ABOVE_BELOW_TO_SEARCH 5
// size of queue wrt # of lines
#define QUEUE_HASH_CRITICAL_SIZE 2 * LINES_ABOVE_BELOW_TO_SEARCH + 1
// compiler will emit a non-inline version of this, since a pointer is taken
// to it when g_queue_foreach is used
void set_bool_if_line_id_match(line_id * prev_line_id,
                               boolean_and_data * bool_data_bundle);
// basically macros
bool is_cur_line_in_prev_queue(GQueue * prev_file_queue,
                               GQueue * cur_file_queue);
#ifdef DEBUG
void print_line_id_first_k_chars(line_id * sid);
#endif

typedef struct {
    line_id * prev_id;
    line_id * cur_id;
} line_id_pair;
line_id_pair * make_line_id_pair(line_id * prev, line_id * cur);
void free_line_id_pair(void * arg);

typedef struct {
    line_id * id;
    bool is_leven_found;
    GSList ** edit_matches;
} line_id_with_edit_match_info;

// compiler will emit a non-inline version of this too, since a pointer is taken
// to it when g_queue_foreach is used
void if_close_levenshtein_dist_add_to_list(
  line_id * prev_line_id, line_id_with_edit_match_info * cur_data);
// basically  macros
boolean_and_data get_if_edit_line_and_if_so_add_to_list(
  GQueue * prev_file_queue, GQueue * cur_file_queue, GSList ** edit_matches);

// CLOBBERS LINES_PROCESSED ARGUMENTS
// i.e. sets them to their new appropriate values
void write_line_and_if_new_add_to_list(
  GQueue * prev_file_line_ids_queue,
  GQueue * cur_file_line_ids_queue,
  size_t * current_streak_of_newly_added_lines,
  mpz_t * input_file_lines_processed_for_edits,
  mpz_t * cur_file_lines_processed,
  mpz_t * output_file_lines_processed,
  bool * break_out_of_vcscmp,
  GSList ** edit_matches,
  FILE * prev_file_used_for_edits,
  FILE * cur_file,
  FILE * out_file);

void initialize_line_id(unsigned long int * ptr_hash,
                        unsigned long int * ptr_length,
                        string_with_size ** sws,
                        bool * ptr_past_k_chars);

void if_within_first_section_write_to_string(unsigned long int ptr_line_length,
                                             string_with_size * sws_first_chars,
                                             string_with_size * sws_block,
                                             size_t ptr_index);

void write_string_and_update_hash_and_line_length(
  unsigned long int * ptr_line_length,
  string_with_size * sws_first_chars,
  string_with_size * sws_block,
  size_t ptr_index,
  unsigned long int * instantaneous_hash,
  char * hash_str,
  bool * ptr_past_k_chars);

// OPTIMIZATION: string probably ok to allocate in loop, but perhaps not
void check_if_past_k_chars_push_tail_and_initialize_line_id(
  bool * ptr_past_k_chars,
  unsigned long int * ptr_line_length,
  GQueue * ids_queue,
  unsigned long int * ptr_line_hash,
  string_with_size ** first_few_chars,
  mpz_t * lines_processed,
  bool is_line_orf);

// TODO: "ironed out" as below
// requires that string be >= CODON_LENGTH chars, which needs to be ironed out
// requires that file be formatted correctly so that lines of orf and non orf
// alternate, but vcsfmt already enforces that
bool is_first_line_orf(string_with_size * first_few_chars);

void react_to_next_character_of_block(string_with_size * input_block,
                                      size_t block_index,
                                      mpz_t * lines_processed,
                                      bool * is_line_orf,
                                      string_with_size ** first_few_chars,
                                      bool * ptr_past_k_chars,
                                      unsigned long int * ptr_line_length,
                                      GQueue * ids_queue,
                                      unsigned long int * ptr_line_hash);

void add_blocks_to_queue(FILE * active_file,
                         GQueue * ids_queue,
                         string_with_size * input_block,
                         bool * ptr_past_k_chars,
                         unsigned long int * ptr_line_length,
                         unsigned long int * ptr_line_hash,
                         string_with_size ** first_few_chars,
                         mpz_t * lines_processed,
                         bool * is_line_orf);

#ifdef DEBUG
void print_line_id_pair(line_id_pair * arg);
#endif

typedef struct {
    FILE * prev_file;
    mpz_t * prev_file_index;
    FILE * cur_file;
    mpz_t * cur_file_index;
    FILE * out_file;
} diff_file_trio_with_indices;

string_with_size * optimal_levenshtein_string_between_lines(FILE * prev_file,
                                                            FILE * cur_file);

// since address being taken for use in for_each, compiler will also emit
// non-inline version
void print_levenshtein_smallest_edits_to_out_file(
  line_id_pair * arg, diff_file_trio_with_indices * files_and_indices);

// TODO: fix clang-format so that pointers are aligned correctly
void vcscmp(const char * prev_filename,
            const char * cur_filename,
            const char * out_filename);

#endif /*___VCS_CMP_H___*/
