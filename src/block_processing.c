#include "block_processing.h"

string_with_size *read_block(FILE *input_file,
                             string_with_size *input_string_with_size) {
  input_string_with_size->readable_bytes =
      fread(input_string_with_size->string, sizeof(char),
            input_string_with_size->size_in_memory, input_file);
  return input_string_with_size;
}

string_with_size *
write_block(FILE *output_file, string_with_size *output_block_with_size) {
  output_block_with_size->readable_bytes =
      fwrite(output_block_with_size->string, sizeof(char),
             output_block_with_size->readable_bytes, output_file);
  return output_block_with_size;
}

mpz_t *increment_mpz_t(mpz_t *in) {
  mpz_add_ui(*in, *in, 1);
  return in;
}

bool less_than_mpz_t(mpz_t *lhs, mpz_t *rhs) {
  return mpz_cmp(*lhs, *rhs) < 0;
}

bool less_than_or_equal_to_mpz_t(mpz_t *lhs, mpz_t *rhs) {
  return mpz_cmp(*lhs, *rhs) <= 0;
}

bool equal_to_mpz_t(mpz_t *lhs, mpz_t *rhs) {
  return mpz_cmp(*lhs, *rhs) == 0;
}

// CLOBBERS CUR_LINE BY SETTING EQUAL TO FINAL_LINE
// starts again from beginning of file if final_line < cur_line
FILE *advance_file_to_line(FILE *file, mpz_t *cur_line, mpz_t *final_line,
                           size_t block_size) {
  if (!less_than_mpz_t(cur_line, final_line)) {
      // if final_line < cur_line
    if (!equal_to_mpz_t(cur_line, final_line)) {
#ifdef DEBUG
      PRINT_ERROR_MPZ_T_NO_NEWLINE(*cur_line);
      PRINT_ERROR_NO_NEWLINE(",");
      PRINT_ERROR_MPZ_T_NO_NEWLINE(*final_line);
      PRINT_ERROR_NEWLINE();
      PRINT_ERROR("REWINDING FILE!!!!!!!");
#endif
      rewind(file); // IFFY: this will DESTROY concurrent access
      mpz_set_ui(*cur_line, 1);
      return advance_file_to_line(file, cur_line, final_line, block_size);
    }
    return file;
  } else {
    string_with_size *in_block = make_new_string_with_size(block_size);
    bool succeeded = false;
    while (!succeeded && !(feof(file) || ferror(file))) {
      read_block(file, in_block);
      for (size_t block_index = 0; block_index < in_block->readable_bytes;
           ++block_index) {
        if (NEWLINE == in_block->string[block_index]) {
          increment_mpz_t(cur_line);
          if (!less_than_mpz_t(cur_line, final_line)) { // if ==
            // go back to beginning of line
            // IFFY: the off-by-one errors here are killer
            fseek(file,
                  ((long)block_index) - ((long)in_block->readable_bytes - 1),
                  SEEK_CUR);
            succeeded = true;
            break;
          }
        }
      }
    }
    free_string_with_size(in_block);
    return file;
  }
}

// CLOBBERS FROM_LINE_NUMBER (increments)
// SETS FILE POINTER TO FIRST CHARACTER OF *NEXT* LINE
// INCLUDES NEWLINE
void write_current_line_of_file(mpz_t *from_line_number, FILE *source_file,
                                FILE *dest_file) {
  string_with_size *io_block = make_new_string_with_size(BIN_BLOCK_SIZE);
  bool succeeded = false;
  while (!succeeded && !(feof(source_file) || ferror(source_file))) {
    read_block(source_file, io_block);
    for (size_t block_index = 0; block_index < io_block->readable_bytes;
         ++block_index) {
      if (NEWLINE == io_block->string[block_index]) {
        // go back to beginning of line
        // IFFY: the off-by-one errors here are killer
        fseek(source_file,
              ((long)block_index) - ((long)io_block->readable_bytes - 1),
              SEEK_CUR);
        // +1 is to include the newline at the end
        set_string_with_size_readable_bytes(io_block, block_index + 1);
        succeeded = true;
        break;
      }
    }
    write_block(dest_file, io_block);
  }
  free_string_with_size(io_block);
  increment_mpz_t(from_line_number);
}

// CLOBBERS FROM_LINE_NUMBER
// i.e. sets it equal to to_line_number + 1
void write_line_number_from_file_to_file(mpz_t *from_line_number,
                                         mpz_t *to_line_number,
                                         FILE *source_file, FILE *dest_file) {
  advance_file_to_line(source_file, from_line_number, to_line_number,
                       BIN_BLOCK_SIZE);
  write_current_line_of_file(from_line_number, source_file, dest_file);
}

string_with_size * get_current_line_of_file(FILE * source_file) {

}

string_with_size *process_block_vcsfmt(string_with_size *input_block_with_size,
                                       string_with_size *output_block_with_size,
                                       bool *is_within_orf, size_t *cur_orf_pos,
                                       char *current_codon_frame,
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
        args->output_block_with_size = make_new_string_with_size(BIN_BLOCK_SIZE);
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

bool is_processing_complete_vcsfmt_concurrent(
    concurrent_read_write_block_args_vcsfmt *args) {
  if (g_async_queue_length(args->active_queue) != 0) {
    return false;
  } else {
    // OPTIMIZATION: make this variable static somehow
    bool result;
    g_mutex_lock(args->process_complete_mutex);
    result = *args->is_processing_complete;
    g_mutex_unlock(args->process_complete_mutex);
    return result;
  }
}

#endif
