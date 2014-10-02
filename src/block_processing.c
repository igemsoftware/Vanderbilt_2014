#include "block_processing.h"

// PROCESS_BLOCK
string_with_size * process_block_vcsfmt(string_with_size * input_block_with_size,
                                 string_with_size * output_block_with_size,
                                 bool * is_within_orf,
                                 size_t * cur_orf_pos,
                                 char * current_codon_frame,
                                 bool is_final_block) {
        output_block_with_size->readable_bytes = 0;
    for (size_t codon_index = 0;
         codon_index < input_block_with_size->readable_bytes;
         ++codon_index) {
        current_codon_frame[CODON_LENGTH - 1] =
          input_block_with_size->string[codon_index];
        // first base is only null at start/end of ORF or at beginning
        if (current_codon_frame[0] !=
            '\0') {
            if (*is_within_orf) { // same here
                if (*cur_orf_pos >= MIN_ORF_LENGTH - CODON_LENGTH &&
                    is_stop_codon(current_codon_frame)) {
                    for (size_t base_index = 0; base_index < CODON_LENGTH;
                         ++base_index) {
                        output_block_with_size
                          ->string[output_block_with_size->readable_bytes +
                                   base_index] =
                          current_codon_frame[base_index];
                        current_codon_frame[base_index] =
                          '\0'; // nullify to read in more characters
                    }
                    output_block_with_size
                      ->string[output_block_with_size->readable_bytes +
                               CODON_LENGTH] = NEWLINE;
                    // output_block_with_size->readable_bytes INCREMENTED AT END
                    // OF LOOP
                    output_block_with_size->readable_bytes += CODON_LENGTH;
                    *is_within_orf = false;
                    *cur_orf_pos = 0;
                } else {
                    for (size_t base_index = 0; base_index < CODON_LENGTH;
                         ++base_index) {
                        output_block_with_size
                          ->string[output_block_with_size->readable_bytes +
                                   base_index] =
                          current_codon_frame[base_index];
                        current_codon_frame[base_index] = '\0';
                    }
                    *cur_orf_pos += 3;
                    output_block_with_size->readable_bytes += 2;
                }
            } else {
                if (is_start_codon(current_codon_frame)) {
                    output_block_with_size
                      ->string[output_block_with_size->readable_bytes] = NEWLINE;
                    for (size_t base_index = 0; base_index < CODON_LENGTH;
                         ++base_index) {
                        output_block_with_size
                          ->string[output_block_with_size->readable_bytes +
                                   base_index + 1] =
                          current_codon_frame[base_index];
                        current_codon_frame[base_index] =
                          '\0'; // nullify to read in more characters
                    }
                    output_block_with_size->readable_bytes += CODON_LENGTH;
                    output_block_with_size
                      ->string[output_block_with_size->readable_bytes +
                               CODON_LENGTH] = NEWLINE;
                    *is_within_orf = true;
                    *cur_orf_pos = 3;
                } else {
                    output_block_with_size
                      ->string[output_block_with_size->readable_bytes] =
                      current_codon_frame[0];
                }
            }
            ++output_block_with_size->readable_bytes;
        }
        // shuffle bases over
        for (size_t base_index = 0; base_index < CODON_LENGTH - 1;
             ++base_index) {
            current_codon_frame[base_index] =
              current_codon_frame[base_index + 1];
        }
        current_codon_frame[CODON_LENGTH - 1] = '\0'; // nullify final
        // leaves first two codons in current_codon_frame pointer for next block
    }
    // if this is the last block, eject the last two bases
    if (is_final_block) {
        for (size_t base_index = 0; base_index < CODON_LENGTH - 1;
             ++base_index) {
            if (output_block_with_size
                  ->string[output_block_with_size->readable_bytes +
                           base_index] != '\0') {
                output_block_with_size
                  ->string[output_block_with_size->readable_bytes +
                           base_index] = current_codon_frame[base_index];
            }
        }
        output_block_with_size->readable_bytes += CODON_LENGTH - 1;
    }

    return output_block_with_size;
}

string_with_size * de_process_block_vcsfmt(string_with_size * input_block_with_size,
                                    string_with_size * output_block_with_size) {
    output_block_with_size->readable_bytes = 0;
    for (size_t bytes_read = 0;
         bytes_read < input_block_with_size->readable_bytes;
         ++bytes_read) {
        if (input_block_with_size->string[bytes_read] != NEWLINE) {
            output_block_with_size
              ->string[output_block_with_size->readable_bytes] =
              input_block_with_size->string[bytes_read];
            ++output_block_with_size->readable_bytes;
        }
    }
    return output_block_with_size;
}

#ifdef CONCURRENT
void concurrent_read_and_process_block_vcsfmt(
  concurrent_read_and_process_block_args_vcsfmt * args) {
    while (!feof(args->input_file) && !ferror(args->input_file)) {
        add_to_bytes_processed(
          args->total_bytes_read,
          read_block(args->input_file, args->input_block_with_size)
            ->readable_bytes);
        // OPTIMIZATION: allocate from (possibly self-growing) pool of memory
        args->output_block_with_size = make_new_string_with_size(BINBLOCK_SIZE);
        process_block_vcsfmt(args->input_block_with_size,
                      args->output_block_with_size,
                      args->is_within_orf,
                      args->cur_orf_pos,
                      args->current_codon_frame,
                      feof(args->input_file));
        if (feof(args->input_file)) { // if last loop
            g_mutex_lock(args->process_complete_mutex);
        }
        g_async_queue_push(args->active_queue, args->output_block_with_size);
    }
    *args->is_processing_complete = true;
    g_mutex_unlock(args->process_complete_mutex);
}

void concurrent_write_block_vcsfmt(concurrent_read_write_block_args_vcsfmt * args) {
    while (!is_processing_complete_vcsfmt_concurrent(args)) {
        g_mutex_unlock(args->process_complete_mutex);
        args->active_block_with_size =
          (string_with_size *) g_async_queue_pop(args->active_queue);
        add_to_bytes_processed(
          args->total_bytes_written,
          write_block(args->active_file, args->active_block_with_size)
            ->readable_bytes);
        free_string_with_size(args->active_block_with_size);
    }
    // get the rest left over (if any)
    for (size_t queue_size = g_async_queue_length(args->active_queue);
         queue_size != 0;
         --queue_size) {
        args->active_block_with_size =
          (string_with_size *) g_async_queue_pop(args->active_queue);
        add_to_bytes_processed(
          args->total_bytes_written,
          write_block(args->active_file, args->active_block_with_size)
            ->readable_bytes);
        free_string_with_size(args->active_block_with_size);
    }
}
#endif
