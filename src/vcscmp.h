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
    unsigned long int
      str_hash; // used because canonical djb2 uses unsigned long int
    unsigned long int
      str_length; // arbitrary choice of length variable bit width
    string_with_size * first_k_chars;
    bool is_orf;
} line_id;
static inline line_id * line_id_set_is_orf(line_id * sid, bool set_is_orf) {
    sid->is_orf = set_is_orf;
    return sid;
}
static inline line_id * line_id_set_str_hash(line_id * sid,
                                             unsigned long int str_hash) {
    sid->str_hash = str_hash;
    return sid;
}
static inline line_id * line_id_set_str_length(line_id * sid,
                                               unsigned long int str_length) {
    sid->str_length = str_length;
    return sid;
}
static inline line_id * line_id_set_line_number(line_id * sid,
                                                mpz_t * number_to_set_to) {
    mpz_set(sid->line_number, *number_to_set_to);
    return sid;
}
static inline line_id * line_id_initialize_line_number(line_id * sid) {
    mpz_init(sid->line_number); // sets to 0
    return sid;
}
static inline line_id *
  line_id_set_first_k_chars(line_id * sid, string_with_size * str_k_chars) {
    sid->first_k_chars = str_k_chars;
    return sid;
}
static inline line_id * make_line_id(unsigned long int str_hash,
                                     unsigned long int str_length) {
    return line_id_set_first_k_chars(
      line_id_set_str_length(
        line_id_set_str_hash(
          line_id_initialize_line_number(malloc(sizeof(line_id))), str_hash),
        str_length),
      set_string_with_size_readable_bytes(
        make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS),
        LEVENSHTEIN_CHECK_CHARS));
}
static inline line_id *
  make_line_id_given_string_with_size(unsigned long int str_hash,
                                      unsigned long int str_length,
                                      string_with_size * str_k_chars) {
    return line_id_set_first_k_chars(
      line_id_set_str_length(
        line_id_set_str_hash(
          line_id_initialize_line_number(malloc(sizeof(line_id))), str_hash),
        str_length),
      str_k_chars);
}
static inline line_id * clone_line_id_with_string_null(line_id * base) {
    line_id * ret = malloc(sizeof(line_id));
    mpz_init_set(ret->line_number, base->line_number);
    ret->str_hash = base->str_hash;
    ret->first_k_chars = NULL;
    ret->is_orf = base->is_orf;
    return ret;
}
static inline void free_line_id(void * arg) {
    if (NULL != arg) {
        line_id * lid = (line_id *) arg;
        free_string_with_size(lid->first_k_chars);
        mpz_clear(lid->line_number);
        free(lid);
    }
}
static inline bool line_id_equal(line_id * a, line_id * b) {
    return a->str_hash == b->str_hash && a->str_length == b->str_length;
}

// QUEUE_PROCESSING
/**
 * @brief:
 *
 */
// TODO: rename struct so intended usage is clear, mention foreach loop
typedef struct {
    void * data;
    bool * boolean;
} boolean_ptr_and_data;

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
static inline void
  set_bool_if_line_id_match(line_id * prev_line_id,
                            boolean_ptr_and_data * bool_data_bundle) {
    if (line_id_equal(prev_line_id, bool_data_bundle->data)) {
        *bool_data_bundle->boolean = true;
    }
}
// basically macros
static inline bool is_line_id_at_top_in_prev_queue(GQueue * prev_file_queue,
                                                   GQueue * cur_file_queue) {
    bool is_line_id_found = false;
    boolean_ptr_and_data bool_data_bundle;
    bool_data_bundle.data = g_queue_peek_head(cur_file_queue);
    bool_data_bundle.boolean = &is_line_id_found;
    g_queue_foreach(
      prev_file_queue, (GFunc) set_bool_if_line_id_match, &bool_data_bundle);
    return is_line_id_found;
}
#ifdef DEBUG
static inline void print_line_id_first_k_chars(line_id * sid) {
    fprintf(stderr,
            "%.*s",
            (int) sid->first_k_chars->readable_bytes, // int cast required
            sid->first_k_chars->string);
}
#endif

typedef struct {
    line_id * id;
    bool is_leven_found;
    GSList ** line_id_pairs;
} line_id_and_metadata;

