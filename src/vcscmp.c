#include "vcscmp.h" // required

extern inline string_id * string_id_set_str_hash(string_id * sid,
                                                 unsigned long str_hash);

extern inline string_id * string_id_set_str_length(string_id * sid,
                                                   unsigned long str_length);

extern inline string_id *
  string_id_set_first_k_chars(string_id * sid, string_with_size * str_k_chars);

extern inline string_id * make_string_id(unsigned long str_hash,
                                         unsigned long str_length);

extern inline void free_string_id(string_id * sid);

extern inline bool string_id_equal(string_id * a, string_id * b);

void set_bool_if_string_id_match(string_id * prev_string_id,
                                 boolean_and_data * bool_data_bundle) {
    if (string_id_equal(prev_string_id, bool_data_bundle->data)) {
        *bool_data_bundle->boolean = true;
    }
}

extern inline bool is_string_id_at_top_in_prev_queue(GQueue * prev_file_queue,
                                                     GQueue * cur_file_queue);

// void set_int_if_levenshtein_match(string_id * prev_string_id,
//                                   index_and_data * index_data_bundle) {
//     size_t levenshtein_dist = get_levenshtein_distance(
//       prev_string_id->first_k_chars, index_data_bundle->data->first_k_chars);
//     if (levenshtein_dist <= LEVENSHTEIN_CHECK_THRESHOLD) {
//         // *index_and_data->index = ;
//     }
// }

// extern inline int
//   where_is_similar_line_to_string_at_top(GQueue * prev_file_queue,
//                                          GQueue * cur_file_queue);

void vcscmp(char * prev_filename, char * cur_filename) {
    FILE * prev_file = open_file_read(prev_filename);
    PRINT_ERROR_AND_RETURN_IF_NULL(prev_file, "Error in reading prev file.");
    FILE * cur_file = open_file_read(cur_filename);
    PRINT_ERROR_AND_RETURN_IF_NULL(cur_file, "Error in reading cur file.");
#ifdef CONCURRENT
#error FUNCTIONALITY NOT IMPLEMENTED YET
#else
    // OPTIMIZATION:
    // implement fixed-size array-based queue for speed
    // written inline instead of in separate function because it is very
    // small and simple
    GQueue * prev_file_string_ids_queue = g_queue_new();
    GQueue * cur_file_string_ids_queue = g_queue_new();

    string_with_size * prev_block = make_new_string_with_size(BINBLOCK_SIZE);
    string_with_size * cur_block = make_new_string_with_size(BINBLOCK_SIZE);

    unsigned long prev_file_instantaneous_hash = DJB2_HASH_BEGIN;
    unsigned long prev_file_instantaneous_length = 0;
    unsigned long cur_file_instantaneous_hash = DJB2_HASH_BEGIN;
    unsigned long cur_file_instantaneous_length = 0;

    mpz_t lines_processed;
    mpz_init(lines_processed);
    mpz_add_ui(lines_processed, lines_processed, 1); // start at line 1!
    size_t current_streak_of_newly_added_lines = 0;
    bool break_out_of_vcscmp = false;

    while ((!(feof(prev_file) || ferror(prev_file)) || // until both files EOF
            !(feof(cur_file) || ferror(cur_file))) &&
           !break_out_of_vcscmp) {
        if (!(feof(prev_file) || ferror(prev_file)) &&
            g_queue_get_length(prev_file_string_ids_queue) <
              QUEUE_HASH_CRITICAL_SIZE) {
            read_block(prev_file, prev_block);
            // OPTIMIZATION: move this index outside of the for loop
            for (size_t prev_block_index = 0; prev_block_index < BINBLOCK_SIZE;
                 ++prev_block_index) {
                if (prev_block->string[prev_block_index] == '\n') {
                    g_queue_push_tail(
                      prev_file_string_ids_queue,
                      make_string_id(prev_file_instantaneous_hash,
                                     prev_file_instantaneous_length));
                    prev_file_instantaneous_hash = DJB2_HASH_BEGIN;
                    prev_file_instantaneous_length = 0;
                } else {
                    prev_file_instantaneous_hash =
                      djb2_hash_on_string_index(prev_file_instantaneous_hash,
                                                prev_block->string,
                                                prev_block_index);
                    ++prev_file_instantaneous_length;
                }
            }
        }
        if (!(feof(cur_file) || ferror(cur_file)) &&
            g_queue_get_length(cur_file_string_ids_queue) <
              QUEUE_HASH_CRITICAL_SIZE) {
            read_block(cur_file, cur_block);
            for (size_t cur_block_index = 0; cur_block_index < BINBLOCK_SIZE;
                 ++cur_block_index) {
                if (cur_block->string[cur_block_index] == '\n') {
                    g_queue_push_tail(
                      cur_file_string_ids_queue,
                      make_string_id(cur_file_instantaneous_hash,
                                     cur_file_instantaneous_length));
                    cur_file_instantaneous_hash = DJB2_HASH_BEGIN;
                    cur_file_instantaneous_length = 0;
                } else {
                    cur_file_instantaneous_hash =
                      djb2_hash_on_string_index(cur_file_instantaneous_hash,
                                                cur_block->string,
                                                cur_block_index);
                    ++cur_file_instantaneous_length;
                }
            }
        }
        if (g_queue_get_length(prev_file_string_ids_queue) >=
              QUEUE_HASH_CRITICAL_SIZE &&
            g_queue_get_length(cur_file_string_ids_queue) >=
              QUEUE_HASH_CRITICAL_SIZE) {
            if (mpz_cmp_ui(lines_processed, LINES_ABOVE_BELOW_TO_SEARCH) < 0) {
                while (mpz_cmp_ui(lines_processed,
                                  LINES_ABOVE_BELOW_TO_SEARCH) < 0 &&
                       !break_out_of_vcscmp) {
                    if_new_line_then_add_to_list(
                      prev_file_string_ids_queue,
                      cur_file_string_ids_queue,
                      &current_streak_of_newly_added_lines,
                      &lines_processed,
                      &break_out_of_vcscmp);
                    free_string_id(g_queue_pop_head(cur_file_string_ids_queue));
                    mpz_add_ui(lines_processed, lines_processed, 1);
                }
            } else {
                if_new_line_then_add_to_list(
                  prev_file_string_ids_queue,
                  cur_file_string_ids_queue,
                  &current_streak_of_newly_added_lines,
                  &lines_processed,
                  &break_out_of_vcscmp);
                free_string_id(g_queue_pop_head(prev_file_string_ids_queue));
                free_string_id(g_queue_pop_head(cur_file_string_ids_queue));
                mpz_add_ui(lines_processed, lines_processed, 1);
            }
        }
    }

    // finish off remainder
    while (!g_queue_is_empty(cur_file_string_ids_queue) &&
           !break_out_of_vcscmp) {
        if_new_line_then_add_to_list(prev_file_string_ids_queue,
                                     cur_file_string_ids_queue,
                                     &current_streak_of_newly_added_lines,
                                     &lines_processed,
                                     &break_out_of_vcscmp);
        free_string_id(g_queue_pop_head(cur_file_string_ids_queue));
        mpz_add_ui(lines_processed, lines_processed, 1);
    }

    // TODO: free all string_with_size in queue
    // also free all bignums!!!!
    // free memory and close open handles
    g_queue_free(prev_file_string_ids_queue);
    g_queue_free(cur_file_string_ids_queue);
#endif
}
