#include "vcscmp.h"           // required
#include "block_processing.h" // for read_block and preprocessor defines

extern inline string_id * string_id_set_str_hash(string_id * sid,
                                                 unsigned long str_hash);

extern inline string_id * string_id_set_str_length(string_id * sid,
                                                   unsigned long str_length);

extern inline string_id * make_string_id(unsigned long str_hash,
                                         unsigned long str_length);

extern inline bool string_id_equal(string_id * a, string_id * b);

void set_bool_if_string_id_match(string_id * prev_string_id,
                                 boolean_and_data * bool_data_bundle) {
    if (string_id_equal(prev_string_id, bool_data_bundle->data)) {
        *bool_data_bundle->boolean = true;
    }
}

extern inline bool is_string_id_in_prev_queue(GQueue * prev_file_queue,
                                              GQueue * cur_file_queue);

compare_two_result_bytes_processed vcscmp(char * prev_filename,
                                          char * cur_filename) {
    // produce two queues of hashes
    // determine which lines are changes of previous lines
    // write out lines to file (from left or right as appropriate)
    // add changes at bottom (as appropriate)
    compare_two_result_bytes_processed results_for_both_files;
    // TODO: initialize the result values
    // TODO: preformat prev file
    // pre_format_file_vcscmp(prev_filename);
    FILE * prev_file = open_file_read(prev_filename);
    PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(
      prev_file,
      "Error in reading prev file.",
      , // no expr needed
      results_for_both_files);
    // TODO: preformat cur file
    // pre_format_file_vcscmp(cur_filename);
    FILE * cur_file = open_file_read(cur_filename);
    PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(
      cur_file,
      "Error in reading cur file.",
      , // no expr needed
      results_for_both_files);

#ifdef CONCURRENT
// TODO: make array-based queues atomic and VERIFY WITH TEST CODE
#error FUNCTIONALITY NOT IMPLEMENTED YET
#else
    // OPTIMIZATION: actually implement this
    // // implement fixed-size array-based queue for speed
    // // written inline instead of in separate function because it is very
    // small
    // and simple
    // string_id prev_string_ids[QUEUE_HASH_CRITICAL_SIZE];
    // string_id cur_string_ids[QUEUE_HASH_CRITICAL_SIZE];
    // size_t prev_front = 0;
    // // location of first element inserted
    // size_t prev_back = 0;	// one past final element, mod
    // QUEUE_HASH_CRITICAL_SIZE, therefore 0
    // size_t prev_string_ids_filled = 0; // fill array up to
    // QUEUE_HASH_CRITICAL_SIZE before moving start and end marks
    // size_t cur_front = 0;
    // size_t cur_back = 0;
    // size_t cur_string_ids_filled = 0;

    // unsigned long prev_instantaneous_hash = DJB2_HASH_BEGIN;
    // unsigned long cur_instantaneous_hash = DJB2_HASH_BEGIN;

    // size_t prev_string_index;
    // size_t cur_string_index;

    // string_with_size * prev_block = make_new_string_with_size(BINBLOCK_SIZE);
    // string_with_size * cur_block = make_new_string_with_size(BINBLOCK_SIZE);

    // while (!feof(prev_file) &&
    // 			 !ferror(prev_file)
    // &&
    // 			 !feof(cur_file)
    // &&
    // 			 !ferror(cur_file)){
    // 	// read in block from left
    // 	if (prev_string_ids_filled <
    // QUEUE_HASH_CRITICAL_SIZE){
    // 		for
    // (prev_string_index = 0;
    // 				 prev_string_index
    // < BINBLOCK_SIZE;
    // 				 ++prev_string_index){
    // 			prev_instantaneous_hash
    // =
    // djb2_hash_on_string_index(prev_instantaneous_hash,
    // 																													)
    // 		}
    // 		// add
    // one and just increment back
    // 	}
    // 	else{
    // 		// add
    // one and move around queue markers if required
    // 	}
    // 	if (cur_string_ids_filled <
    // QUEUE_HASH_CRITICAL_SIZE){
    // 		// add
    // one and just increment back
    // 	}
    // 	else{
    // 		// add
    // one and move around queue markers if required
    // 	}
    // 	// clear out queues
    // }

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
            prev_block =
              read_block(prev_file, // assignment not necessary, but clearer
                         prev_block);
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
                } else { // can do just "else" because previously formatted at
                         // vcsfmt
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
            cur_block =
              read_block(cur_file, // assignment not necessary, but clearer
                         cur_block);
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
                    if (!is_string_id_in_prev_queue(
                          prev_file_string_ids_queue,
                          cur_file_string_ids_queue)) {
                        ++current_streak_of_newly_added_lines;
#ifdef DEBUG
                        fprintf(stderr, "NEWLY ADDED LINE AT LINE ");
                        mpz_out_str(stderr, 10, lines_processed);
                        fprintf(stderr, "\n");
#else
#error FUNCTIONALITY NOT IMPLEMENTED YET
#endif
                    }
                    if (current_streak_of_newly_added_lines >
                        QUEUE_HASH_CRITICAL_SIZE) {
                        break_out_of_vcscmp = true;
                    }
                    g_queue_pop_head(
                      cur_file_string_ids_queue); // NOT popping prev queue
                    mpz_add_ui(lines_processed, lines_processed, 1);
                }
            } else {
                if (!is_string_id_in_prev_queue(prev_file_string_ids_queue,
                                                cur_file_string_ids_queue)) {
                    ++current_streak_of_newly_added_lines;
#ifdef DEBUG
                    fprintf(stderr, "NEWLY ADDED LINE AT LINE ");
                    mpz_out_str(stderr, 10, lines_processed);
                    fprintf(stderr, "\n");
#else
#error FUNCTIONALITY NOT IMPLEMENTED YET
#endif
                }
                if (current_streak_of_newly_added_lines >
                    QUEUE_HASH_CRITICAL_SIZE) {
                    break_out_of_vcscmp = true;
                }
                g_queue_pop_head(prev_file_string_ids_queue);
                g_queue_pop_head(cur_file_string_ids_queue);
                mpz_add_ui(lines_processed, lines_processed, 1);
            }
        }
    }

    // finish off remainder
    while (!g_queue_is_empty(cur_file_string_ids_queue)) {
        if (!is_string_id_in_prev_queue(prev_file_string_ids_queue,
                                        cur_file_string_ids_queue)) {
            ++current_streak_of_newly_added_lines;
#ifdef DEBUG
            fprintf(stderr, "NEWLY ADDED LINE AT LINE ");
            mpz_out_str(stderr, 10, lines_processed);
            fprintf(stderr, "\n");
#else
#error "FUNCTIONALITY NOT IMPLEMENTED YET"
#endif
        }
        if (current_streak_of_newly_added_lines > QUEUE_HASH_CRITICAL_SIZE) {
            break_out_of_vcscmp = true;
        }
        g_queue_pop_head(cur_file_string_ids_queue); // NOT popping prev
        mpz_add_ui(lines_processed, lines_processed, 1);
    }

    // free memory and close open handles
    g_queue_free(prev_file_string_ids_queue);
    g_queue_free(cur_file_string_ids_queue);
#endif
    return results_for_both_files;
}
