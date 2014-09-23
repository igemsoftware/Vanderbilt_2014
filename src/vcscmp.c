#include "vcscmp.h" // required

void vcscmp(const char * prev_filename, const char * cur_filename) {
    FILE * prev_file = open_file_read(prev_filename);
    PRINT_ERROR_AND_RETURN_IF_NULL(prev_file, "Error in reading prev file.");
    FILE * cur_file = open_file_read(cur_filename);
    PRINT_ERROR_AND_RETURN_IF_NULL(cur_file, "Error in reading cur file.");
#ifdef CONCURRENT
#error FUNCTIONALITY NOT IMPLEMENTED YET
#else
    // OPTIMIZATION:
    // implement fixed-size array-based queue for speed
    GQueue * prev_file_line_ids_queue = g_queue_new();
    GQueue * cur_file_line_ids_queue = g_queue_new();

    string_with_size * prev_block = make_new_string_with_size(BINBLOCK_SIZE);
    string_with_size * cur_block = make_new_string_with_size(BINBLOCK_SIZE);

    string_with_size * prev_first_few_chars =
      make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);
    string_with_size * cur_first_few_chars =
      make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS);

    unsigned long int prev_file_line_hash = DJB2_HASH_BEGIN;
    unsigned long int prev_file_line_length = 0;
    unsigned long int cur_file_line_hash = DJB2_HASH_BEGIN;
    unsigned long int cur_file_line_length = 0;
    bool prev_length_past_k_chars = false;
    bool cur_length_past_k_chars = false;

    mpz_t prev_lines_processed;
    mpz_init(prev_lines_processed);
    mpz_add_ui(prev_lines_processed, prev_lines_processed, 1);
    mpz_t cur_lines_processed;
    mpz_init(cur_lines_processed);
    mpz_add_ui(cur_lines_processed, cur_lines_processed, 1);

    bool prev_is_line_orf;      // switches every line
    bool cur_is_line_orf;

    mpz_t output_lines_processed;
    mpz_init(output_lines_processed);
    mpz_add_ui(output_lines_processed, output_lines_processed, 1); // start at 1
    size_t current_streak_of_newly_added_lines = 0;
    bool break_out_of_vcscmp = false;

    while ((!(feof(prev_file) || ferror(prev_file)) || // until both files EOF
            !(feof(cur_file) || ferror(cur_file))) &&
           !break_out_of_vcscmp) {
        add_blocks_to_queue(prev_file,
                            prev_file_line_ids_queue,
                            prev_block,
                            &prev_length_past_k_chars,
                            &prev_file_line_length,
                            &prev_file_line_hash,
                            &prev_first_few_chars,
                            &prev_lines_processed,
                            &prev_is_line_orf);
        add_blocks_to_queue(cur_file,
                            cur_file_line_ids_queue,
                            cur_block,
                            &cur_length_past_k_chars,
                            &cur_file_line_length,
                            &cur_file_line_hash,
                            &cur_first_few_chars,
                            &cur_lines_processed,
                            &cur_is_line_orf);
        if (g_queue_get_length(prev_file_line_ids_queue) >=
              QUEUE_HASH_CRITICAL_SIZE &&
            g_queue_get_length(cur_file_line_ids_queue) >=
              QUEUE_HASH_CRITICAL_SIZE) {
            if (mpz_cmp_ui(output_lines_processed,
                           LINES_ABOVE_BELOW_TO_SEARCH) < 0) {
                while (mpz_cmp_ui(output_lines_processed,
                                  LINES_ABOVE_BELOW_TO_SEARCH) < 0 &&
                       !break_out_of_vcscmp) {
                    if_new_line_then_add_to_list(
                      prev_file_line_ids_queue,
                      cur_file_line_ids_queue,
                      &current_streak_of_newly_added_lines,
                      &output_lines_processed,
                      &break_out_of_vcscmp);
                    free_line_id(g_queue_pop_head(cur_file_line_ids_queue));
                    mpz_add_ui(
                      output_lines_processed, output_lines_processed, 1);
                }
            } else {
                if_new_line_then_add_to_list(
                  prev_file_line_ids_queue,
                  cur_file_line_ids_queue,
                  &current_streak_of_newly_added_lines,
                  &output_lines_processed,
                  &break_out_of_vcscmp);
                free_line_id(g_queue_pop_head(prev_file_line_ids_queue));
                free_line_id(g_queue_pop_head(cur_file_line_ids_queue));
                mpz_add_ui(output_lines_processed, output_lines_processed, 1);
            }
        }
    }
    // finish off remainder
    while (!g_queue_is_empty(cur_file_line_ids_queue) &&
           !break_out_of_vcscmp) {
        if_new_line_then_add_to_list(prev_file_line_ids_queue,
                                     cur_file_line_ids_queue,
                                     &current_streak_of_newly_added_lines,
                                     &output_lines_processed,
                                     &break_out_of_vcscmp);
        free_line_id(g_queue_pop_head(cur_file_line_ids_queue));
        mpz_add_ui(output_lines_processed, output_lines_processed, 1);
    }
    // free memory and close open handles
    // TODO: free all string_with_size in queue, and all bignums
    g_queue_free(prev_file_line_ids_queue);
    g_queue_free(cur_file_line_ids_queue);
#endif
}