typedef struct {
    line_id * prev_id;
    line_id * cur_id;
} line_id_pair;
static inline line_id_pair * make_line_id_pair(line_id * prev, line_id * cur) {
    line_id_pair * ret = malloc(sizeof(line_id_pair));
    ret->prev_id = prev;
    ret->cur_id = cur;
    return ret;
}
static inline void free_line_id_pair(void * arg) {
    if (NULL != arg) {
        line_id_pair * lip = (line_id_pair *) arg;
        free_line_id(lip->prev_id);
        free_line_id(lip->cur_id);
        free(lip);
    }
}

// compiler will emit a non-inline version of this too, since a pointer is taken
// to it when g_queue_foreach is used
static inline void
  if_close_levenshtein_dist_add_to_list(line_id * prev_line_id,
                                        line_id_and_metadata * cur_data) {
    if (prev_line_id->is_orf && cur_data->id->is_orf) {
        // alternative: use &&
        // instead of ==
        // using == makes all the really short non-orfs match by
        // levenshtein which is annoying
        // it could actually be quite useful, though, as long as:
        // TODO: consider instead of absolute levenshtein distance, use
        // levenshtein dist as proportion of overall string length
        size_t leven_dist = get_levenshtein_distance(
          prev_line_id->first_k_chars, cur_data->id->first_k_chars);
        if (leven_dist < LEVENSHTEIN_CHECK_THRESHOLD) {
            cur_data->is_leven_found = true;
            cur_data->id = prev_line_id;
            *cur_data->line_id_pairs =
              g_slist_prepend( // adds in reverse order!!!
                *cur_data->line_id_pairs,
                make_line_id_pair(
                  clone_line_id_with_string_null(prev_line_id),
                  clone_line_id_with_string_null(cur_data->id)));

#ifdef DEBUG
            PRINT_ERROR("CLOSE STRING FOUND BY LEVENSHTEIN EDITS");
            PRINT_ERROR_NO_NEWLINE("LEVEN_DIST: ");
            PRINT_ERROR_SIZE_T_NO_NEWLINE(leven_dist);
            PRINT_ERROR_NEWLINE();
            PRINT_ERROR_NO_NEWLINE("PREV_STRING (LINE ");
            PRINT_ERROR_MPZ_T_NO_NEWLINE(prev_line_id->line_number);
            PRINT_ERROR_NO_NEWLINE(") (CHARS ");
            PRINT_ERROR_SIZE_T_NO_NEWLINE(
              prev_line_id->first_k_chars->readable_bytes);
            PRINT_ERROR_NO_NEWLINE(") (ORF ");
            if (prev_line_id->is_orf) {
                PRINT_ERROR_NO_NEWLINE("YES): ");
            } else {
                PRINT_ERROR_NO_NEWLINE("NO):  ");
            }
            print_line_id_first_k_chars(prev_line_id);
            PRINT_ERROR_NO_NEWLINE("\nCUR_STRING  (LINE ");
            PRINT_ERROR_MPZ_T_NO_NEWLINE(cur_data->id->line_number);
            PRINT_ERROR_NO_NEWLINE(") (CHARS ");
            PRINT_ERROR_SIZE_T_NO_NEWLINE(
              cur_data->id->first_k_chars->readable_bytes);
            PRINT_ERROR_NO_NEWLINE(") (ORF ");
            if (cur_data->id->is_orf) {
                PRINT_ERROR_NO_NEWLINE("YES): ");
            } else {
                PRINT_ERROR_NO_NEWLINE("NO):  ");
            }
            print_line_id_first_k_chars(cur_data->id);
            PRINT_ERROR_NEWLINE();
            PRINT_ERROR("-----------");
#endif

        }
    }
}

// basically  macros
static inline boolean_and_data get_if_edit_line_and_if_so_add_to_list(
  GQueue * prev_file_queue,
  GQueue * cur_file_queue,
  GSList ** edit_matches) {
    line_id_and_metadata liam;
    liam.id = g_queue_peek_head(cur_file_queue);
    liam.is_leven_found = false;
    liam.line_id_pairs = edit_matches;

    g_queue_foreach(
      prev_file_queue, (GFunc) if_close_levenshtein_dist_add_to_list, &liam);
    boolean_and_data return_val;
    if (liam.is_leven_found) {
        return_val.boolean = true;
        return_val.data = liam.id;
    } else {
        return_val.boolean = false;
    }
    return return_val;
}

