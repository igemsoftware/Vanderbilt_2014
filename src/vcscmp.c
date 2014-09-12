#include "vcscmp.h" // required

void set_bool_if_string_id_match(string_id * prev_string_id,
                                 boolean_and_data * bool_data_bundle) {
    if (string_id_equal(prev_string_id, bool_data_bundle->data)) {
        *bool_data_bundle->boolean = true;
    }
}

void if_close_levenshtein_dist_add_to_list(string_id * prev_string_id,
                                           string_id * cur_string_id) {
    size_t leven_dist = get_levenshtein_distance(prev_string_id->first_k_chars,
                                                 cur_string_id->first_k_chars);
    if (leven_dist < LEVENSHTEIN_CHECK_THRESHOLD) {
#ifdef DEBUG
        PRINT_ERROR("LEVENSHTEIN FOUND");
        PRINT_DIAGNOSTIC_SIZE_T(leven_dist);
#endif
    }
}
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
    GQueue * prev_file_string_ids_queue = g_queue_new();
    GQueue * cur_file_string_ids_queue = g_queue_new();

    string_with_size * prev_block = make_new_string_with_size(BINBLOCK_SIZE);
    string_with_size * cur_block = make_new_string_with_size(BINBLOCK_SIZE);

    string_with_size * prev_first_80_chars =
      make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);
    string_with_size * cur_first_80_chars =
      make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);

    unsigned long prev_file_instantaneous_hash = DJB2_HASH_BEGIN;
    unsigned long prev_file_instantaneous_length = 0;
    unsigned long cur_file_instantaneous_hash = DJB2_HASH_BEGIN;
    unsigned long cur_file_instantaneous_length = 0;

    mpz_t lines_processed;
    mpz_init(lines_processed);
    mpz_add_ui(lines_processed, lines_processed, 1); // start at line 1
    size_t current_streak_of_newly_added_lines = 0;
    bool break_out_of_vcscmp = false;

    while ((!(feof(prev_file) || ferror(prev_file)) || // until both files EOF
            !(feof(cur_file) || ferror(cur_file))) &&
           !break_out_of_vcscmp) {
        if (!(feof(prev_file) || ferror(prev_file)) &&
            g_queue_get_length(prev_file_string_ids_queue) <
              QUEUE_HASH_CRITICAL_SIZE) {
            read_block(prev_file, prev_block);
            for (size_t prev_block_index = 0; prev_block_index < BINBLOCK_SIZE;
                 ++prev_block_index) {
                if (prev_block->string[prev_block_index] == '\n') {
                    g_queue_push_tail(prev_file_string_ids_queue,
                                      make_string_id_given_string_with_size(
                                        prev_file_instantaneous_hash,
                                        prev_file_instantaneous_length,
                                        set_string_with_size_readable_bytes(
                                          prev_first_80_chars,
                                          prev_file_instantaneous_length)));
                    initialize_string_id(&prev_file_instantaneous_hash,
                                         &prev_file_instantaneous_length,
                                         &prev_first_80_chars);
                } else {
                    if_within_first_section_write_to_string(
                      &prev_file_instantaneous_length,
                      prev_first_80_chars,
                      prev_block,
                      &prev_block_index);
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
                      make_string_id_given_string_with_size(
                        cur_file_instantaneous_hash,
                        cur_file_instantaneous_length,
                        set_string_with_size_readable_bytes(
                          cur_first_80_chars, cur_file_instantaneous_length)));
                    initialize_string_id(&cur_file_instantaneous_hash,
                                         &cur_file_instantaneous_length,
                                         &cur_first_80_chars);
                } else {
                    if_within_first_section_write_to_string(
                      &cur_file_instantaneous_length,
                      cur_first_80_chars,
                      cur_block,
                      &cur_block_index);
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
    // free memory and close open handles
    // TODO: free all string_with_size in queue, and all bignums
    g_queue_free(prev_file_string_ids_queue);
    g_queue_free(cur_file_string_ids_queue);
#endif
}
