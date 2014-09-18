#include "vcscmp.h" // required

void set_bool_if_string_id_match(string_id * prev_string_id,
                                 boolean_and_data * bool_data_bundle) {
    if (string_id_equal(prev_string_id, bool_data_bundle->data)) {
        *bool_data_bundle->boolean = true;
    }
}

#ifdef DEBUG
static inline void print_string_id_first_k_chars(string_id * sid) {
    fprintf(
      stderr, "%.*s", LEVENSHTEIN_CHECK_CHARS, sid->first_k_chars->string);
}
#endif

void if_close_levenshtein_dist_add_to_list(string_id * prev_string_id,
                                           string_id * cur_string_id) {
    size_t leven_dist = get_levenshtein_distance(prev_string_id->first_k_chars,
                                                 cur_string_id->first_k_chars);
    if (leven_dist < LEVENSHTEIN_CHECK_THRESHOLD) {
#ifdef DEBUG
        PRINT_ERROR_WITH_NEWLINE("LEVENSHTEIN FOUND");
        PRINT_ERROR_NO_NEWLINE("LEVEN_DIST: ");
        PRINT_ERROR_SIZE_T_NO_NEWLINE(leven_dist);
        PRINT_ERROR_NEWLINE();
        PRINT_ERROR_NO_NEWLINE("PREV_STRING (");
        PRINT_ERROR_SIZE_T_NO_NEWLINE(
          prev_string_id->first_k_chars->readable_bytes);
        PRINT_ERROR_NO_NEWLINE("):\t");
        print_string_id_first_k_chars(prev_string_id);
        PRINT_ERROR_NO_NEWLINE("\nCUR_STRING: (");
        PRINT_ERROR_SIZE_T_NO_NEWLINE(
          cur_string_id->first_k_chars->readable_bytes);
        PRINT_ERROR_NO_NEWLINE("):\t");
        print_string_id_first_k_chars(cur_string_id);
        PRINT_ERROR_NEWLINE();
        PRINT_ERROR_WITH_NEWLINE("-----------");
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

    string_with_size * prev_first_few_chars =
      make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);
    string_with_size * cur_first_few_chars =
      make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);

    unsigned long prev_file_line_hash = DJB2_HASH_BEGIN;
    unsigned long prev_file_line_length = 0;
    unsigned long cur_file_line_hash = DJB2_HASH_BEGIN;
    unsigned long cur_file_line_length = 0;
    bool prev_length_past_k_chars = false;
    bool cur_length_past_k_chars = false;

    mpz_t lines_processed;
    mpz_init(lines_processed);
    mpz_add_ui(lines_processed, lines_processed, 1); // start at line 1
    size_t current_streak_of_newly_added_lines = 0;
    bool break_out_of_vcscmp = false;

    while ((!(feof(prev_file) || ferror(prev_file)) || // until both files EOF
            !(feof(cur_file) || ferror(cur_file))) &&
           !break_out_of_vcscmp) {
        add_blocks_to_queue(prev_file,
                            prev_file_string_ids_queue,
                            prev_block,
                            &prev_length_past_k_chars,
                            &prev_file_line_length,
                            &prev_file_line_hash,
                            &prev_first_few_chars);
        add_blocks_to_queue(cur_file,
                            cur_file_string_ids_queue,
                            cur_block,
                            &cur_length_past_k_chars,
                            &cur_file_line_length,
                            &cur_file_line_hash,
                            &cur_first_few_chars);
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