// CLOBBERS LINES_PROCESSED ARGUMENTS
// i.e. sets them to their new appropriate values
static inline void write_line_and_if_new_add_to_list(
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
  FILE * out_file) {
    // TODO: explain why we chose QUEUE_HASH_CRITICAL_SIZE here and what this is
    if (*current_streak_of_newly_added_lines < QUEUE_HASH_CRITICAL_SIZE &&
        !is_line_id_at_top_in_prev_queue(prev_file_line_ids_queue,
                                         cur_file_line_ids_queue)) {
#ifdef DEBUG
        PRINT_ERROR_NO_NEWLINE("NEWLY ADDED LINE AT LINE ");
        PRINT_ERROR_MPZ_T_NO_NEWLINE(*output_file_lines_processed);
        PRINT_ERROR_NO_NEWLINE(" (CUR: ");
        if (!((line_id *) g_queue_peek_head(cur_file_line_ids_queue))->is_orf) {
            PRINT_ERROR_NO_NEWLINE("NO ");
        }
        PRINT_ERROR_NO_NEWLINE("ORF)");
        PRINT_ERROR_NEWLINE();
        PRINT_ERROR_NO_NEWLINE("PREV_QUEUE SIZE: ");
        PRINT_ERROR_SIZE_T_NO_NEWLINE(
          (size_t) g_queue_get_length(prev_file_line_ids_queue));
        PRINT_ERROR_NEWLINE();
        PRINT_ERROR_NO_NEWLINE("CUR_QUEUE SIZE: ");
        PRINT_ERROR_SIZE_T_NO_NEWLINE(
          (size_t) g_queue_get_length(cur_file_line_ids_queue));
        PRINT_ERROR_NEWLINE();
#endif
        boolean_and_data is_edit_and_line_id_if_so =
          get_if_edit_line_and_if_so_add_to_list(
            prev_file_line_ids_queue, cur_file_line_ids_queue, edit_matches);
        // TODO: actually add to edit_matches list!
        if (is_edit_and_line_id_if_so.boolean) { // if current line is edit line
            write_line_number_from_file_to_file(
              input_file_lines_processed_for_edits,
              &((line_id *) is_edit_and_line_id_if_so.data)->line_number,
              prev_file_used_for_edits,
              out_file);
        } else { // if just new line
            write_single_line_from_file_to_file(
              cur_file_lines_processed, cur_file, out_file);
            ++*current_streak_of_newly_added_lines;
        }
    } else {
        write_line_number_from_file_to_file(
          cur_file_lines_processed,
          &((line_id *) g_queue_peek_head(cur_file_line_ids_queue))
             ->line_number,
          cur_file,
          out_file);
    }
    if (*current_streak_of_newly_added_lines > QUEUE_HASH_CRITICAL_SIZE) {
        *break_out_of_vcscmp = true;
    }
    increment_mpz_t(output_file_lines_processed);
}

static inline void initialize_line_id(unsigned long int * ptr_hash,
                                      unsigned long int * ptr_length,
                                      string_with_size ** sws,
                                      bool * ptr_past_k_chars) {
    *ptr_hash = DJB2_HASH_BEGIN;
    *ptr_length = 0;
    *sws = make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);
    *ptr_past_k_chars = false;
}

static inline void
  if_within_first_section_write_to_string(unsigned long int ptr_line_length,
                                          string_with_size * sws_first_chars,
                                          string_with_size * sws_block,
                                          size_t ptr_index) {
    if (ptr_line_length < LEVENSHTEIN_CHECK_CHARS) {
        sws_first_chars->string[ptr_line_length] = sws_block->string[ptr_index];
    }
}

static inline void write_string_and_update_hash_and_line_length(
  unsigned long int * ptr_line_length,
  string_with_size * sws_first_chars,
  string_with_size * sws_block,
  size_t ptr_index,
  unsigned long int * instantaneous_hash,
  char * hash_str,
  bool * ptr_past_k_chars) {
    if_within_first_section_write_to_string(
      *ptr_line_length, sws_first_chars, sws_block, ptr_index);
    *instantaneous_hash =
      djb2_hash_on_string_index(*instantaneous_hash, hash_str, ptr_index);
    ++*ptr_line_length;
    if (*ptr_line_length >= LEVENSHTEIN_CHECK_CHARS && !*ptr_past_k_chars) {
        *ptr_past_k_chars = true;
    }
}

// TODO: string_is probably ok to allocate in loop, but perhaps not
static inline void check_if_past_k_chars_push_tail_and_initialize_line_id(
  bool * ptr_past_k_chars,
  unsigned long int * ptr_line_length,
  GQueue * ids_queue,
  unsigned long int * ptr_line_hash,
  string_with_size ** first_few_chars,
  mpz_t * lines_processed,
  bool is_line_orf) {
    if (*ptr_past_k_chars) {
        *ptr_line_length = LEVENSHTEIN_CHECK_CHARS;
    }
    g_queue_push_tail(
      ids_queue,
      line_id_set_is_orf(
        line_id_set_line_number(make_line_id_given_string_with_size(
                                  *ptr_line_hash,
                                  *ptr_line_length,
                                  set_string_with_size_readable_bytes(
                                    *first_few_chars, *ptr_line_length)),
                                lines_processed),
        is_line_orf));
    initialize_line_id(
      ptr_line_hash, ptr_line_length, first_few_chars, ptr_past_k_chars);
}

// TODO: "ironed out" as below
// requires that string be >= CODON_LENGTH chars, which needs to be ironed out
// requires that file be formatted correctly so that lines of orf and non orf
// alternate, but vcsfmt already enforces that
static inline bool is_first_line_orf(string_with_size * first_few_chars) {
    return is_start_codon(first_few_chars->string);
}

static inline void
  react_to_next_character_of_block(string_with_size * input_block,
                                   size_t block_index,
                                   mpz_t * lines_processed,
                                   bool * is_line_orf,
                                   string_with_size ** first_few_chars,
                                   bool * ptr_past_k_chars,
                                   unsigned long int * ptr_line_length,
                                   GQueue * ids_queue,
                                   unsigned long int * ptr_line_hash) {
    if (NEWLINE == input_block->string[block_index]) {
        if (mpz_cmp_ui(*lines_processed, 1)) { // if first line
            *is_line_orf = is_first_line_orf(*first_few_chars);
        }
        check_if_past_k_chars_push_tail_and_initialize_line_id(ptr_past_k_chars,
                                                               ptr_line_length,
                                                               ids_queue,
                                                               ptr_line_hash,
                                                               first_few_chars,
                                                               lines_processed,
                                                               *is_line_orf);
        increment_mpz_t(lines_processed);
        *is_line_orf = !*is_line_orf; // flip it
    } else {
        write_string_and_update_hash_and_line_length(ptr_line_length,
                                                     *first_few_chars,
                                                     input_block,
                                                     block_index,
                                                     ptr_line_hash,
                                                     input_block->string,
                                                     ptr_past_k_chars);
    }
}

static inline void add_blocks_to_queue(FILE * active_file,
                                       GQueue * ids_queue,
                                       string_with_size * input_block,
                                       bool * ptr_past_k_chars,
                                       unsigned long int * ptr_line_length,
                                       unsigned long int * ptr_line_hash,
                                       string_with_size ** first_few_chars,
                                       mpz_t * lines_processed,
                                       bool * is_line_orf) {
    if (!(feof(active_file) || ferror(active_file)) &&
        g_queue_get_length(ids_queue) < QUEUE_HASH_CRITICAL_SIZE) {
        read_block(active_file, input_block);
        for (size_t block_index = 0; block_index < input_block->readable_bytes;
             ++block_index) {
            react_to_next_character_of_block(input_block,
                                             block_index,
                                             lines_processed,
                                             is_line_orf,
                                             first_few_chars,
                                             ptr_past_k_chars,
                                             ptr_line_length,
                                             ids_queue,
                                             ptr_line_hash);
        }
        if (feof(active_file) || ferror(active_file)) { // if at end of file
            check_if_past_k_chars_push_tail_and_initialize_line_id(
              ptr_past_k_chars,
              ptr_line_length,
              ids_queue,
              ptr_line_hash,
              first_few_chars,
              lines_processed,
              *is_line_orf);
        }
    }
}

void vcscmp(const char * prev_filename,
            const char * cur_filename,
            const char * out_filename);

#endif /*___VCS_CMP_H___*/
